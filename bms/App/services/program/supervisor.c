#include "supervisor.h"
#include "bms_limits.h"
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

/*
 * GUI CAN job publishes charger_power_setpoint_t into
 * charger_power_setpoint_queueHandle (depth 1). The charging FSM is the sole
 * consumer. Overwrite-on-full is handled by the publisher (evict then put).
 * The CAN ISR only enqueues frames; it does not touch charger setpoints.
 */
charger_t g_charger;

const osMessageQueueAttr_t charger_power_setpoint_queue_attributes = {
    .name = "chg_pwr_setpoint",
};

// params for charging, see bms_limits.h. Setpoints are 0.1 V / 0.1 A per bit.
static const uint16_t MIN_CHARGER_DV = LIMIT_CHARGER_VOLTAGE_MIN_V * 10U;
static const uint16_t MAX_PACK_CHARGING_DV = LIMIT_PACK_CHARGE_VOLTAGE_MAX_V * 10U;
static const uint16_t MIN_PACK_CHARGING_DA = LIMIT_PACK_CHARGE_CURRENT_MIN_A * 10U;
static const uint16_t MAX_PACK_CHARGING_DA = LIMIT_PACK_CHARGE_CURRENT_MAX_A * 10U;
static const uint32_t ELCON_HEARTBEAT_STALE_MS = ELCON_COMM_TIMEOUT_MS;

// low level helpers
static void update_charge_command(charger_t *charger_ctx);
static void clear_charge_command(charger_t *charger_ctx);
static void update_reported_output(charger_t *charger_ctx);

// high level helpers
static bool is_pack_full(void);
static bool is_pack_okay(void);
static bool is_elcon_ready(charger_t *charger_ctx);
static bool is_elcon_okay(charger_t *charger_ctx);
static bool pack_needs_balancing(bool refresh);

typedef void (*charging_handler_t)(charger_t *charger_ctx);

static const charging_handler_t chg_state_handlers[] = {
    [CHARGING_STATE_STANDBY] = charging_state_standby,
    [CHARGING_STATE_READY2CHARGE] = charging_state_ready2charge,
    [CHARGING_STATE_REQUEST4POWER] = charging_state_request4power,
    [CHARGING_STATE_BALANCING] = charging_state_balancing,
    [CHARGING_STATE_FAULT] = charging_state_fault,
};

/****************** Queue Overrun Counter ****************/
void charger_update_requested_setpoints(uint16_t volts_dv, uint16_t amps_da) {
  charger_power_setpoint_t new_setpoint = {.voltage_dv = volts_dv, .current_da = amps_da};

  // Attempt non-blocking push
  osStatus_t status =
      osMessageQueuePut(charger_power_setpoint_queueHandle, &new_setpoint, 0U, 0U);

  // If full, evict the stale setpoint and swap in the newest one
  if (status == osErrorResource) {
    charger_power_setpoint_t stale_dummy;
    (void)osMessageQueueGet(charger_power_setpoint_queueHandle, &stale_dummy,
                            NULL, 0U);
    (void)osMessageQueuePut(charger_power_setpoint_queueHandle, &new_setpoint,
                            0U, 0U);
  }
}
/**************** CHARGING FSM Helpers ****************/

void charging_fsm_init(charger_t *charger_ctx) {
  charger_ctx->state = CHARGING_STATE_STANDBY;
  charger_ctx->elcon = &g_elcon;
  charger_ctx->elcon->charge_disable = true;
  /* no power is ever requested until the GUI sends an in-range setpoint */
  charger_ctx->setpoint_valid = false;
  charger_ctx->requested_voltage_dv = 0;
  charger_ctx->requested_current_da = 0;
  charger_ctx->reported_voltage_dv = 0;
  charger_ctx->reported_current_da = 0;
  clear_charge_command(charger_ctx);
}

void charging_fsm_transition(charger_t *charger_ctx, charging_state_t new_state) {
  charger_ctx->state = new_state;
}

/**
 * @brief true when a GUI setpoint is a real, in-range charge request
 *
 * Anything outside the pack limits (bms_limits.h) is rejected outright, never
 * clamped, so a bad frame can't turn into a max-power request.
 */
bool charger_setpoint_in_range(uint16_t volts_dv, uint16_t amps_da) {
  return (volts_dv >= MIN_CHARGER_DV) &&
         (volts_dv <= MAX_PACK_CHARGING_DV) &&
         (amps_da >= MIN_PACK_CHARGING_DA) &&
         (amps_da <= MAX_PACK_CHARGING_DA);
}

