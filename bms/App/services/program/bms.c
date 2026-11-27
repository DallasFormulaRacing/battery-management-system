#include "bms.h"
#include "bms_comms.h"
#include "bms_enums.h"
#include "bms_types.h"
#include "charger.h"
#include "cmsis_os2.h"
#include "config.h"
#include "parse.h"
#include "segment.h"
#include "spi.h"
#include "stm32g4xx_hal.h"
#include "thermistor.h"
#include <stdbool.h>
#include <stdint.h>

extern osMutexId_t bms_mutex_id;

cell_asic_ctx_t asic[NUM_IC_COUNT_CHAIN];
uint8_t write_buffer[WRITE_SIZE];

static pack_data_t g_pack;
static pcb_ctx_t g_pcb;

static bms_cfg_t g_bms_cfg = {
    .adc = &g_cell_profile,
    .voltage = &g_voltage_cfg,
    .measurement = &g_meas_cfg,
};

bms_handler_t hbms = {
    .config = &g_bms_cfg,
    .state =
        {
            .current_state = BMS_STATE_BOOT,
            .previous_state = BMS_STATE_BOOT,
            .error_code = BMS_ERR_NONE,
            .state_entry_tick = 0,
        },
    .asic = asic,
    .pcb = &g_pcb,
    .pack = &g_pack,
};

/* ---- isoSPI reading health -------------------------------------------------
 * See BMS_PEC_FAIL_LIMIT and BMS_PEC_REINIT_AFTER in config.h. Each kind of
 * reading keeps its own count, so a chain where only the aux reads fail still
 * faults.
 */
typedef enum {
  READ_CELL, /* filtered C-ADC (OV/UV) */
  READ_S,    /* S-ADC (cell open wire) */
  READ_AUX,  /* aux (thermistors, thermistor open wire) */
  READ_KIND_COUNT,
} read_kind_t;

#define RAW_RESET_VALUE INT16_MIN /* 0x8000: what a 6830 result register holds after reset */

static uint32_t s_fail_streak[READ_KIND_COUNT];
static uint32_t s_reinit_count;
static bool s_reinit_due;
static bool s_cells_valid[NUM_IC_COUNT_CHAIN];
static bool s_temps_valid[NUM_IC_COUNT_CHAIN];

static bool all_reset_value(const voltage_readings_t *v, uint16_t n) {
  for (uint16_t i = 0; i < n; i++) {
    if (v[i] != RAW_RESET_VALUE) {
      return false;
    }
  }
  return true;
}

/*
 * After a read: valid[ic] = that IC's data can be used (PEC passed on every
 * group and not all at the reset value). Returns true if any IC failed.
 */
static bool check_ics(read_kind_t kind, bool *valid) {
  bool any_failed = false;

  for (uint8_t ic = 0; ic < NUM_IC_COUNT_CHAIN; ic++) {
    const cell_asic_ctx_t *a = &hbms.asic[ic];
    bool ok;

    switch (kind) {
    case READ_CELL:
      ok = (a->crc_err.cell_pec == 0U) &&
           !all_reset_value(a->filt_cell.filt_cell_voltages_array,
                            NUM_CELLS_PER_SEGMENT);
      break;
    case READ_S:
      ok = (a->crc_err.cell_pec == 0U) && /* S-ADC shares the cell PEC flag */
           !all_reset_value(a->s_cell.s_cell_voltages_array,
                            NUM_CELLS_PER_SEGMENT);
      break;
    case READ_AUX:
    default:
      ok = (a->crc_err.aux_channel_pec == 0U) &&
           !all_reset_value(a->aux.aux_voltages_array, NUM_THERM_PER_SEGMENT);
      break;
    }
    valid[ic] = ok;
    any_failed = any_failed || !ok;
  }
  return any_failed;
}

/* Writes the 6830 config again and restarts the ADCs, as bms_state_init()
 * does. Balancing PWM is cleared; the balancing code sets it again on its
 * next pass. */
