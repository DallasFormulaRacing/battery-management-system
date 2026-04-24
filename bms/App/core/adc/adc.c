#include "adc.h"

float adc_convert_voltage(const uint16_t raw) {
  float scalar = ADC_REF_V * (float)raw;
  return scalar / ADC_RESOLUTION;
}

float adc_convert_to_diff_voltage(int16_t raw) {
  return ((float)raw * ADC_REF_V) / ADC_RESOLUTION;
}

int16_t adc_one_shot_diff() {
  HAL_ADC_Start(&hadc1);
  if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK) {
    return (int16_t)HAL_ADC_GetValue(&hadc1);
  }
  return 0;
}