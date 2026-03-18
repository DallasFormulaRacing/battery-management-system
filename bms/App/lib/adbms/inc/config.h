#ifndef CONFIG_H
#define CONFIG_H

#include "bms_enums.h"
#include "bms_types.h"
#include <stdint.h>

// This has to be at compile time.
#define NUM_IC_COUNT_CHAIN 2
#define WRITE_SIZE (ADBMS_TX_FRAME_BYTES * NUM_IC_COUNT_CHAIN)
// #define READ_SIZE (ADBMS_RX_FRAME_BYTES * NUM_IC_COUNT_CHAIN)
#define NUM_CELLS_PER_SEGMENT 12
#define NUM_THERM_PER_SEGMENT 10
#define NUM_CELL_USING (NUM_IC_COUNT_CHAIN * NUM_CELLS_PER_SEGMENT)

#define SINGLEBOARD !(NUM_IC_COUNT_CHAIN > 1)

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
extern voltage_config_t g_voltage_cfg;
extern measurement_config_t g_meas_cfg;

#endif

/*redundant_enable_t RD_en = RD_OFF;
    cont_measurement_mode_t CONT_en = SINGLE;
    open_wire_detect_mode_t OW_CS_en = OW_OFF_ALL_CH;
    aux_open_wire_detect_mode_t aux_ow_mode = AUX_OW_OFF;
    pull_down_current_mode_t pull_up_resistor_ow = PUP_DOWN;
    discharge_permission_t discharge_permit = DCP_OFF;
    reset_filter_mode_t reset_filter = RSTF_OFF;
    inject_err_spi_read_t error_injection_mode; = WITHOUT_ERR;*/
