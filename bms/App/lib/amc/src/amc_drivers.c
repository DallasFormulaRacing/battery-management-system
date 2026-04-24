#include "amc_drivers.h"

float get_hv_voltage(bool *is_fault) {
  int16_t raw_diff = adc_one_shot_diff();
  float v_diff = adc_convert_to_diff_voltage(raw_diff);

  if (v_diff < FAILSAFE_THRESHOLD) {
    *is_fault = true;
    return 0.0F;
  }

  *is_fault = false;
  return v_diff * RESISTOR_DIV_RATIO;
}
