#include "supervisor.h"
#include <stdbool.h>

// static function headers (they are in the c file on purpose)
static void report_internal_state();
static bool is_charging_permitted();

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
static bool is_charging_permitted() {
  // return is
  return false;
}

static bool is_pack_full();

/**
 * @brief periodic charging supervisor task
 *
 * updates faults, runs the charge state machine, sends elcon command and
 * reports state
 */
void charger_supervisor(void) {}

/**
 * @brief publishes the supervisor's internal state on can
 */
//  static void report_internal_state() {}