#ifndef DATA_READ_HANDLERS
#define DATA_READ_HANDLERS

#include "bms_enums.h"
#include "bms_types.h"
#include "parse.h"
#include <stdint.h>

comm_status_t handle_read_type(bms_op_t type, cell_asic_ctx_t *asic_ctx,
                               bms_group_select_t group,
                               asic_status_buffers_t *status_buffers);

void read_cfg_group(cell_asic_ctx_t *asic_ctx, bms_group_select_t group,
                    asic_status_buffers_t *status_buffers);

void read_cell_voltage(cell_asic_ctx_t *asic_ctx, bms_group_select_t group,
                       asic_status_buffers_t *status_buffers);

void read_avg_cell_voltage(cell_asic_ctx_t *asic_ctx, bms_group_select_t group,
                           asic_status_buffers_t *status_buffers);

void read_s_cell_voltage(cell_asic_ctx_t *asic_ctx, bms_group_select_t group,
                         asic_status_buffers_t *status_buffers);

void read_filtered_cell_voltage(cell_asic_ctx_t *asic_ctx,
                                bms_group_select_t group,
                                asic_status_buffers_t *status_buffers);

void read_aux_voltage(cell_asic_ctx_t *asic_ctx, bms_group_select_t group,
                      asic_status_buffers_t *status_buffers);

void read_rednt_aux_voltage(cell_asic_ctx_t *asic_ctx, bms_group_select_t group,
                            asic_status_buffers_t *status_buffers);

void read_status_select(cell_asic_ctx_t *asic_ctx, bms_group_select_t group,
                        asic_status_buffers_t *status_buffers);

void read_comm(cell_asic_ctx_t *asic_ctx, bms_group_select_t group,
               asic_status_buffers_t *status_buffers);

void read_pwm(cell_asic_ctx_t *asic_ctx, bms_group_select_t group,
              asic_status_buffers_t *status_buffers);

void read_sid(cell_asic_ctx_t *asic_ctx, bms_group_select_t group,
              asic_status_buffers_t *status_buffers);

void read_cell_and_s_cell(cell_asic_ctx_t *asic_ctx, bms_group_select_t group,
                          asic_status_buffers_t *status_buffers);

void read_avg_and_s_cell(cell_asic_ctx_t *asic_ctx, bms_group_select_t group,
                         asic_status_buffers_t *status_buffers);

void read_aux_rednt_aux_status(cell_asic_ctx_t *asic_ctx,
                               bms_group_select_t group,
                               asic_status_buffers_t *status_buffers);

#endif // !DATA_READ_HANDLERS
