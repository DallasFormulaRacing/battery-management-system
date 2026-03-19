#include "gui_data_handler.h"
#include "cb.h"
#include "config.h"

/*
 * @brief populates data_arr with cell voltage readings, 2 byte big endian
 * @param asic: array of asics
 * @param start_ic: asic to start with (inclusive)
 * @param end_ic: asic to end with (exclusive)
 * @param data_arr: buffer to populate cell voltages with
 * @return none

 */
void cell_voltage_readings(cell_asic_ctx_t *asic, int start_ic, int end_ic, uint8_t *data_arr){

    //counter array to keep track of index outside of each segment loop
    uint8_t cell_counter = 0;
    for (int ic = start_ic; ic < end_ic; ic++){
        //grab cell reading from asic array
        for (int cell_idx = 0; cell_idx < ADBMS_NUM_CELLS_PER_IC; cell_idx++){
        int16_t voltage = asic[ic].filt_cell.filt_cell_voltages_array[cell_idx];

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
#define NUM_THERM_PER_IC 10

/*
 * @brief populates data_arr with cell thermistor readings, most significant byte only
 * @param asic: array of asics
 * @param start_ic: asic to start with (inclusive)
 * @param end_ic: asic to end with (exclusive)
 * @param data_arr: buffer to populate cell thermistor temperatures with
 * @return none
 */

void therm_temp_readings(cell_asic_ctx_t *asic, int start_ic, int end_ic, uint8_t *data_arr){
    int therm_counter = FOUR_BYTE_OFFSET;
    for (int ic = start_ic; ic < end_ic; ic++){

        for (int therm_num = 0; therm_num < NUM_THERM_PER_IC; therm_num++){
            uint16_t temp = asic[ic].aux.aux_voltages_array[therm_num];

            //only take top 8 MSB
            uint8_t byte_0 = (uint8_t)((temp >> 8) & 0xFF);
            //uint8_t byte_1 = (uint8_t)(voltage & 0xFF);

        data_arr[therm_counter] = byte_0;
        //data_arr[j+1] = byte_1;

        therm_counter++;
    }

    }

}

/*
 * @brief populates data_arr with pack voltage, state of charge, and cell balancing boolean status
 * @param pack: bms pack struct
 * @param pcb: bms cell balancing struct
 * @param data_arr: buffer to populate metadata
 * @return none
 */
void metadata_readings(pack_data_t *pack, pcb_ctx_t *pcb, uint8_t *data_arr){
    uint16_t pack_voltage = pack->packvoltage;
    uint16_t soc = pack->state_of_charge;
    uint16_t current = pack->instantaneous_current;

    uint8_t pv0 = (uint8_t)((pack_voltage >> 8) & 0xFF);
    uint8_t pv1 = (uint8_t)(pack_voltage & 0xFF);

    uint8_t soc0 = (uint8_t)((soc >> 8) & 0xFF);
    uint8_t soc1 = (uint8_t)(soc & 0xFF);

    uint8_t c0 = (uint8_t)((current >> 8) & 0xFF);
    uint8_t c1 = (uint8_t)(current & 0xFF);

    data_arr[0] = pv0;
    data_arr[1] = pv1;
    data_arr[2] = soc0;
    data_arr[3] = soc1;
    data_arr[4] = c0;
    data_arr[5] = c1;

    //pack 144 cell bools into bytes
    /*
    bool *cell_balancing_status = pcb->cell_balancing_status;

    for (int i = 0; i < NUM_CELL_USING; i++) {
        int byte_index = (i / 8) + 6; // 6 is the offset from the previous metadata
        int bit_index  = 7 - (i % 8);   //pack left to right

        if (cell_balancing_status[i]) {
            data_arr[byte_index] |= (1U << bit_index);
        }
    }
    */
}