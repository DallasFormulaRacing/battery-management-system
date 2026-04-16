#include "gui_drivers.h"
#include "cb.h"
#include "config.h"

static void send_filtered_voltage_frame(uint8_t start_ic, uint8_t end_ic,
                                        can_command_id_t resp_id);
static void send_therm_temp_frame(uint8_t start_ic, uint8_t end_ic,
                                  can_command_id_t resp_id);
static void send_metadata_frame(can_command_id_t resp_id);

/*
 * NON-RTOS IMPL
 */
// void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan,
//                                uint32_t rx_fifo0_its) {
//   if ((rx_fifo0_its & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) == 0U) {
//     return;
//   }

//   FDCAN_RxHeaderTypeDef rx_header;
//   uint8_t rx_data[64];

//   if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &rx_header, rx_data) ==
//       HAL_OK) {
//     gui_process_can_command(
//         rx_header.Identifier,
//         rx_data); // rx_data never used since only cmd id matters
//   } else {
//     // send rxfifo received error
//   }
// }

/*
 * @brief uses can frame id to perform command
 * @param ext_id: can frame id to branch off of
 * @param data: buffer with any data given along with the command (unused for
 * now)
 * @return none
 */
void gui_process_can_command(uint32_t ext_id, uint8_t *data) {
  // redundant checks for header id
  if (!can_id_is_valid(ext_id)) {
    send_can_error(ERROR_ID_INVALID_ID);
    return;
  }
  if (can_id_get_target(ext_id) != BMS_DEVICE_ID) {
    send_can_error(ERROR_ID_INVALID_TARGET);
    return;
  }

  switch (can_id_get_cmd(ext_id)) {
  case CMD_ID_FIRST_24_CELLS:
    send_filtered_voltage_frame(0, 2, CMD_ID_FIRST_24_CELLS);
    break;
  case CMD_ID_SECOND_24_CELLS:
    send_filtered_voltage_frame(2, 4, CMD_ID_SECOND_24_CELLS);
    break;
  case CMD_ID_THIRD_24_CELLS:
    send_filtered_voltage_frame(4, 6, CMD_ID_THIRD_24_CELLS);
    break;
  case CMD_ID_FOURTH_24_CELLS:
    send_filtered_voltage_frame(6, 8, CMD_ID_FOURTH_24_CELLS);
    break;
  case CMD_ID_FIFTH_24_CELLS:
    send_filtered_voltage_frame(8, 10, CMD_ID_FIFTH_24_CELLS);
    break;
  case CMD_ID_SIXTH_24_CELLS:
    send_filtered_voltage_frame(10, 12, CMD_ID_SIXTH_24_CELLS);
    break;
  case CMD_ID_FIRST_60_TEMPS:
    // technically means seg [0,6)
    send_therm_temp_frame(0, 6, CMD_ID_FIRST_60_TEMPS);
    break;
  case CMD_ID_LAST_60_TEMPS:
    // technically means seg [6,12)
    send_therm_temp_frame(6, 12, CMD_ID_LAST_60_TEMPS);
    break;
  case CMD_ID_PACK_METADATA:
    send_metadata_frame(CMD_ID_PACK_METADATA);
    break;
  case CMD_ID_IMD_DATA:
    break;
  default:
    send_can_error(ERROR_ID_INVALID_CMD);
    return;
  }
}

/*
 * @ Builds and populates buffer for cell voltages of a given ic range, then
 * transmits the frame
 * @param start_ic: beginning ic (inclusive)
 * @param end_ic: asic to start with (exclusive)
 * @param resp_id: can frame id to send to gui
 * @return none
 */
void send_filtered_voltage_frame(uint8_t start_ic, uint8_t end_ic,
                                 can_command_id_t resp_id) {
  cell_asic_ctx_t *asic_array = hbms.asic;

  // build data function call with first 24 cell configured in parameters
  uint8_t tx_frame[48];
  cell_voltage_readings(asic_array, start_ic, end_ic, tx_frame);

  // send can frame with first_24_cells as command id
  can_ext_id_t tx_header = can_id_build(CAN_PRIORITY_P0, GUI_DEVICE_ID,
                                        (uint16_t)resp_id, BMS_DEVICE_ID);
  fdcan_send(tx_header, tx_frame, FDCAN_DLC_BYTES_48);
}

/*
 * @ Builds and populates buffer for thermistor temperatures of a given ic
 * range, then transmits the frame
 * @param start_ic: beginning ic (inclusive)
 * @param end_ic: asic to start with (exclusive)
 * @param resp_id: can frame id to send to gui
 * @return none
 */
static void send_therm_temp_frame(uint8_t start_ic, uint8_t end_ic,
                                  can_command_id_t resp_id) {
  cell_asic_ctx_t *asic_array = hbms.asic;

  uint8_t tx_frame[64] = {0}; // since first 4 bytes are 0
  therm_temp_readings(asic_array, start_ic, end_ic, tx_frame);

  can_ext_id_t tx_header = can_id_build(CAN_PRIORITY_P0, GUI_DEVICE_ID,
                                        (uint16_t)resp_id, BMS_DEVICE_ID);
  fdcan_send(tx_header, tx_frame, FDCAN_DLC_BYTES_64);
}

