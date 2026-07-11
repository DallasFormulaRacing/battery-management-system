#include "supervisor.h"
#include <stdbool.h>

/*

1. The BMS sends operating information(Message 1) to charger at fixed interval
of 1s. After receiving the message, the charger will work under the Voltage and
Current in Message. If the Message is not received within 5s, it will enter into
communication error state and stop charging.


2. The charger send broadcast message (Message 2) at intervals of 1s. The
display meter can show the status of the charger according to up-to-date
information.

*/

// static function headers (they are in the c file on purpose)
static void report_internal_state();
static bool is_pack_full();
static bool is_elcon_ready();
static bool is_charging_command_stale();
static bool is_pack_okay();

typedef void (*charging_handler_t)(charger_t *hchg);

static const charging_handler_t chg_state_handlers[] = {
    [CHARGING_STATE_STANDBY] = charging_state_standby,
    [CHARGING_STATE_READY2CHARGE] = charging_state_ready2charge,
    [CHARGING_STATE_REQUEST4POWER] = charging_state_request4power,
    [CHARGING_STATE_BALANCING] = charging_state_balancing,
    [CHARGING_STATE_FAULT] = charging_state_fault,
};

void charging_fsm_init(charger_t *hchg) {
  hchg->state = CHARGING_STATE_STANDBY;
}

void charging_fsm_transition(charger_t *hchg, charging_state_t new_state) {
  hchg->state = new_state;
}

void charging_state_standby(charger_t *hchg) {
  //
  bool is_charging_ok = is_charging_permitted() && !is_pack_full() &&
                        !is_charging_command_stale() && is_pack_okay();

  if (is_charging_ok) {
    charging_fsm_transition(hchg, CHARGING_STATE_READY2CHARGE);
  }

  else {
    charging_fsm_transition(hchg, CHARGING_STATE_FAULT);
  }
}

void charging_state_ready2charge(charger_t *hchg) {
  //
}

void charging_state_request4power(charger_t *hchg) {
  //
}

void charging_state_balancing(charger_t *hchg) {
  //
}

void charging_state_fault(charger_t *hchg) {
  //
}

/**
 * @brief periodic charging supervisor task (runs the FSM)
 *
 * updates faults, runs the charge state machine, sends elcon command and
 * reports state
 */
bms_fault_t charger_supervisor_fsm(charger_t *hchg) {
  // exits the charging fsm to the greater BMS fsm above
  if (CHARGING_STATE_FAULT == hchg->state)
    return BMS_ERR_CHARGING;

  chg_state_handlers[hchg->state](hchg);

  return BMS_ERR_NONE;
}

/**
 * @brief checks if the elcon charger status can message is fresh and not
 * reporting a startup failure
 *
 * @return true if elcon status is fresh and no startup failure
 * @return false otherwise
 */
static bool is_elcon_ready() {
  // get can msg from message queue and check timestamp
  report_internal_state();
  // if internal state good and can msg not stale, return true
  return false;
}

static bool is_pack_full() {
  // if highest cell is >= ov error AND no cells violating delta
}

static bool is_pack_okay() {
  // idk what to check for here
}

/**
 * @brief reads the host charge request from can, applies voltage/current caps
 * and refreshes local command and enable flags
 */
static void update_charge_command() {}

/**
 * @brief checks if charging is permitted
 *
 * true when the pack and vehicle interlocks allow charging (faults clear, elcon
 * ready, etc)
 *
 * @return true if charging is permitted
 * @return false otherwise
 */
bool is_charging_permitted() {
  // return is
  return false;
}