static void reinit_chain(void) {
  (void)adbms_init_config(hbms.asic);
  (void)adbms_start_aux_voltage_measurement(hbms.asic);
  (void)adbms_clear_all_pwm(hbms.asic);
  (void)adbms_start_cell_voltage_measurement(hbms.asic);
  s_reinit_count++;
}

/* At the end of a measurement pass: re-initialise once if any kind of reading
 * reached a multiple of BMS_PEC_REINIT_AFTER failures during the pass. */
static void reinit_if_due(void) {
  if (s_reinit_due) {
    s_reinit_due = false;
    reinit_chain();
  }
}

/* Counts one reading. Returns BMS_ERR_PEC_FAIL once BMS_PEC_FAIL_LIMIT
 * readings of this kind have failed in a row. */
static bms_fault_t count_reading(read_kind_t kind, bool failed) {
  if (!failed) {
    s_fail_streak[kind] = 0U;
    return BMS_ERR_NONE;
  }
  s_fail_streak[kind]++;
  if ((s_fail_streak[kind] % BMS_PEC_REINIT_AFTER) == 0U) {
    s_reinit_due = true;
  }
  return (s_fail_streak[kind] >= BMS_PEC_FAIL_LIMIT) ? BMS_ERR_PEC_FAIL
                                                     : BMS_ERR_NONE;
}

bool bms_ic_cells_valid(uint8_t ic) {
  return (ic < NUM_IC_COUNT_CHAIN) && s_cells_valid[ic];
}

bool bms_ic_temps_valid(uint8_t ic) {
  return (ic < NUM_IC_COUNT_CHAIN) && s_temps_valid[ic];
}

bool bms_readings_failing(void) {
  for (uint32_t k = 0; k < READ_KIND_COUNT; k++) {
    if (s_fail_streak[k] != 0U) {
      return true;
    }
  }
  return false;
}

uint32_t bms_chain_reinit_count(void) { return s_reinit_count; }

/* ---- Measurement checks ----------------------------------------------------- */

/*
 * @brief Check thermistor temperature and see if between range
 * @params None
 * @return BMS_ERR_THERM_OVER_TEMP / BMS_ERR_THERM_UNDER_TEMP, BMS_ERR_PEC_FAIL
 * after BMS_PEC_FAIL_LIMIT failed aux readings in a row, else no error.
 */
bms_fault_t therm_temp_in_range_check() {
  adbms_start_aux_voltage_measurement(hbms.asic);
  delay(5);
  (void)adbms_read_aux_voltages(hbms.asic);
  const bms_fault_t pec = count_reading(READ_AUX, check_ics(READ_AUX, s_temps_valid));

  bool over_temp_flag = false;
  bool under_temp_flag = false;
  for (uint8_t seg_num = 0; seg_num < NUM_IC_COUNT_CHAIN; seg_num++) {
    if (!s_temps_valid[seg_num]) {
      continue; /* garbage or reset IC: keep its last good temperatures */
    }
    for (uint16_t therm_num = 0; therm_num < NUM_THERM_PER_SEGMENT;
         therm_num++) {

      // converts here
      float temp = thermistor_from_adc(
          hbms.asic[seg_num].aux.aux_voltages_array[therm_num]);

      hbms.asic[seg_num].thermistor[therm_num] = temp;
      if (temp > g_voltage_cfg.overtemp_threshold_c) {
        over_temp_flag = true;
        if (hbms.asic[seg_num].thermistor_fault_status[therm_num] !=
            OPEN_WIRE_FAULT) {
          hbms.asic[seg_num].thermistor_fault_status[therm_num] = OVER_FAULT;
        }
      }

      if (temp < g_voltage_cfg.undertemp_threshold_c) {
        under_temp_flag = true;
        if (hbms.asic[seg_num].thermistor_fault_status[therm_num] !=
            OPEN_WIRE_FAULT) {
          hbms.asic[seg_num].thermistor_fault_status[therm_num] = UNDER_FAULT;
        }
      }
    }
  }

  // Over temp will take precedent over under temp in the near impossible event
  // that both will happen
  if (over_temp_flag) {
    return BMS_ERR_THERM_OVER_TEMP;
  }
  if (under_temp_flag) {
    return BMS_ERR_THERM_UNDER_TEMP;
  }

  return pec;
}

