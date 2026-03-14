#include "gui_drivers.h"
#include "gui_types.h"

/*
 * HAL callback invoked when a new message arrives in RX FIFO0
 * (assuming notifications are enabled).
 */
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t rx_fifo0_its)
{
    if ((rx_fifo0_its & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) == 0U) {
        return;
    }

    FDCAN_RxHeaderTypeDef rx_header;
    uint8_t rx_data[64];

    if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &rx_header, rx_data) == HAL_OK) {
            // Execute registered callback assuming its been assigned
            //if (rx_handler != NULL) {
            //    rx_handler(&rx_header, rx_data, rx_ctx);
            //}
            //IMPLEMENT THIS: 
            process_can_command(rx_header.Identifier, rx_data);
    }
}


void process_can_command(uint32_t ext_id, uint8_t* data){
    //redundant checks for header id
    if(!can_id_is_valid(ext_id)) return;
    if (can_id_get_target(ext_id) != BMS_DEVICE_ID) return;

    switch(can_id_get_cmd(ext_id)){
        case CMD_ID_FIRST_24_CELLS:
            //build data function call with first 24 cell configured in parameters
            //send can frame with first_24_cells_resp as command id
            //same for other branches
            break;
        case CMD_ID_SECOND_24_CELLS:
            break;
        case CMD_ID_THIRD_24_CELLS:
            break;
        case CMD_ID_FOURTH_24_CELLS:
            break;
        case CMD_ID_FIFTH_24_CELLS:
            break;
        case CMD_ID_SIXTH_24_CELLS:
            break;
        case CMD_ID_FIRST_60_TEMPS:
            break;
        case CMD_ID_LAST_60_TEMPS:
            break;
        case CMD_ID_PACK_METADATA:
            break;
        case CMD_ID_IMD_DATA:
            break;
        default: return;
    }
    //switch case statement here to process the command and data received from the BMS
    return;
}