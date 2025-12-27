#include "api.h"
#include "bms_enums.h"

// ----------- CONFIG ;; EDIT AS NEEDED -----------
adc_config_t g_adc_cfg = {
    .redundant_measurement_mode = RD_OFF,
    .channel_to_convert = AUX_ALL,
    .continuous_measurement = SINGLE,
    .cell_open_wire_detection_mode = OW_OFF_ALL_CH,
    .AUX_OW_en = AUX_OW_OFF,
    .PUP_en = PUP_DOWN,
    .DCP_en = DCP_OFF,
    .RSTF_en = RSTF_OFF,
    .ERR_en = WITHOUT_ERR,
};

voltage_config_t g_voltage_cfg = {
    .overvoltage_threshold_v = 4.2F,
    .undervoltage_threshold_v = 3.0F, // TODO GET THIS VALUE FOR REAL LIFE
    .openwire_cell_threshold_mv = 2000,
    .openwire_aux_threshold_mv = 50000,
    .loop_meas_count = 4,
    .meas_loop_time_ms = 1000,
};

measurement_config_t g_meas_cfg = {
    .measure_cell = ENABLED,
    .measure_avg_cell = ENABLED,
    .measure_f_cell = ENABLED,
    .measure_s_voltage = ENABLED,
    .measure_aux = DISABLED,
    .measure_raux = DISABLED,
    .measure_stat = DISABLED,
};
// --------------------------------------------------

comm_status_t adbms_init_config(cell_asic_ctx_t *asic_ctx) {
  bms_cfg_reg_a_t *cfg_a;
  bms_cfg_reg_b_t *cfg_b;
  for (uint8_t ic = 0; ic < asic_ctx->ic_count; ic++) {
    cfg_a = &asic_ctx[ic].tx_cfg_a;
    cfg_b = &asic_ctx[ic].tx_cfg_b;

    cfg_a->REFON = POWER_UP;
    cfg_a->GPIOx = 0X3FF; // All GPIO pull down off

    cfg_b->VOV =
        set_ov_voltage_threshold(g_voltage_cfg.overvoltage_threshold_v);
    cfg_b->VUV =
        set_uv_voltage_threshold(g_voltage_cfg.undervoltage_threshold_v);
  }

  asic_wakeup(asic_ctx->ic_count);
  //   todo
  //   bms_write_data(asic_ctx, WRCFGA, Config, A);
  //   bms_write_data(asic_ctx, WRCFGB, Config, B);

  return COMM_OK;
}

comm_status_t adbms_write_read_config(cell_asic_ctx_t *asic_ctx) {
  // TODO
  return COMM_OK;
}

comm_status_t adbms_write_config(cell_asic_ctx_t *asic_ctx) {
  // TODO
  return COMM_OK;
}

comm_status_t adbms_read_config(cell_asic_ctx_t *asic_ctx) {
  // TODO
  return COMM_OK;
}

comm_status_t adbms_start_adc_cell_voltage_measurment() {
  // TODO
  return COMM_OK;
}

comm_status_t adbms_read_cell_voltages(cell_asic_ctx_t *asic_ctx) {
  // TODO
  return COMM_OK;
}

comm_status_t adbms_start_adc_s_voltage_measurment() {
  // TODO
  return COMM_OK;
}

comm_status_t adbms_read_s_voltages(cell_asic_ctx_t *asic_ctx) {
  // TODO
  return COMM_OK;
}

comm_status_t adbms_start_avgcell_voltage_measurment() {
  // TODO
  return COMM_OK;
}

comm_status_t adbms_read_avgcell_voltages(cell_asic_ctx_t *asic_ctx) {
  // TODO
  return COMM_OK;
}

comm_status_t adbms_start_fcell_voltage_measurment() {
  // TODO
  return COMM_OK;
}

comm_status_t adbms_read_fcell_voltages(cell_asic_ctx_t *asic_ctx) {
  // TODO
  return COMM_OK;
}

comm_status_t adbms_start_aux_voltage_measurment(cell_asic_ctx_t *asic_ctx) {
  // TODO
  return COMM_OK;
}

comm_status_t adbms_read_aux_voltages(cell_asic_ctx_t *asic_ctx) {
  // TODO
  return COMM_OK;
}

comm_status_t adbms_start_raux_voltage_measurment(cell_asic_ctx_t *asic_ctx) {
  // TODO
  return COMM_OK;
}

comm_status_t adbms_read_raux_voltages(cell_asic_ctx_t *asic_ctx) {
  // TODO
  return COMM_OK;
}

comm_status_t adbms_read_status_registers(cell_asic_ctx_t *asic_ctx) {
  // TODO
  return COMM_OK;
}

comm_status_t measurement_loop(void) {
  // TODO
  return COMM_OK;
}

comm_status_t adbms_read_device_sid(cell_asic_ctx_t *asic_ctx) {
  // TODO
  return COMM_OK;
}

comm_status_t adbms_set_reset_gpio_pins(cell_asic_ctx_t *asic_ctx) {
  // TODO
  return COMM_OK;
}

comm_status_t adbms_enable_mute(cell_asic_ctx_t *asic_ctx) {
  // TODO
  return COMM_OK;
}

comm_status_t adbms_disable_mute(cell_asic_ctx_t *asic_ctx) {
  // TODO
  return COMM_OK;
}

