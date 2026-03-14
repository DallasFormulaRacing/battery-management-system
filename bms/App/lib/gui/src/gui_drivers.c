#include "gui_drivers.h"


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
    //switch case statement here to process the command and data received from the BMS
    return;
}