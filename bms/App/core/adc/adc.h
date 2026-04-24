#ifndef ADC_H
#define ADC_H

#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_adc.h"
#include "stm32g4xx_hal_adc_ex.h"
#include <stdint.h>

#define ADC_BITS_DP 12
#define ADC_RESOLUTION (1 << (ADC_BITS_DP - 1))
#define ADC_REF_V 3.3F

extern ADC_HandleTypeDef hadc1;

#include "adc.h"

void adc_poll_no_DMA();
void adc_start_no_DMA();
void adc_stop_no_DMA();

uint16_t adc_one_shot();

uint16_t adc_get_no_DMA();
float adc_convert_voltage(uint16_t raw);

#endif
