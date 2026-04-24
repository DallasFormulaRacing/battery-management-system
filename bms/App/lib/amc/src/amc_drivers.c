#include "amc_drivers.h"
#include "adc.h"
#include <stdint.h>

uint16_t get_hv_voltage() {
  return (uint16_t)adc_convert_voltage(adc_one_shot()) * RESISTOR_DIV_RATIO;
}