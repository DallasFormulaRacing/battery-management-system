#include "parse.h"

// ! This file needs to be verified

static inline void parse_cell_register(cell_asic_ctx_t *asic_ctx,
                                       cfg_reg_group_select_t group,
                                       const uint8_t *data, uint8_t curr_ic,
                                       parse_measurement_type_t mtype);

voltage_readings_t convert_voltage_machine_readable(float voltage) {
  voltage = (voltage - 1.5F);
  voltage = voltage / (0.000150F);
  return (voltage_readings_t)voltage;
}

uint16_t set_ov_voltage_threshold(float volt) {
  uint16_t vov_value;
  uint8_t shift_bits = 12;
  volt = (volt - 1.5F);
  volt = volt / (16 * 0.000150F);
  vov_value = ((uint16_t)volt + (2U * (1U << (shift_bits - 1U))));
  vov_value &= 0xFFF;
  return vov_value;
}

uint16_t set_uv_voltage_threshold(float volt) {
  uint16_t vuv_value;
  uint8_t shift_bits = 12;
  volt = (volt - 1.5F);
  volt = volt / (16 * 0.000150F);
  vuv_value = ((uint16_t)volt + (2U * (1U << (shift_bits - 1U))));
  vuv_value &= 0xFFFU;
  return vuv_value;
}

