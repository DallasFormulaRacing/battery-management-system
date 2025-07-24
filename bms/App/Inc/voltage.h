/**
 * @author Will Kim
 * @email <wkim@utdallas.edu>
 * @create date 2025-07-23 03:10:13
 * @modify date 2025-07-23 03:15:50
 * @desc Voltage measurement header file
 */

#ifndef VOLTAGE_H
#define VOLTAGE_H

#include <stdint.h> 




typedef enum { 
    ADBMS6830_OK, 
    ADBMS6830_ERROR 
} adbms6830_state_t;

void adbms6830_init();
void adbms6830_start_adc_conversion();
uint32_t adbms6830_read_cell_voltage();
adbms6830_state_t adbms6830_get_status();

#endif