#ifndef AMC_DRIVERS_H
#define AMC_DRIVERS_H

#include "adc.h"
#include <stdint.h>

#define RESISTOR_DIV_RATIO 1001

uint16_t get_hv_voltage();

#endif