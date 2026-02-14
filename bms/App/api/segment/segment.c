#include "segment.h"
#include "bms_comms.h"
#include "bms_enums.h"
#include "bms_types.h"
#include "command_list.h"
#include "config.h"
#include "data.h"
#include "parse.h"
#include <assert.h>
#include <stdint.h>

extern adc_config_t g_adc_cfg;
extern voltage_config_t g_voltage_cfg;
extern measurement_config_t g_meas_cfg;

#define RETURN_IF_ERROR(status)                                                \
  do {                                                                         \
    comm_status_t _s = (status);                                               \
    if (_s != COMM_OK)                                                         \
      return _s;                                                               \
  } while (0)

comm_status_t adbms_init_config(cell_asic_ctx_t *asic_ctx) {
  bms_cfg_reg_a_t *cfg_a; // table 102
  bms_cfg_reg_b_t *cfg_b; // table 103
  for (uint8_t ic = 0; ic < asic_ctx->ic_count; ic++) {
    cfg_a = &asic_ctx[ic].tx_cfg_a;
    cfg_b = &asic_ctx[ic].tx_cfg_b;

    cfg_a->REFON = POWER_UP;
    cfg_a->GPIOx = 0x3FF; // All GPIO pull down off (10 1s)

    cfg_b->VOV =
        set_ov_voltage_threshold(g_voltage_cfg.overvoltage_threshold_v);
    cfg_b->VUV =
        set_uv_voltage_threshold(g_voltage_cfg.undervoltage_threshold_v);
  }

  asic_wakeup(asic_ctx->ic_count);

  RETURN_IF_ERROR(
      bms_write_data(asic_ctx, BMS_REG_CONFIG, WRCFGA, REG_GROUP_A));
  RETURN_IF_ERROR(
      bms_write_data(asic_ctx, BMS_REG_CONFIG, WRCFGB, REG_GROUP_B));

  return COMM_OK;
}

comm_status_t adbms_write_read_config(cell_asic_ctx_t *asic_ctx) {
  asic_wakeup(asic_ctx->ic_count);
  RETURN_IF_ERROR(
      bms_write_data(asic_ctx, BMS_REG_CONFIG, WRCFGA, REG_GROUP_A));
  RETURN_IF_ERROR(
      bms_write_data(asic_ctx, BMS_REG_CONFIG, WRCFGB, REG_GROUP_B));
  RETURN_IF_ERROR(bms_read_data(asic_ctx, BMS_REG_CONFIG, RDCFGA, REG_GROUP_A));
  RETURN_IF_ERROR(bms_read_data(asic_ctx, BMS_REG_CONFIG, RDCFGB, REG_GROUP_B));
  return COMM_OK;
}

comm_status_t adbms_write_config(cell_asic_ctx_t *asic_ctx) {
  asic_wakeup(asic_ctx->ic_count);
  RETURN_IF_ERROR(
      bms_write_data(asic_ctx, BMS_REG_CONFIG, WRCFGA, REG_GROUP_A));
  RETURN_IF_ERROR(
      bms_write_data(asic_ctx, BMS_REG_CONFIG, WRCFGB, REG_GROUP_B));
  return COMM_OK;
}

comm_status_t adbms_read_config(cell_asic_ctx_t *asic_ctx) {
  asic_wakeup(asic_ctx->ic_count);
  RETURN_IF_ERROR(bms_read_data(asic_ctx, BMS_REG_CONFIG, RDCFGA, REG_GROUP_A));
  RETURN_IF_ERROR(bms_read_data(asic_ctx, BMS_REG_CONFIG, RDCFGB, REG_GROUP_B));
  return COMM_OK;
}

comm_status_t
adbms_start_adc_cell_voltage_measurment(cell_asic_ctx_t *asic_ctx) {
  // TODO
  asic_wakeup(asic_ctx->ic_count);
  spi_adcv_command(g_adc_cfg.redundant_measurement_mode,
                   g_adc_cfg.continuous_measurement, g_adc_cfg.DCP_en,
                   g_adc_cfg.RSTF_en, g_adc_cfg.ow_mode);
  return COMM_OK;
}

