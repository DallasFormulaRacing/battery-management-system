/*
 * The BMS's own main-bus messages (bms/submodules/ev27-can/can/main/bms.yaml):
 * the four hooks the generic tasks in ev27_can_tasks.c call.
 *
 * Data comes from hbms (program/bms.c) and g_charger (program/supervisor.c).
 * The BMS state machine updates those under bms_mutex_id, sometimes holding it
 * for tens of ms, so the TX task never waits for it: it copies what it needs
 * into a snapshot when the mutex is free and sends from the snapshot.
 *
 * Pack layout on the bus: 6 segments x 2 ADBMS6830 x 12 cells, 2 x 10
 * thermistors per segment. Chain IC i is segment i / 2, cells 1-12 (even i) or
 * 13-24 (odd i). ICs not in the chain (NUM_IC_COUNT_CHAIN < 12 on the bench)
 * are sent as NotPresent, and so are the cells or temperatures of an IC whose
 * last reading failed (PEC error, or the IC was reset).
 *
 * Status sensor bits (BMS):  bit0 = cell monitors (ADBMS6830 chain)
 *                            bit1 = thermistors
 * Status ErrorFlags (BMS):   same bits as BMS_State.FaultFlags
 */
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "bms.h"
#include "bms_limits.h"
#include "config.h"
#include "imd.h"
#include "main.h"
#include "state.h"
#include "supervisor.h"

#include "ev27_device_config.h"
#include "ev27_main.h"
#include "ev27_node.h"
#include "ev27_port.h"

extern osMutexId_t bms_mutex_id;
extern charger_t g_charger;

#define EV27_SEGMENTS        6u
#define EV27_ICS_PER_SEGMENT 2u
#define EV27_CELLS_PER_IC    12u
#define EV27_THERMS_PER_IC   10u
#define EV27_ICS_IN_PACK     (EV27_SEGMENTS * EV27_ICS_PER_SEGMENT)

_Static_assert(NUM_CELLS_PER_SEGMENT == EV27_CELLS_PER_IC,
               "bms.yaml packs 12 cells per ADBMS6830 (NUM_CELLS_PER_SEGMENT is per IC)");
_Static_assert(NUM_THERM_PER_SEGMENT == EV27_THERMS_PER_IC,
               "bms.yaml packs 10 thermistors per ADBMS6830 (NUM_THERM_PER_SEGMENT is per IC)");
_Static_assert(NUM_IC_COUNT_CHAIN <= EV27_ICS_IN_PACK, "more ICs than bms.yaml has room for");

_Static_assert(IMD_CONFIG_NOT_SEEN == EV27_MAIN_BMS_STATE_IMD_CONFIG_NOT_SEEN_CHOICE &&
               IMD_CONFIG_CHECKING == EV27_MAIN_BMS_STATE_IMD_CONFIG_CHECKING_CHOICE &&
               IMD_CONFIG_OK == EV27_MAIN_BMS_STATE_IMD_CONFIG_OK_CHOICE &&
               IMD_CONFIG_FAILED == EV27_MAIN_BMS_STATE_IMD_CONFIG_FAILED_CHOICE &&
               IMD_CONFIG_LOCKED == EV27_MAIN_BMS_STATE_IMD_CONFIG_LOCKED_CHOICE &&
               IMD_CONFIG_LOST == EV27_MAIN_BMS_STATE_IMD_CONFIG_LOST_CHOICE,
               "imd_config_state_t must match BMS_State.ImdConfig in bms.yaml");

#define CELL_NOT_PRESENT ((int16_t)EV27_MAIN_BMS_SEG1_CELLS_CELL01_NOT_PRESENT_CHOICE) /* raw 0x8000 */
#define TEMP_NOT_PRESENT ((int8_t)EV27_MAIN_BMS_SEG1_TEMPS_TEMP01_NOT_PRESENT_CHOICE)  /* -128 */

/* When each message goes out, as the tick number n (10 ms each) modulo its
 * period in ticks. The phases are spread so that each tick queues at most one
 * data frame (plus Status every 500 ms), which keeps FDCAN2's 3-deep TX FIFO
 * free for the GUI. */
