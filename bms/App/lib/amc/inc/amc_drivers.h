#ifndef AMC_DRIVERS_H
#define AMC_DRIVERS_H

#include "adc.h"
#include <stdbool.h>
#include <stdint.h>

#define RESISTOR_DIV_RATIO 1001
#define FAILSAFE_THRESHOLD -2.5F

float get_hv_voltage(bool *is_fault);

#endif