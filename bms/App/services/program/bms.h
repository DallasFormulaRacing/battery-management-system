#ifndef BMS_H
#define BMS_H

#include "cb.h"
#include "charger.h"
#include "segment.h"
#include "spi.h"
#include <stdbool.h>
#include <stdint.h>

#define IDLE_POLLING_INTERVAL_MS 1000
#define MEASUREMENT_POLLING_INTERVAL_MS 10
#define FAULT_RETRY_COUNT 2
#define CELL_BALANCING_CHECK_INTERVAL_MS 3000

typedef enum {
  BMS_STATE_BOOT,
  BMS_STATE_INIT,
  BMS_STATE_MEASURE,
  BMS_STATE_CHARGING,
  BMS_STATE_FAULT,
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
  BMS_ERR_CHARGING,
  BMS_ERR_TIMEOUT,
  BMS_ERR_WATCHDOG, /* only in the last BMS_State frame before a watchdog restart (bms_watchdog.c) */
} bms_fault_t;

typedef struct {
  bms_state_t current_state;
  bms_state_t previous_state;
  bms_fault_t error_code;
  uint32_t state_entry_tick;
} bms_fsm_ctx_t;

typedef struct {
  uint16_t packvoltage;
  uint16_t state_of_charge;
  uint16_t instantaneous_current;
} pack_data_t;

typedef struct {
  bms_cfg_t *config;
  bms_fsm_ctx_t state;
  cell_asic_ctx_t *asic;
  pcb_ctx_t *pcb;
  pack_data_t *pack;
} bms_handler_t;

extern bms_handler_t hbms;

// ****** MEASUREMENT FUNCTIONS ******
bms_fault_t therm_open_wire_check();
bms_fault_t cell_open_wire_check_odd();
bms_fault_t cell_open_wire_check_even();
bms_fault_t therm_temp_in_range_check();
bms_fault_t cell_voltage_in_range_check();
/* One full pass: cell OV/UV, temperatures, cell and thermistor open wire.
 * Returns the first fault (BMS_ERR_PEC_FAIL included). Caller holds
 * bms_mutex_id. Used by the Measure and Charging states. */
bms_fault_t bms_run_measurement_checks(void);

/* Result of the last reading of IC ic (chain order): PEC passed on every group
 * and the IC was not at its reset values. False before the first reading. */
bool bms_ic_cells_valid(uint8_t ic);
bool bms_ic_temps_valid(uint8_t ic);
/* True while the last reading of some kind (cell, S-ADC, aux) failed. */
bool bms_readings_failing(void);
/* How many times the 6830 chain was re-initialised after failed readings. */
uint32_t bms_chain_reinit_count(void);

void force_sync_s_adc();
void measure_during_fault();
void open_shutdown_circuit();
void hard_fault_disable_openwire_on_profiles();

//***** TESTING FUNCTIONS *****/

/*
 * Bench testing only: 1 keeps reading the thermistors but never faults on
 * over/under temp or thermistor open wire (for boards with no NTCs fitted).
 * Must be 0 on the car.
 */
#define BMS_BENCH_IGNORE_THERM_FAULTS 1

/*
 * Bench testing only, bootloader build: 1 lets the CM5 reboot the BMS into the
 * CAN bootloader without a VCU_State saying FlashAllowed (there is no VCU on
 * the bench yet). The other conditions still apply: not faulted, no charging
 * session, no balancing. Must be 0 on the car.
 */
#define BMS_BENCH_REBOOT_WITHOUT_VCU 1

void bms_test_init();
void bms_test_run();

#endif
