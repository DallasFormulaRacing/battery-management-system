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
  uint8_t flag_value;
  if (flag == FLAG_SET) {
    flag_value = (1U << flag_d);
  } else {
    flag_value = (0U << flag_d);
  }
  return flag_value;
}

uint16_t make_cfg_a_gpo(bms_gpo_output_pin_t gpo,
                        bms_gpo_output_pin_state_t stat) {
  uint16_t gpovalue;
  if (stat == GPO_SET) {
    gpovalue = (1U << gpo);
  } else {
    gpovalue = (0U << gpo);
  }
  return gpovalue;
}

uint16_t make_cfg_b_dcc_bit(discharge_cell_x_ctl_t dcc, dcc_bit_ctl_t dccbit) {
  uint16_t dcc_value;
  if (dccbit == DISCHARGE_CELL_BIT_SET) {
    dcc_value = (1U << dcc);
  } else {
    dcc_value = (0U << dcc);
  }
  return dcc_value;
}

void set_cfg_b_discharge_time_out_value(
    cell_asic_ctx_t *asic_ctx, discharge_timer_range_t timer_rang,
    discharge_timer_timeout_t timeout_value) {
  for (uint8_t current_ic_idx = 0; current_ic_idx < asic_ctx->ic_count;
       current_ic_idx++) {
    asic_ctx[current_ic_idx].tx_cfg_b.DTRNG = timer_rang;

    if (timer_rang == RANG_0_TO_63_MIN) {
      asic_ctx[current_ic_idx].tx_cfg_b.DCTO = timeout_value;
    }

    else if (timer_rang == RANG_0_TO_16_8_HR) {
      asic_ctx[current_ic_idx].tx_cfg_b.DCTO = timeout_value;
    }
  }
}

void set_pwm_duty_cycle_target_single(cell_asic_ctx_t *asic_ctx) {
  // TODO stdargs
}

void set_pwm_duty_cycle_all(cell_asic_ctx_t *asic_ctx,
                            pwm_duty_cycle_t duty_cycle) {
  for (uint8_t current_ic_idx = 0; current_ic_idx < asic_ctx->ic_count;
       current_ic_idx++) {
    for (uint8_t current_pwm_channel_idx = 0;
         current_pwm_channel_idx < ADBMS_NUM_PWMA_CHANNELS;
         current_pwm_channel_idx++) {
      asic_ctx[current_ic_idx]
          .pwm_ctl_a.pwm_channel_a_ctl_array[current_pwm_channel_idx] =
          duty_cycle;
    }
    for (uint8_t current_pwm_channel_idx = 0;
         current_pwm_channel_idx < ADBMS_NUM_PWMB_CHANNELS;
         current_pwm_channel_idx++) {
      asic_ctx[current_ic_idx]
          .pwm_ctl_b.pwm_channel_b_ctl_array[current_pwm_channel_idx] =
          duty_cycle;
    }
  }
}

void bms_parse_cfg_a(cell_asic_ctx_t *asic_ctx, uint8_t *data) {
  // TODO these numbers are incorrect
  uint8_t address = 0;
  for (uint8_t curr_ic = 0; curr_ic < asic_ctx->ic_count; curr_ic++) {
    memcpy(&asic_ctx[curr_ic].configa.rx_data_array[0], &data[address],
           RX_DATA);
    address = ((curr_ic + 1) * (RX_DATA));

    asic_ctx[curr_ic].rx_cfg_a.CTH =
        (asic_ctx[curr_ic].configa.rx_data_array[0] & 0x07) >> 0;

    asic_ctx[curr_ic].rx_cfg_a.REFON =
        (asic_ctx[curr_ic].configa.rx_data_array[0] & 0x80) >> 7;

    asic_ctx[curr_ic].rx_cfg_a.FLAG_D =
        (asic_ctx[curr_ic].configa.rx_data_array[1] & 0xFF) >> 0;

    asic_ctx[curr_ic].rx_cfg_a.SOAKON =
        (asic_ctx[curr_ic].configa.rx_data_array[2] & 0x80);

    asic_ctx[curr_ic].rx_cfg_a.OWRNG =
        (asic_ctx[curr_ic].configa.rx_data_array[2] & 0x40) >> 1;

    asic_ctx[curr_ic].rx_cfg_a.OWA =
        (asic_ctx[curr_ic].configa.rx_data_array[2] & 0x38) >> 2;

    asic_ctx[curr_ic].rx_cfg_a.GPIOx =
        (asic_ctx[curr_ic].configa.rx_data_array[3] & 0xFF);

    asic_ctx[curr_ic].rx_cfg_a.SNAP_ST =
        (asic_ctx[curr_ic].configa.rx_data_array[5] & 0x20);

    asic_ctx[curr_ic].rx_cfg_a.MUTE_ST =
        (asic_ctx[curr_ic].configa.rx_data_array[5] & 0x10) >> 1;

    asic_ctx[curr_ic].rx_cfg_a.COMM_BK =
        (asic_ctx[curr_ic].configa.rx_data_array[5] & 0x08) >> 2;

    asic_ctx[curr_ic].rx_cfg_a.FC =
        (asic_ctx[curr_ic].configa.rx_data_array[5] & 0x07) >> 0;
  }
}

