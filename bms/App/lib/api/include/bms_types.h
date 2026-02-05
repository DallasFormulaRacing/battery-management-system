#ifndef CDATATYPES_H
#define CDATATYPES_H

#include "bms_enums.h"
#include <stddef.h>
#include <stdint.h>

// Number of cell channels per BMS IC
#define ADBMS_NUM_CELLS_PER_IC 16

// Number of auxiliary (GPIO/voltage/temp) channels
#define ADBMS_NUM_AUX_CHANNELS 12

// Number of redundant auxiliary channels
#define ADBMS_NUM_REDUNDANT_AUX 10

// Number of PWMA channels per IC
#define ADBMS_NUM_PWMA_CHANNELS 12

// Number of PWMB channels per IC
#define ADBMS_NUM_PWMB_CHANNELS 4

// GPIO communication register count
#define ADBMS_NUM_COMM_REGISTERS 3

// Unique serial ID length in bytes
#define ADBMS_NUM_SERIAL_ID_BYTES 6

// Length of SPI transmit frame
#define ADBMS_TX_FRAME_BYTES 6

// Length of SPI receive frame
#define ADBMS_RX_FRAME_BYTES 8

// Total bytes returned by RDCVALL (cell voltages)
#define ADBMS_RDCVALL_FRAME_SIZE 34

// Total bytes returned by RDSALL (status registers)
#define ADBMS_RDSALL_FRAME_SIZE 34

// Total bytes returned by RDACALL (aux voltages)
#define ADBMS_RDACALL_FRAME_SIZE 34

// Total bytes returned by RDFCALL (filtered cells)
#define ADBMS_RDFCALL_FRAME_SIZE 34

// Total bytes returned by RDCSALL (cell+status)
#define ADBMS_RDCSALL_FRAME_SIZE 66

// Total bytes returned by RDASALL (aux+status)
#define ADBMS_RDASALL_FRAME_SIZE 70

// Total bytes returned by RDACSALL (aux+cell+status)
#define ADBMS_RDACSALL_FRAME_SIZE 66

/* ----------------------------------------------------- */

typedef int16_t voltage_readings_t;
typedef float voltage_human_readable_t;
typedef uint32_t resistance_readings_t;
typedef float temp_c_readings_t;

/* ----------------------------------------------------- */

/** abbreviations:
 * cfg = config
 * reg = register* depends tho
 * ctx = context
 * stat = status / state
 * rednt = redundant
 * ctl = control/ controller
 */

typedef struct {
  uint8_t REFON : 1;   // Reference remains power up/down
  uint8_t CTH : 3;     // Comparison voltages threshold C vs S
  uint8_t FLAG_D : 8;  // Diagnostics flags for x
  uint8_t SOAKON : 1;  // Soak enable for AUX ADC
  uint8_t OWRNG : 1;   // Soak time range
  uint8_t OWA : 3;     // OW Soak times
  uint16_t GPIOx : 10; // GPIOx Pin Control
  uint8_t SNAP_ST : 1; // Snapshot status
  uint8_t MUTE_ST : 1; // Mute status
  uint8_t COMM_BK : 1; // Communication Break feature
  uint8_t FC : 3;      // IIR filter parameter
} bms_cfg_reg_a_t;

typedef struct {
  uint16_t VUV : 16;  // undervoltage comparison voltage
  uint16_t VOV : 16;  // overvoltage comparison voltage
  uint8_t DTMEN : 1;  // enable discharge timer monitor
  uint8_t DTRNG : 1;  // discharge timer range setting
  uint8_t DCTO : 6;   // discharge
  uint16_t DCCx : 16; // discharge cell x control
} bms_cfg_reg_b_t;

typedef struct {
  voltage_readings_t cell_voltages_array[ADBMS_NUM_CELLS_PER_IC];
} cell_voltage_t;

typedef struct {
  voltage_readings_t avg_cell_voltages_array[ADBMS_NUM_CELLS_PER_IC];
} avg_cell_voltage_t;

typedef struct {
  voltage_readings_t s_cell_voltages_array[ADBMS_NUM_CELLS_PER_IC];
} s_cell_voltage_t;