#define PHASE_STATE         0u   /* n % 10 */
#define PHASE_PACK          1u   /* n % 10 */
#define PHASE_CELL_SUMMARY  2u   /* n % 10 */
#define PHASE_LIMITS        3u   /* n % 10 */
#define PHASE_SEG_CELLS     4u   /* n % 20: segment i at 4 + i (4..9) */
#define PHASE_SEG_TEMPS     14u  /* n % 100: segment i at 14 + i (14..19) */
#define PHASE_BALANCING     34u  /* n % 100 */
#define PHASE_CHARGING      54u  /* n % 100 */

#define EVERY(n, period_ms, phase) (((n) % ((period_ms) / EV27_TICK_MS)) == (phase))

/* ---- Snapshot of the BMS data -------------------------------------------- */

typedef struct {
    int16_t  cell[EV27_ICS_IN_PACK][EV27_CELLS_PER_IC];   /* raw filtered C-ADC codes */
    int8_t   temp[EV27_ICS_IN_PACK][EV27_THERMS_PER_IC];  /* degC, TEMP_NOT_PRESENT if open or invalid */
    uint16_t balancing[EV27_ICS_IN_PACK];                 /* bit c = cell c+1 balance PWM on */
    uint16_t fault_flags;                                 /* bit n = bms_fault_t n */
    bool     any_cell;                                    /* at least one cell has a reading */
    bool     any_temp;                                    /* at least one thermistor has a reading */
    uint8_t  state;                                       /* bms_state_t as reported */
    uint8_t  error_code;                                  /* bms_fault_t */
    uint8_t  charging_state;                              /* charging_state_t */
    bool     session_active;
    bool     setpoint_valid;
    uint16_t requested_voltage_dv;
    uint16_t requested_current_da;
} bms_snapshot_t;

static bms_snapshot_t s_snap;
static bool s_snap_due = true;
static uint8_t s_state_counter;
static volatile uint8_t s_charge_last_error = EV27_MAIN_BMS_CHARGING_LAST_ERROR_NONE_CHOICE;
static bool s_session_was_active;

/* VCU_State.FlashAllowed and when it last came (RX task writes, RX task reads
 * through the Command handler). */
static bool s_vcu_flash_allowed;
static uint32_t s_vcu_state_ms;
static bool s_vcu_seen;
#define VCU_STATE_FRESH_MS (EV27_MAIN_VCU_STATE_CYCLE_TIME_MS * 4u)

#if BMS_BENCH_REBOOT_WITHOUT_VCU
#warning "BMS_BENCH_REBOOT_WITHOUT_VCU is on: the CM5 can reboot the BMS without VCU FlashAllowed"
#endif

#define FAULT_BIT(f) ((uint16_t)(1u << (unsigned)(f)))

static float raw_to_volts(int16_t raw)
{
    return ((float)raw * 0.00015f) + 1.5f;   /* same as convert_voltage_human_readable() */
}

static int8_t temp_to_i8(float t, segment_fault_type_t status)
{
    /* -273.15 = thermistor_from_adc() out of its table; !(t >= x) also catches NaN */
    if (status == OPEN_WIRE_FAULT || !(t >= -127.0f)) {
        return TEMP_NOT_PRESENT;
    }
    if (t > 127.0f) {
        return 127;
    }
    return (int8_t)((t >= 0.0f) ? (t + 0.5f) : (t - 0.5f));
}

/* Copies everything the messages need, if the BMS isn't holding bms_mutex_id.
 * Returns false (and keeps the old snapshot) if it is. */
