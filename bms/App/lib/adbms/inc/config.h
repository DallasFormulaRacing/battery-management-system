#ifndef CONFIG_H
#define CONFIG_H

#include "bms_enums.h"
#include "bms_limits.h"
#include "bms_types.h"
#include <stdint.h>

// This has to be at compile time.
#define NUM_IC_COUNT_CHAIN 1
#define WRITE_SIZE (ADBMS_TX_FRAME_BYTES * NUM_IC_COUNT_CHAIN)
#define COMMAND_HEADER_SIZE 4
// #define READ_SIZE (ADBMS_RX_FRAME_BYTES * NUM_IC_COUNT_CHAIN)
#define NUM_CELLS_PER_SEGMENT 12
#define NUM_THERM_PER_SEGMENT 10
#define NUM_CELL_USING (NUM_IC_COUNT_CHAIN * NUM_CELLS_PER_SEGMENT)

#define SINGLEBOARD !(NUM_IC_COUNT_CHAIN > 1)

/* ---- isoSPI error handling (program/bms.c) ----
 * A "reading" is one read of the cell (C-ADC filtered), S-ADC or aux results
 * from the whole chain. It fails if any IC's frame fails its PEC, or if an IC
 * answers with every value at 0x8000, the reset value (it lost power or was
 * reset and no longer measures).
 *
 * BMS_PEC_FAIL_LIMIT failed readings of the same kind in a row fault the BMS
 * with BMS_ERR_PEC_FAIL. One good reading resets the count. Every
 * BMS_PEC_REINIT_AFTER failures in a row, the BMS writes the 6830 config
 * again and restarts the ADCs, in case an IC was reset; this keeps going in
 * the fault state so the readings can come back. A reading that failed on an
 * IC is not used for that IC's OV/UV/temperature/open wire checks.
 */
#define BMS_PEC_FAIL_LIMIT 10U
#define BMS_PEC_REINIT_AFTER 3U

_Static_assert(BMS_PEC_REINIT_AFTER < BMS_PEC_FAIL_LIMIT,
               "re-init must get a chance before the PEC fault");

/* ---- Charging (program/supervisor.c) ----
 * The car's pack: 144 cells in series (6 segments x 24). This is the real
 * pack, not NUM_CELL_USING, which is what this build measures.
 */
#define BMS_PACK_SERIES_CELLS 144U

/* The highest pack voltage the BMS asks the charger for: a GUI or CM5
 * setpoint above it is refused. Charging also stops (is_pack_full) when any
 * cell reaches this divided by the cell count. Must be at or below the
 * overvoltage limit, or a full charge would fault the BMS. */
#define BMS_PACK_CHARGE_VOLTAGE_LIMIT_V 597.0F /* ~4.146 V per cell */
#define BMS_CHARGE_CELL_VOLTAGE_LIMIT_V (BMS_PACK_CHARGE_VOLTAGE_LIMIT_V / (float)BMS_PACK_SERIES_CELLS)

_Static_assert(BMS_PACK_CHARGE_VOLTAGE_LIMIT_V <=
                   LIMIT_CELL_OVERVOLTAGE_V * (float)BMS_PACK_SERIES_CELLS,
               "pack charge voltage limit above the pack overvoltage limit");
_Static_assert(BMS_PACK_CHARGE_VOLTAGE_LIMIT_V <=
                   (float)LIMIT_PACK_CHARGE_VOLTAGE_MAX_V,
               "pack charge voltage limit above LIMIT_PACK_CHARGE_VOLTAGE_MAX_V");
_Static_assert(BMS_PACK_CHARGE_VOLTAGE_LIMIT_V >=
                   (float)LIMIT_CHARGER_VOLTAGE_MIN_V,
               "pack charge voltage limit below the charger's output range");

extern cell_asic_ctx_t *asic_ctx;
extern uint8_t write_buffer[WRITE_SIZE];
// asic_status_buffers_t read_buffer[NUM_IC_COUNT_CHAIN];

typedef struct {
  redundant_enable_t redundant_measurement_mode;
  aux_select_t channels;
  cont_measurement_mode_t continuous_measurement;
  open_wire_detect_mode_t ow_mode;
  aux_open_wire_detect_mode_t aux_ow_mode;
  pull_down_current_mode_t pull_up_resistor_ow;
  discharge_permission_t discharge_permit;
  reset_filter_mode_t reset_filter;
  inject_err_spi_read_t error_injection_mode;
} adc_config_t;

typedef struct {
  const float overvoltage_threshold_v;
  const float undervoltage_threshold_v;

  const float overtemp_threshold_c;
  const float undertemp_threshold_c;

  const uint32_t openwire_cell_threshold_mv;
  const uint32_t openwire_aux_threshold_mv;

  const uint32_t loop_meas_count;
  const uint16_t meas_loop_time_ms;

  uint32_t loop_counter;
  uint32_t periodic_adc_count;
} voltage_config_t;

typedef struct {
  loop_measurement_enable_t measure_cell;
  loop_measurement_enable_t measure_avg_cell;
  loop_measurement_enable_t measure_f_cell;
  loop_measurement_enable_t measure_s_voltage;
  loop_measurement_enable_t measure_aux;
  loop_measurement_enable_t measure_raux;
  loop_measurement_enable_t measure_stat;
} measurement_config_t;

typedef struct {
  adc_config_t *adc;
  voltage_config_t *voltage;
  measurement_config_t *measurement;
} bms_cfg_t;

extern adc_config_t g_cell_profile;
extern adc_config_t g_cell_filtered_profile;
extern adc_config_t g_thermistor_profile;
extern adc_config_t g_thermistor_open_wire_check_profile;
extern adc_config_t g_cell_open_wire_check_profile_even;
extern adc_config_t g_cell_open_wire_check_profile_odd;
extern adc_config_t g_cell_open_wire_check_profile;
extern adc_config_t g_cell_force_sync_s_adc;
extern voltage_config_t g_voltage_cfg;
extern measurement_config_t g_meas_cfg;
extern const uint32_t g_cell_balance_pwm_ms;

#endif

/*redundant_enable_t RD_en = RD_OFF;
    cont_measurement_mode_t CONT_en = SINGLE;
    open_wire_detect_mode_t OW_CS_en = OW_OFF_ALL_CH;
    aux_open_wire_detect_mode_t aux_ow_mode = AUX_OW_OFF;
    pull_down_current_mode_t pull_up_resistor_ow = PUP_DOWN;
    discharge_permission_t discharge_permit = DCP_OFF;
    reset_filter_mode_t reset_filter = RSTF_OFF;
    inject_err_spi_read_t error_injection_mode; = WITHOUT_ERR;*/
