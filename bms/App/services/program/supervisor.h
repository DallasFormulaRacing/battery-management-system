#ifndef CHARGING_SUPERVISOR_H
#define CHARGING_SUPERVISOR_H

#include "bms.h"
#include "cmsis_os2.h"
#include "elcon_comms.h"
#include <stdbool.h>
#include <stdint.h>

typedef enum {
  CHARGING_STATE_STANDBY,
  CHARGING_STATE_READY2CHARGE,
  CHARGING_STATE_REQUEST4POWER,
  CHARGING_STATE_BALANCING,
  CHARGING_STATE_FAULT
} charging_state_t;

typedef struct {
  charging_state_t state;
  elcon_internal_t *elcon;
  bool setpoint_valid; /* GUI sent an in-range setpoint this session */
  /* all voltages 0.1 V/bit, all currents 0.1 A/bit */
  uint16_t reported_voltage_dv; /* charger output readback */
  uint16_t requested_voltage_dv;
  uint16_t reported_current_da;
  uint16_t requested_current_da;
} charger_t;

typedef struct {
  uint16_t voltage_dv; /* 0.1 V/bit */
  uint16_t current_da; /* 0.1 A/bit */
} charger_power_setpoint_t;

extern osMessageQueueId_t charger_power_setpoint_queueHandle;
const extern osMessageQueueAttr_t charger_power_setpoint_queue_attributes;

void charger_update_requested_setpoints(uint16_t volts_dv, uint16_t amps_da);
bool charger_setpoint_in_range(uint16_t volts_dv, uint16_t amps_da);

void charging_fsm_init(charger_t *charger_ctx);
void charging_fsm_transition(charger_t *charger_ctx, charging_state_t new_state);
void charger_supervisor_stop(charger_t *charger_ctx);

bms_fault_t charger_supervisor_fsm(charger_t *charger_ctx);

bool is_charging_permitted(charger_t *charger_ctx);

void charging_state_standby(charger_t *charger_ctx);
void charging_state_ready2charge(charger_t *charger_ctx);
void charging_state_request4power(charger_t *charger_ctx);
void charging_state_balancing(charger_t *charger_ctx);
void charging_state_fault(charger_t *charger_ctx);

#endif