static bool take_snapshot(void)
{
    const float ov = g_voltage_cfg.overvoltage_threshold_v;
    const float uv = g_voltage_cfg.undervoltage_threshold_v;
    const float ot = g_voltage_cfg.overtemp_threshold_c;
    const float ut = g_voltage_cfg.undertemp_threshold_c;
    uint16_t flags = 0;
    bool any_cell = false;
    bool any_temp = false;
    uint32_t ic, k;

    if (osMutexAcquire(bms_mutex_id, 0) != osOK) {
        return false;
    }

    for (ic = 0; ic < EV27_ICS_IN_PACK; ic++) {
        const cell_asic_ctx_t *a;
        uint16_t bal = 0;
        bool cells_ok, temps_ok;

        if (ic >= NUM_IC_COUNT_CHAIN) {
            for (k = 0; k < EV27_CELLS_PER_IC; k++) {
                s_snap.cell[ic][k] = CELL_NOT_PRESENT;
            }
            for (k = 0; k < EV27_THERMS_PER_IC; k++) {
                s_snap.temp[ic][k] = TEMP_NOT_PRESENT;
            }
            s_snap.balancing[ic] = 0;
            continue;
        }
        a = &hbms.asic[ic];
        cells_ok = bms_ic_cells_valid((uint8_t)ic);
        temps_ok = bms_ic_temps_valid((uint8_t)ic);

        for (k = 0; k < EV27_CELLS_PER_IC; k++) {
            /* last reading failed its PEC or the IC was reset: no value */
            const int16_t raw = cells_ok ? a->filt_cell.filt_cell_voltages_array[k] : CELL_NOT_PRESENT;
            s_snap.cell[ic][k] = raw;
            if (a->cell_fault_status[k] == OPEN_WIRE_FAULT) {
                flags |= FAULT_BIT(BMS_ERR_CELL_OPENWIRE);
            }
            if (raw != CELL_NOT_PRESENT) {
                const float v = raw_to_volts(raw);
                any_cell = true;
                if (v > ov) {
                    flags |= FAULT_BIT(BMS_ERR_CELL_OV);
                }
                if (v < uv) {
                    flags |= FAULT_BIT(BMS_ERR_CELL_UV);
                }
            }
            if (a->pwm_ctl_a.pwm_a_ctl_array[k] != PWM_0_0_PERCENT_DUTY_CYCLE) {
                bal |= (uint16_t)(1u << k);
            }
        }
        s_snap.balancing[ic] = bal;

        /* Open wire is latched by therm_open_wire_check() until reset. */
        for (k = 0; k < EV27_THERMS_PER_IC; k++) {
            const int8_t t = temps_ok ? temp_to_i8(a->thermistor[k], a->thermistor_fault_status[k])
                                      : TEMP_NOT_PRESENT;
            s_snap.temp[ic][k] = t;
            if (a->thermistor_fault_status[k] == OPEN_WIRE_FAULT) {
                flags |= FAULT_BIT(BMS_ERR_AUX_OPENWIRE);
            }
            if (t != TEMP_NOT_PRESENT) {
                any_temp = true;
                if ((float)t > ot) {
                    flags |= FAULT_BIT(BMS_ERR_THERM_OVER_TEMP);
                }
                if ((float)t < ut) {
                    flags |= FAULT_BIT(BMS_ERR_THERM_UNDER_TEMP);
                }
            }
        }
    }

    if (bms_readings_failing()) {
        flags |= FAULT_BIT(BMS_ERR_PEC_FAIL);   /* counting towards BMS_PEC_FAIL_LIMIT */
    }
    s_snap.state = (uint8_t)hbms.state.current_state;
    s_snap.error_code = (uint8_t)hbms.state.error_code;
    s_snap.charging_state = (uint8_t)g_charger.state;
    s_snap.setpoint_valid = g_charger.setpoint_valid;
    s_snap.requested_voltage_dv = g_charger.requested_voltage_dv;
    s_snap.requested_current_da = g_charger.requested_current_da;
    (void)osMutexRelease(bms_mutex_id);

    s_snap.session_active = charging_session_active();
    /* Charging with no session lasts one pass on the way back to Measure:
     * report Charging only while a session is on. */
    if (s_snap.state == BMS_STATE_CHARGING && !s_snap.session_active) {
        s_snap.state = BMS_STATE_MEASURE;
    }
    if (s_snap.error_code != BMS_ERR_NONE) {
        flags |= FAULT_BIT(s_snap.error_code);
    }
    s_snap.fault_flags = flags;
    s_snap.any_cell = any_cell;
    s_snap.any_temp = any_temp;
    return true;
}

static bool bms_faulted(void)
{
    return s_snap.state == BMS_STATE_FAULT;
}

/* ---- Receiving -------------------------------------------------------------- */

/* CM5_ChargeControl (0x782): the same actions as the GUI's 29-bit charging
 * commands in lib/gui/src/gui_drivers.c, under the same lock. */
