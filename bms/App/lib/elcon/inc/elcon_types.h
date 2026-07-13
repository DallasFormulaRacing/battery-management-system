#ifndef ELCON_TYPES_H
#define ELCON_TYPES_H

#include <stdint.h>

typedef struct {
  uint8_t hw : 1;
  uint8_t temp : 1;
  uint8_t input_voltage : 1;
  uint8_t starting_state : 1;
  uint8_t comm_state : 1;
  uint8_t charger_OKAY : 1;
} elcon_status_t;

typedef struct {
  uint16_t max_voltage;
  uint16_t max_current;
  uint8_t charge_disable; /* 0 = charging on, 1 = charging off */
  elcon_status_t heartbeat_msg;
  uint32_t heartbeat_tick;
} elcon_internal_t;

extern elcon_internal_t hard_stop;
extern elcon_internal_t g_elcon;

#endif