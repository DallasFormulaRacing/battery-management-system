#include "parse.h"
#include "bms_enums.h"
#include "bms_types.h"

uint16_t set_ov_voltage_threshold(float volt) {
  uint16_t vov_value;
  uint8_t shift_bits = 12;
  volt = (volt - 1.5);
  volt = volt / (16 * 0.000150F);
  vov_value = (uint16_t)(volt + (2U * (1U << (shift_bits - 1U))));
  vov_value &= 0xFFF;
  return vov_value;
}

uint16_t set_uv_voltage_threshold(float volt) {
  uint16_t vuv_value;
  uint8_t shift_bits = 12;
  volt = (volt - 1.5F);
  volt = volt / (16 * 0.000150F);
  vuv_value = (uint16_t)(volt + (2U * (1U << (shift_bits - 1U))));
  vuv_value &= 0xFFFU;
  return vuv_value;
}

uint8_t make_cfg_a_flag(diagnostics_flags_for_x_t flag_d, flag_ctl_t flag) {
  // TODO
}

uint16_t make_cfg_a_gpo(bms_gpo_output_pin_t gpo,
                        bms_gpo_output_pin_state_t stat) {
  // TODO
}

uint16_t make_cfg_b_dcc_bit(discharge_cell_x_ctl_t dcc, dcc_bit_ctl_t dccbit) {
  // TODO
}

void set_cfg_b_discharge_time_out_value(
    cell_asic_ctx_t *ic, discharge_timer_range_t timer_rang,
    discharge_timer_timeout_t timeout_value) {
  // TODO
}

void set_pwm_duty_cycle(cell_asic_ctx_t *ic, pwm_duty_cycle_t duty_cycle) {
  // TODO
}

void parse_cfg_a(cell_asic_ctx_t *ic, uint8_t *data) {
  // TODO
}

void bms_parse_cfg_b(cell_asic_ctx_t *ic, uint8_t *data) {
  // TODO
}

void bms_parse_cfg_(cell_asic_ctx_t *ic, cfg_reg_group_select_t grp,
                    uint8_t *data) {
  // TODO
}

// --- cell & voltage parses ---
void bms_parse_cell(cell_asic_ctx_t *ic, cfg_reg_group_select_t grp,
                    uint8_t *cv_data) {
  // TODO
}

void bms_parse_avg_cell(cell_asic_ctx_t *ic, cfg_reg_group_select_t grp,
                        uint8_t *acv_data) {
  // TODO
}

void bms_parse_s_cell(cell_asic_ctx_t *ic, cfg_reg_group_select_t grp,
                      uint8_t *scv_data) {
  // TODO
}

void bms_parse_f_cell(cell_asic_ctx_t *ic, cfg_reg_group_select_t grp,
                      uint8_t *fcv_data) {
  // TODO
}

void bms_parse_aux(cell_asic_ctx_t *ic, cfg_reg_group_select_t grp,
                   uint8_t *aux_data) {
  // TODO
}

void bms_parse_rednt_aux(cell_asic_ctx_t *ic, cfg_reg_group_select_t grp,
                         uint8_t *raux_data) {
  // TODO
}

// --- status parses ---
void bms_parse_status_a(cell_asic_ctx_t *ic, uint8_t *data) {
  // TODO
}

void bms_parse_status_b(cell_asic_ctx_t *ic, uint8_t *data) {
  // TODO
}

void bms_parse_status_c(cell_asic_ctx_t *ic, uint8_t *data) {
  // TODO
}

void bms_parse_status_d(cell_asic_ctx_t *ic, uint8_t *data) {
  // TODO
}

void bms_parse_status_e(cell_asic_ctx_t *ic, uint8_t *data) {
  // TODO
}

void bms_parse_status_(cell_asic_ctx_t *ic, cfg_reg_group_select_t grp,
                       uint8_t *data) {
  // TODO
}

void bms_parse_comm(cell_asic_ctx_t *ic, uint8_t *data) {
  // TODO
}

// --- pwm parses ---
void bms_parse_pwm_a(cell_asic_ctx_t *ic, uint8_t *data) {
  // TODO
}

void bms_parse_pwm_b(cell_asic_ctx_t *ic, uint8_t *data) {
  // TODO
}

void bms_parse_pwm(cell_asic_ctx_t *ic, cfg_reg_group_select_t grp,
                   uint8_t *data) {
  // TODO
}

// --- create helpers ---
void bms_create_cfg_a(cell_asic_ctx_t *ic) {
  // TODO
}

void bms_create_cfg_b(cell_asic_ctx_t *ic) {
  // TODO
}

void bms_create_clrflag_data(cell_asic_ctx_t *ic) {
  // TODO
}

void bms_create_comm(cell_asic_ctx_t *ic) {
  // TODO
}

void bms_create_pwm_a(cell_asic_ctx_t *ic) {
  // TODO
}

void bms_create_pwm_b(cell_asic_ctx_t *ic) {
  // TODO
}

void bms_parse_sid(cell_asic_ctx_t *ic, uint8_t *data) {
  // TODO
}