static void handle_charge_control(const ev27_rx_frame_t *fr)
{
    struct ev27_main_cm5_charge_control_t m;
    uint8_t err = EV27_MAIN_BMS_CHARGING_LAST_ERROR_NONE_CHOICE;

    if (ev27_main_cm5_charge_control_unpack(&m, fr->data, fr->len) != 0) {
        s_charge_last_error = EV27_MAIN_BMS_CHARGING_LAST_ERROR_INVALID_COMMAND_CHOICE;
        return;
    }
    if (osMutexAcquire(bms_mutex_id, 1000) != osOK) {
        return;   /* the BMS is stuck; the GUI's next keepalive retries */
    }
    switch (m.command) {
    case EV27_MAIN_CM5_CHARGE_CONTROL_COMMAND_KEEPALIVE_CHOICE:
        if (!charger_setpoint_in_range(m.voltage_setpoint, m.current_setpoint)) {
            err = EV27_MAIN_BMS_CHARGING_LAST_ERROR_INVALID_SETPOINT_CHOICE;
        }
        /* forwarded either way: an out-of-range setpoint cancels the current one */
        charger_update_requested_setpoints(m.voltage_setpoint, m.current_setpoint);
        charging_session_kick_wdt();
        break;
    case EV27_MAIN_CM5_CHARGE_CONTROL_COMMAND_CONNECT_CHOICE:
        charging_session_enable();
        break;
    case EV27_MAIN_CM5_CHARGE_CONTROL_COMMAND_DISCONNECT_CHOICE:
    case EV27_MAIN_CM5_CHARGE_CONTROL_COMMAND_SOFT_STOP_CHOICE:
        charging_session_disable();
        break;
    default:
        err = EV27_MAIN_BMS_CHARGING_LAST_ERROR_INVALID_COMMAND_CHOICE;
        break;
    }
    (void)osMutexRelease(bms_mutex_id);
    s_charge_last_error = err;
}

static void handle_vcu_state(const ev27_rx_frame_t *fr)
{
    struct ev27_main_vcu_state_t m;

    if (ev27_main_vcu_state_unpack(&m, fr->data, fr->len) != 0) {
        return;
    }
    s_vcu_flash_allowed = (m.flash_allowed != 0u);
    s_vcu_state_ms = ev27_port_uptime_ms();
    s_vcu_seen = true;
}

bool ev27_device_reboot_allowed(void)
{
    const bool vcu_allows = s_vcu_seen && s_vcu_flash_allowed &&
                            (ev27_port_uptime_ms() - s_vcu_state_ms) < VCU_STATE_FRESH_MS;

    if (!vcu_allows && !BMS_BENCH_REBOOT_WITHOUT_VCU) {
        return false;   /* car may be driving */
    }
    /* Stop the charging session first (CM5 ChargeControl SoftStop), so the
     * charger is told to stop rather than left to time out. Balancing only
     * happens inside a session; the 6830s' PWM is cleared again at boot.
     * Fault does not block: the AMS latch is hardware, so it stays open. */
    return !charging_session_active();
}

void ev27_device_rx(const ev27_rx_frame_t *fr)
{
    switch (fr->id) {
    case EV27_MAIN_CM5_CHARGE_CONTROL_FRAME_ID:
        handle_charge_control(fr);
        break;
    case EV27_MAIN_VCU_STATE_FRAME_ID:
        handle_vcu_state(fr);
        break;
    default:
        break;
    }
}

/* ---- Sending ---------------------------------------------------------------- */

static void send_state(void)
{
    struct ev27_main_bms_state_t m;
    uint8_t buf[EV27_MAIN_BMS_STATE_LENGTH];
    uint32_t ic;
    bool balancing = false;

    for (ic = 0; ic < EV27_ICS_IN_PACK; ic++) {
        balancing = balancing || (s_snap.balancing[ic] != 0u);
    }
    memset(&m, 0, sizeof(m));
    m.state = s_snap.state;
    m.charging_state = s_snap.charging_state;
    m.fault_code = s_snap.error_code;
    m.fault_flags = s_snap.fault_flags;
    /* SDC pin: high = closed (OK), low = opened by the BMS */
    m.shutdown_open = (HAL_GPIO_ReadPin(SDC_Pin_GPIO_Port, SDC_Pin_Pin) == GPIO_PIN_RESET) ? 1u : 0u;
    m.balancing = balancing ? 1u : 0u;
    m.ic_count = NUM_IC_COUNT_CHAIN;
    m.therm_faults_ignored = BMS_BENCH_IGNORE_THERM_FAULTS ? 1u : 0u;
    m.imd_config = (uint8_t)imd_get_status().config;
    m.counter = s_state_counter;
    s_state_counter = (uint8_t)((s_state_counter + 1u) & 0x0Fu);
    if (ev27_main_bms_state_pack(buf, &m, sizeof(buf)) > 0) {
        (void)ev27_node_send(EV27_MAIN_BMS_STATE_FRAME_ID, buf, sizeof(buf));
    }
}

