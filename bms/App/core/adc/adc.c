#include "adc.h"

void adc_poll_no_DMA() {
  (void)HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
}
void adc_start_no_DMA() { //
  (void)HAL_ADC_Start(&hadc1);
}
void adc_stop_no_DMA() {
  //
  (void)HAL_ADC_Stop(&hadc1);
}

uint16_t adc_one_shot() {
  adc_start_no_DMA();
  adc_poll_no_DMA();
  return adc_get_no_DMA();
}

uint16_t adc_get_no_DMA() {
  //
  return HAL_ADC_GetValue(&hadc1);
}

float adc_convert_voltage(const uint16_t raw) {
  float scalar = ADC_REF_V * raw;
  return scalar / ADC_RESOLUTION;
}