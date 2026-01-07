#include "data.h"
#include "bms_enums.h"
#include "bms_types.h"
#include "config.h"
#include "parse.h"
#include <stdint.h>

static uint8_t *get_pec(cell_asic_ctx_t *asic_ctx, bms_op_t reg_group);

static void check_crc_errors(cell_asic_ctx_t *asic_ctx, bms_op_t reg_group,
                             asic_status_buffers_t *status_buffers);

typedef void (*read_handlers_t)(cell_asic_ctx_t *asic_ctx,
                                bms_group_select_t group,
                                asic_status_buffers_t *status_buffers);

static void read_cfg_group(cell_asic_ctx_t *asic_ctx, bms_group_select_t group,
                           asic_status_buffers_t *status_buffers) {
  bms_parse_cfg_group(asic_ctx, switch_group_cfg(group),
                      status_buffers->register_data);
  check_crc_errors(asic_ctx, BMS_REG_CONFIG, status_buffers);
}

static void read_cell_voltage(cell_asic_ctx_t *asic_ctx,
                              bms_group_select_t group,
                              asic_status_buffers_t *status_buffers) {
  bms_parse_cell(asic_ctx, switch_group_cfg(group),
                 status_buffers->register_data);
  check_crc_errors(asic_ctx, BMS_REG_CELL_VOLT, status_buffers);
}

static void read_avg_cell_voltage(cell_asic_ctx_t *asic_ctx,
                                  bms_group_select_t group,
                                  asic_status_buffers_t *status_buffers) {
  bms_parse_avg_cell(asic_ctx, switch_group_cfg(group),
                     status_buffers->register_data);
  check_crc_errors(asic_ctx, BMS_REG_AVG_CELL_VOLT, status_buffers);
}

static void read_s_cell_voltage(cell_asic_ctx_t *asic_ctx,
                                bms_group_select_t group,
                                asic_status_buffers_t *status_buffers) {
  bms_parse_s_cell(asic_ctx, switch_group_cfg(group),
                   status_buffers->register_data);
  check_crc_errors(asic_ctx, BMS_REG_S_VOLT, status_buffers);
}

static void read_filtered_cell_voltage(cell_asic_ctx_t *asic_ctx,
                                       bms_group_select_t group,
                                       asic_status_buffers_t *status_buffers) {
  bms_parse_f_cell(asic_ctx, switch_group_cfg(group),
                   status_buffers->register_data);
  check_crc_errors(asic_ctx, BMS_REG_FILTERED_CELL_VOLT, status_buffers);
}

static void read_aux_voltage(cell_asic_ctx_t *asic_ctx,
                             bms_group_select_t group,
                             asic_status_buffers_t *status_buffers) {
  bms_parse_aux(asic_ctx, switch_group_aux(group),
                status_buffers->register_data);
  check_crc_errors(asic_ctx, BMS_REG_AUX_VOLT, status_buffers);
}

static void read_rednt_aux_voltage(cell_asic_ctx_t *asic_ctx,
                                   bms_group_select_t group,
                                   asic_status_buffers_t *status_buffers) {
  bms_parse_rednt_aux(asic_ctx, switch_group_aux(group),
                      status_buffers->register_data);
  check_crc_errors(asic_ctx, BMS_REG_REDUNDANT_AUX_VOLT, status_buffers);
}

static void read_status_select(cell_asic_ctx_t *asic_ctx,
                               bms_group_select_t group,
                               asic_status_buffers_t *status_buffers) {
  bms_parse_status_select(asic_ctx, switch_group_cfg(group),
                          status_buffers->register_data);
  check_crc_errors(asic_ctx, BMS_REG_STATUS, status_buffers);
}

static void read_comm(cell_asic_ctx_t *asic_ctx, bms_group_select_t group,
                      asic_status_buffers_t *status_buffers) {
  bms_parse_comm(asic_ctx, status_buffers->register_data);
  check_crc_errors(asic_ctx, BMS_REG_COMM, status_buffers);
}

static void read_pwm(cell_asic_ctx_t *asic_ctx, bms_group_select_t group,
                     asic_status_buffers_t *status_buffers) {
  bms_parse_pwm(asic_ctx, switch_group_pwm(group),
                status_buffers->register_data);
  check_crc_errors(asic_ctx, BMS_REG_PWM, status_buffers);
}

