#include "config.h"

adc_config_t g_cell_profile = {
    .redundant_measurement_mode = RD_OFF,
    .channels = AUX_ALL,
    .continuous_measurement = SINGLE,
    .ow_mode = OW_OFF_ALL_CH,
    .AUX_OW_en = AUX_OW_ON,
    .PUP_en = PUP_DOWN,
    .DCP_en = DCP_OFF,
    .RSTF_en = RSTF_OFF,
    .ERR_en = WITHOUT_ERR,
};

adc_config_t g_cell_filtered_profile = {
    .redundant_measurement_mode = RD_OFF,
    .channels = AUX_ALL,
    .continuous_measurement = CONTINUOUS,
    .ow_mode = OW_OFF_ALL_CH,
    .AUX_OW_en = AUX_OW_ON,
    .PUP_en = PUP_DOWN,
    .DCP_en = DCP_OFF,
    .RSTF_en = RSTF_ON,
    .ERR_en = WITHOUT_ERR,
};

adc_config_t g_thermistor_profile = {
    .redundant_measurement_mode = RD_OFF,
    .channels = AUX_ALL,
    .continuous_measurement = SINGLE,
    .ow_mode = OW_OFF_ALL_CH,
    .AUX_OW_en = AUX_OW_OFF,
    .PUP_en = PUP_DOWN,
    .DCP_en = DCP_OFF,
    .RSTF_en = RSTF_OFF,
    .ERR_en = WITHOUT_ERR,
};

adc_config_t g_thermistor_open_wire_check_profile = {
    .redundant_measurement_mode = RD_OFF,
    .channels = AUX_ALL,
    .continuous_measurement = SINGLE,
    .ow_mode = OW_ON_ALL_CH,
    .AUX_OW_en = AUX_OW_ON,
    .PUP_en = PUP_DOWN,
    .DCP_en = DCP_OFF,
    .RSTF_en = RSTF_OFF,
    .ERR_en = WITHOUT_ERR,
};

adc_config_t g_cell_open_wire_check_profile = {
    .redundant_measurement_mode = RD_ON, // RD
    .channels = AUX_ALL,
    .continuous_measurement = CONTINUOUS, // Cont
    .ow_mode = OW_OFF_ALL_CH,             // OW OFF
    .AUX_OW_en = AUX_OW_OFF,              // OW OFF
    .PUP_en = PUP_DOWN,
    .DCP_en = DCP_OFF,
    .RSTF_en = RSTF_OFF,
    .ERR_en = WITHOUT_ERR,
};

voltage_config_t g_voltage_cfg = {
    .overvoltage_threshold_v = 4.15F,
    .undervoltage_threshold_v = 2.51F,
    .openwire_cell_threshold_mv = 2000,
    .openwire_aux_threshold_mv = 2900, // NOTE: Original value was 50000, typo?
    .loop_meas_count = 4,
    .meas_loop_time_ms = 1000,
};

measurement_config_t g_meas_cfg = {
    .measure_cell = ENABLED,
    .measure_avg_cell = ENABLED,
    .measure_f_cell = ENABLED,
    .measure_s_voltage = ENABLED,
    .measure_aux = ENABLED,
    .measure_raux = ENABLED,
    .measure_stat = ENABLED,
};