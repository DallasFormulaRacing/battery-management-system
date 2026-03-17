#ifndef GUI_DATA_HANDLER_H
#define GUI_DATA_HANDLER_H

#include <stdint.h>


void cell_voltage_readings(cell_asic_ctx_t *asic, int start_seg, int end_seg, uint_8 *data_arr);

void therm_temp_readings(cell_asic_ctx_t *asic, int start_seg, int end_seg, uint_8 *data_arr);

void metadata_readings(cell_asic_ctx_t *asic, uint8_t data_arr);

#endif