comm_status_t adbms_read_cell_voltages(cell_asic_ctx_t *asic_ctx) {
  // TODO
  asic_wakeup(asic_ctx->ic_count);
  spi_adc_snap_command();
  RETURN_IF_ERROR(
      bms_read_data(asic_ctx, BMS_REG_CELL_VOLT, RDCVA, REG_GROUP_A));
  RETURN_IF_ERROR(
      bms_read_data(asic_ctx, BMS_REG_CELL_VOLT, RDCVB, REG_GROUP_B));
  RETURN_IF_ERROR(
      bms_read_data(asic_ctx, BMS_REG_CELL_VOLT, RDCVC, REG_GROUP_C));
  RETURN_IF_ERROR(
      bms_read_data(asic_ctx, BMS_REG_CELL_VOLT, RDCVD, REG_GROUP_D));
  RETURN_IF_ERROR(
      bms_read_data(asic_ctx, BMS_REG_CELL_VOLT, RDCVE, REG_GROUP_E));
  RETURN_IF_ERROR(
      bms_read_data(asic_ctx, BMS_REG_CELL_VOLT, RDCVF, REG_GROUP_F));
  spi_adc_unsnap_command();
  return COMM_OK;
}

comm_status_t adbms_start_adc_s_voltage_measurment(cell_asic_ctx_t *asic_ctx) {
  // TODO
  asic_wakeup(asic_ctx->ic_count);
  spi_adsv_command(g_adc_cfg.continuous_measurement, g_adc_cfg.DCP_en,
                   g_adc_cfg.ow_mode);
  return COMM_OK;
}

comm_status_t adbms_read_s_voltages(cell_asic_ctx_t *asic_ctx) {
  // TODO
  asic_wakeup(asic_ctx->ic_count);
  spi_adc_snap_command();
  RETURN_IF_ERROR(bms_read_data(asic_ctx, BMS_REG_S_VOLT, RDSVA, REG_GROUP_A));
  RETURN_IF_ERROR(bms_read_data(asic_ctx, BMS_REG_S_VOLT, RDSVB, REG_GROUP_B));
  RETURN_IF_ERROR(bms_read_data(asic_ctx, BMS_REG_S_VOLT, RDSVC, REG_GROUP_C));
  RETURN_IF_ERROR(bms_read_data(asic_ctx, BMS_REG_S_VOLT, RDSVD, REG_GROUP_D));
  RETURN_IF_ERROR(bms_read_data(asic_ctx, BMS_REG_S_VOLT, RDSVE, REG_GROUP_E));
  RETURN_IF_ERROR(bms_read_data(asic_ctx, BMS_REG_S_VOLT, RDSVF, REG_GROUP_F));
  spi_adc_unsnap_command();
  return COMM_OK;
}

comm_status_t
adbms_start_avgcell_voltage_measurment(cell_asic_ctx_t *asic_ctx) {
  // TODO
  asic_wakeup(asic_ctx->ic_count);
  spi_adcv_command(g_adc_cfg.redundant_measurement_mode,
                   g_adc_cfg.continuous_measurement, g_adc_cfg.DCP_en,
                   g_adc_cfg.RSTF_en, g_adc_cfg.ow_mode);
  return COMM_OK;
}

comm_status_t adbms_read_avgcell_voltages(cell_asic_ctx_t *asic_ctx) {
  // TODO
  spi_adc_snap_command();
  RETURN_IF_ERROR(
      bms_read_data(asic_ctx, BMS_REG_AVG_CELL_VOLT, RDACA, REG_GROUP_A));
  RETURN_IF_ERROR(
      bms_read_data(asic_ctx, BMS_REG_AVG_CELL_VOLT, RDACB, REG_GROUP_B));
  RETURN_IF_ERROR(
      bms_read_data(asic_ctx, BMS_REG_AVG_CELL_VOLT, RDACC, REG_GROUP_C));
  RETURN_IF_ERROR(
      bms_read_data(asic_ctx, BMS_REG_AVG_CELL_VOLT, RDACD, REG_GROUP_D));
  RETURN_IF_ERROR(
      bms_read_data(asic_ctx, BMS_REG_AVG_CELL_VOLT, RDACE, REG_GROUP_E));
  RETURN_IF_ERROR(
      bms_read_data(asic_ctx, BMS_REG_AVG_CELL_VOLT, RDACF, REG_GROUP_F));
  spi_adc_unsnap_command();
  return COMM_OK;
}

