#include "can_utils.h"
#include "fdcan.h"
#include "command_handler.h"

extern FDCAN_HandleTypeDef hfdcan2;
extern NodeHardwareID_t self_node_id;

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
HAL_StatusTypeDef CAN_Transmit(uint8_t priority, uint32_t cmd_type, uint8_t* pData, uint32_t dlc_bytes) {
    FDCAN_TxHeaderTypeDef txHeader;
    CAN_InitHeader(&txHeader); 
    
    txHeader.Identifier = BUILD_CAN_ID(priority, cmd_type, self_node_id);
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

        if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &rxHeader, rxData) == HAL_OK) {
            Process_CAN_Command(rxHeader.Identifier, rxData);
        }
    }
}