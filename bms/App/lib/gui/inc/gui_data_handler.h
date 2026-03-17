#ifndef GUI_DATA_HANDLER_H
#define GUI_DATA_HANDLER_H

#include <stdint.h>


void cell_voltage_readings(int start_idx, int end_idx, uint8_t *data_arr);

void therm_temp_readings_(int start_idx, int end_idx, uint8_t *data_arr);

#endif