comm_status_t adbms_start_fcell_voltage_measurment(cell_asic_ctx_t *asic_ctx) {
  // TODO
  asic_wakeup(asic_ctx->ic_count);
  spi_adcv_command(g_adc_cfg.redundant_measurement_mode,
                   g_adc_cfg.continuous_measurement, g_adc_cfg.DCP_en,
                   g_adc_cfg.RSTF_en, g_adc_cfg.ow_mode);
  return COMM_OK;
}

comm_status_t adbms_read_fcell_voltages(cell_asic_ctx_t *asic_ctx) {
  // TODO
  asic_wakeup(asic_ctx->ic_count);
  spi_adcv_command(g_adc_cfg.redundant_measurement_mode,
                   g_adc_cfg.continuous_measurement, g_adc_cfg.DCP_en,
                   g_adc_cfg.RSTF_en, g_adc_cfg.ow_mode);
  spi_adc_snap_command();
  RETURN_IF_ERROR(
      bms_read_data(asic_ctx, BMS_REG_FILTERED_CELL_VOLT, RDFCA, REG_GROUP_A));
  RETURN_IF_ERROR(
      bms_read_data(asic_ctx, BMS_REG_FILTERED_CELL_VOLT, RDFCB, REG_GROUP_B));
  RETURN_IF_ERROR(
      bms_read_data(asic_ctx, BMS_REG_FILTERED_CELL_VOLT, RDFCC, REG_GROUP_C));
  RETURN_IF_ERROR(
      bms_read_data(asic_ctx, BMS_REG_FILTERED_CELL_VOLT, RDFCD, REG_GROUP_D));
  RETURN_IF_ERROR(
      bms_read_data(asic_ctx, BMS_REG_FILTERED_CELL_VOLT, RDFCE, REG_GROUP_E));
  RETURN_IF_ERROR(
      bms_read_data(asic_ctx, BMS_REG_FILTERED_CELL_VOLT, RDFCF, REG_GROUP_F));
  spi_adc_unsnap_command();
  return COMM_OK;
}

comm_status_t adbms_start_aux_voltage_measurement(cell_asic_ctx_t *asic_ctx) {
  // TODO
  for (uint8_t ic = 0; ic < asic_ctx->ic_count; ic++) {
    asic_ctx[ic].tx_cfg_a.REFON = POWER_UP;
    asic_ctx[ic].tx_cfg_a.GPIOx = 0X3FF; // All GPIO pull down off
  }
  asic_wakeup(asic_ctx->ic_count);
  bms_write_data(asic_ctx, BMS_REG_CONFIG, WRCFGA, REG_GROUP_A);
  spi_adax_command(g_adc_cfg.AUX_OW_en, g_adc_cfg.PUP_en, g_adc_cfg.channels);
  return COMM_OK;
}

comm_status_t adbms_read_aux_voltages(cell_asic_ctx_t *asic_ctx) {
  // TODO
  asic_wakeup(asic_ctx->ic_count);
  spi_adax_command(g_adc_cfg.AUX_OW_en, g_adc_cfg.PUP_en, g_adc_cfg.channels);
  RETURN_IF_ERROR(
      bms_read_data(asic_ctx, BMS_REG_AUX_VOLT, RDAUXA, REG_GROUP_A));
  RETURN_IF_ERROR(
      bms_read_data(asic_ctx, BMS_REG_AUX_VOLT, RDAUXB, REG_GROUP_B));
  RETURN_IF_ERROR(
      bms_read_data(asic_ctx, BMS_REG_AUX_VOLT, RDAUXC, REG_GROUP_C));
  RETURN_IF_ERROR(
      bms_read_data(asic_ctx, BMS_REG_AUX_VOLT, RDAUXD, REG_GROUP_D));
  return COMM_OK;
}

