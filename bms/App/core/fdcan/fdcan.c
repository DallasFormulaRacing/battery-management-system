#include "fdcan.h"

#include "stm32g4xx_hal_fdcan.h"
#include <string.h>

// RX handler registration (application-provided)
static fdcan_rx_handler_t rx_handler = NULL;
static void *rx_ctx = NULL;


// assign callback function to rx handler outside this program
void fdcan_register_rx_handler(fdcan_rx_handler_t handler, void *ctx)
{
    rx_handler = handler;
    rx_ctx = ctx;
}


static void fdcan_init_tx_header(FDCAN_TxHeaderTypeDef *tx_header)
{
    tx_header->IdType              = FDCAN_EXTENDED_ID;
    tx_header->TxFrameType         = FDCAN_DATA_FRAME;
    tx_header->ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    tx_header->BitRateSwitch       = FDCAN_BRS_ON;
    tx_header->FDFormat            = FDCAN_FD_CAN;
    tx_header->TxEventFifoControl  = FDCAN_NO_TX_EVENTS;
    tx_header->MessageMarker       = 0;
}

HAL_StatusTypeDef fdcan_send(uint32_t ext_id, const uint8_t *data, uint32_t dlc_code)
{
    FDCAN_TxHeaderTypeDef tx_header;
    fdcan_init_tx_header(&tx_header);

    tx_header.Identifier = (ext_id & 0x1FFFFFFFU); /* 29-bit */
    tx_header.DataLength = dlc_code;

    /* HAL expects a non-const pointer; we won't mutate it. */
    return HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan2, &tx_header, (uint8_t *)data);
}

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
            if (rx_handler != NULL) {
                rx_handler(&rx_header, rx_data, rx_ctx);
            }
    }
}

/* ---------------------------------------------------------
   Filter configuration (hardcoded for now)
   --------------------------------------------------------- */
void fdcan_configure_filter(void)
{
    /*
     * Hardcoded filter to accept messages whose "target" field (bits 25..21)
     * equals BMS device ID (0x1F), regardless of source/cmd/priority.
     *
     * This assumes your 29-bit layout:
     *   [28:26] priority
     *   [25:21] target
     *   [20:5]  cmd
     *   [4:0]   source
     */

    FDCAN_FilterTypeDef filter_config;
    memset(&filter_config, 0, sizeof(filter_config));

    filter_config.IdType       = FDCAN_EXTENDED_ID;
    filter_config.FilterIndex  = 0;
    filter_config.FilterType   = FDCAN_FILTER_MASK;
    filter_config.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;

    const uint32_t target_mask = (0x1FU << 21);      /* only care about target bits */
    const uint32_t bms_target  = (0x1FU << 21);      /* BMS_DEVICE_ID == 0x1F */

    filter_config.FilterID1 = bms_target;
    filter_config.FilterID2 = target_mask;

    HAL_FDCAN_ConfigFilter(&hfdcan2, &filter_config);
}

/* ---------------------------------------------------------
   Hardware init
   --------------------------------------------------------- */
void can_hardware_init(void)
{
    fdcan_configure_filter();

    if (HAL_FDCAN_Start(&hfdcan2) != HAL_OK) {
        /* handle error */
    }

    if (HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK) {
        /* handle error */
    }
}