#ifndef parse__H
#define parse__H

#include "bms_enums.h"
#include "bms_types.h"

typedef struct {
  uint8_t ic_index;
  uint8_t cell_index;
  uint8_t pwm_channel_index;
  pwm_duty_cycle_t duty_cycle;
} pwm_target_cell_ctl_t;

uint16_t set_ov_voltage_threshold(float volt);
uint16_t set_uv_voltage_threshold(float volt);
uint8_t make_cfg_a_flag(diagnostics_flags_for_x_t flag_d, flag_ctl_t flag);
uint16_t make_cfg_a_gpo(bms_gpo_output_pin_t gpo,
                        bms_gpo_output_pin_state_t stat);
uint16_t make_cfg_b_dcc_bit(discharge_cell_x_ctl_t dcc, dcc_bit_ctl_t dccbit);
void set_cfg_b_discharge_time_out_value(
    cell_asic_ctx_t *asic_ctx, discharge_timer_range_t timer_rang,
    discharge_timer_timeout_t timeout_value);
void set_pwm_duty_cycle_all(cell_asic_ctx_t *asic_ctx,
                            pwm_duty_cycle_t duty_cycle);

void parse_cfg_a(cell_asic_ctx_t *asic_ctx, uint8_t *data);
void bms_parse_cfg_b(cell_asic_ctx_t *asic_ctx, uint8_t *data);
void bms_parse_cfg_(cell_asic_ctx_t *asic_ctx, cfg_reg_group_select_t grp,
                    uint8_t *data);

// --- cell & voltage parses ---
void bms_parse_cell(cell_asic_ctx_t *asic_ctx, cfg_reg_group_select_t grp,
                    uint8_t *cv_data);
void bms_parse_avg_cell(cell_asic_ctx_t *asic_ctx, cfg_reg_group_select_t grp,
                        uint8_t *acv_data);
void bms_parse_s_cell(cell_asic_ctx_t *asic_ctx, cfg_reg_group_select_t grp,
                      uint8_t *scv_data);
void bms_parse_f_cell(cell_asic_ctx_t *asic_ctx, cfg_reg_group_select_t grp,
                      uint8_t *fcv_data);

void bms_parse_aux(cell_asic_ctx_t *asic_ctx, cfg_reg_group_select_t grp,
                   uint8_t *aux_data);
void bms_parse_rednt_aux(cell_asic_ctx_t *asic_ctx, cfg_reg_group_select_t grp,
                         uint8_t *raux_data);

// --- status parses ---
void bms_parse_status_a(cell_asic_ctx_t *asic_ctx, uint8_t *data);
void bms_parse_status_b(cell_asic_ctx_t *asic_ctx, uint8_t *data);
void bms_parse_status_c(cell_asic_ctx_t *asic_ctx, uint8_t *data);
void bms_parse_status_d(cell_asic_ctx_t *asic_ctx, uint8_t *data);
void bms_parse_status_e(cell_asic_ctx_t *asic_ctx, uint8_t *data);
void bms_parse_status_(cell_asic_ctx_t *asic_ctx, cfg_reg_group_select_t grp,
                       uint8_t *data);

void bms_parse_comm(cell_asic_ctx_t *asic_ctx, uint8_t *data);

// --- pwm parses ---
void bms_parse_pwm_a(cell_asic_ctx_t *asic_ctx, uint8_t *data);
void bms_parse_pwm_b(cell_asic_ctx_t *asic_ctx, uint8_t *data);
void bms_parse_pwm(cell_asic_ctx_t *asic_ctx, cfg_reg_group_select_t grp,
                   uint8_t *data);

// --- create helpers ---
void bms_create_cfg_a(cell_asic_ctx_t *asic_ctx);
void bms_create_cfg_b(cell_asic_ctx_t *asic_ctx);
void bms_create_clrflag_data(cell_asic_ctx_t *asic_ctx);
void bms_create_comm(cell_asic_ctx_t *asic_ctx);
void bms_create_pwm_a(cell_asic_ctx_t *asic_ctx);
void bms_create_pwm_b(cell_asic_ctx_t *asic_ctx);

void bms_parse_sid(cell_asic_ctx_t *asic_ctx, uint8_t *data);

#endif