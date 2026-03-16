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