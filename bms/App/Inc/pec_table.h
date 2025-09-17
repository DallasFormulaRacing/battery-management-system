/**
 * @author Will Kim
 * @email <wkim@utdallas.edu>
 * @create date 2025-08-07 12:36:43
 * @modify date 2025-08-07 12:36:43
 * @desc PEC Table as defined in the AD BMS 6830 datasheet.
 */

#ifndef PEC_TABLE_H
#define PEC_TABLE_H

#include <stdint.h>
#include <stddef.h>

extern const uint16_t CMD_PEC15_LUT[256];
extern const uint16_t DATA_PEC10_LUT[256];

#endif // PEC_TABLE_H