typedef struct {
  voltage_readings_t filtered_cell_voltages_array[ADBMS_NUM_CELLS_PER_IC];
} filtered_cell_voltage_t;

typedef struct {
  voltage_readings_t aux_voltages_array[ADBMS_NUM_AUX_CHANNELS];
} aux_voltage_t;

typedef struct {
  voltage_readings_t rednt_aux_voltages_array[ADBMS_NUM_REDUNDANT_AUX];
} rednt_aux_voltage_t;

typedef struct {
  uint16_t VREF2; // reference voltage 2
  uint16_t ITMP;  // internal die temperature
  uint16_t VREF3; // reference voltage 3
} bms_stat_reg_a_t;

typedef struct {
  uint16_t VD;   // discharge voltage
  uint16_t VA;   // analog voltage
  uint16_t VRES; // VREF2 across 4K resistor
} bms_stat_reg_b_t;

typedef struct {
  uint16_t CSxFLT;       // Cell ADC vs S ADC fault on channel X
  uint8_t VA_OV : 1;     // 5v analog voltage overvoltage
  uint8_t VA_UV : 1;     // 5v analog voltage undervoltage
  uint8_t VD_OV : 1;     // 3v digital voltage overvoltage
  uint8_t VD_UV : 1;     // 3v digital voltage undervoltage
  uint8_t OTP1_ED : 1;   // One-Time Prog mem region 1 error detected
  uint8_t OTP1_MED : 1;  // One-Time Prog mem region 1 margin error detected
  uint8_t OTP2_ED : 1;   // One-Time Prog mem region 2 error detected
  uint8_t OTP2_MED : 1;  // One-Time Prog mem region 2 margin error detected
  uint8_t VDE : 1;       // supply rail delta
  uint8_t VDEL : 1;      // supply rail delta latent
  uint8_t COMP : 1;      // comparision enable
  uint8_t SPI_FLT : 1;   // SPI fault
  uint8_t SLEEP : 1;     // sleep mode detection
  uint8_t THSD : 1;      // thermal shutdown status
  uint8_t TMODE_CHK : 1; // test mode detection
  uint8_t OSC_CHK : 1;   // oscillator check
  uint16_t CT : 11;      // conversion counter
  uint8_t CTS : 2;       // conversions subcounter
} bms_stat_reg_c_t;

typedef struct {
  uint16_t CL_CSxFLT;
  uint8_t CL_SMED : 1;
  uint8_t CL_SED : 1;
  uint8_t CL_CMED : 1;
  uint8_t CL_CED : 1;
  uint8_t CL_VDUV : 1;
  uint8_t CL_VDOV : 1;
  uint8_t CL_VAUV : 1;
  uint8_t CL_VAOV : 1;
  uint8_t CL_OSCCHK : 1;
  uint8_t CL_TMODE : 1;
  uint8_t CL_THSD : 1;
  uint8_t CL_SLEEP : 1;
  uint8_t CL_SPIFLT : 1;
  uint8_t CL_VDEL : 1;
  uint8_t CL_VDE : 1;
} clearflag_reg_t;

typedef struct {
  uint8_t cell_overvoltage_array[ADBMS_NUM_CELLS_PER_IC];
  uint8_t cell_undervoltage_array[ADBMS_NUM_CELLS_PER_IC];
  uint8_t osc_chk_counter;
  uint8_t conversion_counter : 6;
  uint8_t conversions_subcounter : 2;
} bms_stat_reg_d_t;

typedef struct {
  uint16_t GPIOx : 10; // GPIOx Pin state
  uint8_t REV : 4;     // device revision code
} bms_stat_reg_e_t;

typedef struct {
  pwm_duty_cycle_t pwm_a_ctl_array[ADBMS_NUM_PWMA_CHANNELS];
} pwm_reg_a_t;

typedef struct {
  pwm_duty_cycle_t pwm_b_ctl_array[ADBMS_NUM_PWMB_CHANNELS];
} pwm_reg_b_t;

