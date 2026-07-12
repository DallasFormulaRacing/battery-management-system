#include "state.h"
#include "bms.h"
#include "cmsis_os2.h"
#include "supervisor.h"

typedef void (*state_handler_t)(bms_handler_t *hbms);
extern osMutexId_t bms_mutex_id;
const osMutexAttr_t bms_mutex_attr = {
    "bms_mutex", osMutexRecursive | osMutexPrioInherit, NULL, 0U};

static charger_t g_charger;

static const state_handler_t state_handlers[] = {
    [BMS_STATE_BOOT] = bms_state_entry,
    [BMS_STATE_INIT] = bms_state_init,
    [BMS_STATE_TRANSMIT_DATA] = bms_state_transmit_data,
    [BMS_STATE_MEASURE] = bms_state_measure,
    [BMS_STATE_CHARGING] = bms_state_charging,
    [BMS_STATE_BALANCING] = bms_state_balancing,
    [BMS_STATE_FAULT] = bms_state_fault,
    [BMS_STATE_SLEEP] = bms_state_sleep,
};

void bms_fsm_init(bms_handler_t *hbms) {
  hbms->state.current_state = BMS_STATE_BOOT;
  hbms->state.previous_state = BMS_STATE_BOOT;
  hbms->state.error_code = BMS_ERR_NONE;
  hbms->state.state_entry_tick = 0;
  hbms->state.fault_flags = 0;
  charging_fsm_init(&g_charger);
}

void bms_fsm_run(bms_handler_t *hbms) {
  if (hbms->state.current_state != BMS_STATE_FAULT &&
      bms_check_for_fault(hbms)) {
    bms_fsm_transition(hbms, BMS_STATE_FAULT);
    return;
  }

  state_handlers[hbms->state.current_state](hbms);
}

void bms_fsm_transition(bms_handler_t *hbms, bms_state_t new_state) {
  hbms->state.previous_state = hbms->state.current_state;
  hbms->state.current_state = new_state;
  hbms->state.state_entry_tick = osKernelGetTickCount();

  if (BMS_STATE_FAULT == new_state)
    open_shutdown_circuit();

  // todo: maybe reset flags here on transition
}

// state handlers

/**
 * @brief sm_run entry point and triggers the rest of the state machine
 *
 * @param hbms, bms handler struct
 */
void bms_state_entry(bms_handler_t *hbms) {
  // todo: maybe reset flags here on entry
  hbms->config->adc = &g_cell_profile;
  hbms->config->voltage = &g_voltage_cfg;
  hbms->config->measurement = &g_meas_cfg;
  bms_fsm_transition(hbms, BMS_STATE_INIT);
}

/**
 * @brief initializes the bms config registers and starts the measurement loop
 *
 * @param hbms, bms handler struct
 */
void bms_state_init(bms_handler_t *hbms) {
  comm_status_t status = adbms_init_config(hbms->asic);

  if (status != COMM_OK) {
    bms_fsm_transition(hbms, BMS_STATE_FAULT);
    return;
  }

  bms_fsm_transition(hbms, BMS_STATE_MEASURE);
}

void bms_state_transmit_data(bms_handler_t *hbms) {
  /*
  - needs to transmit the following data to the host computer:
    - all 144 cell voltages
    - current
    - pack voltage
    - all 120 temperature readings
    - which cells are being balanced
    - any faults that have occurred
    - transmit the onboard SoC calculation
    -

  */
}

// NOTE
// bms will not immediately transfer to fault upon bad value,
// it will finish the current measurement task first.
void bms_state_measure(bms_handler_t *hbms) {
  /*
  - this state must be able to:
    - start all adcs
    - measure from all adcs
    - pack into structs
    - do both
      - check for undeervoltage and overvoltage
      - check for openwire
      - check for over temp
    - if UV or OV or OW, set fault flag and transition to fault state
    - if nothing is wrong, stay in this state
  */

  // adbms_read_aux_open_wire(hbms->asic);

  bms_fault_t status = BMS_ERR_NONE;
  osMutexAcquire(bms_mutex_id, 1000);
  status = cell_voltage_in_range_check();
  if (BMS_ERR_CELL_OV == status || BMS_ERR_CELL_UV == status) {
    bms_fsm_transition(hbms, BMS_STATE_FAULT);
  }

  status = therm_temp_in_range_check();
  if (BMS_ERR_THERM_OVER_TEMP == status || BMS_ERR_THERM_UNDER_TEMP == status) {
    bms_fsm_transition(hbms, BMS_STATE_FAULT);
  }

  status = cell_open_wire_check_odd();
  if (BMS_ERR_CELL_OPENWIRE == status) {
    bms_fsm_transition(hbms, BMS_STATE_FAULT);
  }

  status = cell_open_wire_check_even();
  if (BMS_ERR_CELL_OPENWIRE == status) {
    bms_fsm_transition(hbms, BMS_STATE_FAULT);
  }

  status = therm_open_wire_check();
  if (BMS_ERR_AUX_OPENWIRE == status) {
    bms_fsm_transition(hbms, BMS_STATE_FAULT);
  }

  osMutexRelease(bms_mutex_id);

  bms_fsm_transition(hbms, BMS_STATE_TRANSMIT_DATA);
}

void bms_state_charging(bms_handler_t *hbms) {
  /*
  how do we want to handle this state?

  we need to interface with the charger here

  we need to alternate between charging and measuring states
  */

  if (charger_supervisor_fsm(&g_charger) != BMS_ERR_NONE) {
    hbms->state.error_code = BMS_ERR_CHARGING;
    bms_fsm_transition(hbms, BMS_STATE_FAULT);
    return;
  }

  cell_delta_policy_enforcer(hbms->asic, hbms->pcb);

  bms_fsm_transition(hbms, BMS_STATE_MEASURE);
}

void bms_state_balancing(bms_handler_t *hbms) {
  /*
  adbms_start_cell_voltage_measurement(asic_ctx);
  adbms_read_cell_voltages(asic_ctx);

  --> if bad cell, stop execution and move to fault state

  cell_delta_policy_enforcer;

  */
}

void bms_state_fault(bms_handler_t *hbms) {
  /*
  - this state opens the shut down circuit and stays in fault state until the
  MCU is manually reset via NRST or Power cycled

  - this state cannot transition to any other state
  - once this state is reached it stays.
  */
  // todo: add logging the previous state here before we lose the context
  // keep monitoring so the we can see wtf is happening
  open_shutdown_circuit();
  hard_fault_disable_openwire_on_profiles();
  for (;;)
    measure_during_fault();
}

void bms_state_sleep(bms_handler_t *hbms) {
  /*
  idk what this state does

  maybe triggers lpcm?
  */
}