static void send_pack(void)
{
    struct ev27_main_bms_pack_t m;
    uint8_t buf[EV27_MAIN_BMS_PACK_LENGTH];
    float sum_v = 0.0f;
    uint32_t ic, k;

    for (ic = 0; ic < EV27_ICS_IN_PACK; ic++) {
        for (k = 0; k < EV27_CELLS_PER_IC; k++) {
            if (s_snap.cell[ic][k] != CELL_NOT_PRESENT) {
                sum_v += raw_to_volts(s_snap.cell[ic][k]);
            }
        }
    }
    if (sum_v < 0.0f) {
        sum_v = 0.0f;
    }
    memset(&m, 0, sizeof(m));
    m.pack_cell_voltage = (sum_v >= 655.35f) ? 65535u : (uint16_t)(sum_v * 100.0f + 0.5f);
    m.pack_sensed_voltage = 0;   /* TODO: no pack voltage measurement on the BMS yet */
    m.pack_current = 0;          /* TODO: CAB500 on FDCAN1 (500k work) */
    m.state_of_charge = 0;       /* TODO: no SOC estimate yet */
    m.current_valid = 0;
    if (ev27_main_bms_pack_pack(buf, &m, sizeof(buf)) > 0) {
        (void)ev27_node_send(EV27_MAIN_BMS_PACK_FRAME_ID, buf, sizeof(buf));
    }
}

static void send_cell_summary(void)
{
    struct ev27_main_bms_cell_summary_t m;
    uint8_t buf[EV27_MAIN_BMS_CELL_SUMMARY_LENGTH];
    int32_t v_sum = 0, t_sum = 0;
    uint32_t v_n = 0, t_n = 0;
    uint32_t ic, k;

    memset(&m, 0, sizeof(m));
    m.min_cell_voltage = INT16_MAX;
    m.max_cell_voltage = INT16_MIN;
    m.min_temp = INT8_MAX;
    m.max_temp = INT8_MIN;

    for (ic = 0; ic < EV27_ICS_IN_PACK; ic++) {
        for (k = 0; k < EV27_CELLS_PER_IC; k++) {
            const int16_t v = s_snap.cell[ic][k];
            const uint8_t idx = (uint8_t)(ic * EV27_CELLS_PER_IC + k + 1u);   /* 1..144 */
            if (v == CELL_NOT_PRESENT) {
                continue;
            }
            if (v < m.min_cell_voltage) {
                m.min_cell_voltage = v;
                m.min_cell_index = idx;
            }
            if (v > m.max_cell_voltage) {
                m.max_cell_voltage = v;
                m.max_cell_index = idx;
            }
            v_sum += v;
            v_n++;
        }
        for (k = 0; k < EV27_THERMS_PER_IC; k++) {
            const int8_t t = s_snap.temp[ic][k];
            const uint8_t idx = (uint8_t)(ic * EV27_THERMS_PER_IC + k + 1u);  /* 1..120 */
            if (t == TEMP_NOT_PRESENT) {
                continue;
            }
            if (t < m.min_temp) {
                m.min_temp = t;
                m.min_temp_index = idx;
            }
            if (t > m.max_temp) {
                m.max_temp = t;
                m.max_temp_index = idx;
            }
            t_sum += t;
            t_n++;
        }
    }

    if (v_n > 0u) {
        m.avg_cell_voltage = (int16_t)(v_sum / (int32_t)v_n);
    } else {   /* no readings: NotPresent everywhere, index 0 = none */
        m.min_cell_voltage = m.max_cell_voltage = m.avg_cell_voltage = CELL_NOT_PRESENT;
    }
    if (t_n > 0u) {
        m.avg_temp = (int8_t)(t_sum / (int32_t)t_n);
    } else {
        m.min_temp = m.max_temp = m.avg_temp = TEMP_NOT_PRESENT;
    }
    if (ev27_main_bms_cell_summary_pack(buf, &m, sizeof(buf)) > 0) {
        (void)ev27_node_send(EV27_MAIN_BMS_CELL_SUMMARY_FRAME_ID, buf, sizeof(buf));
    }
}

