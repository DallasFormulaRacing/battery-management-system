#include <stdint.h>

#include "bms_types.h"

void cell_voltage_readings(cell_asic_ctx_t *asic, int start_seg, int end_seg, uint8_t *data_arr){

    //counter array to keep track of index outside of each segment loop
    uint8_t cell_counter = 0;
    for (int seg_idx = start_seg; seg_idx < end_seg; seg_idx++){
        //grab cell reading from asic array
        for (int cell_idx = 0; cell_idx < ADBMS_NUM_CELLS_PER_IC; cell_idx++){
        int16_t voltage = asic[seg_idx].filtered_cell[cell_idx];

        //convert 16 bit signed int into 2 bytes, big endian
        //conversion here:
        uint8_t byte_0 = (uint8_t)((voltage >> 8) & 0xFF);
        uint8_t byte_1 = (uint8_t)(voltage & 0xFF);

        //writing to data array
        data_arr[cell_counter] = byte_0;
        data_arr[cell_counter+1] = byte_1;
        cell_counter += 2;
    }

    }

    //if(cell_counter != 24) error_handler();
}


#define FOUR_BYTE_OFFSET 4

void therm_temp_readings(cell_asic_ctx_t *asic, int start_seg, int end_seg, uint8_t *data_arr){
    int cell_counter = FOUR_BYTE_OFFSET;
    for (int i = start_seg; i < end_seg; i++){
        //grab cell reading from asic array
        for (int j = 0; j < ADBMS_NUM_CELLS_PER_IC; j++){
        uint16_t temp = asic[i]//.thermistor reading, semicolon just for 

        //only take top 8 MSB
        uint8_t byte_0 = (uint8_t)((temp >> 8) & 0xFF);
        //uint8_t byte_1 = (uint8_t)(voltage & 0xFF);

        //writing to data array
        data_arr[cell_counter] = byte_0;
        //data_arr[j+1] = byte_1;

        cell_counter++;
    }

    }
}


void metadata_readings(pack_data_t pack, pcb_ctx_t pcb, uint8_t *data_arr){

        uint16_t voltage = asic[i].filtered_cell[j];

        //convert 16 bit signed int into 2 bytes, big endian
        //conversion here:
        uint8_t byte_0 = (uint8_t)((voltage >> 8) & 0xFF);
        uint8_t byte_1 = (uint8_t)(voltage & 0xFF);

        //writing to data array
        data_arr[j] = byte_0;
        data_arr[j+1] = byte_1;

}