/*
 * @ Builds and populates buffer for pack voltage, soc, and cb status for all
 * cells, then transmits the frame
 * @param resp_id: can frame id to send to gui
 * @return none
 */
void send_metadata_frame(can_command_id_t resp_id) {
  pack_data_t *pack_data = hbms.pack;
  pcb_ctx_t *pcb = hbms.pcb;
  uint8_t tx_frame[24] = {0};
  metadata_readings(pack_data, pcb, tx_frame);

  can_ext_id_t tx_header = can_id_build(CAN_PRIORITY_P0, GUI_DEVICE_ID,
                                        (uint16_t)resp_id, BMS_DEVICE_ID);
  fdcan_send(tx_header, tx_frame, FDCAN_DLC_BYTES_24);
}

/*
 * @ Sends can frame to gui for certain can errors
 * @param resp_id: can frame id to send to gui
 * @return none
 */
void send_can_error(can_error_id_t error_id) {
  uint8_t tx_frame = 0;
  can_ext_id_t tx_header = can_id_build(CAN_PRIORITY_P0, GUI_DEVICE_ID,
                                        (uint16_t)error_id, BMS_DEVICE_ID);
  fdcan_send(tx_header, &tx_frame, FDCAN_DLC_BYTES_0);
}

/*
 * @brief populates data_arr with cell voltage readings, 2 byte big endian
 * @param asic: array of asics
 * @param start_ic: asic to start with (inclusive)
 * @param end_ic: asic to end with (exclusive)
 * @param data_arr: buffer to populate cell voltages with
 * @return none

 */
void cell_voltage_readings(cell_asic_ctx_t *asic, uint8_t start_ic,
                           uint8_t end_ic, uint8_t *data_arr) {

  // counter array to keep track of index outside of each segment loop
  uint8_t cell_counter = 0;
  for (uint8_t ic = start_ic; ic < end_ic; ic++) {
    // grab cell reading from asic array
    for (uint8_t cell_idx = 0; cell_idx < ADBMS_NUM_CELLS_PER_IC; cell_idx++) {
      int16_t voltage = asic[ic].filt_cell.filt_cell_voltages_array[cell_idx];

      // convert 16 bit signed uint8_t into 2 bytes, big endian
      // conversion here:
      uint8_t byte_0 = (uint8_t)((voltage >> 8) & 0xFF);
      uint8_t byte_1 = (uint8_t)(voltage & 0xFF);

      // writing to data array
      data_arr[cell_counter] = byte_0;
      data_arr[cell_counter + 1] = byte_1;
      cell_counter += 2;
    }
  }

  // if(cell_counter != 24) error_handler();
}

/*
 * @brief populates data_arr with cell thermistor readings, most significant
 * byte only
 * @param asic: array of asics
 * @param start_ic: asic to start with (inclusive)
 * @param end_ic: asic to end with (exclusive)
 * @param data_arr: buffer to populate cell thermistor temperatures with
 * @return none
 */

void therm_temp_readings(cell_asic_ctx_t *asic, uint8_t start_ic,
                         uint8_t end_ic, uint8_t *data_arr) {
  uint8_t therm_counter = THERM_CAN_OFFSET;
  for (uint8_t ic = start_ic; ic < end_ic; ic++) {

    for (uint8_t therm_num = 0; therm_num < NUM_THERM_PER_SEGMENT;
         therm_num++) {
      uint16_t temp = asic[ic].aux.aux_voltages_array[therm_num];

      // only take top 8 MSB
      uint8_t byte_0 = (uint8_t)((temp >> 8) & 0xFF);
      // uint8_t byte_1 = (uint8_t)(voltage & 0xFF);

      data_arr[therm_counter] = byte_0;
      // data_arr[j+1] = byte_1;

      therm_counter++;
    }
  }
}

/*
 * @brief populates data_arr with pack voltage, state of charge, and cell
 * balancing boolean status
 * @param pack: bms pack struct
 * @param pcb: bms cell balancing struct
 * @param data_arr: buffer to populate metadata
 * @return none
 */
void metadata_readings(pack_data_t *pack, pcb_ctx_t *pcb, uint8_t *data_arr) {
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

  // pack 144 cell bools into bytes
  /*
  bool *cell_balancing_status = pcb->cell_balancing_status;

  for (uint8_t i = 0; i < NUM_CELL_USING; i++) {
      uint8_t byte_index = (i / 8) + 6; // 6 is the offset from the previous
  metadata uint8_t bit_index  = 7 - (i % 8);   //pack left to right

      if (cell_balancing_status[i]) {
          data_arr[byte_index] |= (1U << bit_index);
      }
  }
  */
}