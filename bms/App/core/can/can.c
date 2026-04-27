#include "can.h"
#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_fdcan.h"

// Configured for CAN2.0, specifically IMD transmission
void configure_can2_tx_header(FDCAN_TxHeaderTypeDef *header) {
  header->TxFrameType = FDCAN_DATA_FRAME;
  header->ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  header->BitRateSwitch = FDCAN_BRS_OFF;
  header->FDFormat = FDCAN_CLASSIC_CAN;
  header->TxEventFifoControl = FDCAN_NO_TX_EVENTS;
  header->MessageMarker = 0;
}

HAL_StatusTypeDef can2_send(FDCAN_TxHeaderTypeDef *header, uint8_t *data) {

  return HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, header, data);
}

void can2_configure_filter(void) {
  (void)HAL_FDCAN_ConfigGlobalFilter(&hfdcan1, FDCAN_ACCEPT_IN_RX_FIFO0,
                                     FDCAN_ACCEPT_IN_RX_FIFO0,
                                     FDCAN_REJECT_REMOTE, FDCAN_REJECT_REMOTE);
}

void can2_hardware_init(void) {
  can2_configure_filter();

  if (HAL_FDCAN_Start(&hfdcan1) != HAL_OK) {
    // Handle error
  }

  if (HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE,
                                     0) != HAL_OK) {
    /* handle error */
  }
}