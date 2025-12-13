#ifndef CONFIG_H
#define CONFIG_H

#include "bms_types.h"

#define IC_COUNT_CHAIN 1 // this has to be at compile time.
#define WRITE_SIZE (ADBMS_TX_FRAME_BYTES * IC_COUNT_CHAIN)

cell_asic_ctx_t asic_ctx[IC_COUNT_CHAIN];
uint8_t write_buffer[WRITE_SIZE];

typedef struct {
  redundant_enable_t redundant_measurement_mode;
  aux_adc_input_channel_select_t channel_to_convert;
  cont_measurement_mode_t continuous_measurement;
  open_wire_detection_mode_t cell_open_wire_detection_mode;
  aux_open_wire_detection_mode_t AUX_OW_en;
  pull_down_current_mode_t PUP_en;
  discharge_permitted_mode_t DCP_en;
  reset_filter_mode_t RSTF_en;
  inject_err_spi_read_t ERR_en;
} adc_config_t;

typedef struct {
  const float overvoltage_threshold;
  const float undervoltage_threshold;

  const int OW_cell_threshold;
  const int OW_aux_threshold;

  const uint32_t loop_measurement_count;
  const uint16_t measurement_loop_time;

  uint32_t loop_count;
  uint32_t pladc_count;
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

  cell_asic_ctx_t asic_ctx[IC_COUNT_CHAIN];
} bms_t;

#endif

/*redundant_enable_t RD_en = RD_OFF;
    cont_measurement_mode_t CONT_en = SINGLE;
    open_wire_detection_mode_t OW_CS_en = OW_OFF_ALL_CH;
    aux_open_wire_detection_mode_t AUX_OW_en = AUX_OW_OFF;
    pull_down_current_mode_t PUP_en = PUP_DOWN;
    discharge_permitted_mode_t DCP_en = DCP_OFF;
    reset_filter_mode_t RSTF_en = RSTF_OFF;
    inject_err_spi_read_t ERR_en; = WITHOUT_ERR;*/