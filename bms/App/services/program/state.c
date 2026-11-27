#include "state.h"
#include "bms.h"
#include "bms_watchdog.h"
#include "cmsis_os2.h"
#include "supervisor.h"

#if BMS_BENCH_IGNORE_THERM_FAULTS
#warning "BMS_BENCH_IGNORE_THERM_FAULTS is on: thermistor faults are ignored"
#endif

#define FAULT_MEASURE_PERIOD_MS 200U

_Static_assert(BMS_ERR_WATCHDOG == BMS_WDG_FAULT_CODE,
               "bms_watchdog.c sends BMS_WDG_FAULT_CODE as the BMS_State FaultCode");

typedef void (*state_handler_t)(bms_handler_t *hbms);
extern osMutexId_t bms_mutex_id;
const osMutexAttr_t bms_mutex_attr = {
    "bms_mutex", osMutexRecursive | osMutexPrioInherit, NULL, 0U};

extern charger_t g_charger;
const osEventFlagsAttr_t charging_session_active_event_attr = {
    .name = "charging_session_active",
};
osEventFlagsId_t charging_session_active_osEventFlags;

static const state_handler_t state_handlers[] = {
    [BMS_STATE_BOOT] = bms_state_entry,
    [BMS_STATE_INIT] = bms_state_init,
    [BMS_STATE_MEASURE] = bms_state_measure,
    [BMS_STATE_CHARGING] = bms_state_charging,
    [BMS_STATE_FAULT] = bms_state_fault,
};

void bms_fsm_init(bms_handler_t *hbms) {
  charging_fsm_init(&g_charger);
  init_cell_balancing(hbms->pcb, 67); 
  // about 10mV (see cb.c)
}

void bms_fsm_run(bms_handler_t *hbms) {
  state_handlers[hbms->state.current_state](hbms);
}

void bms_fsm_transition(bms_handler_t *hbms, bms_state_t new_state) {
  hbms->state.previous_state = hbms->state.current_state;
  hbms->state.current_state = new_state;
  hbms->state.state_entry_tick = osKernelGetTickCount();

  if (BMS_STATE_FAULT == new_state) {
    open_shutdown_circuit();
    charger_supervisor_stop(&g_charger); 
  }

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

  for (int i = 0; i < NUM_IC_COUNT_CHAIN; i++) {
    hbms->asic[i].ic_count = NUM_IC_COUNT_CHAIN;
  }

  hbms->state.current_state = BMS_STATE_BOOT;
  hbms->state.previous_state = BMS_STATE_BOOT;
  hbms->state.error_code = BMS_ERR_NONE;
  hbms->state.state_entry_tick = 0;
  bms_fsm_transition(hbms, BMS_STATE_INIT);
}

/**
 * @brief initializes the bms config registers and starts the measurement loop
 *
 * @param hbms, bms handler struct
 */
void bms_state_init(bms_handler_t *hbms) {
  if (adbms_init_config(hbms->asic) != COMM_OK ||
      adbms_start_aux_voltage_measurement(hbms->asic) != COMM_OK ||
      adbms_clear_all_pwm(hbms->asic) != COMM_OK ||
      adbms_start_cell_voltage_measurement(hbms->asic) != COMM_OK) {
    bms_fsm_transition(hbms, BMS_STATE_FAULT);
    return;
  }

  bms_fsm_transition(hbms, BMS_STATE_MEASURE);
}

/*
 * One measurement pass under bms_mutex_id, run by both Measure and Charging.
 * Any fault (OV, UV, temperature, open wire, BMS_ERR_PEC_FAIL) goes to Fault.
 * Returns false if it faulted.
 */