bms_fault_t therm_open_wire_check() {
  bool valid[NUM_IC_COUNT_CHAIN];

  adbms_start_aux_voltage_measurement(hbms.asic);
  delay(5);
  (void)adbms_read_aux_voltages(hbms.asic);
  const bms_fault_t pec = count_reading(READ_AUX, check_ics(READ_AUX, valid));

  bool open_wire_flag = false;
  for (uint8_t seg_num = 0; seg_num < NUM_IC_COUNT_CHAIN; seg_num++) {
    if (!valid[seg_num]) {
      continue;
    }
    for (uint16_t i = 0; i < NUM_THERM_PER_SEGMENT; i++) {
      // if voltage is greater than 2.9 V, there is probably an OW or it's
      // really cold
      float aux_v = convert_voltage_human_readable(hbms.asic[seg_num].aux.aux_voltages_array[i]);
      if (aux_v * 1000.0F > (float)g_voltage_cfg.openwire_aux_threshold_mv) {
        hbms.asic[seg_num].thermistor_fault_status[i] = OPEN_WIRE_FAULT;
        open_wire_flag = true;
      }
    }
  }

  if (open_wire_flag) {
    return BMS_ERR_AUX_OPENWIRE;
  }
  return pec;
}

/*
 * @brief Filtered cell voltages, read with balancing paused, checked against
 * OV/UV. ICs whose reading failed (PEC or reset) are skipped this time.
 * @return BMS_ERR_CELL_OV / BMS_ERR_CELL_UV, BMS_ERR_PEC_FAIL after
 * BMS_PEC_FAIL_LIMIT failed cell readings in a row, else no error.
 */
bms_fault_t cell_voltage_in_range_check() {
  (void)adbms_read_fcell_voltages_muted(hbms.asic);
  const bms_fault_t pec = count_reading(READ_CELL, check_ics(READ_CELL, s_cells_valid));

  bool cell_over_flag = false;
  bool cell_under_flag = false;
  for (uint8_t seg_num = 0; seg_num < NUM_IC_COUNT_CHAIN; seg_num++) {
    if (!s_cells_valid[seg_num]) {
      continue;
    }
    for (uint16_t cell_num = 0; cell_num < NUM_CELLS_PER_SEGMENT; cell_num++) {
      float this_cell = convert_voltage_human_readable(
          hbms.asic[seg_num].filt_cell.filt_cell_voltages_array[cell_num]);
      if (this_cell > g_voltage_cfg.overvoltage_threshold_v) {
        cell_over_flag = true;
        hbms.asic[seg_num].cell_fault_status[cell_num] = OVER_FAULT;
      } // endif

      if (this_cell < g_voltage_cfg.undervoltage_threshold_v) {
        cell_under_flag = true;
        hbms.asic[seg_num].cell_fault_status[cell_num] = UNDER_FAULT;
      } // endif
    } // end inner fl
  }

  if (cell_over_flag) {
    return BMS_ERR_CELL_OV;
  }

  if (cell_under_flag) {
    return BMS_ERR_CELL_UV;
  }

  return pec;
}

void adbms_set_watchdog() {}

static inline void get_odd_openwire_voltages() {
  adbms_start_adc_s_voltage_measurement(hbms.asic,
                                        g_cell_open_wire_check_profile_odd);
  osDelay(10);
  spi_adc_snap_command();
  adbms_read_s_voltages(hbms.asic);
  spi_adc_unsnap_command();
}