comm_status_t adbms_soft_reset() {
  // TODO
  return COMM_OK;
}

comm_status_t adbms_reset_cmd_count() {
  // TODO
  return COMM_OK;
}

comm_status_t adbms_reset_pec_error_flag(cell_asic_ctx_t *asic_ctx) {
  // TODO
  return COMM_OK;
}

comm_status_t adbms_snap() {
  // TODO
  return COMM_OK;
}

comm_status_t adbms_unsnap() {
  // TODO
  return COMM_OK;
}

comm_status_t adbms_clear_cell_measurement() {
  // TODO
  return COMM_OK;
}

comm_status_t adbms_clear_aux_measurement() {
  // TODO
  return COMM_OK;
}

comm_status_t adbms_clear_spin_measurement() {
  // TODO
  return COMM_OK;
}

comm_status_t adbms_clear_fcell_measurement() {
  // TODO
  return COMM_OK;
}

comm_status_t adbms_clear_ovuv_measurement() {
  // TODO
  return COMM_OK;
}

comm_status_t adbms_clear_all_flags(cell_asic_ctx_t *asic_ctx) {
  // TODO
  return COMM_OK;
}

comm_status_t adbms_set_dcc_discharge(cell_asic_ctx_t *asic_ctx) {
  // TODO
  return COMM_OK;
}

comm_status_t adbms_clear_dcc_discharge(cell_asic_ctx_t *asic_ctx) {
  // TODO
  return COMM_OK;
}

comm_status_t adbms_write_read_pwm_duty_cycle(cell_asic_ctx_t *asic_ctx) {
  // TODO
  return COMM_OK;
}

comm_status_t adbms_gpio_spi_communication(cell_asic_ctx_t *asic_ctx) {
  // TODO
  return COMM_OK;
}

comm_status_t adbms_set_dtrng_dcto_value(cell_asic_ctx_t *asic_ctx) {
  // TODO
  return COMM_OK;
}

comm_status_t adbms_run_osc_mismatch_self_test(cell_asic_ctx_t *asic_ctx) {
  // TODO
  return COMM_OK;
}

comm_status_t adbms_run_thermal_shutdown_self_test(cell_asic_ctx_t *asic_ctx) {
  // TODO
  return COMM_OK;
}

comm_status_t
adbms_run_supply_error_detection_self_test(cell_asic_ctx_t *asic_ctx) {
  // TODO
  return COMM_OK;
}

comm_status_t adbms_run_fuse_ed_self_test(cell_asic_ctx_t *asic_ctx) {
  // TODO
  return COMM_OK;
}

comm_status_t adbms_run_fuse_med_self_test(cell_asic_ctx_t *asic_ctx) {
  // TODO
  return COMM_OK;
}

comm_status_t adbms_run_tmodchk_self_test(cell_asic_ctx_t *asic_ctx) {
  // TODO
  return COMM_OK;
}

comm_status_t
adbms_check_latent_fault_csflt_status_bits(cell_asic_ctx_t *asic_ctx) {
  // TODO
  return COMM_OK;
}

comm_status_t
adbms_check_rdstatc_err_bit_functionality(cell_asic_ctx_t *asic_ctx) {
  // TODO
  return COMM_OK;
}

comm_status_t adbms_cell_openwire_test(cell_asic_ctx_t *asic_ctx) {
  // TODO
  return COMM_OK;
}

comm_status_t adbms_redundant_cell_openwire_test(cell_asic_ctx_t *asic_ctx) {
  // TODO
  return COMM_OK;
}

comm_status_t adbms_cell_ow_voltage_collect(cell_asic_ctx_t *asic_ctx,
                                            bms_op_t type,
                                            open_wire_detection_mode_t ow_c_s) {
  // TODO
  return COMM_OK;
}

comm_status_t adbms_aux_openwire_test(cell_asic_ctx_t *asic_ctx) {
  // TODO
  return COMM_OK;
}

comm_status_t
adbms_gpio_pup_up_down_volatge_collect(cell_asic_ctx_t *asic_ctx,
                                       pull_down_current_mode_t pup) {
  // TODO
  return COMM_OK;
}

comm_status_t
adbms_open_wire_detection_condtion_check(cell_asic_ctx_t *asic_ctx,
                                         bms_op_t type) {
  // TODO
  return COMM_OK;
}

comm_status_t adbms_read_rdcvall_voltage(cell_asic_ctx_t *asic_ctx) {
  // TODO
  return COMM_OK;
}

comm_status_t adbms_read_rdacall_voltage(cell_asic_ctx_t *asic_ctx) {
  // TODO
  return COMM_OK;
}

comm_status_t adbms_read_rdsall_voltage(cell_asic_ctx_t *asic_ctx) {
  // TODO
  return COMM_OK;
}

comm_status_t adbms_read_rdfcall_voltage(cell_asic_ctx_t *asic_ctx) {
  // TODO
  return COMM_OK;
}

comm_status_t adbms_read_rdcsall_voltage(cell_asic_ctx_t *asic_ctx) {
  // TODO
  return COMM_OK;
}

comm_status_t adbms_read_rdacsall_voltage(cell_asic_ctx_t *asic_ctx) {
  // TODO
  return COMM_OK;
}

comm_status_t adbms_read_rdasall_voltage(cell_asic_ctx_t *asic_ctx) {
  // TODO
  return COMM_OK;
}
