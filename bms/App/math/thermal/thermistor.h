/* Auto-generated thermistor LUT */
/* DO NOT EDIT MANUALLY */

#ifndef THERMISTOR_H
#define THERMISTOR_H

#include <stdint.h>

#define THERM_LUT_SHIFT      6
#define THERM_LUT_BASE_CODE -141
#define THERM_LUT_SIZE      288

extern const float thermistor_lut[THERM_LUT_SIZE];

float thermistor_from_adc(int16_t adc16);

#endif /* THERMISTOR_H */
