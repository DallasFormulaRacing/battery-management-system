#include "supervisor.h"
#include "charger.h"
#include "cmsis_os2.h"
#include "config.h"
#include "parse.h"
#include <stdbool.h>
#include <stdint.h>

/*

1. The BMS sends operating information(Message 1) to charger at fixed interval
of 1s. After receiving the message, the charger will work under the Voltage and
Current in Message. If the Message is not received within 5s, it will enter into
communication error state and stop charging.

2. The charger send broadcast message (Message 2) at intervals of 1s. The
display meter can show the status of the charger according to up-to-date
information.

*/
volatile charger_t g_charger;

// params for charging
static const uint16_t MAX_PACK_CHARGING_VOLTS = 600;
static const uint16_t MAX_PACK_CHARGING_AMPS = 10;
static const uint32_t ELCON_HEARTBEAT_STALE_MS = 5000;

// low level helpers
static void update_charge_command(volatile charger_t *hchg);
static void clear_charge_command(volatile charger_t *hchg);

// high level helpers
static bool is_pack_full(void);
static bool is_pack_okay(void);
static bool is_elcon_ready(volatile charger_t *hchg);
static bool is_elcon_okay(volatile charger_t *hchg);
static bool pack_needs_balancing(bool refresh);

typedef void (*charging_handler_t)(volatile charger_t *hchg);

static const charging_handler_t chg_state_handlers[] = {
    [CHARGING_STATE_STANDBY] = charging_state_standby,
    [CHARGING_STATE_READY2CHARGE] = charging_state_ready2charge,
    [CHARGING_STATE_REQUEST4POWER] = charging_state_request4power,
    [CHARGING_STATE_BALANCING] = charging_state_balancing,
    [CHARGING_STATE_FAULT] = charging_state_fault,
};

/**************** CHARGING FSM Helpers ****************/

void charging_fsm_init(volatile charger_t *hchg) {
  hchg->state = CHARGING_STATE_STANDBY;
  hchg->elcon = &g_elcon;
  hchg->elcon->charge_disable = true;
  hchg->requested_voltage = MAX_PACK_CHARGING_VOLTS;
  hchg->requested_current = MAX_PACK_CHARGING_AMPS;
  hchg->reported_voltage = 0;
  hchg->reported_current = 0;
  clear_charge_command(hchg);
}

void charging_fsm_transition(volatile charger_t *hchg, charging_state_t new_state) {
  hchg->state = new_state;
}

/**************** CHARGING FSM State functions ****************/

void charging_state_standby(volatile charger_t *hchg) {
  /* charger must stay off while idle */
  clear_charge_command(hchg);

  if (!is_pack_okay() || !is_elcon_okay(hchg)) {
    charging_fsm_transition(hchg, CHARGING_STATE_FAULT);
  } 
  
  else if (is_charging_permitted(hchg)) {
    /* pack/elcon interlocks allow charging */
    charging_fsm_transition(hchg, CHARGING_STATE_READY2CHARGE);
  }
}

void charging_state_ready2charge(volatile charger_t *hchg) {
  /* no power request until REQUEST4POWER */
  clear_charge_command(hchg);

  if (!is_pack_okay() || !is_elcon_okay(hchg)) {
    charging_fsm_transition(hchg, CHARGING_STATE_FAULT);
  } 
  
  else if (!is_charging_permitted(hchg)) {
    charging_fsm_transition(hchg, CHARGING_STATE_STANDBY);
  } 
  
  else if (pack_needs_balancing(true)) {
    /* cell spread above limit so balance before requesting power */
    charging_fsm_transition(hchg, CHARGING_STATE_BALANCING);
  } 
  
  else {
    /* permitted and balanced so request charger power */
    charging_fsm_transition(hchg, CHARGING_STATE_REQUEST4POWER);
  }
}

void charging_state_request4power(volatile charger_t *hchg) {
  if (!is_pack_okay() || !is_elcon_okay(hchg)) {
    clear_charge_command(hchg);
    charging_fsm_transition(hchg, CHARGING_STATE_FAULT);
  } 
  
  else if (!is_charging_permitted(hchg)) {
    /* soft deny so stop charger and wait */
    clear_charge_command(hchg);
    charging_fsm_transition(hchg, CHARGING_STATE_STANDBY);
  } 
  
  else if (pack_needs_balancing(true)) {
    /* imbalance while charging so stop charger and balance */
    clear_charge_command(hchg);
    charging_fsm_transition(hchg, CHARGING_STATE_BALANCING);
  } 
  
  else {
    /* still permitted and balanced so keep requesting power */
    update_charge_command(hchg);
  }
}

void charging_state_balancing(volatile charger_t *hchg) {
  /* charger must stay off while balancing */
  clear_charge_command(hchg);

  if (!is_pack_okay() || !is_elcon_okay(hchg)) {
    charging_fsm_transition(hchg, CHARGING_STATE_FAULT);
  } 
  
  else if (hbms.asic == NULL || hbms.pcb == NULL) {
    /* cannot safely balance without asic/pcb context */
    charging_fsm_transition(hchg, CHARGING_STATE_FAULT);
  } 
  
  else if (!is_charging_permitted(hchg)) {
    /* soft deny so abort balancing */
    charging_fsm_transition(hchg, CHARGING_STATE_STANDBY);
  } 
  
  else {
    cell_delta_policy_enforcer(hbms.asic, hbms.pcb);

    if (!pack_needs_balancing(false)) {
      /* pack within delta limit so resume charge sequence */
      charging_fsm_transition(hchg, CHARGING_STATE_READY2CHARGE);
    }
  }
}