static void invalidate_setpoint(charger_t *charger_ctx) {
  charger_ctx->setpoint_valid = false;
  charger_ctx->requested_voltage_dv = 0;
  charger_ctx->requested_current_da = 0;
}

/**
 * @brief stops the charger right away when the BMS leaves the charging state
 *
 * Without this the charger keeps running the last request until its own 5 s
 * comm timeout. A latched fault is kept, anything else goes back to standby so
 * the next session starts from the top of the sequence.
 * 
 * Better to stop the charger immediately and let the next session start fresh than to
 * let the charger keep running the last request until its own 5 s comm timeout.
 */
void charger_supervisor_stop(charger_t *charger_ctx) {
  if (charger_ctx == NULL || charger_ctx->elcon == NULL) {
    elcon_hard_stop();
    return;
  }

  clear_charge_command(charger_ctx);

  /* the next session has to send its own setpoint */
  invalidate_setpoint(charger_ctx);
  if (charger_power_setpoint_queueHandle != NULL) {
    (void)osMessageQueueReset(charger_power_setpoint_queueHandle);
  }

  if (charger_ctx->state != CHARGING_STATE_FAULT) {
    charging_fsm_transition(charger_ctx, CHARGING_STATE_STANDBY);
  }
}

/**************** CHARGING FSM State functions ****************/

static inline bool is_okay(charger_t *charger_ctx) {
  return is_pack_okay() && is_elcon_okay(charger_ctx);
}

void charging_state_standby(charger_t *charger_ctx) {
  /* charger must stay off while idle */
  clear_charge_command(charger_ctx);

  if (!is_okay(charger_ctx)) {
    charging_fsm_transition(charger_ctx, CHARGING_STATE_FAULT);
  } 
  
  else if (is_charging_permitted(charger_ctx)) {
    /* pack/elcon interlocks allow charging */
    charging_fsm_transition(charger_ctx, CHARGING_STATE_READY2CHARGE);
  }
}

void charging_state_ready2charge(charger_t *charger_ctx) {
  /* no power request until REQUEST4POWER */
  clear_charge_command(charger_ctx);

  if (!is_okay(charger_ctx)) {
    charging_fsm_transition(charger_ctx, CHARGING_STATE_FAULT);
  } 
  
  else if (!is_charging_permitted(charger_ctx)) {
    charging_fsm_transition(charger_ctx, CHARGING_STATE_STANDBY);
  } 
  
  else if (pack_needs_balancing(true)) {
    /* cell spread above limit so balance before requesting power */
    charging_fsm_transition(charger_ctx, CHARGING_STATE_BALANCING);
  } 
  
  else {
    /* permitted and balanced so request charger power */
    charging_fsm_transition(charger_ctx, CHARGING_STATE_REQUEST4POWER);
  }
}

void charging_state_request4power(charger_t *charger_ctx) {
  if (!is_okay(charger_ctx)) {
    clear_charge_command(charger_ctx);
    charging_fsm_transition(charger_ctx, CHARGING_STATE_FAULT);
  } 
  
  else if (!is_charging_permitted(charger_ctx)) {
    /* soft deny so stop charger and wait */
    clear_charge_command(charger_ctx);
    charging_fsm_transition(charger_ctx, CHARGING_STATE_STANDBY);
  } 
  
  else if (pack_needs_balancing(true)) {
    /* imbalance while charging so stop charger and balance */
    clear_charge_command(charger_ctx);
    charging_fsm_transition(charger_ctx, CHARGING_STATE_BALANCING);
  } 
  
  else {
    /* still permitted and balanced so keep requesting power */
    update_charge_command(charger_ctx);
  }
}

void charging_state_balancing(charger_t *charger_ctx) {
  /* charger must stay off while balancing */
  clear_charge_command(charger_ctx);

  if (!is_okay(charger_ctx) || (hbms.asic == NULL || hbms.pcb == NULL)) {
    /* cannot safely balance without asic/pcb context */
    charging_fsm_transition(charger_ctx, CHARGING_STATE_FAULT);
  } 
  
  else if (!is_charging_permitted(charger_ctx)) {
    /* soft deny so abort balancing */
    charging_fsm_transition(charger_ctx, CHARGING_STATE_STANDBY);
  } 
  
  else {
    cell_delta_policy_enforcer(hbms.asic, hbms.pcb);

    if (!pack_needs_balancing(true)) {
      /* pack within delta limit so resume charge sequence */
      charging_fsm_transition(charger_ctx, CHARGING_STATE_READY2CHARGE);
    }
  }
}

void charging_state_fault(charger_t *charger_ctx) {
  /* charger must stay off in fault */
  clear_charge_command(charger_ctx);
  // report fault to GUI with CAN message
}

