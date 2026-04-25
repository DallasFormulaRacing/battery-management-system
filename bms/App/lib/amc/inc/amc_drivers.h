#ifndef AMC_DRIVERS_H
#define AMC_DRIVERS_H

#include "adc.h"
#include <stdbool.h>
#include <stdint.h>

#define RESISTOR_DIV_RATIO 1001
#define FAILSAFE_THRESHOLD -2.57F

#define HV_BUS_ADC_HANDLE hadc1
#define PRECHARGE_CAP_ADC_HANDLE hadc2

float get_hv_bus_voltage();
float get_hv_prchrg_cap_voltage();

#endif