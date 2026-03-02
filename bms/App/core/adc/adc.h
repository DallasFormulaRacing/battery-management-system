#ifndef ADC_H
#define ADC_H

#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_adc.h"

#define C_VREF_ADC 3.3F
#define C_ADC_RESOLUTION_12B 12

extern ADC_HandleTypeDef hadc1;
void adc_poll_no_DMA();
void adc_start_no_DMA();
void adc_stop_no_DMA();
uint16_t adc_get_no_DMA();
uint16_t adc_one_shot();
float adc_convert_voltage(const uint16_t raw);

#endif