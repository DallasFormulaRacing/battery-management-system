#include "config.h"
#include "bms_enums.h"

adc_config_t g_cell_profile = {
    .redundant_measurement_mode = RD_OFF,
    .channels = AUX_ALL,
    .continuous_measurement = SINGLE,
    .ow_mode = OW_OFF_ALL_CH,
    .aux_ow_mode = AUX_OW_ON,
    .pull_up_resistor_ow = PUP_DOWN,
    .discharge_permit = DCP_OFF,
    .reset_filter = RSTF_OFF,
    .error_injection_mode = WITHOUT_ERR,
};

adc_config_t g_cell_filtered_profile = {
    .redundant_measurement_mode = RD_OFF,
    .channels = AUX_ALL,
    .continuous_measurement = CONTINUOUS,
    .ow_mode = OW_OFF_ALL_CH,
    .aux_ow_mode = AUX_OW_ON,
    .pull_up_resistor_ow = PUP_DOWN,
    .discharge_permit = DCP_OFF,
    .reset_filter = RSTF_ON,
    .error_injection_mode = WITHOUT_ERR,
};

adc_config_t g_thermistor_profile = {
    .redundant_measurement_mode = RD_OFF,
    .channels = AUX_ALL,
    .continuous_measurement = SINGLE,
    .ow_mode = OW_OFF_ALL_CH,
    .aux_ow_mode = AUX_OW_OFF,
    .pull_up_resistor_ow = PUP_DOWN,
    .discharge_permit = DCP_OFF,
    .reset_filter = RSTF_OFF,
    .error_injection_mode = WITHOUT_ERR,
};

adc_config_t g_thermistor_open_wire_check_profile = {
    .redundant_measurement_mode = RD_OFF,
    .channels = AUX_ALL,
    .continuous_measurement = SINGLE,
    .ow_mode = OW_ON_ALL_CH,
    .aux_ow_mode = AUX_OW_ON,
    .pull_up_resistor_ow = PUP_DOWN,
    .discharge_permit = DCP_OFF,
    .reset_filter = RSTF_OFF,
    .error_injection_mode = WITHOUT_ERR,
};

adc_config_t g_cell_open_wire_check_profile = {
    .redundant_measurement_mode = RD_ON, // RD
    .channels = AUX_ALL,
    .continuous_measurement = SINGLE, // Cont
    .ow_mode = OW_OFF_ALL_CH,         // OW OFF
    .aux_ow_mode = AUX_OW_OFF,        // OW OFF
    .pull_up_resistor_ow = PUP_DOWN,
    .discharge_permit = DCP_OFF,
    .reset_filter = RSTF_OFF,
    .error_injection_mode = WITHOUT_ERR,
};

adc_config_t g_cell_open_wire_check_profile_even = {
    .redundant_measurement_mode = RD_OFF, // RD
    .channels = AUX_ALL,
    .continuous_measurement = SINGLE, // Cont
    .ow_mode = OW_ON_EVEN_CH,
    .aux_ow_mode = AUX_OW_OFF, // OW OFF FOR AUX
    .pull_up_resistor_ow = PUP_DOWN,
    .discharge_permit = DCP_OFF,
    .reset_filter = RSTF_OFF,
    .error_injection_mode = WITHOUT_ERR,
};

adc_config_t g_cell_open_wire_check_profile_odd = {
    .redundant_measurement_mode = RD_OFF, // RD
    .channels = AUX_ALL,
    .continuous_measurement = SINGLE, // Cont
    .ow_mode = OW_ON_ODD_CH,
    .aux_ow_mode = AUX_OW_OFF, // OW OFF FOR AUX
    .pull_up_resistor_ow = PUP_DOWN,
    .discharge_permit = DCP_OFF,
    .reset_filter = RSTF_OFF,
    .error_injection_mode = WITHOUT_ERR,
};

voltage_config_t g_voltage_cfg = {
    .overvoltage_threshold_v = 4.15F,
    .undervoltage_threshold_v = 2.51F,
    .openwire_cell_threshold_mv = 1000,
    .openwire_aux_threshold_mv = 2900,
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
