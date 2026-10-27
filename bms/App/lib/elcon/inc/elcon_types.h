#ifndef ELCON_TYPES_H
#define ELCON_TYPES_H

#include <stdint.h>

/* decoded status byte of the charger broadcast, 1 = fault */
typedef struct {
  uint8_t hw : 1;             /* bit0 hardware failure */
  uint8_t temp : 1;           /* bit1 over temperature protection */
  uint8_t input_voltage : 1;  /* bit2 AC input voltage wrong, charger stopped */
  uint8_t starting_state : 1; /* bit3 battery not connected or reversed */
  uint8_t comm_state : 1;     /* bit4 charger has not received our request */
  uint8_t charger_OKAY : 1;   /* derived: none of the above set */
} elcon_status_t;

typedef struct {
  /* outgoing request, same scaling as the CAN frame */
  uint16_t max_voltage_dv; /* 0.1 V/bit */
  uint16_t max_current_da; /* 0.1 A/bit */
  uint8_t charge_disable; /* 0 = charging on, 1 = charging off */

  /* latest charger broadcast */
  elcon_status_t heartbeat_msg;
  uint32_t heartbeat_tick;
  uint16_t output_voltage_dv; /* 0.1 V/bit */
  uint16_t output_current_da; /* 0.1 A/bit, magnitude only, direction given below */
  uint8_t output_discharging; /* 0 = charging, 1 = discharging */
} elcon_internal_t;

extern elcon_internal_t g_elcon;

#endif
