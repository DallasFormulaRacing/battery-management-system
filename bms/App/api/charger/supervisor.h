#ifndef CHARGING_SUPERVISOR_H
#define CHARGING_SUPERVISOR_H

#include "elcon_comms.h"
#include <stdbool.h>

void charger_supervisor_fsm(void);

bool is_charging_permitted();

typedef enum {
  CHARGING_STATE_IDLE,
  CHARGING_STATE_READY2CHARGE,
  CHARGING_STATE_REQUEST4POWER,
  CHARGING_STATE_BALANCING,
  CHARGING_STATE_FAULT
} charging_state_t;

typedef struct {
  charging_state_t state;
  elcon_internal_t elcon;
  uint16_t reported_voltage;
  uint16_t requested_voltage;
  uint16_t reported_current;
  uint16_t requested_current;
} charger_t;

#endif