comm_status_t adbms_start_raux_voltage_measurment(
    cell_asic_ctx_t *asic_ctx, redundant_enable_t redundant_measurement_mode) {
  // TODO
  for (uint8_t ic = 0; ic < asic_ctx->ic_count; ic++) {
    asic_ctx[ic].tx_cfg_a.REFON = POWER_UP;
    asic_ctx[ic].tx_cfg_a.GPIOx = 0X3FF; // All GPIO pull down off
  }
  asic_wakeup(asic_ctx->ic_count);
  bms_write_data(asic_ctx, BMS_REG_CONFIG, WRCFGA, REG_GROUP_A);
  spi_adax_command(g_adc_cfg.AUX_OW_en, g_adc_cfg.PUP_en, g_adc_cfg.channels);
  return COMM_OK;
}

comm_status_t adbms_read_raux_voltages(cell_asic_ctx_t *asic_ctx) {
  // TODO
  asic_wakeup(asic_ctx->ic_count);
  spi_adax2_command(AUX_ALL);
  RETURN_IF_ERROR(
      bms_read_data(asic_ctx, BMS_REG_REDUNDANT_AUX_VOLT, RDRAXA, REG_GROUP_A));
  RETURN_IF_ERROR(
      bms_read_data(asic_ctx, BMS_REG_REDUNDANT_AUX_VOLT, RDRAXB, REG_GROUP_B));
  RETURN_IF_ERROR(
      bms_read_data(asic_ctx, BMS_REG_REDUNDANT_AUX_VOLT, RDRAXC, REG_GROUP_C));
  RETURN_IF_ERROR(
      bms_read_data(asic_ctx, BMS_REG_REDUNDANT_AUX_VOLT, RDRAXD, REG_GROUP_D));
  return COMM_OK;
}

comm_status_t adbms_read_status_registers(cell_asic_ctx_t *asic_ctx) {
  // TODO
  asic_wakeup(asic_ctx->ic_count);
  bms_write_data(asic_ctx, BMS_REG_CONFIG, WRCFGA, REG_GROUP_A);
  bms_write_data(asic_ctx, BMS_REG_CONFIG, WRCFGB, REG_GROUP_B);

  spi_adax_command(g_adc_cfg.AUX_OW_en, g_adc_cfg.PUP_en, g_adc_cfg.channels);
  spi_adcv_command(g_adc_cfg.redundant_measurement_mode,
                   g_adc_cfg.continuous_measurement, g_adc_cfg.DCP_en,
                   g_adc_cfg.RSTF_en, g_adc_cfg.ow_mode);

  RETURN_IF_ERROR(
      bms_read_data(asic_ctx, BMS_REG_STATUS, RDSTATA, REG_GROUP_A));
  RETURN_IF_ERROR(
      bms_read_data(asic_ctx, BMS_REG_STATUS, RDSTATB, REG_GROUP_B));
  RETURN_IF_ERROR(
      bms_read_data(asic_ctx, BMS_REG_STATUS, RDSTATC, REG_GROUP_C));
  RETURN_IF_ERROR(
      bms_read_data(asic_ctx, BMS_REG_STATUS, RDSTATD, REG_GROUP_D));
  RETURN_IF_ERROR(
      bms_read_data(asic_ctx, BMS_REG_STATUS, RDSTATE, REG_GROUP_E));
  return COMM_OK;
}

comm_status_t measurement_loop(cell_asic_ctx_t *asic_ctx) {
  // ! might not work due to duplicate asicwake
  // todo: finish this if we need it
  if (g_meas_cfg.measure_cell == ENABLED) {
    adbms_start_adc_cell_voltage_measurment(asic_ctx);
    adbms_read_cell_voltages(asic_ctx);
  }
  if (g_meas_cfg.measure_avg_cell == ENABLED) {
    adbms_start_avgcell_voltage_measurment(asic_ctx);
    adbms_read_avgcell_voltages(asic_ctx);
  }
  return COMM_OK;
}

