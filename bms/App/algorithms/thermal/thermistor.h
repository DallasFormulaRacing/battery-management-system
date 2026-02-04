#pragma once // THERMISTOR_H

#include "bms_types.h"

#define NUM_THERMISTORS 1 // TODO: Change

typedef struct {
  voltage_human_readable_t voltage;
  temp_c_readings_t temperature;
  // resistance_readings_t resistance;
} thermistor_t;

resistance_readings_t get_resistance_from_voltage(voltage_readings_t voltage);
temp_c_readings_t get_temp_from_voltage(voltage_human_readable_t voltage);
