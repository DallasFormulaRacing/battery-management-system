#ifndef BMS_H
#define BMS_H

#include "cb.h"
#include "charger.h"
#include "segment.h"
#include "spi.h"

#define IDLE_POLLING_INTERVAL_MS 1000
#define MEASUREMENT_POLLING_INTERVAL_MS 10
#define FAULT_RETRY_COUNT 2
#define CELL_BALANCING_CHECK_INTERVAL_MS 3000

typedef enum {
  BMS_STATE_BOOT,
  BMS_STATE_INIT,
  BMS_STATE_TRANSMIT_DATA,
  BMS_STATE_MEASURE,
  BMS_STATE_CHARGING,
  BMS_STATE_BALANCING,
  BMS_STATE_FAULT,
  BMS_STATE_SLEEP,
} bms_state_t;

typedef enum {
  BMS_ERR_NONE = 0,
  BMS_ERR_PEC_FAIL,
  BMS_ERR_CMD_COUNTER,
  BMS_ERR_SPI_FAULT,
  BMS_ERR_CELL_OV,
  BMS_ERR_CELL_UV,
  BMS_ERR_CELL_IMBALANCE,
  BMS_ERR_CS_MISMATCH,
  BMS_ERR_CELL_OPENWIRE,
  BMS_ERR_AUX_OPENWIRE,
  BMS_ERR_THERM_OVER_TEMP,
  BMS_ERR_THERM_UNDER_TEMP,
  BMS_ERR_VA_OV,
  BMS_ERR_VA_UV,
  BMS_ERR_VD_OV,
  BMS_ERR_VD_UV,
  BMS_ERR_VDE,
  BMS_ERR_THSD,
  BMS_ERR_OSC_MISMATCH,
  BMS_ERR_FUSE_ED,
  BMS_ERR_FUSE_MED,
  BMS_ERR_TMODCHK,
} bms_fault_t;

typedef struct {
  bms_state_t current_state;
  bms_state_t previous_state;
  bms_fault_t error_code;
  uint32_t state_entry_tick;
  uint32_t fault_flags;
} bms_sm_ctx_t;

typedef struct {
  bms_cfg_t *config;
  bms_sm_ctx_t state;
  cell_asic_ctx_t *asic;
  pcb_ctx_t *pcb;
} bms_handler_t;

// ****** MEASUREMENT FUNCTIONS ******
bms_fault_t therm_open_wire_check();
bms_fault_t cell_open_wire_check_odd();
bms_fault_t cell_open_wire_check_even();
bms_fault_t therm_over_temp_check();
bms_fault_t cell_voltage_in_range_check();

//***** TESTING FUNCTIONS *****/
void bms_test_init();
void bms_test_run();

#endif
