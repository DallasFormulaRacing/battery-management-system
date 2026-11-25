#ifndef DATA_H
#define DATA_H

#include "bms_comms.h"
#include "bms_enums.h"
#include "bms_types.h"
#include "command_list.h"
#include "config.h"
#include "parse.h"
#include <stdint.h>

cfg_reg_group_select_t switch_group_cfg(bms_group_select_t group);

aux_reg_group_select_t switch_group_aux(bms_group_select_t group);

pwm_reg_group_select_t switch_group_pwm(bms_group_select_t group);

/**
 * @brief Read one register group from every IC in the chain.
 *
 * Returns COMM_OK, COMM_INVALID_PEC_ERROR_FLAG if any IC's frame failed its
 * PEC (the data is parsed anyway; bms_pec_flag() says which ICs failed), or
 * another error if nothing was read.
 */
comm_status_t bms_read_data(cell_asic_ctx_t *asic_ctx, bms_op_t type,
                            const command_t cmd_arg, bms_group_select_t group);

/**
 * @brief The crc_err field the last read of this register type set on one IC:
 * 1 if that IC's frame failed its PEC. NULL for a type with no field.
 */
uint8_t *bms_pec_flag(cell_asic_ctx_t *ic_ctx, bms_op_t type);

comm_status_t bms_write_data(cell_asic_ctx_t *asic_ctx, bms_op_t type,
                             const command_t cmd_arg, bms_group_select_t group);

#endif