static bool measure_pass(bms_handler_t *hbms) {
  if (osOK != osMutexAcquire(bms_mutex_id, 1000)) {
    // failed to acquire mutex - why did this happen?
    hbms->state.error_code = BMS_ERR_TIMEOUT;
    bms_fsm_transition(hbms, BMS_STATE_FAULT);
    return false;
  }

  const bms_fault_t status = bms_run_measurement_checks();
  if (status != BMS_ERR_NONE) {
    hbms->state.error_code = status;
    bms_fsm_transition(hbms, BMS_STATE_FAULT);
  }
  osMutexRelease(bms_mutex_id);
  return status == BMS_ERR_NONE;
}

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
  - Charging measures the same way, so a session only starts after a clean
    pass and the cells are still watched while it runs.
  */
  if (measure_pass(hbms) && charging_session_active()) {
    bms_fsm_transition(hbms, BMS_STATE_CHARGING);
  }
}

// ======== CHARGING SESSION ========

#define CHARGING_SESSION_ACTIVE_FLAG (1U)

static uint32_t g_last_run_cmd_tick = 0;
static const uint32_t CAN_RUN_CMD_TIMEOUT_MS = 3000;

static uint32_t charging_session_get_last_tick(void) {
  int32_t lock = osKernelLock();
  uint32_t tick = g_last_run_cmd_tick;
  (void)osKernelRestoreLock(lock);
  return tick;
}

static void charging_session_set_last_tick(uint32_t tick) {
  int32_t lock = osKernelLock();
  g_last_run_cmd_tick = tick;
  (void)osKernelRestoreLock(lock);
}

void charging_session_enable(void) {
  /* tick before flag so a concurrent reader never sees active + stale zero tick */
  charging_session_set_last_tick(osKernelGetTickCount());
  osEventFlagsSet(charging_session_active_osEventFlags, CHARGING_SESSION_ACTIVE_FLAG);
}

void charging_session_disable(void) {
  osEventFlagsClear(charging_session_active_osEventFlags, CHARGING_SESSION_ACTIVE_FLAG);
}

void charging_session_kick_wdt(void) {
  uint32_t flags = osEventFlagsGet(charging_session_active_osEventFlags);
  if ((flags & CHARGING_SESSION_ACTIVE_FLAG) != 0U) {
    charging_session_set_last_tick(osKernelGetTickCount());
  }
}

bool charging_session_active(void) {
  return (osEventFlagsGet(charging_session_active_osEventFlags) &
          CHARGING_SESSION_ACTIVE_FLAG) != 0U;
}

/* ms since the last Connect or keepalive */
uint32_t charging_session_ms_since_kick(void) {
  return osKernelGetTickCount() - charging_session_get_last_tick();
}

void bms_state_charging(bms_handler_t *hbms) {
  uint32_t flags = osEventFlagsGet(charging_session_active_osEventFlags);
  if ((flags & CHARGING_SESSION_ACTIVE_FLAG) == 0U) {
    charger_supervisor_stop(&g_charger);
    bms_fsm_transition(hbms, BMS_STATE_MEASURE);
    return;
  }

  uint32_t elapsed_ms =
      osKernelGetTickCount() - charging_session_get_last_tick();
  if (elapsed_ms > CAN_RUN_CMD_TIMEOUT_MS) {
    charging_session_disable();
    charger_supervisor_stop(&g_charger);
    bms_fsm_transition(hbms, BMS_STATE_MEASURE);
    return;
  }

  /* Same checks as Measure, every pass. Cell voltages are read with balancing
   * paused (adbms_read_fcell_voltages_muted), so a balancing cell does not
   * read low. */
  if (!measure_pass(hbms)) {
    return;
  }

  if (charger_supervisor_fsm(&g_charger) != BMS_ERR_NONE) {
    hbms->state.error_code = BMS_ERR_CHARGING;
    bms_fsm_transition(hbms, BMS_STATE_FAULT);
    return;
  }
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

  // stop balancing and restart both ADCs with open wire off, in case the
  // fault hit mid balance or mid open wire check
  (void)adbms_clear_all_pwm(hbms->asic);
  force_sync_s_adc();
  (void)adbms_start_cell_voltage_measurement(hbms->asic);

  for (;;) {
    measure_during_fault();
    bms_watchdog_feed(); /* this loop never returns to bms_safety_task */
    osDelay(FAULT_MEASURE_PERIOD_MS);
  }
}
