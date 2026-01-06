#ifndef BMS_REG_ENUMS_H
#define BMS_REG_ENUMS_H

typedef enum {
  DISABLED = 0,
  ENABLED = 1,
} loop_measurement_enable_t;

typedef enum {
  AUX_ALL = 0,
  GPIO1,
  GPIO2,
  GPIO3,
  GPIO4,
  GPIO5,
  GPIO6,
  GPIO7,
  GPIO8,
  GPIO9,
  GPIO10,
  VREF2,                          // vref2
  VD,                             // ldo3v
  VA,                             // ldo5v
  ITEMP,                          // temp
  VPV,                            // vpos_2_neg
  VMV,                            // vneg
  RES,                            // vr4k
  RESERVED                        // vref3
} aux_adc_input_channel_select_t; // CH

typedef enum {
  RD_OFF = 0,
  RD_ON = 1,
} redundant_enable_t; // RD

typedef enum {
  SINGLE = 0,
  CONTINUOUS = 1,
} cont_measurement_mode_t; // CONT

typedef enum {
  OW_OFF_ALL_CH = 0X0,
  OW_ON_EVEN_CH,
  OW_ON_ODD_CH,
  OW_ON_ALL_CH
} open_wire_detection_mode_t; // OW_CS

typedef enum {
  AUX_OW_OFF = 0X0,
  AUX_OW_ON = 0X1,
} aux_open_wire_detection_mode_t; // OW_AUX

typedef enum {
  PUP_DOWN = 0X0,
  PUP_UP = 0X1,
} pull_down_current_mode_t; // PUP // during aux conversion

typedef enum {
  DCP_OFF = 0X0,
  DCP_ON = 0X1,
} discharge_permitted_mode_t; // DCP

typedef enum {
  RSTF_OFF = 0X0,
  RSTF_ON = 0X1,
} reset_filter_mode_t; // RSTF

typedef enum {
  WITHOUT_ERR = 0X0,
  WITH_ERR = 0X1,
} inject_err_spi_read_t; // ERR

typedef enum {
  POWER_DOWN = 0X0,
  POWER_UP = 0X1,
} reference_power_mode_t; // REFON

typedef enum {
  CVT_5_1mV = 0,                   /* 5.1mV                */
  CVT_8_1mV,                       /* 8.1mV (Default)      */
  CVT_10_05mV,                     /* 10.05mV              */
  CVT_15mV,                        /* 15mV                 */
  CVT_22_5mV,                      /* 22.5mV               */
  CVT_45mV,                        /* 45mV                 */
  CVT_75mV,                        /* 75mV                 */
  CVT_135mV,                       /* 135mV                */
} comparison_voltages_threshold_t; // CTH

typedef enum {
  FLAG_D0 = 0, /* Force oscillator counter fast */
  FLAG_D1,     /* Force oscillator counter slow */
  FLAG_D2,     /* Force Supply Error detection  */
  FLAG_D3,     /* FLAG_D[3]: 1--> Select Supply OV and delta detection, 0 -->
                  Selects UV */
  FLAG_D4,     /* Set THSD */
  FLAG_D5,     /* Force Fuse ED */
  FLAG_D6,     /* Force Fuse MED */
  FLAG_D7,     /* Force TMODCHK  */
} diagnostics_flags_for_x_t; // FLAG_D

typedef enum { FLAG_CLR = 0x0, FLAG_SET = 0x1 } flag_ctl_t; // CFGA_FLAG

typedef enum { SOAKON_CLR = 0x0, SOAKON_SET = 0x1 } adc_soak_enable_t; // SOAKON

typedef enum {
  OWA0 = 0x0,
  OWA1,
  OWA2,
  OWA3,
  OWA4,
  OWA5,
  OWA6,
  OWA7
} open_wire_soak_time_t; // OWA

typedef enum { SHORT = 0x0, LONG = 0x1 } open_wire_soak_time_range_t; // OWRNG

typedef enum {
  TIME_32US_TO_4_1MS = 0x0,
  TIME_41MS_TO_524MS = 0x1
} open_wire_time_range_t; // OW_TIME

typedef enum {
  GPO1 = 0x0,
  GPO2 = 0x1,
  GPO3 = 0x2,
  GPO4 = 0x3,
  GPO5 = 0x4,
  GPO6 = 0x5,
  GPO7 = 0x6,
  GPO8 = 0x7,
  GPO9 = 0x8,
  GPO10 = 0x9,
} bms_gpo_output_pin_t; // GPO

