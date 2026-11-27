#include "can.h"
#include "cmsis_os2.h"
#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_fdcan.h"

#define CAN2_TX_RETRIES 3U

// Classic CAN 2.0 frame for the tb bus (FDCAN1)
void configure_can2_tx_header(FDCAN_TxHeaderTypeDef *header) {
  header->TxFrameType = FDCAN_DATA_FRAME;
  header->ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  header->BitRateSwitch = FDCAN_BRS_OFF;
  header->FDFormat = FDCAN_CLASSIC_CAN;
  header->TxEventFifoControl = FDCAN_NO_TX_EVENTS;
  header->MessageMarker = 0;
}

HAL_StatusTypeDef can2_send(FDCAN_TxHeaderTypeDef *header, uint8_t *data) {
  for (uint32_t attempt = 0U;; attempt++) {
    HAL_StatusTypeDef st = HAL_BUSY;

    /* scheduler lock, not a mutex: this is a few us of register writes */
    const int32_t lock = osKernelLock();
    if (HAL_FDCAN_GetTxFifoFreeLevel(&hfdcan1) > 0U) {
      st = HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, header, data);
    }
    (void)osKernelRestoreLock(lock);

    if (st != HAL_BUSY || attempt >= CAN2_TX_RETRIES) {
      return st;
    }
    (void)osDelay(1U); /* TX FIFO full: one frame takes ~0.25 ms at 500k */
  }
}

HAL_StatusTypeDef can2_send_ext(uint32_t ext_id, const uint8_t *data,
                                uint8_t len) {
  static const uint32_t dlc[9] = {
      FDCAN_DLC_BYTES_0, FDCAN_DLC_BYTES_1, FDCAN_DLC_BYTES_2,
      FDCAN_DLC_BYTES_3, FDCAN_DLC_BYTES_4, FDCAN_DLC_BYTES_5,
      FDCAN_DLC_BYTES_6, FDCAN_DLC_BYTES_7, FDCAN_DLC_BYTES_8,
  };
  FDCAN_TxHeaderTypeDef header = {0};
  uint8_t buf[8] = {0};

  if (len > 8U) {
    return HAL_ERROR;
  }
  for (uint8_t i = 0U; i < len; i++) {
    buf[i] = data[i];
  }
  configure_can2_tx_header(&header);
  header.Identifier = ext_id & 0x1FFFFFFFU;
  header.IdType = FDCAN_EXTENDED_ID;
  header.DataLength = dlc[len];
  return can2_send(&header, buf);
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

void can2_service(void) {
  /* After bus-off the FDCAN sets CCCR.INIT and stays off the bus until
   * software clears it. Clearing it starts the normal recovery (128 x 11
   * recessive bits). */
  if ((hfdcan1.Instance->PSR & FDCAN_PSR_BO) != 0U) {
    CLEAR_BIT(hfdcan1.Instance->CCCR, FDCAN_CCCR_INIT);
  }
}
