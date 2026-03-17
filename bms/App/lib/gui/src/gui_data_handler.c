#include <stdint.h>

#include "bms_types.h"

cell_asic_ctx_t *asic_array = hbms.asic;

/**
 * @brief Sends all cell voltages (for each IC) in the requested voltage domain.
 *
 * Payload format:
 *   byte0  = IC index
 *   bytes1..32 = 16 cell voltages, 2 bytes each, little-endian (total 32 bytes)
 * Total logical bytes = 33
 */
// void bms_send_voltages_all(cell_asic_ctx_t *asic,
//                            cell_voltage_type_t voltage_type) {
//   if (asic == NULL) {
//     return;
//   }

//   for (uint8_t ic = 0; ic < (uint8_t)IC_COUNT_CHAIN; ic++) {

//     uint8_t tx[CANFD_MAX_DATA_BYTES] = {0};
//     tx[0] = ic;

//     uint32_t off = 1U;

//     for (uint8_t current_cell = 0;
//          current_cell < (uint8_t)ADBMS_NUM_CELLS_PER_IC; current_cell++) {
//       int16_t voltage;

//       switch (voltage_type) {
//       case CELL:
//         voltage = (int16_t)asic->cell.cell_voltages_array[current_cell];
//         break;

//       case S_CELL:
//         voltage = (int16_t)asic->s_cell.s_cell_voltages_array[current_cell];
//         break;

//       case AVG_CELL:
//         voltage =
//         (int16_t)asic->avg_cell.avg_cell_voltages_array[current_cell]; break;

//       case FILTERED_CELL:
//         voltage =
//             (int16_t)
//                 asic->filtered_cell.filtered_cell_voltages_array[current_cell];
//         break;

//       default:
//         voltage = 0;
//         break;
//       }

//       tx[off] = (uint8_t)((uint16_t)voltage & 0xFFU);
//       tx[off + 1] = (uint8_t)(((uint16_t)voltage >> 8) & 0xFFU);
//       off += 2U;
//     }

//     bms_transmit_voltage_can_frame(voltage_type, tx);
//   }
// }



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