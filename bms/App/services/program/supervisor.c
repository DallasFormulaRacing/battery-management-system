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
static bool pack_needs_balancing(bool refresh);

typedef void (*charging_handler_t)(charger_t *hchg);

static const charging_handler_t chg_state_handlers[] = {
    [CHARGING_STATE_STANDBY] = charging_state_standby,
    [CHARGING_STATE_READY2CHARGE] = charging_state_ready2charge,
    [CHARGING_STATE_REQUEST4POWER] = charging_state_request4power,
    [CHARGING_STATE_BALANCING] = charging_state_balancing,
    [CHARGING_STATE_FAULT] = charging_state_fault,
};

/**************** CHARGING FSM Helpers ****************/

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

/**************** CHARGING FSM State functions ****************/

void charging_state_standby(charger_t *hchg) {
  /* charger must stay off while idle */
  clear_charge_command(hchg);

  if (is_charging_permitted(hchg)) {
    /* pack/elcon interlocks allow charging */
    charging_fsm_transition(hchg, CHARGING_STATE_READY2CHARGE);
  }
}

void charging_state_ready2charge(charger_t *hchg) {
  /* no power request until REQUEST4POWER */
  clear_charge_command(hchg);

  if (!is_charging_permitted(hchg)) {
    /* lost charge permission */
    charging_fsm_transition(hchg, CHARGING_STATE_STANDBY);
  } else if (pack_needs_balancing(true)) {
    /* cell spread above limit so balance before requesting power */
    charging_fsm_transition(hchg, CHARGING_STATE_BALANCING);
  } else {
    /* permitted and balanced so request charger power */
    charging_fsm_transition(hchg, CHARGING_STATE_REQUEST4POWER);
  }
}

void charging_state_request4power(charger_t *hchg) {
  if (!is_charging_permitted(hchg)) {
    /* lost charge permission so stop charger */
    clear_charge_command(hchg);
    charging_fsm_transition(hchg, CHARGING_STATE_STANDBY);
  } else if (pack_needs_balancing(true)) {
    /* imbalance while charging so stop charger and balance */
    clear_charge_command(hchg);
    charging_fsm_transition(hchg, CHARGING_STATE_BALANCING);
  } else {
    /* still permitted and balanced so keep requesting power */
    update_charge_command(hchg);
  }
}

void charging_state_balancing(charger_t *hchg) {
  /* charger must stay off while balancing */
  clear_charge_command(hchg);

  if (!is_charging_permitted(hchg)) {
    /* lost charge permission so abort balancing */
    charging_fsm_transition(hchg, CHARGING_STATE_STANDBY);
  } else if (hbms.asic == NULL || hbms.pcb == NULL) {
    /* cannot balance without asic/pcb context */
    charging_fsm_transition(hchg, CHARGING_STATE_STANDBY);
  } else {
    cell_delta_policy_enforcer(hbms.asic, hbms.pcb);

    if (!pack_needs_balancing(false)) {
      /* pack within delta limit so resume charge sequence */
      charging_fsm_transition(hchg, CHARGING_STATE_READY2CHARGE);
    }
  }
}

void charging_state_fault(charger_t *hchg) {
  /* charger must stay off in fault */
  clear_charge_command(hchg);
  send_to_charger(hchg->elcon);
  report_internal_state(hchg);
}

/**************** CHARGING FSM Periodic Task ****************/

/**
 * @brief periodic charging supervisor task (runs the FSM)
 *
 * runs the active charge state, then always sends the elcon command and
 * reports mirrored voltage/current
 */
bms_fault_t charger_supervisor_fsm(charger_t *hchg) {
  if (hchg == NULL || hchg->elcon == NULL) {
    return BMS_ERR_CHARGING;
  }

  if (CHARGING_STATE_FAULT == hchg->state) {
    charging_state_fault(hchg);
    return BMS_ERR_CHARGING;
  }

  chg_state_handlers[hchg->state](hchg);

  send_to_charger(hchg->elcon);
  report_internal_state(hchg);

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
static bool is_elcon_ready(charger_t *hchg) {
  const elcon_status_t *status = &hchg->elcon->heartbeat_msg;
  uint32_t age_ms = osKernelGetTickCount() - hchg->elcon->heartbeat_tick;

  if (age_ms > ELCON_HEARTBEAT_STALE_MS) {
    return false;
  }

  return status->charger_OKAY && !status->starting_state;
}

/**
 * @brief checks if the pack is full by looking at the difference between the highest and lowest cell voltage
 * if the lowest cell voltage is greater than (CELL MAX), 
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

  elcon_send_command(hchg->elcon);
}

static void clear_charge_command(charger_t *hchg) {
  if (hchg == NULL || hchg->elcon == NULL) {
    return;
  }

  hchg->elcon->max_voltage = 0;
  hchg->elcon->max_current = 0;
  hchg->charge_enable = false;
  hchg->elcon->enable = 1;
  elcon_send_command(hchg->elcon);
}

/**
 * @brief reports the internal state of the charger to the host computer
 *
 * @param hchg charger context
 */
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
