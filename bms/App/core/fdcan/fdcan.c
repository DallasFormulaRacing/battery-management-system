#include "fdcan.h"
#include "stm32g4xx_hal_fdcan.h"

extern FDCAN_HandleTypeDef hfdcan2; //check if this configured to fdcan or standard

// function pointer variables
static fdcan_rx_handler_t s_rx_handler = NULL;
static void *s_rx_ctx = NULL;

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
            if (s_rx_handler)
                Process_CAN_Command(&rxHeader, rxData);
        }
    }
}

/**
 * @brief Moves processing away from generated code and into application code in bms_can_handler.c
 */
void Process_CAN_Command(const FDCAN_RxHeaderTypeDef *hdr, uint8_t* data){
    // forward to application handler
    if (s_rx_handler)
        s_rx_handler(hdr, data, s_rx_ctx);
}

/**
 * @brief Registers the application-level CAN RX handler 
 */
void FDCAN_RegisterRxHandler(fdcan_rx_handler_t handler, void *ctx)
{
    s_rx_handler = handler;
    s_rx_ctx = ctx;
}

/**
 * @brief FDCAN filter configuration to accept only messages to bms (from any source)
 */
void Configure_FDCAN_Filter(void){
    FDCAN_FilterTypeDef filterconfig;
    filterconfig.IdType = FDCAN_EXTENDED_ID;
    filterconfig.FilterIndex = 0;
    filterconfig.FilterType = FDCAN_FILTER_MASK;
    filterconfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;

    uint32_t targetIDMask = (0x1F << 21); // Mask for target ID
    //uint32_t sourceIDMask = (0x1F << 5);   // Mask for source ID

    filterconfig.FilterID1 = (BMS_DEVICE_ID << 21); // Accept only messages from GUI to BMS
    filterconfig.FilterID2 = targetIDMask; // Mask to ignore all bits except target and source ID

    if(HAL_FDCAN_ConfigFilter(&hfdcan2, &filterconfig) != HAL_OK) {
        // Handle error
    }
}