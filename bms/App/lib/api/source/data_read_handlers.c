#include "data_read_handlers.h"
#include "bms_enums.h"
#include "bms_types.h"
#include "comms.h"
#include "config.h"
#include "data.h"
#include "parse.h"
#include <stdint.h>

static uint8_t *get_pec(cell_asic_ctx_t *asic_ctx, bms_op_t reg_group);

static void check_crc_errors(cell_asic_ctx_t *asic_ctx, bms_op_t reg_group,
                             asic_status_buffers_t *status_buffers);

typedef void (*read_handlers_t)(cell_asic_ctx_t *asic_ctx,
                                bms_group_select_t group,
                                asic_status_buffers_t *status_buffers);

static const read_handlers_t read_handlers[] = {
    [BMS_REG_CONFIG] = read_cfg_group,
    [BMS_REG_CELL_VOLT] = read_cell_voltage,
    [BMS_REG_AVG_CELL_VOLT] = read_avg_cell_voltage,
    [BMS_REG_S_VOLT] = read_s_cell_voltage,
    [BMS_REG_FILTERED_CELL_VOLT] = read_filtered_cell_voltage,
    [BMS_REG_AUX_VOLT] = read_aux_voltage,
    [BMS_REG_REDUNDANT_AUX_VOLT] = read_rednt_aux_voltage,

    // NOTE: Check this one later, seems odd
    [BMS_REG_STATUS] = read_status_select,
    [BMS_REG_COMM] = read_comm,
    [BMS_REG_PWM] = read_pwm,
    [BMS_REG_SID] = read_sid,

    [BMS_CMD_RDCVALL] = read_cell_voltage,
    [BMS_CMD_RDACALL] = read_avg_cell_voltage,
    [BMS_CMD_RDSALL] = read_s_cell_voltage,
    [BMS_CMD_RDFCALL] = read_filtered_cell_voltage,
    [BMS_CMD_RDCSALL] = read_cell_and_s_cell,
    [BMS_CMD_RDACSALL] = read_avg_and_s_cell,
    [BMS_CMD_RDASALL] = read_aux_rednt_aux_status,

};

comm_status_t handle_read_type(bms_op_t type, cell_asic_ctx_t *asic_ctx,
                               bms_group_select_t group,
                               asic_status_buffers_t *status_buffers) {
  read_handlers[type](asic_ctx, group, status_buffers);
  return COMM_OK;
}

void read_cfg_group(cell_asic_ctx_t *asic_ctx, bms_group_select_t group,
                    asic_status_buffers_t *status_buffers) {
  bms_parse_cfg_group(asic_ctx, switch_group_cfg(group),
                      status_buffers->register_data);
  check_crc_errors(asic_ctx, BMS_REG_CONFIG, status_buffers);
}

void read_cell_voltage(cell_asic_ctx_t *asic_ctx, bms_group_select_t group,
                       asic_status_buffers_t *status_buffers) {
  bms_parse_cell(asic_ctx, switch_group_cfg(group),
                 status_buffers->register_data);
  check_crc_errors(asic_ctx, BMS_REG_CELL_VOLT, status_buffers);
}

void read_avg_cell_voltage(cell_asic_ctx_t *asic_ctx, bms_group_select_t group,
                           asic_status_buffers_t *status_buffers) {
  bms_parse_avg_cell(asic_ctx, switch_group_cfg(group),
                     status_buffers->register_data);
  check_crc_errors(asic_ctx, BMS_REG_AVG_CELL_VOLT, status_buffers);
}

void read_s_cell_voltage(cell_asic_ctx_t *asic_ctx, bms_group_select_t group,
                         asic_status_buffers_t *status_buffers) {
  bms_parse_s_cell(asic_ctx, switch_group_cfg(group),
                   status_buffers->register_data);
  check_crc_errors(asic_ctx, BMS_REG_S_VOLT, status_buffers);
}

void read_filtered_cell_voltage(cell_asic_ctx_t *asic_ctx,
                                bms_group_select_t group,
                                asic_status_buffers_t *status_buffers) {
  bms_parse_f_cell(asic_ctx, switch_group_cfg(group),
                   status_buffers->register_data);
  check_crc_errors(asic_ctx, BMS_REG_FILTERED_CELL_VOLT, status_buffers);
}

void read_aux_voltage(cell_asic_ctx_t *asic_ctx, bms_group_select_t group,
                      asic_status_buffers_t *status_buffers) {
  bms_parse_aux(asic_ctx, switch_group_aux(group),
                status_buffers->register_data);
  check_crc_errors(asic_ctx, BMS_REG_AUX_VOLT, status_buffers);
}

