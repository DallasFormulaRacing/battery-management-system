#include "adc.h"
#include "stm32g474xx.h"

float adc_convert_voltage(const uint16_t raw) {
  float scalar = ADC_REF_V * (float)raw;
  return scalar / ADC_RESOLUTION;
}

float adc_convert_to_diff_voltage(int16_t raw) {
  return ((float)raw * ADC_REF_V) / ADC_RESOLUTION;
}

int16_t adc_one_shot_diff(ADC_HandleTypeDef *hadc) {
  HAL_ADC_Start(hadc);
  if (HAL_ADC_PollForConversion(hadc, 10) == HAL_OK) {
    return (int16_t)HAL_ADC_GetValue(hadc);
  }
  return 0;
}