typedef struct {
  uint8_t final_comm_array[ADBMS_NUM_COMM_REGISTERS];
  uint8_t initial_comm_array[ADBMS_NUM_COMM_REGISTERS];
  uint8_t comm_data_array[ADBMS_NUM_COMM_REGISTERS];
} comms_reg_t;

typedef struct {
  uint8_t *register_data;
  uint8_t *pec_error_flags;
  uint8_t *command_counter;
} asic_status_buffers_t;

typedef struct {
  uint8_t serial_id_array[ADBMS_NUM_SERIAL_ID_BYTES];
} serial_id_reg_t;

typedef struct {
  uint8_t tx_data_array[ADBMS_TX_FRAME_BYTES];
  uint8_t rx_data_array[ADBMS_RX_FRAME_BYTES];
} asic_mailbox_t; // this is ic_register_ in the old files

typedef struct {
  uint8_t command_counter;
  uint8_t config_pec;
  uint8_t cell_pec;
  uint8_t aux_channel_pec;
  uint8_t rednt_aux_channel_pec;
  uint8_t status_channel_pec;
  uint8_t comms_pec;
  uint8_t pwm_pec;
  uint8_t serial_id_pec;
} error_detection_t; // this is cmdcnt_pec_ in the old files

typedef struct {
  uint8_t osc_mismatch;
  uint8_t supply_error;
  uint8_t supply_ovuv;
  uint8_t thsd;
  uint8_t fuse_ed;
  uint8_t fuse_med;
  uint8_t tmodchk;
  uint8_t cell_ow[ADBMS_NUM_CELLS_PER_IC];
  uint8_t cellred_ow[ADBMS_NUM_CELLS_PER_IC];
  uint8_t aux_ow[(ADBMS_NUM_AUX_CHANNELS - 2)];
} diag_test_t;

typedef struct {
  voltage_readings_t cell_ow_even_array[ADBMS_NUM_CELLS_PER_IC];
  voltage_readings_t cell_ow_odd_array[ADBMS_NUM_CELLS_PER_IC];
} cell_openwire_t;

typedef struct {
  voltage_readings_t aux_ow_pull_up_array[ADBMS_NUM_AUX_CHANNELS - 2];
  voltage_readings_t aux_ow_pull_down_array[ADBMS_NUM_AUX_CHANNELS - 2];
} aux_openwire_t;

typedef struct {
  uint8_t ic_count;
  bms_cfg_reg_a_t tx_cfg_a;
  bms_cfg_reg_a_t rx_cfg_a;

  bms_cfg_reg_b_t tx_cfg_b;
  bms_cfg_reg_b_t rx_cfg_b;

  clearflag_reg_t clr_flag;

  cell_voltage_t cell;
  avg_cell_voltage_t avg_cell;
  s_cell_voltage_t s_cell;
  filtered_cell_voltage_t filtered_cell;

  aux_voltage_t aux;
  rednt_aux_voltage_t rednt_aux;

  bms_stat_reg_a_t stat_a;
  bms_stat_reg_b_t stat_b;
  bms_stat_reg_c_t stat_c;
  bms_stat_reg_d_t stat_d;
  bms_stat_reg_e_t stat_e;

  pwm_reg_a_t pwm_ctl_a;
  pwm_reg_b_t pwm_ctl_b;

  comms_reg_t comm; // WARN: may be confused with asic_mailbox_t com
  serial_id_reg_t sid;

  asic_mailbox_t config_a_mb;
  asic_mailbox_t config_b_mb;

  asic_mailbox_t clrflag_mb;
  asic_mailbox_t stat_mb;
  asic_mailbox_t com_mb; // WARN: may be confused with comms_reg_t comm

  asic_mailbox_t pwm_a_mb;
  asic_mailbox_t pwm_b_mb;
  asic_mailbox_t rsid_mb;

  error_detection_t crc_err;
  aux_openwire_t gpio;
  cell_openwire_t owcell;
  diag_test_t diag_result;
} cell_asic_ctx_t;

/* ----------------------------------------------------- */

#endif /* CDATATYPES_H */