bms_fault_t cell_open_wire_check_odd() {
  bool valid[NUM_IC_COUNT_CHAIN];

  get_odd_openwire_voltages();
  const bms_fault_t pec = count_reading(READ_S, check_ics(READ_S, valid));
  bool cell_open_wire_flag = false;

  //////////////

  // todo: how to detect? value reading is fine now
  // do odd cell taps (even indexs due to array indexing)
  for (uint8_t seg_num = 0; seg_num < NUM_IC_COUNT_CHAIN; seg_num++) {
    if (!valid[seg_num]) {
      continue;
    }
    for (uint16_t cell_num = 0; cell_num < NUM_CELLS_PER_SEGMENT;
         cell_num += 2) {
      float this_cell = convert_voltage_human_readable(
          hbms.asic[seg_num].s_cell.s_cell_voltages_array[cell_num]);
      // look[2][cell_num + 1] = this_cell;

      if (this_cell < 1.0F) {
        hbms.asic[seg_num].cell_fault_status[cell_num] = OPEN_WIRE_FAULT;
        cell_open_wire_flag = true;
      } // endif
    } // end inner fl
  }
  if (cell_open_wire_flag)
    return BMS_ERR_CELL_OPENWIRE;

  return pec;
}

static inline void get_even_openwire_voltages() {
  adbms_start_adc_s_voltage_measurement(hbms.asic,
                                        g_cell_open_wire_check_profile_even);
  osDelay(10);
  spi_adc_snap_command();
  adbms_read_s_voltages(hbms.asic);
  spi_adc_unsnap_command();
}

bms_fault_t cell_open_wire_check_even() {
  bool valid[NUM_IC_COUNT_CHAIN];

  get_even_openwire_voltages();
  const bms_fault_t pec = count_reading(READ_S, check_ics(READ_S, valid));
  bool cell_open_wire_flag = false;

  // todo: how to detect? value reading is fine now
  // do even cell taps (odd indexs due to array indexing)
  for (uint8_t seg_num = 0; seg_num < NUM_IC_COUNT_CHAIN; seg_num++) {
    if (!valid[seg_num]) {
      continue;
    }
    for (uint16_t cell_num = 1; cell_num < NUM_CELLS_PER_SEGMENT;
         cell_num += 2) {
      float this_cell = convert_voltage_human_readable(
          hbms.asic[seg_num].s_cell.s_cell_voltages_array[cell_num]);
      // look[3][cell_num] = this_cell;

      if (this_cell < 1.0F) {
        hbms.asic[seg_num].cell_fault_status[cell_num] = OPEN_WIRE_FAULT;
        cell_open_wire_flag = true;
      } // endif
    } // end inner fl
  }
  if (cell_open_wire_flag)
    return BMS_ERR_CELL_OPENWIRE;

  return pec;
}

void force_sync_s_adc() {
  adbms_start_adc_s_voltage_measurement(hbms.asic, g_cell_force_sync_s_adc);
  osDelay(30);
}

/* Under BMS_BENCH_IGNORE_THERM_FAULTS thermistor results are still read but
 * only a PEC fault from the aux reads counts. */
static bool therm_result_faults(bms_fault_t f) {
#if BMS_BENCH_IGNORE_THERM_FAULTS
  return f == BMS_ERR_PEC_FAIL;
#else
  return f != BMS_ERR_NONE;
#endif
}

static bms_fault_t run_checks(void) {
  bms_fault_t f = cell_voltage_in_range_check();
  if (f != BMS_ERR_NONE) {
    return f;
  }

  f = therm_temp_in_range_check();
  if (therm_result_faults(f)) {
    return f;
  }

  f = cell_open_wire_check_odd();
  if (f != BMS_ERR_NONE) {
    return f;
  }

  f = cell_open_wire_check_even();
  if (f != BMS_ERR_NONE) {
    return f;
  }

  f = therm_open_wire_check();
  if (therm_result_faults(f)) {
    return f;
  }
  return BMS_ERR_NONE;
}