float convert_voltage_human_readable(int16_t voltage) {
  return ((float)voltage * 0.000150F) + 1.5F;
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

void set_cfg_b_discharge_time_out_value(cell_asic_ctx_t *asic_ctx,
                                        discharge_timer_range_t range,
                                        discharge_timer_timeout_t value) {

  bms_cfg_reg_b_t *cfg_b;
  for (uint8_t current_ic_idx = 0; current_ic_idx < asic_ctx->ic_count;
       current_ic_idx++) {
    cfg_b = &asic_ctx[current_ic_idx].tx_cfg_b;
    cfg_b->DTRNG = range;

    if (range == RANG_0_TO_63_MIN || range == RANG_0_TO_16_8_HR) {
      cfg_b->DCTO = value;
    }
  }
}

void bms_parse_cfg_a(cell_asic_ctx_t *asic_ctx, uint8_t *data) {
  uint8_t address = 0;
  bms_cfg_reg_a_t *cfg_a;
  asic_mailbox_t *mailbox;
  for (uint8_t curr_ic = 0; curr_ic < asic_ctx->ic_count; curr_ic++) {
    cfg_a = &asic_ctx[curr_ic].rx_cfg_a; // nickname
    mailbox = &asic_ctx[curr_ic].config_a_mb;
    memcpy(&mailbox->rx_data_array[0], &data[address], ADBMS_RX_FRAME_BYTES);
    address = ((curr_ic + 1) * (ADBMS_RX_FRAME_BYTES));

    cfg_a->CTH = (mailbox->rx_data_array[0] & 0x07) >> 0;
    cfg_a->REFON = (mailbox->rx_data_array[0] & 0x80) >> 7;
    cfg_a->FLAG_D = (mailbox->rx_data_array[1] & 0xFF) >> 0;
    cfg_a->SOAKON = (mailbox->rx_data_array[2] & 0x80) >> 7;
    cfg_a->OWRNG = (mailbox->rx_data_array[2] & 0x40) >> 6;
    cfg_a->OWA = (mailbox->rx_data_array[2] & 0x38) >> 3;
    cfg_a->GPIOx = (mailbox->rx_data_array[3] & 0xFF) |
                   ((mailbox->rx_data_array[4] & 0x03) << 8);
    cfg_a->SNAP_ST = (mailbox->rx_data_array[5] & 0x20) >> 5;
    cfg_a->MUTE_ST = (mailbox->rx_data_array[5] & 0x10) >> 4;
    cfg_a->COMM_BK = (mailbox->rx_data_array[5] & 0x08) >> 3;
    cfg_a->FC = (mailbox->rx_data_array[5] & 0x07) >> 0;
  }
}

void bms_parse_cfg_b(cell_asic_ctx_t *asic_ctx, uint8_t *data) {
  uint8_t address = 0;
  bms_cfg_reg_b_t *cfg_b;
  asic_mailbox_t *mailbox;
  for (uint8_t curr_ic = 0; curr_ic < asic_ctx->ic_count; curr_ic++) {
    cfg_b = &asic_ctx[curr_ic].rx_cfg_b; // nickname
    mailbox = &asic_ctx[curr_ic].config_b_mb;
    memcpy(&mailbox->rx_data_array[0], &data[address], ADBMS_RX_FRAME_BYTES);

    address = ((curr_ic + 1) * (ADBMS_RX_FRAME_BYTES));

    cfg_b->VUV = ((mailbox->rx_data_array[0]) |
                  ((mailbox->rx_data_array[1] & 0x0F) << 8));
    cfg_b->VOV = (mailbox->rx_data_array[2] << 4) +
                 ((mailbox->rx_data_array[1] & 0xF0) >> 4);
    cfg_b->DTMEN = (((mailbox->rx_data_array[3] & 0x80) >> 7));
    cfg_b->DTRNG = ((mailbox->rx_data_array[3] & 0x40) >> 6);
    cfg_b->DCTO = ((mailbox->rx_data_array[3] & 0x3F));
    cfg_b->DCCx = ((mailbox->rx_data_array[4]) |
                   ((mailbox->rx_data_array[5] & 0xFF) << 8));
  }
}

void bms_parse_cfg_group(cell_asic_ctx_t *asic_ctx,
                         cfg_reg_group_select_t group, uint8_t *data) {
  switch (group) {
  case CFG_REG_GROUP_A:
    bms_parse_cfg_a(asic_ctx, data);
    break;

  case CFG_REG_GROUP_B:
    bms_parse_cfg_b(asic_ctx, data);
    break;

  default:
    break;
  }
}

static inline void parse_aux_register(cell_asic_ctx_t *asic_ctx,
                                      aux_reg_group_select_t group,
                                      const uint8_t *data, uint8_t curr_ic,
                                      parse_adc_measurement_type_t atype) {
  voltage_readings_t *readings;

  switch (atype) {
  case MEASURE_AUX_ADC:
    readings = &asic_ctx[curr_ic].aux.aux_voltages_array[0];
    break;
  case MEASURE_AUX_ADC_REDNT:
    readings = &asic_ctx[curr_ic].rednt_aux.rednt_aux_voltages_array[0];
    break;
  }
  switch (group) {
  case AUX_REG_GROUP_A: /* Aux Register group A */
    readings[0] = (voltage_readings_t)(data[0] + (data[1] << 8));
    readings[1] = (voltage_readings_t)(data[2] + (data[3] << 8));
    readings[2] = (voltage_readings_t)(data[4] + (data[5] << 8));
    break;

  case AUX_REG_GROUP_B: /* Aux Register group B */
    readings[3] = (voltage_readings_t)(data[0] + (data[1] << 8));
    readings[4] = (voltage_readings_t)(data[2] + (data[3] << 8));
    readings[5] = (voltage_readings_t)(data[4] + (data[5] << 8));
    break;

  case AUX_REG_GROUP_C: /* Aux Register group C */
    readings[6] = (voltage_readings_t)(data[0] + (data[1] << 8));
    readings[7] = (voltage_readings_t)(data[2] + (data[3] << 8));
    readings[8] = (voltage_readings_t)(data[4] + (data[5] << 8));
    break;

  case AUX_REG_GROUP_D: /* Aux Register group D */
    readings[9] = (voltage_readings_t)(data[0] + (data[1] << 8));
    readings[10] = (voltage_readings_t)(data[2] + (data[3] << 8));
    readings[11] = (voltage_readings_t)(data[4] + (data[5] << 8));
    break;

  case NO_AUX_REG_GROUP: /* Aux Register group ALL */
    readings[0] = (voltage_readings_t)(data[0] + (data[1] << 8));
    readings[1] = (voltage_readings_t)(data[2] + (data[3] << 8));
    readings[2] = (voltage_readings_t)(data[4] + (data[5] << 8));
    readings[3] = (voltage_readings_t)(data[6] + (data[7] << 8));
    readings[4] = (voltage_readings_t)(data[8] + (data[9] << 8));
    readings[5] = (voltage_readings_t)(data[10] + (data[11] << 8));
    readings[6] = (voltage_readings_t)(data[12] + (data[13] << 8));
    readings[7] = (voltage_readings_t)(data[14] + (data[15] << 8));
    readings[8] = (voltage_readings_t)(data[16] + (data[17] << 8));
    readings[9] = (voltage_readings_t)(data[18] + (data[19] << 8));
    readings[10] = (voltage_readings_t)(data[20] + (data[21] << 8));
    readings[11] = (voltage_readings_t)(data[22] + (data[23] << 8));
    break;

  default:
    break;
  }
}

static inline void parse_cell_register(cell_asic_ctx_t *asic_ctx,
                                       cfg_reg_group_select_t group,
                                       const uint8_t *data, uint8_t curr_ic,
                                       parse_measurement_type_t mtype) {
  voltage_readings_t *readings;

  switch (mtype) {
  case MEASURE_RAW:
    readings = &asic_ctx[curr_ic].cell.cell_voltages_array[0];
    break;
  case MEASURE_FILTERED:
    readings = &asic_ctx[curr_ic].filtered_cell.filtered_cell_voltages_array[0];
    break;
  case MEASURE_AVG:
    readings = &asic_ctx[curr_ic].avg_cell.avg_cell_voltages_array[0];
    break;
  case MEASURE_S:
    readings = &asic_ctx[curr_ic].s_cell.s_cell_voltages_array[0];
    break;
  default:
    // WARN: Will only throw error at runtime
    assert(0 && "Unexpected mtype in parse_cell_register.");
    break;
  }

  switch (group) {
  case CFG_REG_GROUP_A: /* Cell Register group A */
    readings[0] = (voltage_readings_t)(data[0] + (data[1] << 8));
    readings[1] = (voltage_readings_t)(data[2] + (data[3] << 8));
    readings[2] = (voltage_readings_t)(data[4] + (data[5] << 8));
    break;

  case CFG_REG_GROUP_B: /* Cell Register group B */
    readings[3] = (voltage_readings_t)(data[0] + (data[1] << 8));
    readings[4] = (voltage_readings_t)(data[2] + (data[3] << 8));
    readings[5] = (voltage_readings_t)(data[4] + (data[5] << 8));
    break;

  case CFG_REG_GROUP_C: /* Cell Register group C */
    readings[6] = (voltage_readings_t)(data[0] + (data[1] << 8));
    readings[7] = (voltage_readings_t)(data[2] + (data[3] << 8));
    readings[8] = (voltage_readings_t)(data[4] + (data[5] << 8));
    break;

  case CFG_REG_GROUP_D: /* Cell Register group D */
    readings[9] = (voltage_readings_t)(data[0] + (data[1] << 8));
    readings[10] = (voltage_readings_t)(data[2] + (data[3] << 8));
    readings[11] = (voltage_readings_t)(data[4] + (data[5] << 8));
    break;

  case CFG_REG_GROUP_E: /* Cell Register group E */
    readings[12] = (voltage_readings_t)(data[0] + (data[1] << 8));
    readings[13] = (voltage_readings_t)(data[2] + (data[3] << 8));
    readings[14] = (voltage_readings_t)(data[4] + (data[5] << 8));
    break;

  case CFG_REG_GROUP_F: /* Cell Register group F */
    readings[15] = (voltage_readings_t)(data[0] + (data[1] << 8));
    break;

  case ALL_CFG_REG_GROUPS: /* Cell Register group ALL */
    readings[0] = (voltage_readings_t)(data[0] + (data[1] << 8));
    readings[1] = (voltage_readings_t)(data[2] + (data[3] << 8));
    readings[2] = (voltage_readings_t)(data[4] + (data[5] << 8));
    readings[3] = (voltage_readings_t)(data[6] + (data[7] << 8));
    readings[4] = (voltage_readings_t)(data[8] + (data[9] << 8));
    readings[5] = (voltage_readings_t)(data[10] + (data[11] << 8));
    readings[6] = (voltage_readings_t)(data[12] + (data[13] << 8));
    readings[7] = (voltage_readings_t)(data[14] + (data[15] << 8));
    readings[8] = (voltage_readings_t)(data[16] + (data[17] << 8));
    readings[9] = (voltage_readings_t)(data[18] + (data[19] << 8));
    readings[10] = (voltage_readings_t)(data[20] + (data[21] << 8));
    readings[11] = (voltage_readings_t)(data[22] + (data[23] << 8));
    readings[12] = (voltage_readings_t)(data[24] + (data[25] << 8));
    readings[13] = (voltage_readings_t)(data[26] + (data[27] << 8));
    readings[14] = (voltage_readings_t)(data[28] + (data[29] << 8));
    readings[15] = (voltage_readings_t)(data[30] + (data[31] << 8));
    break;

  default:
    break;
  }
}

// --- cell & voltage parses ---
void bms_parse_cell(cell_asic_ctx_t *asic_ctx, cfg_reg_group_select_t group,
                    uint8_t *cv_data) {
  uint8_t data_size;
  uint8_t address = 0;
  if (group == ALL_CFG_REG_GROUPS) {
    data_size = ADBMS_RDCVALL_FRAME_SIZE;
  } else {
    data_size = ADBMS_RX_FRAME_BYTES;
  }
  // data = (uint8_t *)calloc(data_size, sizeof(uint8_t));
  uint8_t data[data_size];

  for (uint8_t curr_ic = 0; curr_ic < asic_ctx->ic_count; curr_ic++) {
    memcpy(&data[0], &cv_data[address], data_size);
    address = ((curr_ic + 1) * (data_size));
    parse_cell_register(asic_ctx, group, data, curr_ic, MEASURE_RAW);
  }
}

void bms_parse_avg_cell(cell_asic_ctx_t *asic_ctx, cfg_reg_group_select_t group,
                        uint8_t *acv_data) {
  uint8_t data_size;
  uint8_t address = 0;
  if (group == ALL_CFG_REG_GROUPS) {
    data_size = ADBMS_RDACALL_FRAME_SIZE;
  } else {
    data_size = ADBMS_RX_FRAME_BYTES;
  }
  // data = (uint8_t *)calloc(data_size, sizeof(uint8_t));
  uint8_t data[data_size];

  for (uint8_t curr_ic = 0; curr_ic < asic_ctx->ic_count; curr_ic++) {
    memcpy(&data[0], &acv_data[address], data_size);
    address = ((curr_ic + 1) * (data_size));
    parse_cell_register(asic_ctx, group, data, curr_ic, MEASURE_AVG);
  }
}

void bms_parse_s_cell(cell_asic_ctx_t *asic_ctx, cfg_reg_group_select_t group,
                      uint8_t *scv_data) {
  uint8_t data_size;
  uint8_t address = 0;
  if (group == ALL_CFG_REG_GROUPS) {
    data_size = ADBMS_RDSALL_FRAME_SIZE;
  } else {
    data_size = ADBMS_RX_FRAME_BYTES;
  }
  // data = (uint8_t *)calloc(data_size, sizeof(uint8_t));
  uint8_t data[data_size];

  for (uint8_t curr_ic = 0; curr_ic < asic_ctx->ic_count; curr_ic++) {
    memcpy(&data[0], &scv_data[address], data_size);
    address = ((curr_ic + 1) * (data_size));
    parse_cell_register(asic_ctx, group, data, curr_ic, MEASURE_S);
  }
}

void bms_parse_f_cell(cell_asic_ctx_t *asic_ctx, cfg_reg_group_select_t group,
                      uint8_t *fcv_data) {
  uint8_t data_size;
  uint8_t address = 0;
  if (group == ALL_CFG_REG_GROUPS) {
    data_size = ADBMS_RDFCALL_FRAME_SIZE;
  } else {
    data_size = ADBMS_RX_FRAME_BYTES;
  }
  // data = (uint8_t *)calloc(data_size, sizeof(uint8_t));
  uint8_t data[data_size];

  for (uint8_t curr_ic = 0; curr_ic < asic_ctx->ic_count; curr_ic++) {
    memcpy(&data[0], &fcv_data[address], data_size);
    address = ((curr_ic + 1) * (data_size));
    parse_cell_register(asic_ctx, group, data, curr_ic, MEASURE_FILTERED);
  }
}

void bms_parse_aux(cell_asic_ctx_t *asic_ctx, aux_reg_group_select_t group,
                   uint8_t *aux_data) {
  uint8_t data_size;
  uint8_t address = 0;
  if (group == NO_AUX_REG_GROUP) {
    data_size = ADBMS_RDASALL_FRAME_SIZE;
  } else {
    data_size = ADBMS_RX_FRAME_BYTES;
  }
  // data = (uint8_t *)calloc(data_size, sizeof(uint8_t));
  uint8_t data[data_size];

  for (uint8_t curr_ic = 0; curr_ic < asic_ctx->ic_count; curr_ic++) {
    memcpy(&data[0], &aux_data[address], data_size);
    address = ((curr_ic + 1) * (data_size));
    parse_aux_register(asic_ctx, group, data, curr_ic, MEASURE_AUX_ADC);
  }
}

void bms_parse_rednt_aux(cell_asic_ctx_t *asic_ctx,
                         aux_reg_group_select_t group, uint8_t *raux_data) {
  uint8_t data_size;
  uint8_t address = 0;
  if (group == NO_AUX_REG_GROUP) {
    data_size = ADBMS_RDASALL_FRAME_SIZE;
  } else {
    data_size = ADBMS_RX_FRAME_BYTES;
  }
  // data = (uint8_t *)calloc(data_size, sizeof(uint8_t));
  uint8_t data[data_size];

  for (uint8_t curr_ic = 0; curr_ic < asic_ctx->ic_count; curr_ic++) {
    memcpy(&data[0], &raux_data[address], data_size);
    address = ((curr_ic + 1) * (data_size));
    parse_aux_register(asic_ctx, group, data, curr_ic, MEASURE_AUX_ADC_REDNT);
  }
}

// --- status parses ---
void bms_parse_status_a(cell_asic_ctx_t *asic_ctx, uint8_t *data) {
  uint8_t address = 0;
  bms_stat_reg_a_t *status;
  asic_mailbox_t *mailbox;

  for (uint8_t curr_ic = 0; curr_ic < asic_ctx->ic_count; curr_ic++) {
    status = &asic_ctx[curr_ic].stat_a;
    mailbox = &asic_ctx[curr_ic].stat_mb;
    memcpy(&mailbox->rx_data_array, &data[address], ADBMS_RX_FRAME_BYTES);

    address = ((curr_ic + 1) * (ADBMS_RX_FRAME_BYTES));

    status->VREF2 =
        (mailbox->rx_data_array[0] | (mailbox->rx_data_array[1] << 8));

    status->ITMP =
        (mailbox->rx_data_array[2] | (mailbox->rx_data_array[3] << 8));

    status->VREF3 =
        (mailbox->rx_data_array[4] | (mailbox->rx_data_array[5] << 8));
  }
}

void bms_parse_status_b(cell_asic_ctx_t *asic_ctx, uint8_t *data) {
  uint8_t address = 0;
  bms_stat_reg_b_t *status;
  asic_mailbox_t *mailbox;

  for (uint8_t curr_ic = 0; curr_ic < asic_ctx->ic_count; curr_ic++) {
    status = &asic_ctx[curr_ic].stat_b;
    mailbox = &asic_ctx[curr_ic].stat_mb;

    memcpy(&mailbox->rx_data_array, &data[address], ADBMS_RX_FRAME_BYTES);

    address = ((curr_ic + 1) * (ADBMS_RX_FRAME_BYTES));

    status->VD = (mailbox->rx_data_array[0] | (mailbox->rx_data_array[1] << 8));

    status->VA = (mailbox->rx_data_array[2] | (mailbox->rx_data_array[3] << 8));

    status->VRES =
        (mailbox->rx_data_array[4] | (mailbox->rx_data_array[5] << 8));
  }
}

void bms_parse_status_c(cell_asic_ctx_t *asic_ctx, uint8_t *data) {

  uint8_t address = 0;

  bms_stat_reg_c_t *status;
  asic_mailbox_t *mailbox;

  for (uint8_t curr_ic = 0; curr_ic < asic_ctx->ic_count; curr_ic++) {
    status = &asic_ctx[curr_ic].stat_c;
    mailbox = &asic_ctx[curr_ic].stat_mb;

    memcpy(&mailbox->rx_data_array, &data[address], ADBMS_RX_FRAME_BYTES);

    address = ((curr_ic + 1) * (ADBMS_RX_FRAME_BYTES));

    status->CSxFLT =
        (mailbox->rx_data_array[0] + (mailbox->rx_data_array[1] << 8));
    status->OTP2_MED = (mailbox->rx_data_array[4] & 0x01);
    status->OTP2_ED = ((mailbox->rx_data_array[4] & 0x02) >> 1);
    status->OTP1_MED = ((mailbox->rx_data_array[4] & 0x04) >> 2);
    status->OTP1_ED = ((mailbox->rx_data_array[4] & 0x08) >> 3);
    status->VD_UV = ((mailbox->rx_data_array[4] & 0x10) >> 4);
    status->VD_OV = ((mailbox->rx_data_array[4] & 0x20) >> 5);
    status->VA_UV = ((mailbox->rx_data_array[4] & 0x40) >> 6);
    status->VA_OV = ((mailbox->rx_data_array[4] & 0x80) >> 7);
    status->OSC_CHK = (mailbox->rx_data_array[5] & 0x01);
    status->TMODE_CHK = ((mailbox->rx_data_array[5] & 0x02) >> 1);
    status->THSD = ((mailbox->rx_data_array[5] & 0x04) >> 2);
    status->SLEEP = ((mailbox->rx_data_array[5] & 0x08) >> 3);
    status->SPI_FLT = ((mailbox->rx_data_array[5] & 0x10) >> 4);
    status->COMP = ((mailbox->rx_data_array[5] & 0x20) >> 5);
    status->VDEL = ((mailbox->rx_data_array[5] & 0x40) >> 6);
    status->VDE = ((mailbox->rx_data_array[5] & 0x80) >> 7);
  }
}

void bms_parse_status_d(cell_asic_ctx_t *asic_ctx, uint8_t *data) {
  uint8_t address = 0;
  bms_stat_reg_d_t *status;
  asic_mailbox_t *mailbox;

  for (uint8_t curr_ic = 0; curr_ic < asic_ctx->ic_count; curr_ic++) {
    status = &asic_ctx[curr_ic].stat_d;
    mailbox = &asic_ctx[curr_ic].stat_mb;

    memcpy(&mailbox->rx_data_array, &data[address], ADBMS_RX_FRAME_BYTES);

    address = ((curr_ic + 1) * (ADBMS_RX_FRAME_BYTES));

    status->cell_undervoltage_array[0] = (mailbox->rx_data_array[0] & 0x01);
    status->cell_overvoltage_array[0] =
        ((mailbox->rx_data_array[0] & 0x02) >> 1);
    status->cell_undervoltage_array[1] =
        ((mailbox->rx_data_array[0] & 0x04) >> 2);
    status->cell_overvoltage_array[1] =
        ((mailbox->rx_data_array[0] & 0x08) >> 3);
    status->cell_undervoltage_array[2] =
        ((mailbox->rx_data_array[0] & 0x10) >> 4);
    status->cell_overvoltage_array[2] =
        ((mailbox->rx_data_array[0] & 0x20) >> 5);
    status->cell_undervoltage_array[3] =
        ((mailbox->rx_data_array[0] & 0x40) >> 6);
    status->cell_overvoltage_array[3] =
        ((mailbox->rx_data_array[0] & 0x80) >> 7);
    /* uv, ov bits 5 to 8 status bits */
    status->cell_undervoltage_array[4] = (mailbox->rx_data_array[1] & 0x01);
    status->cell_overvoltage_array[4] =
        ((mailbox->rx_data_array[1] & 0x02) >> 1);
    status->cell_undervoltage_array[5] =
        ((mailbox->rx_data_array[1] & 0x04) >> 2);
    status->cell_overvoltage_array[5] =
        ((mailbox->rx_data_array[1] & 0x08) >> 3);
    status->cell_undervoltage_array[6] =
        ((mailbox->rx_data_array[1] & 0x10) >> 4);
    status->cell_overvoltage_array[6] =
        ((mailbox->rx_data_array[1] & 0x20) >> 5);
    status->cell_undervoltage_array[7] =
        ((mailbox->rx_data_array[1] & 0x40) >> 6);
    status->cell_overvoltage_array[7] =
        ((mailbox->rx_data_array[1] & 0x80) >> 7);
    /* uv, ov bits 9 to 12 status bits */
    status->cell_undervoltage_array[8] = (mailbox->rx_data_array[2] & 0x01);
    status->cell_overvoltage_array[8] =
        ((mailbox->rx_data_array[2] & 0x02) >> 1);
    status->cell_undervoltage_array[9] =
        ((mailbox->rx_data_array[2] & 0x04) >> 2);
    status->cell_overvoltage_array[9] =
        ((mailbox->rx_data_array[2] & 0x08) >> 3);
    status->cell_undervoltage_array[10] =
        ((mailbox->rx_data_array[2] & 0x10) >> 4);
    status->cell_overvoltage_array[10] =
        ((mailbox->rx_data_array[2] & 0x20) >> 5);
    status->cell_undervoltage_array[11] =
        ((mailbox->rx_data_array[2] & 0x40) >> 6);
    status->cell_overvoltage_array[11] =
        ((mailbox->rx_data_array[2] & 0x80) >> 7);
    /* uv, ov bits 13 to 16 status bits */
    status->cell_undervoltage_array[12] = (mailbox->rx_data_array[3] & 0x01);
    status->cell_overvoltage_array[12] =
        ((mailbox->rx_data_array[3] & 0x02) >> 1);
    status->cell_undervoltage_array[13] =
        ((mailbox->rx_data_array[3] & 0x04) >> 2);
    status->cell_overvoltage_array[13] =
        ((mailbox->rx_data_array[3] & 0x08) >> 3);
    status->cell_undervoltage_array[14] =
        ((mailbox->rx_data_array[3] & 0x10) >> 4);
    status->cell_overvoltage_array[14] =
        ((mailbox->rx_data_array[3] & 0x20) >> 5);
    status->cell_undervoltage_array[15] =
        ((mailbox->rx_data_array[3] & 0x40) >> 6);
    status->cell_overvoltage_array[15] =
        ((mailbox->rx_data_array[3] & 0x80) >> 7);
    /* ct and cts */
    status->conversions_subcounter = (mailbox->rx_data_array[4] & 0x03);
    status->conversion_counter = ((mailbox->rx_data_array[4] & 0xFC) >> 2);
    /* oc_cntr */
    status->osc_chk_counter = (mailbox->rx_data_array[5] & 0xFF);
  }
}

void bms_parse_status_e(cell_asic_ctx_t *asic_ctx, uint8_t *data) {
  uint8_t address = 0;
  bms_stat_reg_e_t *status;
  asic_mailbox_t *mailbox;

  for (uint8_t curr_ic = 0; curr_ic < asic_ctx->ic_count; curr_ic++) {
    status = &asic_ctx[curr_ic].stat_e;
    mailbox = &asic_ctx[curr_ic].stat_mb;

    memcpy(&mailbox->rx_data_array, &data[address], ADBMS_RX_FRAME_BYTES);

    address = ((curr_ic + 1) * (ADBMS_RX_FRAME_BYTES));

    status->GPIOx = ((mailbox->rx_data_array[4] +
                      ((mailbox->rx_data_array[5] & 0x03) << 8)));
    status->REV = ((mailbox->rx_data_array[5] & 0xF0) >> 4);
  }
}

void bms_parse_status_select(cell_asic_ctx_t *asic_ctx,
                             cfg_reg_group_select_t group, uint8_t *data) {
  uint8_t status_c[ADBMS_RX_FRAME_BYTES];
  uint8_t status_e[ADBMS_RX_FRAME_BYTES];
  switch (group) {
  case CFG_REG_GROUP_A:
    bms_parse_status_a(asic_ctx, &data[0]);
    break;

  case CFG_REG_GROUP_B:
    bms_parse_status_b(asic_ctx, &data[0]);
    break;

  case CFG_REG_GROUP_C:
    bms_parse_status_c(asic_ctx, &data[0]);
    break;

  case CFG_REG_GROUP_D:
    bms_parse_status_d(asic_ctx, &data[0]);
    break;

  case CFG_REG_GROUP_E:
    bms_parse_status_e(asic_ctx, &data[0]);
    break;

  case ALL_CFG_REG_GROUPS:
    bms_parse_status_a(asic_ctx, &data[0]);
    bms_parse_status_b(asic_ctx, &data[6]);
    status_c[0] = data[12]; // why
    status_c[1] = data[13];
    status_c[4] = data[14];
    status_c[5] = data[15];
    bms_parse_status_c(asic_ctx, &status_c[0]);
    bms_parse_status_d(asic_ctx, &data[16]);
    status_e[4] = data[22];
    status_e[5] = data[23];
    bms_parse_status_e(asic_ctx, &status_e[0]);
    break;

  default:
    break;
  }
}

void bms_parse_comm(cell_asic_ctx_t *asic_ctx, uint8_t *data) {
  // TODO
  uint8_t address = 0;
  comms_reg_t *comms;
  asic_mailbox_t *mailbox;
  for (uint8_t curr_ic = 0; curr_ic < asic_ctx->ic_count; curr_ic++) {
    comms = &asic_ctx[curr_ic].comm;
    mailbox = &asic_ctx[curr_ic].com_mb;
    memcpy(&mailbox->rx_data_array, &data[address], ADBMS_RX_FRAME_BYTES);
    address = ((curr_ic + 1) * (ADBMS_RX_FRAME_BYTES));

    comms->initial_comm_array[0] = ((mailbox->rx_data_array[0] & 0xF0) >> 4);
    comms->final_comm_array[0] = (mailbox->rx_data_array[0] & 0x0F);
    comms->comm_data_array[0] = (mailbox->rx_data_array[1]);
    comms->initial_comm_array[1] = ((mailbox->rx_data_array[2] & 0xF0) >> 4);
    comms->final_comm_array[1] = (mailbox->rx_data_array[2] & 0x0F);
    comms->comm_data_array[1] = (mailbox->rx_data_array[3]);
    comms->initial_comm_array[2] = ((mailbox->rx_data_array[4] & 0xF0) >> 4);
    comms->final_comm_array[2] = (mailbox->rx_data_array[4] & 0x0F);
    comms->comm_data_array[2] = (mailbox->rx_data_array[5]);
    comms->final_comm_array[2] = (mailbox->rx_data_array[4] & 0x0F);
  }
}

// --- pwm parses ---
void bms_parse_pwm_a(cell_asic_ctx_t *asic_ctx, uint8_t *data) {
  // TODO
  uint8_t address = 0;
  pwm_reg_a_t *pwm;
  asic_mailbox_t *mailbox;
  for (uint8_t curr_ic = 0; curr_ic < asic_ctx->ic_count; curr_ic++) {
    pwm = &asic_ctx[curr_ic].pwm_ctl_a;
    mailbox = &asic_ctx[curr_ic].stat_mb;
    memcpy(&mailbox->rx_data_array, &data[address], ADBMS_RX_FRAME_BYTES);
    address = ((curr_ic + 1) * (ADBMS_RX_FRAME_BYTES));

    pwm->pwm_a_ctl_array[0] = (mailbox->rx_data_array[0] & 0x0F);
    pwm->pwm_a_ctl_array[1] = ((mailbox->rx_data_array[0] & 0xF0) >> 4);
    pwm->pwm_a_ctl_array[2] = (mailbox->rx_data_array[1] & 0x0F);
    pwm->pwm_a_ctl_array[3] = ((mailbox->rx_data_array[1] & 0xF0) >> 4);
    pwm->pwm_a_ctl_array[4] = (mailbox->rx_data_array[2] & 0x0F);
    pwm->pwm_a_ctl_array[5] = ((mailbox->rx_data_array[2] & 0xF0) >> 4);
    pwm->pwm_a_ctl_array[6] = (mailbox->rx_data_array[3] & 0x0F);
    pwm->pwm_a_ctl_array[7] = ((mailbox->rx_data_array[3] & 0xF0) >> 4);
    pwm->pwm_a_ctl_array[8] = (mailbox->rx_data_array[4] & 0x0F);
    pwm->pwm_a_ctl_array[9] = ((mailbox->rx_data_array[4] & 0xF0) >> 4);
    pwm->pwm_a_ctl_array[10] = (mailbox->rx_data_array[5] & 0x0F);
    pwm->pwm_a_ctl_array[11] = ((mailbox->rx_data_array[5] & 0xF0) >> 4);
  }
}

void bms_parse_pwm_b(cell_asic_ctx_t *asic_ctx, uint8_t *data) {
  // TODO
  uint8_t address = 0;
  pwm_reg_b_t *pwm;
  asic_mailbox_t *mailbox;
  for (uint8_t curr_ic = 0; curr_ic < asic_ctx->ic_count; curr_ic++) {
    pwm = &asic_ctx[curr_ic].pwm_ctl_b;
    mailbox = &asic_ctx[curr_ic].pwm_b_mb;
    memcpy(&mailbox->rx_data_array, &data[address], ADBMS_RX_FRAME_BYTES);
    address = ((curr_ic + 1) * (ADBMS_RX_FRAME_BYTES));

    pwm->pwm_b_ctl_array[0] = (mailbox->rx_data_array[0] & 0x0F);
    pwm->pwm_b_ctl_array[1] = ((mailbox->rx_data_array[0] & 0xF0) >> 4);
    pwm->pwm_b_ctl_array[2] = (mailbox->rx_data_array[1] & 0x0F);
    pwm->pwm_b_ctl_array[3] = ((mailbox->rx_data_array[1] & 0xF0) >> 4);
  }
}

void bms_parse_pwm(cell_asic_ctx_t *asic_ctx, pwm_reg_group_select_t group,
                   uint8_t *data) {
  // TODO
  switch (group) {
  case PWM_REG_GROUP_A:
    bms_parse_pwm_a(asic_ctx, data);
    break;

  case PWM_REG_GROUP_B:
    bms_parse_pwm_b(asic_ctx, data);
    break;

  default:
    break;
  }
}

void set_cell_pwm_duty_cycle(cell_asic_ctx_t *asic_ctx, uint8_t cell_number,
                             pwm_duty_cycle_t duty_cycle) {
  // set the config struct in the asic ctx
  // then pack it into the appropriate pwm mailbox
  if (cell_number < ADBMS_NUM_PWMA_CHANNELS) {
    asic_ctx->pwm_ctl_a.pwm_a_ctl_array[cell_number] = duty_cycle;
  } else {
    asic_ctx->pwm_ctl_b.pwm_b_ctl_array[cell_number - 12] = duty_cycle;
  }
}

void clear_cell_pwm_duty_cycle(cell_asic_ctx_t *asic_ctx, uint8_t cell_number) {
  for (uint8_t ic = 0; ic < IC_COUNT_CHAIN; ic++) {
    if (cell_number < ADBMS_NUM_PWMA_CHANNELS) {
      asic_ctx[ic].pwm_ctl_a.pwm_a_ctl_array[cell_number] = 0;
    } else {
      asic_ctx[ic].pwm_ctl_b.pwm_b_ctl_array[cell_number - 12] = 0;
    }
  }
}

voltage_readings_t find_lowest_cell_voltage(cell_asic_ctx_t *asic_ctx) {
  voltage_readings_t lowest = INT16_MAX;
  voltage_readings_t *array = NULL;
  for (uint8_t ic = 0; ic < IC_COUNT_CHAIN; ic++) {
    array = asic_ctx[ic].cell.cell_voltages_array;
    // handle chain asic so surround this in a for loop this is O(N^2) 12 x 12
    for (voltage_readings_t i = 0; i < ADBMS_NUM_CELLS_PER_IC; i++) {
      if (array[i] < lowest) {
        lowest = array[i];
      }
    }
  }
  return lowest;
}

// --- create helpers ---
void bms_create_cfg_a(cell_asic_ctx_t *asic_ctx) {
  bms_cfg_reg_a_t *cfg_a;
  asic_mailbox_t *mailbox;

  for (uint8_t curr_ic = 0; curr_ic < asic_ctx->ic_count; curr_ic++) {
    cfg_a = &asic_ctx[curr_ic].tx_cfg_a;
    mailbox = &asic_ctx[curr_ic].config_a_mb;

    mailbox->tx_data_array[0] =
        (((cfg_a->REFON & 0x01) << 7) | (cfg_a->CTH & 0x07));
    mailbox->tx_data_array[1] = (cfg_a->FLAG_D & 0xFF);
    mailbox->tx_data_array[2] =
        (((cfg_a->SOAKON & 0x01) << 7) | ((cfg_a->OWRNG & 0x01) << 6) |
         ((cfg_a->OWA & 0x07) << 3));
    mailbox->tx_data_array[3] = ((cfg_a->GPIOx & 0x00FF));
    mailbox->tx_data_array[4] = ((cfg_a->GPIOx & 0x0300) >> 8);
    mailbox->tx_data_array[5] =
        (((cfg_a->SNAP_ST & 0x01) << 5) | ((cfg_a->MUTE_ST & 0x01) << 4) |
         ((cfg_a->COMM_BK & 0x01) << 3) | (cfg_a->FC & 0x07));
  }
}

void bms_create_cfg_b(cell_asic_ctx_t *asic_ctx) {
  bms_cfg_reg_b_t *cfg_b;
  asic_mailbox_t *mailbox;

  for (uint8_t curr_ic = 0; curr_ic < asic_ctx->ic_count; curr_ic++) {
    cfg_b = &asic_ctx[curr_ic].tx_cfg_b;
    mailbox = &asic_ctx[curr_ic].config_b_mb;

    mailbox->tx_data_array[0] = cfg_b->VUV;

    mailbox->tx_data_array[1] =
        (((cfg_b->VOV & 0x000F) << 4) | (((cfg_b->VUV) >> 8) & 0x0FF));
    mailbox->tx_data_array[2] = ((cfg_b->VOV >> 4) & 0x0FF);
    mailbox->tx_data_array[3] =
        (((cfg_b->DTMEN & 0x01) << 7) | ((cfg_b->DTRNG & 0x01) << 6) |
         ((cfg_b->DCTO & 0x3F) << 0));

    mailbox->tx_data_array[4] = cfg_b->DCCx & 0xFF;
    mailbox->tx_data_array[5] = cfg_b->DCCx >> 8;
  }
}

void bms_create_clrflag_mb_data(cell_asic_ctx_t *asic_ctx) {
  clearflag_reg_t *clr_flag;
  asic_mailbox_t *mailbox;

  for (uint8_t curr_ic = 0; curr_ic < asic_ctx->ic_count; curr_ic++) {
    clr_flag = &asic_ctx[curr_ic].clr_flag;
    mailbox = &asic_ctx[curr_ic].clrflag_mb;

    mailbox->tx_data_array[0] = (clr_flag->CL_CSxFLT & 0x00FF);
    mailbox->tx_data_array[1] = ((clr_flag->CL_CSxFLT & 0xFF00) >> 8);
    mailbox->tx_data_array[2] = 0x00;
    mailbox->tx_data_array[3] = 0x00;
    mailbox->tx_data_array[4] =
        ((clr_flag->CL_VAOV << 7) | (clr_flag->CL_VAUV << 6) |
         (clr_flag->CL_VDUV << 5) | (clr_flag->CL_VDOV << 4) |
         (clr_flag->CL_CED << 3) | (clr_flag->CL_CMED << 2) |
         (clr_flag->CL_SED << 1) | (clr_flag->CL_SMED));
    mailbox->tx_data_array[5] =
        ((clr_flag->CL_VDE << 7) | (clr_flag->CL_VDEL << 6) |
         (clr_flag->CL_SPIFLT << 4) | (clr_flag->CL_SPIFLT << 4) |
         (clr_flag->CL_SLEEP << 3) | (clr_flag->CL_THSD << 2) |
         (clr_flag->CL_TMODE << 1) | (clr_flag->CL_OSCCHK));
  }
}

void bms_create_comm(cell_asic_ctx_t *asic_ctx) {
  // TODO
  comms_reg_t *comms;
  asic_mailbox_t *mailbox;
  for (uint8_t curr_ic = 0; curr_ic < asic_ctx->ic_count; curr_ic++) {
    comms = &asic_ctx[curr_ic].comm;
    mailbox = &asic_ctx[curr_ic].com_mb;
    mailbox->tx_data_array[0] = ((comms->initial_comm_array[0] & 0x0F) << 4 |
                                 (comms->final_comm_array[0] & 0x0F));
    mailbox->tx_data_array[1] = (comms->comm_data_array[0]);
    mailbox->tx_data_array[2] = ((comms->initial_comm_array[1] & 0x0F) << 4 |
                                 (comms->final_comm_array[1] & 0x0F));
    mailbox->tx_data_array[3] = (comms->comm_data_array[1]);
    mailbox->tx_data_array[4] = ((comms->initial_comm_array[2] & 0x0F) << 4 |
                                 (comms->final_comm_array[2] & 0x0F));
    mailbox->tx_data_array[5] = (comms->comm_data_array[2]);
  }
}

// --- pwm create helpers ---
void bms_create_pwm_a(cell_asic_ctx_t *asic_ctx) {
  pwm_reg_a_t *pwm;
  asic_mailbox_t *mailbox;
  for (uint8_t curr_ic = 0; curr_ic < asic_ctx->ic_count; curr_ic++) {
    pwm = &asic_ctx[curr_ic].pwm_ctl_a;
    mailbox = &asic_ctx[curr_ic].pwm_a_mb;
    mailbox->tx_data_array[0] = ((pwm->pwm_a_ctl_array[1] & 0x0F) << 4 |
                                 (pwm->pwm_a_ctl_array[0] & 0x0F));
    mailbox->tx_data_array[1] = ((pwm->pwm_a_ctl_array[3] & 0x0F) << 4 |
                                 (pwm->pwm_a_ctl_array[2] & 0x0F));
    mailbox->tx_data_array[2] = ((pwm->pwm_a_ctl_array[5] & 0x0F) << 4 |
                                 (pwm->pwm_a_ctl_array[4] & 0x0F));
    mailbox->tx_data_array[3] = ((pwm->pwm_a_ctl_array[7] & 0x0F) << 4 |
                                 (pwm->pwm_a_ctl_array[6] & 0x0F));
    mailbox->tx_data_array[4] = ((pwm->pwm_a_ctl_array[9] & 0x0F) << 4 |
                                 (pwm->pwm_a_ctl_array[8] & 0x0F));
    mailbox->tx_data_array[5] = ((pwm->pwm_a_ctl_array[11] & 0x0F) << 4 |
                                 (pwm->pwm_a_ctl_array[10] & 0x0F));
  }
}

void bms_create_pwm_b(cell_asic_ctx_t *asic_ctx) {
  // TODO: this shouldnt be wrong, but we need to check it
  pwm_reg_b_t *pwm;
  asic_mailbox_t *mailbox;
  for (uint8_t curr_ic = 0; curr_ic < asic_ctx->ic_count; curr_ic++) {
    pwm = &asic_ctx[curr_ic].pwm_ctl_b;
    mailbox = &asic_ctx[curr_ic].pwm_b_mb;
    mailbox->tx_data_array[0] = ((pwm->pwm_b_ctl_array[1] & 0x0F) << 4 |
                                 (pwm->pwm_b_ctl_array[0] & 0x0F));
    mailbox->tx_data_array[1] = ((pwm->pwm_b_ctl_array[3] & 0x0F) << 4 |
                                 (pwm->pwm_b_ctl_array[2] & 0x0F));
  }
}

void bms_parse_sid(cell_asic_ctx_t *asic_ctx, uint8_t *data) {
  // TODO
  uint8_t address = 0;
  serial_id_reg_t *sid;
  asic_mailbox_t *mailbox;
  for (uint8_t curr_ic = 0; curr_ic < asic_ctx->ic_count; curr_ic++) {
    sid = &asic_ctx[curr_ic].sid;
    mailbox = &asic_ctx[curr_ic].rsid_mb;
    memcpy(&mailbox->rx_data_array, &data[address], ADBMS_RX_FRAME_BYTES);
    address = ((curr_ic + 1) * (ADBMS_RX_FRAME_BYTES));

    for (uint8_t i = 0; i < 5; i++) {
      sid->serial_id_array[i] = mailbox->rx_data_array[i];
    }
  }
}
