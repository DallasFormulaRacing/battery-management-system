#include "fdcan.h"

extern FDCAN_HandleTypeDef hfdcan2; //check if this configured to fdcan or standard

/**
 * @brief Sets default values for the FDCAN header
 */
void CAN_InitHeader(FDCAN_TxHeaderTypeDef *tx_header) {
    tx_header->IdType = FDCAN_EXTENDED_ID;
    tx_header->TxFrameType = FDCAN_DATA_FRAME;
    tx_header->ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    tx_header->BitRateSwitch = FDCAN_BRS_ON;
    tx_header->FDFormat = FDCAN_FD_CAN;
    tx_header->TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    tx_header->MessageMarker = 0;
}

/**
 * @brief Logic-heavy transmission wrapper
 */
HAL_StatusTypeDef CAN_Transmit(uint32_t commandHeader, uint8_t* pData, uint32_t dlc_bytes) {
    FDCAN_TxHeaderTypeDef txHeader;
    CAN_InitHeader(&txHeader); 
    
    txHeader.Identifier = commandHeader;
    txHeader.DataLength = dlc_bytes;

    return HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan2, &txHeader, pData);
}

/**
 * @brief Move the RX Callback here as well to keep it away from generated code
 */
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs) {
    if ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET) {
        FDCAN_RxHeaderTypeDef rxHeader;
        uint8_t rxData[64];

        // Pop the fifo to clear queue
        if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &rxHeader, rxData) == HAL_OK) {
            Process_CAN_Command(rxHeader.Identifier, rxData);
        }
    }
}

/**
 * @brief Parset the command and execute appropriate logic
 */
void Process_CAN_Command(uint32_t rx_id, uint8_t *data){
    //check if bms device is the target and source is gui
    //check if command is read svoltages, cvoltages, or other
    //if so, move to data gathering
    //have switch case statement for command id and execute appropriate logic for each command
    //find a way to access hbms, or have it as an argument a function in the switch case
}