static void send_limits(void)
{
    struct ev27_main_bms_limits_t m;
    uint8_t buf[EV27_MAIN_BMS_LIMITS_LENGTH];

    memset(&m, 0, sizeof(m));
    if (!bms_faulted()) {
        /* TODO: derate with temperature and SOC. For now the static pack limits. */
        m.discharge_current_limit = (uint16_t)(LIMIT_PACK_DISCHARGE_CURRENT_CONTINUOUS_A * 10.0f);
        m.charge_current_limit = s_snap.session_active
                                     ? (uint16_t)(LIMIT_PACK_CHARGE_CURRENT_MAX_A * 10u)       /* charger */
                                     : (uint16_t)(LIMIT_PACK_CHARGE_CURRENT_FAST_A * 10.0f);   /* regen */
    }
    if (ev27_main_bms_limits_pack(buf, &m, sizeof(buf)) > 0) {
        (void)ev27_node_send(EV27_MAIN_BMS_LIMITS_FRAME_ID, buf, sizeof(buf));
    }
}

static void send_charging(void)
{
    struct ev27_main_bms_charging_t m;
    uint8_t buf[EV27_MAIN_BMS_CHARGING_LENGTH];

    memset(&m, 0, sizeof(m));
    m.session_active = s_snap.session_active ? 1u : 0u;
    m.setpoint_valid = s_snap.setpoint_valid ? 1u : 0u;
    m.last_error = s_charge_last_error;
    m.requested_voltage = s_snap.requested_voltage_dv;   /* both already 0.1 V / 0.1 A per bit */
    m.requested_current = s_snap.requested_current_da;
    if (ev27_main_bms_charging_pack(buf, &m, sizeof(buf)) > 0) {
        (void)ev27_node_send(EV27_MAIN_BMS_CHARGING_FRAME_ID, buf, sizeof(buf));
    }
}

static void send_balancing(void)
{
    struct ev27_main_bms_balancing_t m;
    uint8_t buf[EV27_MAIN_BMS_BALANCING_LENGTH];
    uint32_t seg_bits[EV27_SEGMENTS];
    uint32_t s;

    for (s = 0; s < EV27_SEGMENTS; s++) {
        seg_bits[s] = (uint32_t)s_snap.balancing[2u * s] |
                      ((uint32_t)s_snap.balancing[2u * s + 1u] << EV27_CELLS_PER_IC);
    }
    memset(&m, 0, sizeof(m));
    m.seg1 = seg_bits[0];
    m.seg2 = seg_bits[1];
    m.seg3 = seg_bits[2];
    m.seg4 = seg_bits[3];
    m.seg5 = seg_bits[4];
    m.seg6 = seg_bits[5];
    if (ev27_main_bms_balancing_pack(buf, &m, sizeof(buf)) > 0) {
        (void)ev27_node_send(EV27_MAIN_BMS_BALANCING_FRAME_ID, buf, sizeof(buf));
    }
}

/* SegNCells / SegNTemps: six messages each with the same signals but their own
 * generated struct and pack function, so one small function per segment. */
#define FILL_CELLS(m, c) do { \
    (m).cell01 = (c)[0];  (m).cell02 = (c)[1];  (m).cell03 = (c)[2];  (m).cell04 = (c)[3];  \
    (m).cell05 = (c)[4];  (m).cell06 = (c)[5];  (m).cell07 = (c)[6];  (m).cell08 = (c)[7];  \
    (m).cell09 = (c)[8];  (m).cell10 = (c)[9];  (m).cell11 = (c)[10]; (m).cell12 = (c)[11]; \
    (m).cell13 = (c)[12]; (m).cell14 = (c)[13]; (m).cell15 = (c)[14]; (m).cell16 = (c)[15]; \
    (m).cell17 = (c)[16]; (m).cell18 = (c)[17]; (m).cell19 = (c)[18]; (m).cell20 = (c)[19]; \
    (m).cell21 = (c)[20]; (m).cell22 = (c)[21]; (m).cell23 = (c)[22]; (m).cell24 = (c)[23]; \
} while (0)

