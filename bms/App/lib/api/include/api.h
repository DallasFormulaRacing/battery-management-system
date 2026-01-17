#ifndef API_H
#define API_H

#include "bms_driver.h"
#include "bms_enums.h"
#include "bms_types.h"
#include "command_list.h"
#include "comms.h"
#include "data.h"
#include "parse.h"

comm_status_t adbms_init_config(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_write_read_config(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_write_config(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_read_config(cell_asic_ctx_t *asic_ctx);
comm_status_t
adbms_start_adc_cell_voltage_measurment(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_read_cell_voltages(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_start_adc_s_voltage_measurment(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_read_s_voltages(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_start_avgcell_voltage_measurment(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_read_avgcell_voltages(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_start_fcell_voltage_measurment(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_read_fcell_voltages(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_start_aux_voltage_measurment(
    cell_asic_ctx_t *asic_ctx,
    aux_adc_input_channel_select_t channel_to_convert);
comm_status_t adbms_read_aux_voltages(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_start_raux_voltage_measurment(
    cell_asic_ctx_t *asic_ctx, redundant_enable_t redundant_measurement_mode);
comm_status_t adbms_read_raux_voltages(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_read_status_registers(cell_asic_ctx_t *asic_ctx);

comm_status_t adbms_poll_for_conversion_adc(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_poll_for_conversion_c_adc(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_poll_for_conversion_s_adc(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_poll_for_conversion_aux_adc(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_poll_for_conversion_aux2_adc(cell_asic_ctx_t *asic_ctx);

bool is_conversion_done(const volatile uint8_t *poll_bytes,
                        cell_asic_ctx_t *asic_ctx);

comm_status_t adbms_bleed_cell_pwm(cell_asic_ctx_t *asic_ctx,
                                   uint8_t cell_number, uint8_t segment_number,
                                   pwm_duty_cycle_t duty_cycle);

comm_status_t measurement_loop(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_read_device_sid(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_set_reset_gpio_pins(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_enable_mute(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_disable_mute(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_soft_reset(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_reset_cmd_count(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_reset_pec_error_flag(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_snap(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_unsnap(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_clear_cell_measurement(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_clear_aux_measurement(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_clear_spin_measurement(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_clear_fcell_measurement(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_clear_ovuv_measurement(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_clear_all_flags(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_set_dcc_discharge(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_clear_dcc_discharge(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_write_read_pwm_duty_cycle(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_gpio_spi_communication(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_set_dtrng_dcto_value(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_run_osc_mismatch_self_test(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_run_thermal_shutdown_self_test(cell_asic_ctx_t *asic_ctx);
comm_status_t
adbms_run_supply_error_detection_self_test(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_run_fuse_ed_self_test(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_run_fuse_med_self_test(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_run_tmodchk_self_test(cell_asic_ctx_t *asic_ctx);
comm_status_t
adbms_check_latent_fault_csflt_status_bits(cell_asic_ctx_t *asic_ctx);
comm_status_t
adbms_check_rdstatc_err_bit_functionality(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_cell_openwire_test(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_redundant_cell_openwire_test(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_cell_ow_volatge_collect(cell_asic_ctx_t *asic_ctx,
                                            bms_op_t type,
                                            open_wire_detection_mode_t ow_c_s);
comm_status_t adbms_aux_openwire_test(cell_asic_ctx_t *asic_ctx);
comm_status_t
adbms_gpio_pup_up_down_volatge_collect(cell_asic_ctx_t *asic_ctx,
                                       pull_down_current_mode_t pup);
comm_status_t
adbms_open_wire_detection_condtion_check(cell_asic_ctx_t *asic_ctx,
                                         bms_op_t type);
comm_status_t adbms_read_rdcvall_voltage(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_read_rdacall_voltage(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_read_rdsall_voltage(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_read_rdfcall_voltage(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_read_rdcsall_voltage(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_read_rdacsall_voltage(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_read_rdasall_voltage(cell_asic_ctx_t *asic_ctx);

// i dont think these are needed but im keeping them for now
comm_status_t adbms_gpio_i2c_write_to_slave(cell_asic_ctx_t *asic_ctx);
comm_status_t adbms_gpio_i2c_read_from_slave(cell_asic_ctx_t *asic_ctx);

#endif