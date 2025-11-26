#ifndef BMS_REG_ENUMS_H
#define BMS_REG_ENUMS_H

typedef enum {
  DISABLED = 0,
  ENABLED = 1,
} LOOP_MEASUREMENT_ENABLE;

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
  VREF2,                        // vref2
  VD,                           // ldo3v
  VA,                           // ldo5v
  ITEMP,                        // temp
  VPV,                          // vpos_2_neg
  VMV,                          // vneg
  RES,                          // vr4k
  RESERVED                      // vref3
} AUX_ADC_INPUT_CHANNEL_SELECT; // CH

typedef enum {
  RD_OFF = 0,
  RD_ON = 1,
} READ_DEVICE_ENABLE; // RD

typedef enum {
  SINGLE = 0,
  CONTINUOUS = 1,
} CONT_MEASUREMENT_MODE; // CONT

typedef enum {
  OW_OFF_ALL_CH = 0X0,
  OW_ON_EVEN_CH,
  OW_ON_ODD_CH,
  OW_ON_ALL_CH
} OPEN_WIRE_DETECTION_MODE; // OW_CS

typedef enum {
  AUX_OW_OFF = 0X0,
  AUX_OW_ON = 0X1,
} AUX_OPEN_WIRE_DETECTION_MODE; // OW_AUX

typedef enum {
  PUP_DOWN = 0X0,
  PUP_UP = 0X1,
} PULL_DOWN_CURRENT_MODE; // PUP // during aux conversion

typedef enum {
  DCP_OFF = 0X0,
  DCP_ON = 0X1,
} DISCHARGE_PERMITTED_MODE; // DCP

typedef enum {
  RSTF_OFF = 0X0,
  RSTF_ON = 0X1,
} RESET_FILTER_MODE; // RSTF

typedef enum {
  WITHOUT_ERR = 0X0,
  WITH_ERR = 0X1,
} INJECT_ERR_SPI_READ; // ERR

typedef enum {
  POWER_DOWN = 0X0,
  POWER_UP = 0X1,
} REFERENCE_POWER_MODE; // REFON

typedef enum {
  CVT_5_1mV = 0,                 /* 5.1mV                */
  CVT_8_1mV,                     /* 8.1mV (Default)      */
  CVT_10_05mV,                   /* 10.05mV              */
  CVT_15mV,                      /* 15mV                 */
  CVT_22_5mV,                    /* 22.5mV               */
  CVT_45mV,                      /* 45mV                 */
  CVT_75mV,                      /* 75mV                 */
  CVT_135mV,                     /* 135mV                */
} COMPARISON_VOLTAGES_THRESHOLD; // CTH

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
} DIAGNOSTICS_FLAGS_FOR_X; // FLAG_D

typedef enum { FLAG_CLR = 0x0, FLAG_SET = 0x1 } FLAG_CTL; // CFGA_FLAG

typedef enum { SOAKON_CLR = 0x0, SOAKON_SET = 0x1 } ADC_SOAK_ENABLE; // SOAKON

typedef enum {
  OWA0 = 0x0,
  OWA1,
  OWA2,
  OWA3,
  OWA4,
  OWA5,
  OWA6,
  OWA7
} OPEN_WIRE_SOAK_TIME; // OWA

typedef enum { SHORT = 0x0, LONG = 0x1 } OPEN_WIRE_SOAK_TIME_RANGE; // OWRNG

typedef enum {
  TIME_32US_TO_4_1MS = 0x0,
  TIME_41MS_TO_524MS = 0x1
} OPEN_WIRE_TIME_RANGE; // OW_TIME

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
} BMS_GPO_OUTPUT_PIN; // GPO

typedef enum {
  GPO_CLR = 0x0,
  GPO_SET = 0x1
} BMS_GPO_OUTPUT_PIN_STATE; // CFGA_GPO

