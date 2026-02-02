#pragma once // THERMISTOR_H

#include "bms_types.h"

#define NUM_THERMISTORS 1 // TODO: Change

typedef struct {
  temperature_readings_t temperature;
  resistance_readings_t resistance;
  // voltage_readings_t voltage;
} thermistor_t;

temperature_readings_t get_thermistor_temperature(voltage_readings_t voltage);
