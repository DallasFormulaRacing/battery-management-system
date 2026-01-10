#ifndef PEC_H
#define PEC_H

#include <stdbool.h>
#include <stdint.h>

uint16_t calc_PEC15(uint8_t len, const uint8_t *data);
uint16_t calc_PEC10(bool reciever, uint8_t len, const uint8_t *data);

extern const uint16_t CMD_PEC15_LUT[256];
extern const uint16_t DATA_PEC10_LUT[256];

#endif /* PEC_H */