#define FILL_TEMPS(m, t) do { \
    (m).temp01 = (t)[0];  (m).temp02 = (t)[1];  (m).temp03 = (t)[2];  (m).temp04 = (t)[3];  \
    (m).temp05 = (t)[4];  (m).temp06 = (t)[5];  (m).temp07 = (t)[6];  (m).temp08 = (t)[7];  \
    (m).temp09 = (t)[8];  (m).temp10 = (t)[9];  (m).temp11 = (t)[10]; (m).temp12 = (t)[11]; \
    (m).temp13 = (t)[12]; (m).temp14 = (t)[13]; (m).temp15 = (t)[14]; (m).temp16 = (t)[15]; \
    (m).temp17 = (t)[16]; (m).temp18 = (t)[17]; (m).temp19 = (t)[18]; (m).temp20 = (t)[19]; \
} while (0)

#define DEFINE_SEG_SENDERS(N) \
static void send_seg##N##_cells(const int16_t *c) \
{ \
    struct ev27_main_bms_seg##N##_cells_t m; \
    uint8_t buf[EV27_MAIN_BMS_SEG##N##_CELLS_LENGTH]; \
    FILL_CELLS(m, c); \
    if (ev27_main_bms_seg##N##_cells_pack(buf, &m, sizeof(buf)) > 0) { \
        (void)ev27_node_send(EV27_MAIN_BMS_SEG##N##_CELLS_FRAME_ID, buf, sizeof(buf)); \
    } \
} \
static void send_seg##N##_temps(const int8_t *t) \
{ \
    struct ev27_main_bms_seg##N##_temps_t m; \
    uint8_t buf[EV27_MAIN_BMS_SEG##N##_TEMPS_LENGTH]; \
    FILL_TEMPS(m, t); \
    if (ev27_main_bms_seg##N##_temps_pack(buf, &m, sizeof(buf)) > 0) { \
        (void)ev27_node_send(EV27_MAIN_BMS_SEG##N##_TEMPS_FRAME_ID, buf, sizeof(buf)); \
    } \
}

DEFINE_SEG_SENDERS(1)
DEFINE_SEG_SENDERS(2)
DEFINE_SEG_SENDERS(3)
DEFINE_SEG_SENDERS(4)
DEFINE_SEG_SENDERS(5)
DEFINE_SEG_SENDERS(6)

static void (*const s_send_seg_cells[EV27_SEGMENTS])(const int16_t *) = {
    send_seg1_cells, send_seg2_cells, send_seg3_cells,
    send_seg4_cells, send_seg5_cells, send_seg6_cells,
};
static void (*const s_send_seg_temps[EV27_SEGMENTS])(const int8_t *) = {
    send_seg1_temps, send_seg2_temps, send_seg3_temps,
    send_seg4_temps, send_seg5_temps, send_seg6_temps,
};

static void send_seg_cells(uint32_t seg)
{
    int16_t c[EV27_ICS_PER_SEGMENT * EV27_CELLS_PER_IC];
    memcpy(&c[0], s_snap.cell[2u * seg], sizeof(s_snap.cell[0]));
    memcpy(&c[EV27_CELLS_PER_IC], s_snap.cell[2u * seg + 1u], sizeof(s_snap.cell[0]));
    s_send_seg_cells[seg](c);
}

static void send_seg_temps(uint32_t seg)
{
    int8_t t[EV27_ICS_PER_SEGMENT * EV27_THERMS_PER_IC];
    memcpy(&t[0], s_snap.temp[2u * seg], sizeof(s_snap.temp[0]));
    memcpy(&t[EV27_THERMS_PER_IC], s_snap.temp[2u * seg + 1u], sizeof(s_snap.temp[0]));
    s_send_seg_temps[seg](t);
}

void ev27_device_tick(uint32_t n)
{
    const uint32_t cells_phase = n % (EV27_MAIN_BMS_SEG1_CELLS_CYCLE_TIME_MS / EV27_TICK_MS);
    const uint32_t temps_phase = n % (EV27_MAIN_BMS_SEG1_TEMPS_CYCLE_TIME_MS / EV27_TICK_MS);
    bool session_active;

    /* Fresh data every 100 ms, just before State. If the BMS holds the mutex,
     * try again next tick and keep sending the last snapshot meanwhile. */
    if (EVERY(n, EV27_MAIN_BMS_STATE_CYCLE_TIME_MS, PHASE_STATE)) {
        s_snap_due = true;
    }
    if (s_snap_due && take_snapshot()) {
        s_snap_due = false;
    }

    /* A session that ended without Disconnect or SoftStop ran out of keepalives
     * (state.c stops it after 3 s). */
    session_active = s_snap.session_active;
    if (s_session_was_active && !session_active &&
        charging_session_ms_since_kick() > EV27_MAIN_CM5_CHARGE_CONTROL_CYCLE_TIME_MS * 2u) {
        s_charge_last_error = EV27_MAIN_BMS_CHARGING_LAST_ERROR_KEEPALIVE_TIMEOUT_CHOICE;
    }
    s_session_was_active = session_active;

    if (EVERY(n, EV27_MAIN_BMS_STATE_CYCLE_TIME_MS, PHASE_STATE)) {
        send_state();
    }
    if (EVERY(n, EV27_MAIN_BMS_PACK_CYCLE_TIME_MS, PHASE_PACK)) {
        send_pack();
    }
    if (EVERY(n, EV27_MAIN_BMS_CELL_SUMMARY_CYCLE_TIME_MS, PHASE_CELL_SUMMARY)) {
        send_cell_summary();
    }
    if (EVERY(n, EV27_MAIN_BMS_LIMITS_CYCLE_TIME_MS, PHASE_LIMITS)) {
        send_limits();
    }
    if (cells_phase >= PHASE_SEG_CELLS && cells_phase < PHASE_SEG_CELLS + EV27_SEGMENTS) {
        send_seg_cells(cells_phase - PHASE_SEG_CELLS);
    }
    if (temps_phase >= PHASE_SEG_TEMPS && temps_phase < PHASE_SEG_TEMPS + EV27_SEGMENTS) {
        send_seg_temps(temps_phase - PHASE_SEG_TEMPS);
    }
    if (EVERY(n, EV27_MAIN_BMS_BALANCING_CYCLE_TIME_MS, PHASE_BALANCING)) {
        send_balancing();
    }
    if (EVERY(n, EV27_MAIN_BMS_CHARGING_CYCLE_TIME_MS, PHASE_CHARGING)) {
        send_charging();
    }
}

/* ---- Status ----------------------------------------------------------------- */

#define SENSOR_CELLS  0x0001u
#define SENSOR_THERMS 0x0002u

void ev27_device_status(ev27_device_status_t *st)
{
    const uint16_t cell_faults = FAULT_BIT(BMS_ERR_PEC_FAIL) | FAULT_BIT(BMS_ERR_CMD_COUNTER) |
                                 FAULT_BIT(BMS_ERR_SPI_FAULT) | FAULT_BIT(BMS_ERR_CELL_OV) |
                                 FAULT_BIT(BMS_ERR_CELL_UV) | FAULT_BIT(BMS_ERR_CS_MISMATCH) |
                                 FAULT_BIT(BMS_ERR_CELL_OPENWIRE);
    const uint16_t therm_faults = FAULT_BIT(BMS_ERR_AUX_OPENWIRE) | FAULT_BIT(BMS_ERR_THERM_OVER_TEMP) |
                                  FAULT_BIT(BMS_ERR_THERM_UNDER_TEMP);

    st->sensors_enabled = SENSOR_CELLS | SENSOR_THERMS;
    st->sensors_present = (s_snap.any_cell ? SENSOR_CELLS : 0u) | (s_snap.any_temp ? SENSOR_THERMS : 0u);
    st->sensors_healthy = ((s_snap.any_cell && (s_snap.fault_flags & cell_faults) == 0u) ? SENSOR_CELLS : 0u) |
                          ((s_snap.any_temp && (s_snap.fault_flags & therm_faults) == 0u) ? SENSOR_THERMS : 0u);
    st->error_flags = s_snap.fault_flags;
}

bool ev27_device_config_valid(void)
{
    return true;   /* no config record on the BMS */
}
