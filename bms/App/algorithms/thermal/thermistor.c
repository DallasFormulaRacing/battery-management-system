#include "thermistor.h"
#include "bms_types.h"
#include "parse.h"
#include <stddef.h>

// TODO: Do testing and generate more accurate values
thermistor_t temperature_resistance_lut[] = {
    {-50, 125000}, {-25, 90000}, {0, 11700}, {25, 10000}, {50, 3000},
    {75, 1100},    {100, 1000},  {125, 400}, {150, 200},
};

static resistance_readings_t
get_resistance_from_voltage(voltage_readings_t voltage) {
  float v_ref = 3.0F;
  float v_t = convert_voltage_human_readable(voltage);
  return (resistance_readings_t)((10000 * v_t) / (v_ref - v_t));
}

temperature_readings_t get_temp_from_resistance(resistance_readings_t resistance) {
  // Binary Search algorithm
  size_t num_lut_elems = sizeof(temperature_resistance_lut) /
                         sizeof(temperature_resistance_lut[0]);

  size_t high = num_lut_elems - 1;
  size_t low = 0;
  size_t mid = (high + low) / 2;

  if (resistance >= temperature_resistance_lut[low].resistance) {
    return temperature_resistance_lut[low].temperature;
  }

  if (resistance <= temperature_resistance_lut[high].resistance) {
    return temperature_resistance_lut[high].temperature;
  }

  while (low <= high) {
    mid = (high + low) / 2;

    if (resistance == temperature_resistance_lut[mid].resistance) {
      return temperature_resistance_lut[mid].temperature;
    }

    if (temperature_resistance_lut[mid].resistance > resistance) {
      low = mid + 1;
    } else {
      high = mid - 1;
    }
  }

  return temperature_resistance_lut[low].temperature;
}

temperature_readings_t get_temp_from_voltage(voltage_readings_t voltage) {
  resistance_readings_t resistance = get_resistance_from_voltage(voltage);
  return get_temp_from_resistance(resistance);
}