bms_fault_t bms_run_measurement_checks(void) {
  const bms_fault_t f = run_checks();
  reinit_if_due();
  return f;
}

void hard_fault_disable_openwire_on_profiles() {
  g_cell_profile.ow_mode = OW_OFF_ALL_CH;
  g_cell_profile.aux_ow_mode = AUX_OW_OFF;
  g_cell_profile.discharge_permit = DCP_OFF;

  g_thermistor_profile.ow_mode = OW_OFF_ALL_CH;
  g_thermistor_profile.aux_ow_mode = AUX_OW_OFF;
  g_thermistor_profile.discharge_permit = DCP_OFF;
}

/**
 * @brief this is a special mesurement loop designed to operate under fault
 * it should NOT: bleed cells or do open wire checks, since ow injects current
 * The range checks still run so the per-cell fault flags stay current, but
 * their result is ignored: we are already faulted and only want fresh data.
 * Failed readings still count, so the chain is re-initialised here too.
 */
void measure_during_fault() {
  hard_fault_disable_openwire_on_profiles();

  if (osOK != osMutexAcquire(bms_mutex_id, 1000)) {
    return;
  }
  (void)cell_voltage_in_range_check();
  (void)therm_temp_in_range_check();
  reinit_if_due();
  osMutexRelease(bms_mutex_id);
}
volatile float look[NUM_IC_COUNT_CHAIN][NUM_CELLS_PER_SEGMENT];
static void pop() {
  for (uint8_t seg_num = 0; seg_num < NUM_IC_COUNT_CHAIN; seg_num++) {
    for (uint8_t cell_num = 0; cell_num < NUM_CELLS_PER_SEGMENT; cell_num++) {
      look[seg_num][cell_num] = convert_voltage_human_readable(
          hbms.asic[seg_num].cell.cell_voltages_array[cell_num]);
    }
  }
}

// for (uint8_t i = 0; i < 12; i++) {
//   look[2][i] = convert_voltage_human_readable(
//       hbms.asic[0].cell.cell_voltages_array[i]);
// }

// for (uint8_t i = 0; i < 12; i++) {
//   look[3][i] = convert_voltage_human_readable(
//       hbms.asic[1].cell.cell_voltages_array[i]);
// }

/* ----------------------------------------------------- */
/* testing functions ------------------------------------ */

void bms_test_init() {
  hbms.config->adc = &g_cell_profile;
  hbms.config->voltage = &g_voltage_cfg;
  hbms.config->measurement = &g_meas_cfg;

  for (int i = 0; i < NUM_IC_COUNT_CHAIN; i++) {
    asic[i].ic_count = NUM_IC_COUNT_CHAIN;
  }

  adbms_init_config(hbms.asic);
  adbms_start_aux_voltage_measurement(hbms.asic);
  adbms_clear_all_pwm(hbms.asic);
  adbms_start_cell_voltage_measurement(hbms.asic);

  osDelay(8);
}

void cell_open_wire_test() {
  cell_open_wire_check_even();
  cell_open_wire_check_odd();
  force_sync_s_adc();
}

GPIO_TypeDef *SHUTDOWN_GPIO_Port = GPIOA;
uint16_t SHUTDOWN_Pin = GPIO_PIN_8;

void open_shutdown_circuit() {
  // todo
  // just a gpio flip
  // check rules

  // nFlt so high = good, low = bad
  HAL_GPIO_WritePin(SHUTDOWN_GPIO_Port, SHUTDOWN_Pin, GPIO_PIN_RESET);
}

void bms_test_run() {
  // asic_wakeup(NUM_IC_COUNT_CHAIN);
  // spi_adc_snap_command();
  // adbms_read_fcell_voltages(hbms.asic);
  // spi_adc_unsnap_command();

  adbms_read_aux_voltages(hbms.asic);
  pop();
  delay(1);
}