/**************** CHARGING FSM Periodic Task ****************/

/**
 * @brief periodic charging supervisor task (runs the FSM)
 *
 */
bms_fault_t charger_supervisor_fsm(charger_t *charger_ctx) {
  if (charger_ctx == NULL || charger_ctx->elcon == NULL) {
    return BMS_ERR_CHARGING;
  }

  if (CHARGING_STATE_FAULT == charger_ctx->state) {
    charging_state_fault(charger_ctx);
    return BMS_ERR_CHARGING;
  }
  update_reported_output(charger_ctx);

  charger_power_setpoint_t inbound_setpoint;
  osStatus_t status = osMessageQueueGet(charger_power_setpoint_queueHandle, &inbound_setpoint, NULL, 0U);
  if (status == osOK) {
    if (charger_setpoint_in_range(inbound_setpoint.voltage_dv,
                                  inbound_setpoint.current_da)) {
      charger_ctx->requested_voltage_dv = inbound_setpoint.voltage_dv;
      charger_ctx->requested_current_da = inbound_setpoint.current_da;
      charger_ctx->setpoint_valid = true;
    } else {
      /* out of range: drop back to no setpoint, which stops the charger */
      invalidate_setpoint(charger_ctx);
    }
  }
  chg_state_handlers[charger_ctx->state](charger_ctx);
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
static bool is_elcon_ready(charger_t *charger_ctx) {
  int32_t lock = osKernelLock();
  elcon_status_t status = charger_ctx->elcon->heartbeat_msg;
  uint32_t tick = charger_ctx->elcon->heartbeat_tick;
  (void)osKernelRestoreLock(lock);

  uint32_t age_ms = osKernelGetTickCount() - tick;
  if (age_ms > ELCON_HEARTBEAT_STALE_MS) {
    return false;
  }

  return status.charger_OKAY && !status.starting_state;
}

/**
 * @brief true when elcon reports no hardware safety faults
 *
 * ignores stale heartbeat, starting_state, and comm_state
 */
static bool is_elcon_okay(charger_t *charger_ctx) {
  int32_t lock = osKernelLock();
  elcon_status_t status = charger_ctx->elcon->heartbeat_msg;
  (void)osKernelRestoreLock(lock);

  return !status.hw && !status.temp && !status.input_voltage;
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
 * @brief sends the validated GUI setpoint and enables charging
 *
 * Elcon control byte: 0 = charging on, 1 = charging off
 */
static void update_charge_command(charger_t *charger_ctx) {
  uint16_t requested_voltage_dv = charger_ctx->requested_voltage_dv;
  uint16_t requested_current_da = charger_ctx->requested_current_da;

  /* never clamp a bad setpoint up to the limits, refuse it */
  if (!charger_ctx->setpoint_valid ||
      !charger_setpoint_in_range(requested_voltage_dv, requested_current_da)) {
    clear_charge_command(charger_ctx);
    return;
  }

  charger_ctx->elcon->max_voltage_dv = requested_voltage_dv;
  charger_ctx->elcon->max_current_da = requested_current_da;
  charger_ctx->elcon->charge_disable = false;

  elcon_send_command(charger_ctx->elcon);
}

/**
 * @brief copies the charger's output readback (0.1 V / 0.1 A)
 */
static void update_reported_output(charger_t *charger_ctx) {
  int32_t lock = osKernelLock();
  uint16_t volts_dv = charger_ctx->elcon->output_voltage_dv;
  uint16_t amps_da = charger_ctx->elcon->output_current_da;
  (void)osKernelRestoreLock(lock);

  charger_ctx->reported_voltage_dv = volts_dv;
  charger_ctx->reported_current_da = amps_da;
}

static void clear_charge_command(charger_t *charger_ctx) {
  if (charger_ctx == NULL || charger_ctx->elcon == NULL) {
    return;
  }

  charger_ctx->elcon->max_voltage_dv = 0;
  charger_ctx->elcon->max_current_da = 0;
  charger_ctx->elcon->charge_disable = true;
  elcon_send_command(charger_ctx->elcon);
}

/**
 * @brief checks if charging is permitted
 *
 * true when the pack and vehicle interlocks allow charging (faults clear, elcon
 * ready, etc) and the GUI has sent a valid setpoint
 *
 * @return true if charging is permitted
 * @return false otherwise
 */
bool is_charging_permitted(charger_t *charger_ctx) {
  return charger_ctx->setpoint_valid && !is_pack_full() && is_pack_okay() &&
         is_elcon_ready(charger_ctx);
}