bool is_conversion_done(const volatile uint8_t *poll_bytes,
                        cell_asic_ctx_t *asic_ctx) {
  for (uint8_t ic = 0; ic < asic_ctx->ic_count; ic++) {
    uint8_t status_byte = poll_bytes[(2 * ic) + 1];

    if (status_byte == 0xFF) {
      return false;
    }
  }
  return true;
}

comm_status_t adbms_poll_for_conversion_adc(cell_asic_ctx_t *asic_ctx) {
  uint8_t poll_bytes[2 * asic_ctx->ic_count];

  do {
    spi_poll_command_raw(PLADC, asic_ctx->ic_count, poll_bytes);
  } while (!is_conversion_done(poll_bytes, asic_ctx));
  return COMM_OK;
}

comm_status_t adbms_poll_for_conversion_c_adc(cell_asic_ctx_t *asic_ctx) {
  uint8_t poll_bytes[2 * asic_ctx->ic_count];

  do {
    spi_poll_command_raw(PLCADC, asic_ctx->ic_count, poll_bytes);
  } while (!is_conversion_done(poll_bytes, asic_ctx));
  return COMM_OK;
}

comm_status_t adbms_poll_for_conversion_s_adc(cell_asic_ctx_t *asic_ctx) {
  uint8_t poll_bytes[2 * asic_ctx->ic_count];

  do {
    spi_poll_command_raw(PLSADC, asic_ctx->ic_count, poll_bytes);
  } while (!is_conversion_done(poll_bytes, asic_ctx));
  return COMM_OK;
}

comm_status_t adbms_poll_for_conversion_aux_adc(cell_asic_ctx_t *asic_ctx) {
  uint8_t poll_bytes[2 * asic_ctx->ic_count];

  do {
    spi_poll_command_raw(PLAUX1, asic_ctx->ic_count, poll_bytes);
  } while (!is_conversion_done(poll_bytes, asic_ctx));
  return COMM_OK;
}

comm_status_t adbms_poll_for_conversion_aux2_adc(cell_asic_ctx_t *asic_ctx) {
  uint8_t poll_bytes[2 * asic_ctx->ic_count];

  do {
    spi_poll_command_raw(PLAUX2, asic_ctx->ic_count, poll_bytes);
  } while (!is_conversion_done(poll_bytes, asic_ctx));
  return COMM_OK;
}

/** // TODO: Needs to be tested
 * @brief Sets the pwm duty cycle for each cell, but does not send the command
 * yet. This allows for multiple cells to be bled in parallel as you can call
 * this function as many times as necessary until you need to send the PWM
 * command + mailbox contents.
 *
 * For example (pseudocode):
 * adbms_set_cell_pwm(..., cell 10, segment 0, 50%);
 * adbms_set_cell_pwm(..., cell 15, segment 3, 10%);
 * adbms_set_cell_pwm(..., cell 2, segment 11, 80%);
 *
 * Then, when you are satisfied with the cells you are bleeding at a certain
 * rate, you call adbms_send_pwm_commands(...).
 *
 * This is an improvement over the previous implementation, which you could only
 * bleed one cell at a time; this function now bleed multiple cells at a time.
 *
 * @param asic_ctx -- array of ASIC context
 * @param cell_number -- cell to be bled at designated pwm duty_cycle
 * @param segment_number -- segment number (used by the asic array)
 * @param duty_cycle -- duty cycle you want to bleed cell_number at
 * @return comm_status_t -- status of SPI transmission
 */
comm_status_t adbms_set_cell_pwm(cell_asic_ctx_t *asic_ctx, uint8_t cell_number,
                                 uint8_t segment_number,
                                 pwm_duty_cycle_t duty_cycle) {
  // problem: cell indexing starts at 0, but pwm indexing starts at 1
  uint8_t pwm_number = cell_number + 1;
  if (pwm_number <= ADBMS_NUM_PWMA_CHANNELS) {
    asic_ctx[segment_number].pwm_ctl_a.pwm_a_ctl_array[pwm_number] = duty_cycle;
  }

  // use the second pwm register if we are at cells 12-15
  else {
    assert(pwm_number > ADBMS_NUM_PWMA_CHANNELS &&
           pwm_number <= (ADBMS_NUM_PWMA_CHANNELS + ADBMS_NUM_PWMB_CHANNELS));

    asic_ctx[segment_number]
        .pwm_ctl_b.pwm_b_ctl_array[pwm_number - ADBMS_NUM_PWMA_CHANNELS - 1] =
        duty_cycle;
  }
  return COMM_OK;
}