typedef enum {
  IIR_FPA_OFF = 0,      /* Filter Disabled          */
  IIR_FPA2,             /* 110   Hz -3dB Frequency  */
  IIR_FPA4,             /* 45    Hz -3dB Frequency  */
  IIR_FPA8,             /* 21    Hz -3dB Frequency  */
  IIR_FPA16,            /* 10    Hz -3dB Frequency  */
  IIR_FPA32,            /* 5     Hz -3dB Frequency  */
  IIR_FPA128,           /* 1.25  Hz -3dB Frequency  */
  IIR_FPA256,           /* 0.625 Hz -3dB Frequency  */
} IIR_FILTER_PARAMETER; // IIR_FPA

typedef enum {
  COMM_BK_OFF = 0x0,
  COMM_BK_ON = 0x1,
} COMM_BREAK_ENABLE; // COMM_BK

typedef enum {
  SNAP_OFF = 0x0,
  SNAP_ON = 0x1,
} SNAPSHOT_ENABLE; // SNAPSHOT

typedef enum {
  DTMEN_OFF = 0x0,
  DTMEN_ON = 0x1,
} DISCHARGE_TIMER_MONITOR_ENABLE; // DTMEN

typedef enum {
  RANG_0_TO_63_MIN = 0x0,
  RANG_0_TO_16_8_HR = 0x1,
} DISCHARGE_TIMER_RANGE; // DTRNG

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
} DISCHARGE_TIMER_TIMEOUT; // DCTO

typedef enum {
  PWM_0_0_PCT = 0, /* 0.0%  (default) */
  PWM_6_6_PCT,     /* 6.6%            */
  PWM_13_2_PCT,    /* 13.2%           */
  PWM_19_8_PCT,    /* 19.8%           */
  PWM_26_4_PCT,    /* 26.4%           */
  PWM_33_0_PCT,    /* 33.0%           */
  PWM_39_6_PCT,    /* 39.6%           */
  PWM_46_2_PCT,    /* 46.2%           */
  PWM_52_8_PCT,    /* 52.8%           */
  PWM_59_4_PCT,    /* 59.4%           */
  PWM_66_0_PCT,    /* 66.0%           */
  PWM_72_6_PCT,    /* 72.6%           */
  PWM_79_2_PCT,    /* 79.2%           */
  PWM_85_8_PCT,    /* 85.8%           */
  PWM_92_4_PCT,    /* 92.4%           */
  PWM_100_0_PCT,   /* ~100.0%         */
} PWM_DUTY_CYCLE_CTL;

typedef enum {
  DCC1 = 0,
  DCC2,
  DCC3,
  DCC4,
  DCC5,
  DCC6,
  DCC7,
  DCC8,
  DCC9,
  DCC10,
  DCC11,
  DCC12,
  DCC13,
  DCC14,
  DCC15,
  DCC16,
} DISCHARGE_CELL_CTL; // DCC

typedef enum {
  DCC_BIT_CLR = 0x0,
  DCC_BIT_SET = 0x1,
} DCC_BIT_CTL; // DCC_BIT

typedef enum {
  ALL_GROUPS = 0x0,
  GROUP_A,
  GROUP_B,
  GROUP_C,
  GROUP_D,
  GROUP_E,
  GROUP_F,
  NO_GROUP,
} CFG_REG_GROUP_SELECT; // GRP

typedef enum {
  CELL = 0x0,
  AUX,
  REDNT_AUX,
  STATUS,
  PWM,
  AVG_CELL,
  S_VOLT,
  F_VOLT,
  CONFIG,
  COMM,
  SID,
  CLRFLAG,
  RD_CV_ALL,
  RD_AC_ALL,
  RD_S_ALL,
  RD_CS_ALL,
  RD_ACS_ALL,
  RD_FC_ALL,
  RD_AS_ALL
} REGISTER_ACCESS_TYPE_SELECT;

typedef enum { PASS, FAIL } COMM_RESULT;

typedef enum {
  OSC_MISMATCH = 0x0,
  SUPPLY_ERROR,
  THSD,
  FUSE_ED,
  FUSE_MED,
  TMODCHK
} DIAGNOSTIC_TYPE;

#endif