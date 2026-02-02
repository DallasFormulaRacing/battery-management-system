#pragma once // THERMISTOR_H

#include "bms_types.h"

#define NUM_THERMISTORS 1 // TODO: Change

typedef struct {
  temperature_readings_t temperature;
  resistance_readings_t resistance;
  // voltage_readings_t voltage;
} thermistor_t;

temperature_readings_t get_temp_from_resistance(resistance_readings_t resistance);
temperature_readings_t get_temp_from_voltage(voltage_readings_t voltage);