/**
 * @brief Builds the PWM Mailboxes, and subsequently, the PWM Command packet
 * from the given PWM Config structs. Ideally, use after calling
 * adbms_set_cell_pwm(...) sufficiently.
 *
 * @param asic_ctx
 * @return comm_status_t
 */
comm_status_t adbms_send_pwm_commands(cell_asic_ctx_t *asic_ctx) {
  bms_create_pwm_a(asic_ctx);
  bms_create_pwm_b(asic_ctx);
  RETURN_IF_ERROR(bms_write_data(asic_ctx, BMS_REG_PWM, WRPWMB, REG_GROUP_B));
  RETURN_IF_ERROR(bms_write_data(asic_ctx, BMS_REG_PWM, WRPWMA, REG_GROUP_A));

  return COMM_OK;
}

/**
 * @brief Sets all cell PWM duty cycle half-bytes to zero for every asic in asic
 * chain. Then constructs mailbox packet and transmit over SPI.
 *
 * @param asic_ctx -- array of asic contexts
 * @return comm_status_t -- status of SPI transmission
 */
comm_status_t adbms_clear_all_pwm(cell_asic_ctx_t *asic_ctx) {
  for (uint8_t segment_idx = 0; segment_idx < IC_COUNT_CHAIN; segment_idx++) {
    for (uint8_t cell_idx = 0; cell_idx < ADBMS_NUM_CELLS_PER_IC; cell_idx++) {
      adbms_set_cell_pwm(asic_ctx, cell_idx, segment_idx,
                         PWM_0_0_PERCENT_DUTY_CYCLE);
    }
  }

  return adbms_send_pwm_commands(asic_ctx);
}

// comm_status_t adbms_read_device_sid(cell_asic_ctx_t *asic_ctx) {
//   // TODO

//   return COMM_OK;
// }

// comm_status_t adbms_set_reset_gpio_pins(cell_asic_ctx_t *asic_ctx) {
//   // TODO
//   return COMM_OK;
// }

// comm_status_t adbms_enable_mute(cell_asic_ctx_t *asic_ctx) {
//   // TODO
//   return COMM_OK;
// }

// comm_status_t adbms_disable_mute(cell_asic_ctx_t *asic_ctx) {
//   // TODO
//   return COMM_OK;
// }

// comm_status_t adbms_soft_reset() {
//   // TODO
//   return COMM_OK;
// }

// comm_status_t adbms_reset_cmd_count() {
//   // TODO
//   return COMM_OK;
// }

// comm_status_t adbms_reset_pec_error_flag(cell_asic_ctx_t *asic_ctx) {
//   // TODO
//   return COMM_OK;
// }

// comm_status_t adbms_clear_cell_measurement() {
//   // TODO
//   asic_wakeup(asic_ctx->ic_count);
//   bms_send_command(CLRCELL);
//   return COMM_OK;
// }

// comm_status_t adbms_clear_aux_measurement() {
//   // TODO
//   return COMM_OK;
// }

// comm_status_t adbms_clear_s_pin_measurement() {
//   // TODO
//   return COMM_OK;
// }

// comm_status_t adbms_clear_fcell_measurement() {
//   // TODO
//   return COMM_OK;
// }

// comm_status_t adbms_clear_ovuv_measurement() {
//   // TODO
//   return COMM_OK;
// }

// comm_status_t adbms_clear_all_flags(cell_asic_ctx_t *asic_ctx) {
//   // TODO
//   return COMM_OK;
// }

// comm_status_t adbms_set_dcc_discharge(cell_asic_ctx_t *asic_ctx) {
//   // TODO
//   return COMM_OK;
// }

// comm_status_t adbms_clear_dcc_discharge(cell_asic_ctx_t *asic_ctx) {
//   // TODO
//   return COMM_OK;
// }

