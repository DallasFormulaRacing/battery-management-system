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
  uint16_t reported_voltage;
  uint16_t requested_voltage;
  uint16_t reported_current;
  uint16_t requested_current;
} charger_t;

typedef struct {
  uint16_t voltage;
  uint16_t current;
} charger_power_setpoint_t;

extern osMessageQueueId_t charger_power_setpoint_queueHandle;
const extern osMessageQueueAttr_t charger_power_setpoint_queue_attributes;

void charger_update_requested_setpoints(uint16_t volts, uint16_t amps);

void charging_fsm_init(charger_t *hchg);
void charging_fsm_transition(charger_t *hchg, charging_state_t new_state);

bms_fault_t charger_supervisor_fsm(charger_t *hchg);

bool is_charging_permitted(charger_t *hchg);

void charging_state_standby(charger_t *hchg);
void charging_state_ready2charge(charger_t *hchg);
void charging_state_request4power(charger_t *hchg);
void charging_state_balancing(charger_t *hchg);
void charging_state_fault(charger_t *hchg);

#endif
