#ifndef BMS_H
#define BMS_H

#include "api.h"
#include "bms_driver.h"
#include "bms_enums.h"
#include "bms_types.h"
#include "config.h"

typedef enum {
  BMS_STATE_BOOT,
  BMS_STATE_INIT,
  BMS_STATE_IDLE,
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
  bms_t *config;
  bms_sm_ctx_t state;
  cell_asic_ctx_t *asic;
} bms_handler_t;

void bms_sm_init(bms_handler_t *hbms);
void bms_sm_run(bms_handler_t *hbms);

void bms_state_entry(bms_handle_t *hbms);
void bms_state_init(bms_handle_t *hbms);
void bms_state_idle(bms_handle_t *hbms);
void bms_state_measure(bms_handle_t *hbms);
void bms_state_charging(bms_handle_t *hbms);
void bms_state_balancing(bms_handle_t *hbms);
void bms_state_fault(bms_handle_t *hbms);
void bms_state_sleep(bms_handle_t *hbms);

void bms_sm_transition(bms_handle_t *hbms, bms_state_t new_state);
bool bms_check_for_fault(bms_handle_t *hbms);

#endif