typedef enum {
  GPO_CLR = 0x0,
  GPO_SET = 0x1
} bms_gpo_output_pin_state_t; // CFGA_GPO

typedef enum {
  IIR_FPA_OFF = 0,        /* Filter Disabled          */
  IIR_FPA2,               /* 110   Hz -3dB Frequency  */
  IIR_FPA4,               /* 45    Hz -3dB Frequency  */
  IIR_FPA8,               /* 21    Hz -3dB Frequency  */
  IIR_FPA16,              /* 10    Hz -3dB Frequency  */
  IIR_FPA32,              /* 5     Hz -3dB Frequency  */
  IIR_FPA128,             /* 1.25  Hz -3dB Frequency  */
  IIR_FPA256,             /* 0.625 Hz -3dB Frequency  */
} iir_filter_parameter_t; // IIR_FPA

typedef enum {
  COMM_BK_OFF = 0x0,
  COMM_BK_ON = 0x1,
} comm_break_enable_t; // COMM_BK

typedef enum {
  COMM_OK,
  COMM_ERROR,
  COMM_TIMEOUT,
  COMM_INVALID_NUMBER_OF_ICS,
  COMM_INVALID_COMMAND,
  COMM_INVALID_COMMAND_COUNTER,
  COMM_INVALID_PEC_ERROR_FLAG,
  COMM_INVALID_PARAMETERS,
} comm_status_t;

typedef enum {
  SNAP_OFF = 0x0,
  SNAP_ON = 0x1,
} snapshot_enable_t; // SNAPSHOT

typedef enum {
  DTMEN_OFF = 0x0,
  DTMEN_ON = 0x1,
} discharge_timer_monitor_enable_t; // DTMEN

typedef enum {
  RANG_0_TO_63_MIN = 0x0,
  RANG_0_TO_16_8_HR = 0x1,
} discharge_timer_range_t; // DTRNG

typedef enum {
  DCTO_TIMEOUT = 0,
  TIME_1MIN_OR_0_26HR,
  TIME_2MIN_OR_0_53HR,
  TIME_4MIN_OR_1_06HR,
  TIME_8MIN_OR_2_12HR,
  TIME_16MIN_OR_4_24HR,
  TIME_32MIN_OR_8_48HR,
  TIME_64MIN_OR_16_96HR,
  TIME_128MIN_OR_33_92HR,
} discharge_timer_timeout_t; // DCTO

typedef enum {
  PWM_0_0_PERCENT_DUTY_CYCLE = 0, /* 0.0% (default) duty cycle    */
  PWM_6_6_PERCENT_DUTY_CYCLE,     /* 6.6% duty cycle             */
  PWM_13_2_PERCENT_DUTY_CYCLE,    /* 13.2% duty cycle            */
  PWM_19_8_PERCENT_DUTY_CYCLE,    /* 19.8% duty cycle            */
  PWM_26_4_PERCENT_DUTY_CYCLE,    /* 26.4% duty cycle            */
  PWM_33_0_PERCENT_DUTY_CYCLE,    /* 33.0% duty cycle            */
  PWM_39_6_PERCENT_DUTY_CYCLE,    /* 39.6% duty cycle            */
  PWM_46_2_PERCENT_DUTY_CYCLE,    /* 46.2% duty cycle            */
  PWM_52_8_PERCENT_DUTY_CYCLE,    /* 52.8% duty cycle            */
  PWM_59_4_PERCENT_DUTY_CYCLE,    /* 59.4% duty cycle            */
  PWM_66_0_PERCENT_DUTY_CYCLE,    /* 66.0% duty cycle            */
  PWM_72_6_PERCENT_DUTY_CYCLE,    /* 72.6% duty cycle            */
  PWM_79_2_PERCENT_DUTY_CYCLE,    /* 79.2% duty cycle            */
  PWM_85_8_PERCENT_DUTY_CYCLE,    /* 85.8% duty cycle            */
  PWM_92_4_PERCENT_DUTY_CYCLE,    /* 92.4% duty cycle            */
  PWM_100_0_PERCENT_DUTY_CYCLE,   /* ~100.0% duty cycle          */
} pwm_duty_cycle_t;