// comm_status_t adbms_write_read_pwm_duty_cycle(cell_asic_ctx_t *asic_ctx) {
//   // TODO
//   return COMM_OK;
// }

// comm_status_t adbms_gpio_spi_communication(cell_asic_ctx_t *asic_ctx) {
//   // TODO
//   return COMM_OK;
// }

// comm_status_t adbms_set_dtrng_dcto_value(cell_asic_ctx_t *asic_ctx) {
//   // TODO
//   return COMM_OK;
// }

// comm_status_t adbms_run_osc_mismatch_self_test(cell_asic_ctx_t *asic_ctx) {
//   // TODO
//   return COMM_OK;
// }

// comm_status_t adbms_run_thermal_shutdown_self_test(cell_asic_ctx_t *asic_ctx)
// {
//   // TODO
//   return COMM_OK;
// }

// comm_status_t
// adbms_run_post(cell_asic_ctx_t *asic_ctx) {
//   // TODO
//   return COMM_OK;
// }

// comm_status_t adbms_run_fuse_ed_self_test(cell_asic_ctx_t *asic_ctx) {
//   // TODO
//   return COMM_OK;
// }

// comm_status_t adbms_run_fuse_med_self_test(cell_asic_ctx_t *asic_ctx) {
//   // TODO
//   return COMM_OK;
// }

// comm_status_t adbms_run_tmodchk_self_test(cell_asic_ctx_t *asic_ctx) {
//   // TODO
//   return COMM_OK;
// }

// comm_status_t
// adbms_check_latent_fault_csflt_status_bits(cell_asic_ctx_t *asic_ctx) {
//   // TODO
//   return COMM_OK;
// }

// comm_status_t
// adbms_check_rdstatc_err_fn(cell_asic_ctx_t *asic_ctx) {
//   // TODO
//   return COMM_OK;
// }

// comm_status_t adbms_cell_openwire_test(cell_asic_ctx_t *asic_ctx) {
//   // TODO
//   return COMM_OK;
// }

// comm_status_t adbms_redundant_cell_openwire_test(cell_asic_ctx_t *asic_ctx) {
//   // TODO
//   return COMM_OK;
// }

// comm_status_t adbms_cell_ow_voltage_collect(cell_asic_ctx_t *asic_ctx,
//                                             bms_op_t type,
//                                             open_wire_detect_mode_t
//                                             ow_c_s) {
//   // TODO
//   return COMM_OK;
// }

// comm_status_t adbms_aux_openwire_test(cell_asic_ctx_t *asic_ctx) {
//   // TODO
//   return COMM_OK;
// }

// comm_status_t
// adbms_gpio_pup_voltage_collect(cell_asic_ctx_t *asic_ctx,
//                                        pull_down_current_mode_t pup) {
//   // TODO
//   return COMM_OK;
// }

// comm_status_t
// adbms_open_wire_detection_condition_check(cell_asic_ctx_t *asic_ctx,
//                                          bms_op_t type) {
//   // TODO
//   return COMM_OK;
// }

// comm_status_t adbms_read_rdcvall_voltage(cell_asic_ctx_t *asic_ctx) {
//   // TODO
//   return COMM_OK;
// }

// comm_status_t adbms_read_rdacall_voltage(cell_asic_ctx_t *asic_ctx) {
//   // TODO
//   return COMM_OK;
// }

// comm_status_t adbms_read_rdsall_voltage(cell_asic_ctx_t *asic_ctx) {
//   // TODO
//   return COMM_OK;
// }

// comm_status_t adbms_read_rdfcall_voltage(cell_asic_ctx_t *asic_ctx) {
//   // TODO
//   return COMM_OK;
// }

// comm_status_t adbms_read_rdcsall_voltage(cell_asic_ctx_t *asic_ctx) {
//   // TODO
//   return COMM_OK;
// }

// comm_status_t adbms_read_rdacsall_voltage(cell_asic_ctx_t *asic_ctx) {
//   // TODO
//   return COMM_OK;
// }

// comm_status_t adbms_read_rdasall_voltage(cell_asic_ctx_t *asic_ctx) {
//   // TODO
//   return COMM_OK;
// }
