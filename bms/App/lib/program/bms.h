#ifndef BMS_H
#define BMS_H

#include "api.h"
#include "bms_driver.h"
#include "bms_enums.h"
#include "bms_types.h"
#include "config.h"

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
} bms_handler_t;

extern adc_config_t g_adc_cfg;
extern voltage_config_t g_voltage_cfg;
extern measurement_config_t g_meas_cfg;

void bms_sm_init(bms_handler_t *hbms);
void bms_sm_run(bms_handler_t *hbms);

void bms_state_entry(bms_handler_t *hbms);
void bms_state_init(bms_handler_t *hbms);
void bms_state_transmit_data(bms_handler_t *hbms);
void bms_state_measure(bms_handler_t *hbms);
void bms_state_charging(bms_handler_t *hbms);
void bms_state_balancing(bms_handler_t *hbms);
void bms_state_fault(bms_handler_t *hbms);
void bms_state_sleep(bms_handler_t *hbms);

void bms_sm_transition(bms_handler_t *hbms, bms_state_t new_state);
bool bms_check_for_fault(bms_handler_t *hbms);

//***** TESTING FUNCTIONS *****/
void bms_test_init();
void bms_test_run();

#endif