static void read_sid(cell_asic_ctx_t *asic_ctx, bms_group_select_t group,
                     asic_status_buffers_t *status_buffers) {
  bms_parse_sid(asic_ctx, status_buffers->register_data);
  check_crc_errors(asic_ctx, BMS_REG_SID, status_buffers);
}

static void read_cell_and_s_cell(cell_asic_ctx_t *asic_ctx,
                                 bms_group_select_t group,
                                 asic_status_buffers_t *status_buffers) {
  read_cell_voltage(asic_ctx, group, status_buffers);
  read_s_cell_voltage(asic_ctx, group, status_buffers);
}

static void read_avg_and_s_cell(cell_asic_ctx_t *asic_ctx,
                                bms_group_select_t group,
                                asic_status_buffers_t *status_buffers) {
  read_avg_cell_voltage(asic_ctx, group, status_buffers);
  read_s_cell_voltage(asic_ctx, group, status_buffers);
}

static void read_aux_rednt_aux_status(cell_asic_ctx_t *asic_ctx,
                                      bms_group_select_t group,
                                      asic_status_buffers_t *status_buffers) {
  read_aux_voltage(asic_ctx, group, status_buffers);
  read_rednt_aux_voltage(asic_ctx, group, status_buffers);
  read_status_select(asic_ctx, group, status_buffers);
}
static const read_handlers_t read_handlers[] = {
    [BMS_REG_CONFIG] = read_cfg_group,
    [BMS_REG_CELL_VOLT] = read_cell_voltage,
    [BMS_REG_AVG_CELL_VOLT] = read_avg_cell_voltage,
    [BMS_REG_S_VOLT] = read_s_cell_voltage,
    [BMS_REG_FILTERED_CELL_VOLT] = read_filtered_cell_voltage,
    [BMS_REG_AUX_VOLT] = read_aux_voltage,
    [BMS_REG_REDUNDANT_AUX_VOLT] = read_rednt_aux_voltage,
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

static const read_handlers_t read_handlers[] = {
    [BMS_REG_CONFIG] = read_cfg_group,
    [BMS_REG_CELL_VOLT] = read_cell_voltage,
    [BMS_REG_AVG_CELL_VOLT] = read_avg_cell_voltage,
    [BMS_REG_S_VOLT] = read_s_cell_voltage,
    [BMS_REG_FILTERED_CELL_VOLT] = read_filtered_cell_voltage,
    [BMS_REG_AUX_VOLT] = read_aux_voltage,
    [BMS_REG_REDUNDANT_AUX_VOLT] = read_rednt_aux_voltage,
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

static comm_status_t config_a_b(cell_asic_ctx_t *asic_ctx,
                                cfg_reg_group_select_t group);
static void write_to_all_ics(cell_asic_ctx_t *asic_ctx,
                             asic_mailbox_id_select_t mailbox);
static asic_mailbox_t *get_mailbox_type(cell_asic_ctx_t *asic_ctx,
                                        asic_mailbox_id_select_t id);
static comm_status_t pwm_a_b(cell_asic_ctx_t *asic_ctx,
                             pwm_reg_group_select_t group);

static comm_status_t get_read_buffer_sizes(cell_asic_ctx_t *asic_ctx,
                                           bms_group_select_t group,
                                           bms_op_t type,
                                           uint16_t *read_buffer_size,
                                           uint8_t *reg_data_size);

static void init_status_buffers(asic_status_buffers_t *status_buffers,
                                uint16_t read_buffer_size);

cfg_reg_group_select_t switch_group_cfg(bms_group_select_t group) {
  switch (group) {
  case ALL_REG_GROUPS:
    return ALL_CFG_REG_GROUPS;
  case REG_GROUP_A:
    return CFG_REG_GROUP_A;
  case REG_GROUP_B:
    return CFG_REG_GROUP_B;
  case REG_GROUP_C:
    return CFG_REG_GROUP_C;
  case REG_GROUP_D:
    return CFG_REG_GROUP_D;
  case REG_GROUP_E:
    return CFG_REG_GROUP_E;
  case REG_GROUP_F:
    return CFG_REG_GROUP_F;
  default:
    return NO_CFG_REG_GROUP;
  };
}

aux_reg_group_select_t switch_group_aux(bms_group_select_t group) {
  switch (group) {
  case REG_GROUP_A:
    return AUX_REG_GROUP_A;
  case REG_GROUP_B:
    return AUX_REG_GROUP_B;
  case REG_GROUP_C:
    return AUX_REG_GROUP_C;
  case REG_GROUP_D:
    return AUX_REG_GROUP_D;
  default:
    return NO_AUX_REG_GROUP;
  };
}

pwm_reg_group_select_t switch_group_pwm(bms_group_select_t group) {
  switch (group) {
  case REG_GROUP_A:
    return PWM_REG_GROUP_A;
  case REG_GROUP_B:
    return PWM_REG_GROUP_B;
  default:
    return NO_PWM_REG_GROUP;
  };
}

/**
 * @brief Read data from the BMS
 *
 * @param asic_ctx
 * @param type
 * @param cmd_arg
 * @param group
 * @return comm_status_t
 */
comm_status_t bms_read_data(cell_asic_ctx_t *asic_ctx, bms_op_t type,
                            const command_t cmd_arg, bms_group_select_t group) {
  uint16_t read_buffer_size;
  uint8_t reg_data_size;

  comm_status_t status = get_read_buffer_sizes(
      asic_ctx, group, type, &read_buffer_size, &reg_data_size);

  if (status != COMM_OK) {
    return COMM_INVALID_COMMAND;
  }

  asic_status_buffers_t status_buffers;
  init_status_buffers(&status_buffers, read_buffer_size);

  // bms_read_register_spi(asic_ctx->ic_count, cmd_arg, &status_buffers,
  //                       reg_data_size);

  handle_read_type(type, asic_ctx, group, &status_buffers);

  return COMM_OK;
}

static void init_status_buffers(asic_status_buffers_t *status_buffers,
                                uint16_t read_buffer_size) {
  uint8_t read_buffer[read_buffer_size];
  uint8_t pec_error[asic_ctx->ic_count];
  uint8_t cmd_count[asic_ctx->ic_count];
  status_buffers->register_data = read_buffer;
  status_buffers->pec_error_flags = pec_error;
  status_buffers->command_counter = cmd_count;
}

static comm_status_t get_read_buffer_sizes(cell_asic_ctx_t *asic_ctx,
                                           bms_group_select_t group,
                                           bms_op_t type,
                                           uint16_t *read_buffer_size,
                                           uint8_t *reg_data_size) {
  switch (group) {
  case ALL_REG_GROUPS:
    switch (type) {
    case BMS_CMD_RDCVALL:
      *read_buffer_size = ADBMS_RDCVALL_FRAME_SIZE;
      *reg_data_size = ADBMS_RDCVALL_FRAME_SIZE;
      break;
    case BMS_CMD_RDSALL:
      *read_buffer_size = ADBMS_RDSALL_FRAME_SIZE;
      *reg_data_size = ADBMS_RDSALL_FRAME_SIZE;
      break;
    case BMS_CMD_RDACALL:
      *read_buffer_size = ADBMS_RDACALL_FRAME_SIZE;
      *reg_data_size = ADBMS_RDACALL_FRAME_SIZE;
      break;
    case BMS_CMD_RDFCALL:
      *read_buffer_size = ADBMS_RDFCALL_FRAME_SIZE;
      *reg_data_size = ADBMS_RDFCALL_FRAME_SIZE;
      break;
    case BMS_CMD_RDCSALL:
      *read_buffer_size = ADBMS_RDCSALL_FRAME_SIZE;
      *reg_data_size = ADBMS_RDCSALL_FRAME_SIZE;
      break;
    case BMS_CMD_RDASALL:
      *read_buffer_size = ADBMS_RDASALL_FRAME_SIZE;
      *reg_data_size = ADBMS_RDASALL_FRAME_SIZE;
      break;
    case BMS_CMD_RDACSALL:
      *read_buffer_size = ADBMS_RDACSALL_FRAME_SIZE;
      *reg_data_size = ADBMS_RDACSALL_FRAME_SIZE;
      break;
    default:
      return COMM_INVALID_COMMAND;
      break;
    }

  default:
    *read_buffer_size = (asic_ctx->ic_count * READ_SIZE);
    *reg_data_size = READ_SIZE;
    return COMM_OK;
  }
}

/**
 * @brief Write data to the BMS
 *
 * @param asic_ctx
 * @param type
 * @param cmd_arg
 * @param group
 * @return comm_status_t
 */

comm_status_t bms_write_data(cell_asic_ctx_t *asic_ctx, bms_op_t type,
                             const command_t cmd_arg,
                             bms_group_select_t group) {
  switch (type) {
  case BMS_REG_CONFIG:
    if (config_a_b(asic_ctx, switch_group_cfg(group)) != COMM_OK) {
      return COMM_ERROR;
    };
    break;
  case BMS_REG_COMM:
    bms_create_comm(asic_ctx);
    write_to_all_ics(asic_ctx, ASIC_MAILBOX_COM);
    break;
  case BMS_REG_PWM:
    if (pwm_a_b(asic_ctx, switch_group_pwm(group)) != COMM_OK) {
      return COMM_ERROR;
    }
    break;
  case BMS_CMD_CLRFLAG:
    bms_create_clrflag_data(asic_ctx);
    write_to_all_ics(asic_ctx, ASIC_MAILBOX_CLR_FLAG);
    break;
  default:
    return COMM_INVALID_COMMAND;
    break;
  }

  // asic_wakeup(asic_ctx->ic_count);
  // bms_write_register_spi(asic_ctx->ic_count, cmd_arg, write_buffer,
  // WRITE_SIZE);
  return COMM_OK;
}

static comm_status_t config_a_b(cell_asic_ctx_t *asic_ctx,
                                cfg_reg_group_select_t group) {
  switch (group) {
  case CFG_REG_GROUP_A:
    bms_create_cfg_a(asic_ctx);
    write_to_all_ics(asic_ctx, ASIC_MAILBOX_CONFIG_A);
    return COMM_OK;
    break;
  case CFG_REG_GROUP_B:
    bms_create_cfg_b(asic_ctx);
    write_to_all_ics(asic_ctx, ASIC_MAILBOX_CONFIG_B);
    return COMM_OK;
    break;
  default:
    return COMM_INVALID_COMMAND;
    break;
  }
}

static comm_status_t pwm_a_b(cell_asic_ctx_t *asic_ctx,
                             pwm_reg_group_select_t group) {
  switch (group) {
  case PWM_REG_GROUP_A:
    bms_create_pwm_a(asic_ctx);
    write_to_all_ics(asic_ctx, ASIC_MAILBOX_PWM_A);
    return COMM_OK;
    break;
  case PWM_REG_GROUP_B:
    bms_create_pwm_b(asic_ctx);
    write_to_all_ics(asic_ctx, ASIC_MAILBOX_PWM_B);
    return COMM_OK;
    break;
  default:
    return COMM_INVALID_COMMAND;
    break;
  }
}

static void write_to_all_ics(cell_asic_ctx_t *asic_ctx,
                             asic_mailbox_id_select_t mailbox) {
  uint8_t data_len = ADBMS_TX_FRAME_BYTES;
  for (uint8_t cic = 0; cic < asic_ctx->ic_count; cic++) {
    asic_mailbox_t *mailbox_id = get_mailbox_type(asic_ctx, mailbox);
    // NOTE: There might be a better way for letting the user know if
    // get_mailbox_type returned null or not
    if (mailbox_id == NULL) {
      return;
    }
    for (uint8_t data = 0; data < data_len; data++) {
      write_buffer[(cic * data_len) + data] = mailbox_id->tx_data_array[data];
    }
  }
}

static asic_mailbox_t *get_mailbox_type(cell_asic_ctx_t *asic_ctx,
                                        asic_mailbox_id_select_t id) {
  switch (id) {
  case ASIC_MAILBOX_CONFIG_A:
    return &asic_ctx->config_a;
  case ASIC_MAILBOX_CONFIG_B:
    return &asic_ctx->config_b;
  case ASIC_MAILBOX_CLR_FLAG:
    return &asic_ctx->clrflag;
  case ASIC_MAILBOX_STAT:
    return &asic_ctx->stat;
  case ASIC_MAILBOX_COM:
    return &asic_ctx->com;
  case ASIC_MAILBOX_PWM_A:
    return &asic_ctx->pwm_a;
  case ASIC_MAILBOX_PWM_B:
    return &asic_ctx->pwm_b;
  case ASIC_MAILBOX_RSID:
    return &asic_ctx->rsid;
  default:
    return NULL;
  }
}
