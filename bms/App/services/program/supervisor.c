#include "supervisor.h"
#include "cmsis_os2.h"
#include "config.h"
#include "parse.h"
#include <stdbool.h>
#include <stdint.h>
#include "cb.h"

/*

1. The BMS sends operating information(Message 1) to charger at fixed interval
of 1s. After receiving the message, the charger will work under the Voltage and
Current in Message. If the Message is not received within 5s, it will enter into
communication error state and stop charging.

2. The charger send broadcast message (Message 2) at intervals of 1s. The
display meter can show the status of the charger according to up-to-date
information.

*/

// params for charging
static const uint16_t MAX_PACK_CHARGING_VOLTS = 600;
static const uint16_t MAX_PACK_CHARGING_AMPS = 10;
static const uint32_t ELCON_HEARTBEAT_STALE_MS = 5000;

// low level helpers
static void report_internal_state(charger_t *hchg);
static void update_charge_command(charger_t *hchg);
static void clear_charge_command(charger_t *hchg);

// high level helpers
static bool is_pack_full(void);
static bool is_elcon_ready(charger_t *hchg);
static bool is_pack_okay(void);

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
  hchg->elcon = &g_elcon;
  hchg->charge_enable = false;
  hchg->requested_voltage = MAX_PACK_CHARGING_VOLTS;
  hchg->requested_current = MAX_PACK_CHARGING_AMPS;
  hchg->reported_voltage = 0;
  hchg->reported_current = 0;
  clear_charge_command(hchg);
}

void charging_fsm_transition(charger_t *hchg, charging_state_t new_state) {
  hchg->state = new_state;
}

void charging_state_standby(charger_t *hchg) {
  clear_charge_command(hchg);

  if (is_charging_permitted(hchg)) {
    charging_fsm_transition(hchg, CHARGING_STATE_READY2CHARGE);
  }
}

void charging_state_ready2charge(charger_t *hchg) {
  clear_charge_command(hchg);

  if (!is_charging_permitted(hchg)) {
    charging_fsm_transition(hchg, CHARGING_STATE_STANDBY);
    return;
  }

  charging_fsm_transition(hchg, CHARGING_STATE_REQUEST4POWER);
}

void charging_state_request4power(charger_t *hchg) {
  if (!is_charging_permitted(hchg)) {
    clear_charge_command(hchg);
    charging_fsm_transition(hchg, CHARGING_STATE_STANDBY);
    return;
  }

  update_charge_command(hchg);
}

void charging_state_balancing(charger_t *hchg) {
  clear_charge_command(hchg);

  if (!is_charging_permitted(hchg)) {
    charging_fsm_transition(hchg, CHARGING_STATE_STANDBY);
    return;
  }

  if (!is_pack_full()) {
    charging_fsm_transition(hchg, CHARGING_STATE_READY2CHARGE);
  }
}

void charging_state_fault(charger_t *hchg) {
  clear_charge_command(hchg);
}

/**
 * @brief periodic charging supervisor task (runs the FSM)
 *
 * updates faults, runs the charge state machine, sends elcon command and
 * reports state
 */
bms_fault_t charger_supervisor_fsm(charger_t *hchg) {
  if (hchg == NULL || hchg->elcon == NULL) {
    return BMS_ERR_CHARGING;
  }

  if (CHARGING_STATE_FAULT == hchg->state) {
    clear_charge_command(hchg);
    send_to_charger(hchg->elcon);
    report_internal_state(hchg);
    return BMS_ERR_CHARGING;
  }

  clear_charge_command(hchg);

  if (!is_charging_permitted(hchg)) {
    charging_fsm_transition(hchg, CHARGING_STATE_STANDBY);
  }

  chg_state_handlers[hchg->state](hchg);

  send_to_charger(hchg->elcon);
  report_internal_state(hchg);

  return BMS_ERR_NONE;
}

/**
 * @brief checks if the elcon charger status can message is fresh and not
 * reporting a startup failure
 *
 * @return true if elcon status is fresh and no startup failure
 * @return false otherwise
 */
static bool is_elcon_ready(charger_t *hchg) {
  const elcon_status_t *status = &hchg->elcon->heartbeat_msg;
  uint32_t age_ms = osKernelGetTickCount() - hchg->elcon->heartbeat_tick;

  if (age_ms > ELCON_HEARTBEAT_STALE_MS) {
    return false;
  }

  return status->charger_OKAY && !status->starting_state;
}

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
 * @brief applies voltage/current caps and refreshes local command and enable
 * flags from the charger request fields
 */
static void update_charge_command(charger_t *hchg) {
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
  hchg->charge_enable = true;
  /* Elcon control byte: 0 = charging allowed, 1 = charger closed */
  hchg->elcon->enable = 0;
}

static void clear_charge_command(charger_t *hchg) {
  if (hchg == NULL || hchg->elcon == NULL) {
    return;
  }

  hchg->elcon->max_voltage = 0;
  hchg->elcon->max_current = 0;
  hchg->charge_enable = false;
  hchg->elcon->enable = 1;
}

static void report_internal_state(charger_t *hchg) {
  hchg->reported_voltage = hchg->elcon->max_voltage;
  hchg->reported_current = hchg->elcon->max_current;
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
bool is_charging_permitted(charger_t *hchg) {
  return !is_pack_full() && is_pack_okay() && is_elcon_ready(hchg);
}