void charging_state_fault(volatile charger_t *hchg) {
  /* charger must stay off in fault */
  clear_charge_command(hchg);
  // report fault to GUI with CAN message
}

/**************** CHARGING FSM Periodic Task ****************/

/**
 * @brief periodic charging supervisor task (runs the FSM)
 *
 */
bms_fault_t charger_supervisor_fsm(volatile charger_t *hchg) {
  if (hchg == NULL || hchg->elcon == NULL) {
    return BMS_ERR_CHARGING;
  }

  if (CHARGING_STATE_FAULT == hchg->state) {
    charging_state_fault(hchg);
    return BMS_ERR_CHARGING;
  }

  chg_state_handlers[hchg->state](hchg);
  // report internal state will need to happen in the CAN2 job in ISR.

  return BMS_ERR_NONE;
}

/**************** CHARGING FSM Helper functions ****************/

/**
 * @brief checks if the elcon charger status can message is fresh and not
 * reporting a startup failure
 *
 * @return true if elcon status is fresh and no startup failure
 * @return false otherwise
 */
static bool is_elcon_ready(volatile charger_t *hchg) {
  const elcon_status_t *status = &hchg->elcon->heartbeat_msg;
  uint32_t age_ms = osKernelGetTickCount() - hchg->elcon->heartbeat_tick;

  if (age_ms > ELCON_HEARTBEAT_STALE_MS) {
    return false;
  }

  return status->charger_OKAY && !status->starting_state;
}

/**
 * @brief true when elcon reports no hardware safety faults
 *
 * ignores stale heartbeat, starting_state, and comm_state
 */
static bool is_elcon_okay(volatile charger_t *hchg) {
  const elcon_status_t *status = &hchg->elcon->heartbeat_msg;

  return !status->hw && !status->temp && !status->input_voltage;
}

/**
 * @brief checks if the pack is full by checking if ANY cell voltage is greater than the overvoltage threshold
 * this is about not overcharging a cell
 *
 * @return true if the pack is full
 * @return false otherwise
 */
static bool is_pack_full(void) {
  for (uint8_t seg_num = 0; seg_num < NUM_IC_COUNT_CHAIN; seg_num++) {
    for (uint16_t cell_num = 0; cell_num < NUM_CELLS_PER_SEGMENT; cell_num++) {
      float cell_v = convert_voltage_human_readable(
          hbms.asic[seg_num].filt_cell.filt_cell_voltages_array[cell_num]);
      if (cell_v >= g_voltage_cfg.overvoltage_threshold_v) {
        return true;
      }
    }
  }
  return false;
}

/**
 * @brief checks if the pack is okay by looking at the current state and error code
 *
 * @return true if the pack is okay
 * @return false otherwise
 */
static bool is_pack_okay(void) {
  if (hbms.state.current_state == BMS_STATE_FAULT) {
    return false;
  }

  switch (hbms.state.error_code) {
  case BMS_ERR_PEC_FAIL:
  case BMS_ERR_SPI_FAULT:
  case BMS_ERR_CMD_COUNTER:
    return false;
  default:
    return true;
  }
}

/**
 * @brief checks if the pack needs balancing by looking at the difference between the highest and lowest cell voltage
 * the refresh flag is used to refresh the cell voltages and deltas from the asic and pcb
 * @return true if the pack needs balancing
 * @return false otherwise
 */
static bool pack_needs_balancing(bool refresh) {
  if (hbms.asic == NULL || hbms.pcb == NULL) {
    return false;
  }

  if (refresh) {
    copy_cell_voltages(hbms.asic, hbms.pcb);
    find_cell_deltas(hbms.pcb);
  }

  voltage_readings_t spread =
      (voltage_readings_t)(hbms.pcb->highest_cell.cell_voltage -
                           hbms.pcb->lowest_cell.cell_voltage);

  return spread > hbms.pcb->config.maximum_cell_delta_allowed;
}

/**
 * @brief applies voltage/current caps and enables charging
 *
 * Elcon control byte: 0 = charging on, 1 = charging off
 */
static void update_charge_command(volatile charger_t *hchg) {
  uint16_t requested_voltage = hchg->requested_voltage;
  uint16_t requested_current = hchg->requested_current;

  if (requested_voltage > MAX_PACK_CHARGING_VOLTS) {
    requested_voltage = MAX_PACK_CHARGING_VOLTS;
  }

  if (requested_current > MAX_PACK_CHARGING_AMPS) {
    requested_current = MAX_PACK_CHARGING_AMPS;
  }

  hchg->elcon->max_voltage = requested_voltage;
  hchg->elcon->max_current = requested_current;
  hchg->elcon->charge_disable = false;

  elcon_send_command(hchg->elcon);
}

static void clear_charge_command(volatile charger_t *hchg) {
  if (hchg == NULL || hchg->elcon == NULL) {
    return;
  }

  hchg->elcon->max_voltage = 0;
  hchg->elcon->max_current = 0;
  hchg->elcon->charge_disable = true;
  elcon_send_command(hchg->elcon);
}

/**
 * @brief checks if charging is permitted
 *
 * true when the pack and vehicle interlocks allow charging (faults clear, elcon
 * ready, etc)
 *
 * @return true if charging is permitted
 * @return false otherwise
 */
bool is_charging_permitted(volatile charger_t *hchg) {
  return !is_pack_full() && is_pack_okay() && is_elcon_ready(hchg);
}
