#include "segment.h"
#include "bms_comms.h"
#include "bms_enums.h"
#include "bms_types.h"
#include "command_list.h"
#include "config.h"
#include "data.h"
#include "parse.h"
#include "spi.h"
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <cmsis_os2.h>

#define RETURN_IF_ERROR(status)                                                \
  do {                                                                         \
    comm_status_t _s = (status);                                               \
    if (_s != COMM_OK)                                                         \
      return _s;                                                               \
  } while (0)

typedef struct {
  const uint8_t *cmd;
  bms_group_select_t group;
} group_read_t;

/*
 * Reads each register group in turn, all of them even when one fails, so one
 * bad frame does not leave the other groups stale. Afterwards the crc_err flag
 * for this register type (bms_pec_flag()) is 1 on every IC whose frame failed
 * in any of the groups, or on every IC if a read did not happen at all.
 * Returns COMM_OK, or the last error.
 */
static comm_status_t read_groups(cell_asic_ctx_t *asic_ctx, bms_op_t type,
                                 const group_read_t *reads, uint8_t n) {
  uint8_t failed[NUM_IC_COUNT_CHAIN] = {0};
  const uint8_t ic_count = (asic_ctx->ic_count < NUM_IC_COUNT_CHAIN)
                               ? asic_ctx->ic_count
                               : NUM_IC_COUNT_CHAIN;
  comm_status_t worst = COMM_OK;

  for (uint8_t i = 0U; i < n; i++) {
    const comm_status_t st =
        bms_read_data(asic_ctx, type, reads[i].cmd, reads[i].group);
    if (st != COMM_OK) {
      worst = st;
    }
    for (uint8_t ic = 0U; ic < ic_count; ic++) {
      const uint8_t *flag = bms_pec_flag(&asic_ctx[ic], type);
      const bool not_read = (st != COMM_OK && st != COMM_INVALID_PEC_ERROR_FLAG);
      if (not_read || (flag != NULL && *flag != 0U)) {
        failed[ic] = 1U;
      }
    }
  }
  for (uint8_t ic = 0U; ic < ic_count; ic++) {
    uint8_t *flag = bms_pec_flag(&asic_ctx[ic], type);
    if (flag != NULL) {
      *flag = failed[ic];
    }
  }
  return worst;
}

#define GROUP_COUNT(a) ((uint8_t)(sizeof(a) / sizeof((a)[0])))

