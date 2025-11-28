#ifndef API_H
#define API_H

#include "bms_driver.h"
#include "bms_enums.h"
#include "bms_types.h"
#include "command_list.h"
#include "comms.h"
#include "data.h"
#include "parse.h"

void bms_init_config(cell_asic_ctx_t *asic_ctx);
void bms_write_read_config(cell_asic_ctx_t *asic_ctx);
void bms_write_config(cell_asic_ctx_t *asic_ctx);
void bms_read_config(cell_asic_ctx_t *asic_ctx);
void bms_start_adc_cell_voltage_measurment();
void bms_read_cell_voltages(cell_asic_ctx_t *asic_ctx);
void bms_start_adc_s_voltage_measurment();
void bms_read_s_voltages(cell_asic_ctx_t *asic_ctx);
void bms_start_avgcell_voltage_measurment();
void bms_read_avgcell_voltages(cell_asic_ctx_t *asic_ctx);
void bms_start_fcell_voltage_measurment();
void bms_read_fcell_voltages(cell_asic_ctx_t *asic_ctx);
void bms_start_aux_voltage_measurment(cell_asic_ctx_t *asic_ctx);
void bms_read_aux_voltages(cell_asic_ctx_t *asic_ctx);
void bms_start_raux_voltage_measurment(cell_asic_ctx_t *asic_ctx);
void bms_read_raux_voltages(cell_asic_ctx_t *asic_ctx);
void bms_read_status_registers(cell_asic_ctx_t *asic_ctx);
void measurement_loop(void);
void bms_read_device_sid(cell_asic_ctx_t *asic_ctx);
void bms_set_reset_gpio_pins(cell_asic_ctx_t *asic_ctx);
void bms_enable_mute(cell_asic_ctx_t *asic_ctx);
void bms_disable_mute(cell_asic_ctx_t *asic_ctx);
void bms_soft_reset();
void bms_reset_cmd_count();
void bms_reset_pec_error_flag(cell_asic_ctx_t *asic_ctx);
void bms_snap();
void bms_unsnap();
void bms_clear_cell_measurement();
void bms_clear_aux_measurement();
void bms_clear_spin_measurement();
void bms_clear_fcell_measurement();
void bms_clear_ovuv_measurement();
void bms_clear_all_flags(cell_asic_ctx_t *asic_ctx);
void bms_set_dcc_discharge(cell_asic_ctx_t *asic_ctx);
void bms_clear_dcc_discharge(cell_asic_ctx_t *asic_ctx);
void bms_write_read_pwm_duty_cycle(cell_asic_ctx_t *asic_ctx);
void bms_gpio_spi_communication(cell_asic_ctx_t *asic_ctx);

void bms_set_dtrng_dcto_value(cell_asic_ctx_t *asic_ctx);
void bms_run_osc_mismatch_self_test(cell_asic_ctx_t *asic_ctx);
void bms_run_thermal_shutdown_self_test(cell_asic_ctx_t *asic_ctx);
void bms_run_supply_error_detection_self_test(cell_asic_ctx_t *asic_ctx);
void bms_run_fuse_ed_self_test(cell_asic_ctx_t *asic_ctx);
void bms_run_fuse_med_self_test(cell_asic_ctx_t *asic_ctx);
void bms_run_tmodchk_self_test(cell_asic_ctx_t *asic_ctx);
void bms_check_latent_fault_csflt_status_bits(cell_asic_ctx_t *asic_ctx);
void bms_check_rdstatc_err_bit_functionality(cell_asic_ctx_t *asic_ctx);
void bms_cell_openwire_test(cell_asic_ctx_t *asic_ctx);
void bms_redundant_cell_openwire_test(cell_asic_ctx_t *asic_ctx);
void bms_cell_ow_volatge_collect(cell_asic_ctx_t *asic_ctx, bms_op_t type,
                                 open_wire_detection_mode_t ow_c_s);
void bms_aux_openwire_test(cell_asic_ctx_t *asic_ctx);
void bms_gpio_pup_up_down_volatge_collect(cell_asic_ctx_t *asic_ctx,
                                          pull_down_current_mode_t pup);
void bms_open_wire_detection_condtion_check(cell_asic_ctx_t *asic_ctx,
                                            bms_op_t type);
void bms_read_rdcvall_voltage(cell_asic_ctx_t *asic_ctx);
void bms_read_rdacall_voltage(cell_asic_ctx_t *asic_ctx);
void bms_read_rdsall_voltage(cell_asic_ctx_t *asic_ctx);
void bms_read_rdfcall_voltage(cell_asic_ctx_t *asic_ctx);
void bms_read_rdcsall_voltage(cell_asic_ctx_t *asic_ctx);
void bms_read_rdacsall_voltage(cell_asic_ctx_t *asic_ctx);
void bms_read_rdasall_voltage(cell_asic_ctx_t *asic_ctx);

// i dont think these are needed but im keeping them for now
void bms_gpio_i2c_write_to_slave(cell_asic_ctx_t *asic_ctx);
void bms_gpio_i2c_read_from_slave(cell_asic_ctx_t *asic_ctx);

#endif