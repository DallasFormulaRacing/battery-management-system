#include "fdcan.h"
#include "stm32g4xx_hal_fdcan.h"

FDCAN_HandleTypeDef hfdcan2; // check if this configured to fdcan or standard

// rx handler registration (application-provided)
static fdcan_rx_handler_t s_rx_handler = NULL;
static void *s_rx_ctx = NULL;

/**
 * @brief Set default values for an FDCAN TX header (caller fills identifier + data length).
 */
void can_init_tx_header(FDCAN_TxHeaderTypeDef *tx_header)
{
    tx_header->IdType            = FDCAN_EXTENDED_ID;
    tx_header->TxFrameType       = FDCAN_DATA_FRAME;
    tx_header->ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    tx_header->BitRateSwitch     = FDCAN_BRS_ON;
    tx_header->FDFormat          = FDCAN_FD_CAN;
    tx_header->TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    tx_header->MessageMarker     = 0;
}

/**
 * @brief Logic-heavy transmission wrapper.
 */
HAL_StatusTypeDef can_transmit(uint32_t identifier, uint8_t *data, uint32_t dlc_bytes)
{
    FDCAN_TxHeaderTypeDef tx_header;
    can_init_tx_header(&tx_header);

    tx_header.Identifier = identifier;
    tx_header.DataLength = dlc_bytes;

    return HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan2, &tx_header, data);
}

/**
 * @brief Dispatch a received CAN frame to the registered application RX handler (if any).
 */
static void fdcan_rx_dispatch(const FDCAN_RxHeaderTypeDef *rx_header, uint8_t *rx_data)
{
    if (s_rx_handler) {
        s_rx_handler(rx_header, rx_data, s_rx_ctx);
    }
}

/**
 * @brief Move the RX callback here as well to keep it away from generated code.
 */
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t rx_fifo0_its)
{
    if ((rx_fifo0_its & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET) {
        FDCAN_RxHeaderTypeDef rx_header;
        uint8_t rx_data[64];

        // pop the fifo to clear queue
        if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &rx_header, rx_data) == HAL_OK) {
            fdcan_rx_dispatch(&rx_header, rx_data);
        }
    }
}

/**
 * @brief Register the application-level CAN RX handler.
 */
void fdcan_register_rx_handler(fdcan_rx_handler_t handler, void *ctx)
{
    s_rx_handler = handler;
    s_rx_ctx = ctx;
}

/**
 * @brief FDCAN filter configuration to accept only messages to BMS (from any source).
 */
void fdcan_configure_filter(void)
{
    FDCAN_FilterTypeDef filter_config;
    filter_config.IdType       = FDCAN_EXTENDED_ID;
    filter_config.FilterIndex  = 0;
    filter_config.FilterType   = FDCAN_FILTER_MASK;
    filter_config.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;

    uint32_t target_id_mask = (0x1F << 21); // mask for target id
    // uint32_t source_id_mask = (0x1F << 5); // mask for source id

    filter_config.FilterID1 = (BMS_DEVICE_ID << 21); // accept only messages from gui to bms
    filter_config.FilterID2 = target_id_mask;        // mask to ignore all bits except target and source id

    if (HAL_FDCAN_ConfigFilter(&hfdcan2, &filter_config) != HAL_OK) {
        // handle error
    }
}