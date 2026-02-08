#ifndef DATA_H
#define DATA_H

#include "bms_comms.h"
#include "bms_enums.h"
#include "bms_types.h"
#include "command_list.h"
#include <stdint.h>

cfg_reg_group_select_t switch_group_cfg(bms_group_select_t group);

aux_reg_group_select_t switch_group_aux(bms_group_select_t group);

pwm_reg_group_select_t switch_group_pwm(bms_group_select_t group);

comm_status_t bms_read_data(cell_asic_ctx_t *asic_ctx, bms_op_t type,
                            const command_t cmd_arg, bms_group_select_t group);

comm_status_t bms_write_data(cell_asic_ctx_t *asic_ctx, bms_op_t type,
                             const command_t cmd_arg, bms_group_select_t group);

#endif