typedef enum {
  DISCHARGE_CELL_1 = 0,
  DISCHARGE_CELL_2,
  DISCHARGE_CELL_3,
  DISCHARGE_CELL_4,
  DISCHARGE_CELL_5,
  DISCHARGE_CELL_6,
  DISCHARGE_CELL_7,
  DISCHARGE_CELL_8,
  DISCHARGE_CELL_9,
  DISCHARGE_CELL_10,
  DISCHARGE_CELL_11,
  DISCHARGE_CELL_12,
  DISCHARGE_CELL_13,
  DISCHARGE_CELL_14,
  DISCHARGE_CELL_15,
  DISCHARGE_CELL_16,
} discharge_cell_x_ctl_t; // DCCx

typedef enum {
  DISCHARGE_CELL_BIT_CLR = 0x0,
  DISCHARGE_CELL_BIT_SET = 0x1,
} dcc_bit_ctl_t; // DCC_BIT

typedef enum {
  ALL_REG_GROUPS = 0x0,
  REG_GROUP_A,
  REG_GROUP_B,
  REG_GROUP_C,
  REG_GROUP_D,
  REG_GROUP_E,
  REG_GROUP_F,
  NO_REG_GROUP,
} bms_group_select_t;

typedef enum {
  ALL_CFG_REG_GROUPS = 0x0,
  CFG_REG_GROUP_A,
  CFG_REG_GROUP_B,
  CFG_REG_GROUP_C,
  CFG_REG_GROUP_D,
  CFG_REG_GROUP_E,
  CFG_REG_GROUP_F,
  NO_CFG_REG_GROUP,
} cfg_reg_group_select_t; // GRP

typedef enum {
  AUX_REG_GROUP_A = 0x0,
  AUX_REG_GROUP_B,
  AUX_REG_GROUP_C,
  AUX_REG_GROUP_D,
  NO_AUX_REG_GROUP,
} aux_reg_group_select_t; // AUX_GRP

typedef enum {
  PWM_REG_GROUP_A = 0x0,
  PWM_REG_GROUP_B,
  NO_PWM_REG_GROUP,
} pwm_reg_group_select_t; // AUX_GRP

typedef enum {
  ASIC_MAILBOX_CONFIG_A = 0x0,
  ASIC_MAILBOX_CONFIG_B,
  ASIC_MAILBOX_CLR_FLAG,
  ASIC_MAILBOX_STAT,
  ASIC_MAILBOX_COM,
  ASIC_MAILBOX_PWM_A,
  ASIC_MAILBOX_PWM_B,
  ASIC_MAILBOX_RSID,
} asic_mailbox_id_select_t;

typedef enum {
  /* Register groups ----------------------------------------------------- */
  BMS_REG_CELL_VOLT,          // Cell voltage groups A–F (RDCVA..RDCVF)
  BMS_REG_AUX_VOLT,           // Aux voltage groups A–D (RDAUXA..RDAUXD)
  BMS_REG_REDUNDANT_AUX_VOLT, // Redundant aux groups A–D (RDRAXA..RDRAXD)
  BMS_REG_STATUS,             // Status groups A–E (RDSTATA..RDSTATE)
  BMS_REG_PWM,                // PWM registers (RDPWMa/RDPWMb)
  BMS_REG_AVG_CELL_VOLT,      // Averaged cell groups A–F (RDACA..RDACF)
  BMS_REG_S_VOLT,             // S-voltage groups A–F (RDSVA..RDSVF)
  BMS_REG_FILTERED_CELL_VOLT, // Filtered cell groups A–F (RDFCA..RDFCF)
  BMS_REG_CONFIG,             // Config A/B (RDCFGx/WRCFGx)
  BMS_REG_COMM,               // Comm registers
  BMS_REG_SID,                // SID / device ID

  /* Commands operations -------------------------------------------------- */
  BMS_CMD_CLRFLAG,  // CLRFLAG command
  BMS_CMD_RDCVALL,  // Read all cell results
  BMS_CMD_RDACALL,  // Read all avg cell results
  BMS_CMD_RDSALL,   // Read all S-voltage results
  BMS_CMD_RDCSALL,  // Read all C & S results
  BMS_CMD_RDACSALL, // Read all avg C & S results
  BMS_CMD_RDFCALL,  // Read all filtered C results
  BMS_CMD_RDASALL   // Read all AUX / Status registers
} bms_op_t;         // TYPE

typedef enum { PASS, FAIL } comm_result_t; // RESULT

typedef enum {
  OSC_MISMATCH = 0x0,
  SUPPLY_ERROR,
  THSD,
  FUSE_ED,
  FUSE_MED,
  TMODCHK
} diagnostic_type_t; // DIAG

#endif