comm_status_t adbms_init_config(cell_asic_ctx_t *asic_ctx) {
  bms_cfg_reg_a_t *cfg_a; // table 102
  bms_cfg_reg_b_t *cfg_b; // table 103
  for (uint8_t ic = 0; ic < asic_ctx->ic_count; ic++) {
    cfg_a = &asic_ctx[ic].tx_cfg_a;
    cfg_b = &asic_ctx[ic].tx_cfg_b;

    cfg_a->REFON = POWER_UP;
    cfg_a->GPIOx = 0x3FF; // All GPIO pull down off (10 1s)
    cfg_a->FC = 0x4; // Sets IIR to -3dB at 10 Hz for filtered cell readings

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

comm_status_t adbms_start_cell_voltage_measurement(cell_asic_ctx_t *asic_ctx) {

  asic_wakeup(asic_ctx->ic_count);
  spi_adcv_command(g_cell_profile.redundant_measurement_mode,
                   g_cell_profile.continuous_measurement,
                   g_cell_profile.discharge_permit, g_cell_profile.reset_filter,
                   g_cell_profile.ow_mode);
  return COMM_OK;
}

comm_status_t adbms_read_cell_voltages(cell_asic_ctx_t *asic_ctx) {
  static const group_read_t reads[] = {
      {RDCVA, REG_GROUP_A}, {RDCVB, REG_GROUP_B}, {RDCVC, REG_GROUP_C},
      {RDCVD, REG_GROUP_D}, {RDCVE, REG_GROUP_E}, {RDCVF, REG_GROUP_F},
  };

  asic_wakeup(asic_ctx->ic_count);
  return read_groups(asic_ctx, BMS_REG_CELL_VOLT, reads, GROUP_COUNT(reads));
}

comm_status_t
adbms_start_adc_s_voltage_measurement(cell_asic_ctx_t *asic_ctx,
                                      adc_config_t measurement_profile) {

  asic_wakeup(asic_ctx->ic_count);
  spi_adsv_command(measurement_profile.continuous_measurement,
                   measurement_profile.discharge_permit,
                   measurement_profile.ow_mode);
  return COMM_OK;
}

comm_status_t adbms_read_s_voltages(cell_asic_ctx_t *asic_ctx) {
  static const group_read_t reads[] = {
      {RDSVA, REG_GROUP_A}, {RDSVB, REG_GROUP_B}, {RDSVC, REG_GROUP_C},
      {RDSVD, REG_GROUP_D}, {RDSVE, REG_GROUP_E}, {RDSVF, REG_GROUP_F},
  };

  asic_wakeup(asic_ctx->ic_count);
  return read_groups(asic_ctx, BMS_REG_S_VOLT, reads, GROUP_COUNT(reads));
}

comm_status_t
adbms_start_avgcell_voltage_measurement(cell_asic_ctx_t *asic_ctx) {

  asic_wakeup(asic_ctx->ic_count);
  spi_adcv_command(g_cell_profile.redundant_measurement_mode,
                   g_cell_profile.continuous_measurement,
                   g_cell_profile.discharge_permit, g_cell_profile.reset_filter,
                   g_cell_profile.ow_mode);
  return COMM_OK;
}

comm_status_t adbms_read_avgcell_voltages(cell_asic_ctx_t *asic_ctx) {
  static const group_read_t reads[] = {
      {RDACA, REG_GROUP_A}, {RDACB, REG_GROUP_B}, {RDACC, REG_GROUP_C},
      {RDACD, REG_GROUP_D}, {RDACE, REG_GROUP_E}, {RDACF, REG_GROUP_F},
  };

  asic_wakeup(asic_ctx->ic_count);
  return read_groups(asic_ctx, BMS_REG_AVG_CELL_VOLT, reads,
                     GROUP_COUNT(reads));
}

comm_status_t adbms_start_fcell_voltage_measurement(cell_asic_ctx_t *asic_ctx) {

  asic_wakeup(asic_ctx->ic_count);
  spi_adcv_command(g_cell_profile.redundant_measurement_mode,
                   g_cell_profile.continuous_measurement,
                   g_cell_profile.discharge_permit, g_cell_profile.reset_filter,
                   g_cell_profile.ow_mode);
  return COMM_OK;
}

comm_status_t adbms_read_fcell_voltages(cell_asic_ctx_t *asic_ctx) {
  static const group_read_t reads[] = {
      {RDFCA, REG_GROUP_A}, {RDFCB, REG_GROUP_B}, {RDFCC, REG_GROUP_C},
      {RDFCD, REG_GROUP_D}, {RDFCE, REG_GROUP_E}, {RDFCF, REG_GROUP_F},
  };

  asic_wakeup(asic_ctx->ic_count);
  return read_groups(asic_ctx, BMS_REG_FILT_CELL_VOLT, reads,
                     GROUP_COUNT(reads));
}

/**
 * @brief read filtered cell voltages with balancing paused so voltage drop doesn't have 
 * cells read low (4ms delay plus ~1ms per IC in chain )
 */
comm_status_t adbms_read_fcell_voltages_muted(cell_asic_ctx_t *asic_ctx) {
  asic_wakeup(asic_ctx->ic_count);
  bms_send_command(MUTE); 
  osDelay(2);            
  spi_adcv_command(g_cell_profile.redundant_measurement_mode,
                   g_cell_profile.continuous_measurement, DCP_OFF, RSTF_ON,
                   g_cell_profile.ow_mode);
  osDelay(2);             

  comm_status_t status = adbms_read_fcell_voltages(asic_ctx);
  bms_send_command(UNMUTE);
  return status;
}

comm_status_t adbms_start_aux_voltage_measurement(cell_asic_ctx_t *asic_ctx) {

  for (uint8_t ic = 0; ic < asic_ctx->ic_count; ic++) {
    asic_ctx[ic].tx_cfg_a.REFON = POWER_UP;
    asic_ctx[ic].tx_cfg_a.GPIOx = 0X3FF; // All GPIO pull down off
  }
  asic_wakeup(asic_ctx->ic_count);
  bms_write_data(asic_ctx, BMS_REG_CONFIG, WRCFGA, REG_GROUP_A);
  spi_adax_command(g_thermistor_profile.aux_ow_mode,
                   g_thermistor_profile.pull_up_resistor_ow,
                   g_thermistor_profile.channels);
  return COMM_OK;
}

comm_status_t adbms_read_aux_voltages(cell_asic_ctx_t *asic_ctx) {
  static const group_read_t reads[] = {
      {RDAUXA, REG_GROUP_A},
      {RDAUXB, REG_GROUP_B},
      {RDAUXC, REG_GROUP_C},
      {RDAUXD, REG_GROUP_D},
  };

  asic_wakeup(asic_ctx->ic_count);
  return read_groups(asic_ctx, BMS_REG_AUX_VOLT, reads, GROUP_COUNT(reads));
}

comm_status_t adbms_read_aux_open_wire(cell_asic_ctx_t *asic_ctx) {
  static const group_read_t reads[] = {
      {RDAUXA, REG_GROUP_A},
      {RDAUXB, REG_GROUP_B},
      {RDAUXC, REG_GROUP_C},
      {RDAUXD, REG_GROUP_D},
  };

  asic_wakeup(asic_ctx->ic_count);
  spi_adax_command(g_thermistor_open_wire_check_profile.aux_ow_mode,
                   g_thermistor_open_wire_check_profile.pull_up_resistor_ow,
                   g_thermistor_open_wire_check_profile.channels);
  return read_groups(asic_ctx, BMS_REG_AUX_VOLT, reads, GROUP_COUNT(reads));
}

comm_status_t adbms_start_raux_voltage_measurement(cell_asic_ctx_t *asic_ctx) {

  for (uint8_t ic = 0; ic < asic_ctx->ic_count; ic++) {
    asic_ctx[ic].tx_cfg_a.REFON = POWER_UP;
    asic_ctx[ic].tx_cfg_a.GPIOx = 0X3FF; // All GPIO pull down off
  }
  asic_wakeup(asic_ctx->ic_count);
  bms_write_data(asic_ctx, BMS_REG_CONFIG, WRCFGA, REG_GROUP_A);
  spi_adax_command(g_thermistor_profile.aux_ow_mode,
                   g_thermistor_profile.pull_up_resistor_ow,
                   g_thermistor_profile.channels);
  return COMM_OK;
}

/**
 * @brief Read all AUX/Status Registers
 *
 * @param asic_ctx
 * @return comm_status_t
 */
comm_status_t adbms_read_raux_voltages(cell_asic_ctx_t *asic_ctx) {
  static const group_read_t reads[] = {
      {RDRAXA, REG_GROUP_A},
      {RDRAXB, REG_GROUP_B},
      {RDRAXC, REG_GROUP_C},
      {RDRAXD, REG_GROUP_D},
  };

  asic_wakeup(asic_ctx->ic_count);
  return read_groups(asic_ctx, BMS_REG_REDUNDANT_AUX_VOLT, reads,
                     GROUP_COUNT(reads));
}

comm_status_t adbms_read_status_registers(cell_asic_ctx_t *asic_ctx) {

  asic_wakeup(asic_ctx->ic_count);
  bms_write_data(asic_ctx, BMS_REG_CONFIG, WRCFGA, REG_GROUP_A);
  bms_write_data(asic_ctx, BMS_REG_CONFIG, WRCFGB, REG_GROUP_B);

  spi_adax_command(g_thermistor_profile.aux_ow_mode,
                   g_thermistor_profile.pull_up_resistor_ow,
                   g_thermistor_profile.channels);

  spi_adcv_command(g_cell_profile.redundant_measurement_mode,
                   g_cell_profile.continuous_measurement,
                   g_cell_profile.discharge_permit, g_cell_profile.reset_filter,
                   g_cell_profile.ow_mode);

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

/** : Needs to be tested
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
  // PWM channel arrays are 0-based s
  if (cell_number < ADBMS_NUM_PWMA_CHANNELS) {
    asic_ctx[segment_number].pwm_ctl_a.pwm_a_ctl_array[cell_number] =
        duty_cycle;
  } 
  
  else {
    assert(cell_number <
           (ADBMS_NUM_PWMA_CHANNELS + ADBMS_NUM_PWMB_CHANNELS));

    asic_ctx[segment_number]
        .pwm_ctl_b.pwm_b_ctl_array[cell_number - ADBMS_NUM_PWMA_CHANNELS] =
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
  for (uint8_t segment_idx = 0; segment_idx < NUM_IC_COUNT_CHAIN;
       segment_idx++) {
    for (uint8_t cell_idx = 0; cell_idx < ADBMS_NUM_CELLS_PER_IC; cell_idx++) {
      adbms_set_cell_pwm(asic_ctx, cell_idx, segment_idx,
                         PWM_0_0_PERCENT_DUTY_CYCLE);
    }
  }

  return adbms_send_pwm_commands(asic_ctx);
}

/* =============== READ ALL COMMANDS ================== */
#if SINGLEBOARD
comm_status_t adbms_read_rdcvall_voltage(cell_asic_ctx_t *asic_ctx) {
  asic_wakeup(asic_ctx->ic_count);
  spi_adcv_command(g_cell_profile.redundant_measurement_mode,
                   g_cell_profile.continuous_measurement,
                   g_cell_profile.discharge_permit, g_cell_profile.reset_filter,
                   g_cell_profile.ow_mode);

  RETURN_IF_ERROR(
      bms_read_data(asic_ctx, BMS_CMD_RDCVALL, RDCVALL, ALL_REG_GROUPS));

  return COMM_OK;
}

comm_status_t adbms_read_rdfcall_voltage(cell_asic_ctx_t *asic_ctx) {
  asic_wakeup(asic_ctx->ic_count);

  RETURN_IF_ERROR(
      bms_read_data(asic_ctx, BMS_CMD_RDFCALL, RDFCALL, ALL_REG_GROUPS));

  return COMM_OK;
}

comm_status_t adbms_read_rdcsall_voltage(cell_asic_ctx_t *asic_ctx,
                                         open_wire_detect_mode_t ow_mode) {
  asic_wakeup(asic_ctx->ic_count);
  spi_adcv_command(g_cell_profile.redundant_measurement_mode,
                   g_cell_profile.continuous_measurement,
                   g_cell_profile.discharge_permit, g_cell_profile.reset_filter,
                   g_cell_profile.ow_mode);
  spi_adsv_command(g_cell_open_wire_check_profile.continuous_measurement,
                   g_cell_open_wire_check_profile.discharge_permit, ow_mode);

  RETURN_IF_ERROR(
      bms_read_data(asic_ctx, BMS_REG_S_VOLT, RDCSALL, ALL_REG_GROUPS));

  return COMM_OK;
}

comm_status_t adbms_read_rdsall_voltage(cell_asic_ctx_t *asic_ctx,
                                        open_wire_detect_mode_t ow_mode) {
  asic_wakeup(asic_ctx->ic_count);
  spi_adsv_command(g_cell_open_wire_check_profile.continuous_measurement,
                   g_cell_open_wire_check_profile.discharge_permit, ow_mode);
  // NOTE: may need to wait before the snap

  RETURN_IF_ERROR(
      // TODO: clear S-ADCs to ensure that the value is anything but the default
      // of 0x8000, CLRCELL cmd or clear array in struct?
      bms_read_data(asic_ctx, BMS_CMD_RDSALL, RDSALL, ALL_REG_GROUPS));

  return COMM_OK;
}

/**
 * @brief Reads all AUX and Status Registers
 * BMS_CMD_RDASALL,
 * @param asic_ctx
 * @return comm_status_t
 */
comm_status_t adbms_read_rdasall_voltage(cell_asic_ctx_t *asic_ctx) {
  asic_wakeup(asic_ctx->ic_count);
  bms_write_data(asic_ctx, BMS_REG_CONFIG, WRCFGA, REG_GROUP_A);
  bms_write_data(asic_ctx, BMS_REG_CONFIG, WRCFGB, REG_GROUP_B);

  spi_adax_command(g_thermistor_profile.aux_ow_mode,
                   g_thermistor_profile.pull_up_resistor_ow,
                   g_thermistor_profile.channels);

  // NOTE: why is C-ADC command being sent inside of the read command of AUX?
  spi_adcv_command(g_cell_profile.redundant_measurement_mode,
                   g_cell_profile.continuous_measurement,
                   g_cell_profile.discharge_permit, g_cell_profile.reset_filter,
                   g_cell_profile.ow_mode);
  spi_adax2_command(g_thermistor_profile.channels);

  //
  RETURN_IF_ERROR(
      bms_read_data(asic_ctx, BMS_CMD_RDASALL, RDASALL, ALL_REG_GROUPS));
  //
  return COMM_OK;
}

#endif
