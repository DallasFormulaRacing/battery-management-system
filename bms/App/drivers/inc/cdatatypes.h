#ifndef CDATATYPES_H
#define CDATATYPES_H

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

typedef voltage_readings_t int16_t;

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
  uint16_t GPO : 10;   // GPIOx Pin Control
  uint8_t SNAP_ST : 1; // Snapshot status
  uint8_t MUTE_ST : 1; // Mute status
  uint8_t COMM_BK : 1; // Communication Break feature
  uint8_t FC : 3;      // IIR filter parameter
} bms_cfg_reg_a_t;

typedef struct {

} bms_cfg_reg_b_t;

typedef struct {

} cell_voltage_t;

typedef struct {

} avg_cell_voltage_t;

typedef struct {

} s_cell_voltage_t;

typedef struct {

} filtered_cell_voltage_t;

typedef struct {

} aux_voltage_t;

typedef struct {

} rednt_aux_voltage_t;

typedef struct {

} bms_stat_reg_a_t;

typedef struct {

} bms_stat_reg_b_t;

typedef struct {

} bms_stat_reg_c_t;

typedef struct {

} clearflag_reg_t;

typedef struct {

} bms_stat_reg_d_t;

typedef struct {

} bms_stat_reg_e_t;

typedef struct {

} pwm_reg_a_t;

typedef struct {

} pwm_reg_b_t;

typedef struct {

} comms_reg_t;

typedef struct {

} serial_id_reg_t;

typedef struct {

} asic_mailbox_t; // this is ic_register_ in the old files

typedef struct {

} error_detection_t;

typedef struct {

} diag_test_t;

typedef struct {

} cell_openwire_t;

typedef struct {

} aux_openwire_t;

typedef struct {

} cell_asic_ctx_t;

#endif /* CDATATYPES_H */