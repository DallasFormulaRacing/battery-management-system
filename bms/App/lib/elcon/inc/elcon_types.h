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
  uint8_t enable;
  elcon_status_t heartbeat_msg;
} elcon_internal_t;

elcon_internal_t hard_stop =
    (elcon_internal_t){.enable = 0, .max_current = 0, .max_voltage = 0};

#endif