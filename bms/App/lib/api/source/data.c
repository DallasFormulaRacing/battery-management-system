#include "data.h"
#include "bms_enums.h"
#include "bms_types.h"
#include "comms.h"
#include "config.h"
#include "parse.h"
#include <stdint.h>

static comm_status_t config_a_b(cell_asic_ctx_t *asic_ctx,
                                cfg_reg_group_select_t group);
static void write_to_all_ics(cell_asic_ctx_t *asic_ctx,
                             asic_mailbox_id_select_t mailbox);
static asic_mailbox_t *get_mailbox(cell_asic_ctx_t *asic_ctx,
                                   asic_mailbox_id_select_t id);
static comm_status_t pwm_a_b(cell_asic_ctx_t *asic_ctx,
                             pwm_reg_group_select_t group);

/**
 * @brief Read data from the BMS
 *
 * @param ic_count
 * @param asic_ctx
 * @param type
 * @param data
 * @return comm_status_t
 */
comm_status_t bms_read_data(cell_asic_ctx_t *asic_ctx, bms_op_t type,
                            cfg_reg_group_select_t group, uint8_t *data) {
  // TODO:
  return COMM_ERROR;
}

/**
 * @brief Write data to the BMS
 *
 * @param asic_ctx
 * @param type
 * @param data
 * // FIX: find way to switch between cfg and pwm groups
 * @param group
 * @return comm_status_t
 */
comm_status_t bms_write_data(cell_asic_ctx_t *asic_ctx, bms_op_t type,
                             cfg_reg_group_select_t group, uint8_t *data) {
  // NOTE: *data may not be needed???
  // TODO: figure out what to do with *data

  switch (type) {
  case BMS_REG_CONFIG:
    // FIX: group needs to be switchable
    if (config_a_b(asic_ctx, group) != COMM_OK) {
      return COMM_ERROR;
    };
    return COMM_OK;
    break;
  case BMS_REG_COMM:
    bms_create_comm(asic_ctx);
    write_to_all_ics(asic_ctx, ASIC_MAILBOX_COM);
    return COMM_OK;
    break;
  case BMS_REG_PWM:
    // FIX: group needs to be switchable
    if (pwm_a_b(asic_ctx, group) != COMM_OK) {
      return COMM_ERROR;
    }
    return COMM_OK;
    break;
  case BMS_CMD_CLRFLAG:
    bms_create_clrflag_data(asic_ctx);
    write_to_all_ics(asic_ctx, ASIC_MAILBOX_CLR_FLAG);
    return COMM_OK;
    break;
  default:
    return COMM_INVALID_COMMAND;
    break;
  }
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
    bms_create_pwm_a(asic_ctx);
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
    asic_mailbox_t *mbid = get_mailbox(asic_ctx, mailbox);
    for (uint8_t data = 0; data < data_len; data++) {
      write_buffer[(cic * data_len) + data] = mbid->tx_data_array[data];
    }
  }
}

static asic_mailbox_t *get_mailbox(cell_asic_ctx_t *asic_ctx,
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
