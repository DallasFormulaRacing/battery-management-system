#include <stdint.h>

#include "bms_types.h"

cell_asic_ctx_t *asic_array = hbms.asic;

void cell_voltage_readings(cell_asic_ctx_t *asic, int start_seg, int end_seg, uint_8 *data_arr){
    for (int i = start_seg; i < end_seg; i++){
        //grab cell reading from asic array
        for (int j = 0; j < ADBMS_NUM_CELLS_PER_IC; j++){
        uint16_t voltage = asic[i].filtered_cell[j];

        //convert 16 bit signed int into 2 bytes, big endian
        //conversion here:
        uint8_t byte_0 = (uint8_t)((voltage >> 8) & 0xFF);
        uint8_t byte_1 = (uint8_t)(voltage & 0xFF);

        //writing to data array
        data_arr[j] = byte_0;
        data_arr[j+1] = byte_1;
    }

    }
}

/*
void therm_temp_readings(cell_asic_ctx_t *asic, int start_seg, int end_seg, uint_8 *data_arr){
    for (int i = start_seg; i < end_seg; i++){
        //grab cell reading from asic array
        for (int j = 0; j < ADBMS_NUM_CELLS_PER_IC; j++){
        //uint16_t voltage = asic[i].filtered_cell[j];

        //convert 16 bit signed int into 2 bytes, big endian
        uint8_t byte_0 = (uint8_t)((voltage >> 8) & 0xFF);
        //uint8_t byte_1 = (uint8_t)(voltage & 0xFF);

        //writing to data array
        data_arr[j] = byte_0;
        data_arr[j+1] = byte_1;
    }

    }
}

*/