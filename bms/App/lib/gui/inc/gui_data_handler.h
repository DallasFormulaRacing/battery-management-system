#ifndef GUI_DATA_HANDLER_H
#define GUI_DATA_HANDLER_H

#include <stdint.h>
#include <stdbool.h>

#include "bms.h"
#include "bms_types.h"



void cell_voltage_readings(cell_asic_ctx_t *asic, int start_ic, int end_ic, uint8_t *data_arr);

void therm_temp_readings(cell_asic_ctx_t *asic, int start_ic, int end_ic, uint8_t *data_arr);

void metadata_readings(pack_data_t *pack, pcb_ctx_t *pcb, uint8_t *data_arr);

#endif