#include "thermistor.h"
#include "bms_types.h"
#include "parse.h"
#include <stddef.h>

// TODO: Do testing and generate more accurate values
thermistor_t temperature_resistance_lut[] = {
    {-50.0F, 125000}, {-25.0F, 90000}, {0.0F, 11700},
    {25.0F, 10000},   {50.0F, 3000},   {75.0F, 1100},
    {100.0F, 1000},   {125.0F, 400},   {150.0F, 200},
};

static resistance_readings_t
get_resistance_from_voltage(voltage_readings_t voltage) {
  float v_ref = 3.0F;
  float v_t = convert_voltage_human_readable(voltage);
  return (resistance_readings_t)((10000 * v_t) / (v_ref - v_t));
}

static temperature_readings_t binary_search(resistance_readings_t resistance) {
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

temperature_readings_t get_thermistor_temperature(voltage_readings_t voltage) {
  resistance_readings_t resistance = get_resistance_from_voltage(voltage);
  return binary_search(resistance);
}
