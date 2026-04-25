#include "amc_drivers.h"
#include "stm32g4xx_hal_adc.h"

static float get_hv_voltage(bool *is_fault, ADC_HandleTypeDef *hadc) {
  int16_t raw_diff = adc_one_shot_diff(hadc);
  float v_diff = adc_convert_to_diff_voltage(raw_diff);

  if (v_diff < FAILSAFE_THRESHOLD) {
    *is_fault = true;
    return 0.0F;
  }

  *is_fault = false;
  return v_diff * RESISTOR_DIV_RATIO;
}

float get_hv_bus_voltage() {
  bool fault;
  return get_hv_voltage(&fault, &HV_BUS_ADC_HANDLE);
}
float get_hv_prchrg_cap_voltage() {
  bool fault;
  return get_hv_voltage(&fault, &PRECHARGE_CAP_ADC_HANDLE);
}