void read_rednt_aux_voltage(cell_asic_ctx_t *asic_ctx, bms_group_select_t group,
                            asic_status_buffers_t *status_buffers) {
  bms_parse_rednt_aux(asic_ctx, switch_group_aux(group),
                      status_buffers->register_data);
  check_crc_errors(asic_ctx, BMS_REG_REDUNDANT_AUX_VOLT, status_buffers);
}

// WARN: Check on this one, something seems off about this
void read_status_select(cell_asic_ctx_t *asic_ctx, bms_group_select_t group,
                        asic_status_buffers_t *status_buffers) {
  bms_parse_status_select(asic_ctx, switch_group_cfg(group),
                          status_buffers->register_data);
  check_crc_errors(asic_ctx, BMS_REG_STATUS, status_buffers);
}

void read_comm(cell_asic_ctx_t *asic_ctx, bms_group_select_t group,
               asic_status_buffers_t *status_buffers) {
  bms_parse_comm(asic_ctx, status_buffers->register_data);
  check_crc_errors(asic_ctx, BMS_REG_COMM, status_buffers);
}

void read_pwm(cell_asic_ctx_t *asic_ctx, bms_group_select_t group,
              asic_status_buffers_t *status_buffers) {
  bms_parse_pwm(asic_ctx, switch_group_pwm(group),
                status_buffers->register_data);
  check_crc_errors(asic_ctx, BMS_REG_PWM, status_buffers);
}

void read_sid(cell_asic_ctx_t *asic_ctx, bms_group_select_t group,
              asic_status_buffers_t *status_buffers) {
  bms_parse_sid(asic_ctx, status_buffers->register_data);
  check_crc_errors(asic_ctx, BMS_REG_SID, status_buffers);
}

void read_cell_and_s_cell(cell_asic_ctx_t *asic_ctx, bms_group_select_t group,
                          asic_status_buffers_t *status_buffers) {
  read_cell_voltage(asic_ctx, group, status_buffers);
  read_s_cell_voltage(asic_ctx, group, status_buffers);
}

void read_avg_and_s_cell(cell_asic_ctx_t *asic_ctx, bms_group_select_t group,
                         asic_status_buffers_t *status_buffers) {
  read_avg_cell_voltage(asic_ctx, group, status_buffers);
  read_s_cell_voltage(asic_ctx, group, status_buffers);
}

void read_aux_rednt_aux_status(cell_asic_ctx_t *asic_ctx,
                               bms_group_select_t group,
                               asic_status_buffers_t *status_buffers) {
  read_aux_voltage(asic_ctx, group, status_buffers);
  read_rednt_aux_voltage(asic_ctx, group, status_buffers);
  read_status_select(asic_ctx, group, status_buffers);
}

static void check_crc_errors(cell_asic_ctx_t *asic_ctx, bms_op_t reg_group,
                             asic_status_buffers_t *status_buffers) {
  // NOTE: There might be a better way for letting the user know if
  // get_mailbox_type returned null or not
  uint8_t *pec = get_pec(asic_ctx, reg_group);
  if (pec == NULL) {
    return;
  }
  for (uint8_t cic = 0; cic < asic_ctx->ic_count; cic++) {
    *pec = status_buffers->pec_error_flags[cic];
    asic_ctx[cic].crc_err.command_counter =
        status_buffers->command_counter[cic];
  }
}

static uint8_t *get_pec(cell_asic_ctx_t *asic_ctx, bms_op_t reg_group) {
  switch (reg_group) {
  case BMS_REG_CONFIG:
    return &asic_ctx->crc_err.config_pec;
  // NOTE: Might need to add pec buffers for s, avg, filtered as well. Right now
  // I will just make all of these fall through to cell case.
  case BMS_REG_CELL_VOLT:
  case BMS_REG_S_VOLT:
  case BMS_REG_AVG_CELL_VOLT:
  case BMS_REG_FILTERED_CELL_VOLT:
    return &asic_ctx->crc_err.cell_pec;
  case BMS_REG_AUX_VOLT:
    return &asic_ctx->crc_err.aux_channel_pec;
  case BMS_REG_REDUNDANT_AUX_VOLT:
    return &asic_ctx->crc_err.rednt_aux_channel_pec;
  case BMS_REG_STATUS:
    return &asic_ctx->crc_err.status_channel_pec;
  case BMS_REG_COMM:
    return &asic_ctx->crc_err.comms_pec;
  case BMS_REG_PWM:
    return &asic_ctx->crc_err.pwm_pec;
  case BMS_REG_SID:
    return &asic_ctx->crc_err.serial_id_pec;
  default:
    return NULL;
  }
}