void bms_parse_cfg_b(cell_asic_ctx_t *asic_ctx, uint8_t *data) {
  // TODO
}

void bms_parse_cfg_grp(cell_asic_ctx_t *asic_ctx, cfg_reg_group_select_t grp,
                       uint8_t *data) {
  // TODO
}

// --- cell & voltage parses ---
void bms_parse_cell(cell_asic_ctx_t *asic_ctx, cfg_reg_group_select_t grp,
                    uint8_t *cv_data) {
  // TODO
}

void bms_parse_avg_cell(cell_asic_ctx_t *asic_ctx, cfg_reg_group_select_t grp,
                        uint8_t *acv_data) {
  // TODO
}

void bms_parse_s_cell(cell_asic_ctx_t *asic_ctx, cfg_reg_group_select_t grp,
                      uint8_t *scv_data) {
  // TODO
}

void bms_parse_f_cell(cell_asic_ctx_t *asic_ctx, cfg_reg_group_select_t grp,
                      uint8_t *fcv_data) {
  // TODO
}

void bms_parse_aux(cell_asic_ctx_t *asic_ctx, cfg_reg_group_select_t grp,
                   uint8_t *aux_data) {
  // TODO
}

void bms_parse_rednt_aux(cell_asic_ctx_t *asic_ctx, cfg_reg_group_select_t grp,
                         uint8_t *raux_data) {
  // TODO
}

// --- status parses ---
void bms_parse_status_a(cell_asic_ctx_t *asic_ctx, uint8_t *data) {
  // TODO
}

void bms_parse_status_b(cell_asic_ctx_t *asic_ctx, uint8_t *data) {
  // TODO
}

void bms_parse_status_c(cell_asic_ctx_t *asic_ctx, uint8_t *data) {
  // TODO
}

void bms_parse_status_d(cell_asic_ctx_t *asic_ctx, uint8_t *data) {
  // TODO
}

void bms_parse_status_e(cell_asic_ctx_t *asic_ctx, uint8_t *data) {
  // TODO
}

void bms_parse_status_(cell_asic_ctx_t *asic_ctx, cfg_reg_group_select_t grp,
                       uint8_t *data) {
  // TODO
}

void bms_parse_comm(cell_asic_ctx_t *asic_ctx, uint8_t *data) {
  // TODO
}

// --- pwm parses ---
void bms_parse_pwm_a(cell_asic_ctx_t *asic_ctx, uint8_t *data) {
  // TODO
}

void bms_parse_pwm_b(cell_asic_ctx_t *asic_ctx, uint8_t *data) {
  // TODO
}

void bms_parse_pwm(cell_asic_ctx_t *asic_ctx, cfg_reg_group_select_t grp,
                   uint8_t *data) {
  // TODO
}

// --- create helpers ---
void bms_create_cfg_a(cell_asic_ctx_t *asic_ctx) {
  // TODO
}

void bms_create_cfg_b(cell_asic_ctx_t *asic_ctx) {
  // TODO
}

void bms_create_clrflag_data(cell_asic_ctx_t *asic_ctx) {
  // TODO
}

void bms_create_comm(cell_asic_ctx_t *asic_ctx) {
  // TODO
}

void bms_create_pwm_a(cell_asic_ctx_t *asic_ctx) {
  // TODO
}

void bms_create_pwm_b(cell_asic_ctx_t *asic_ctx) {
  // TODO
}

void bms_parse_sid(cell_asic_ctx_t *asic_ctx, uint8_t *data) {
  // TODO
}