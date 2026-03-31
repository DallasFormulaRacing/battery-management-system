#include "can.h"
#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_fdcan.h"

// Configured for CAN2.0, specifically IMD transmission
void configure_tx_header(FDCAN_TxHeaderTypeDef *header) {
  header->TxFrameType = FDCAN_DATA_FRAME;
  header->DataLength = FDCAN_DLC_BYTES_8;
  header->ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  header->BitRateSwitch = FDCAN_BRS_OFF;
  header->FDFormat = FDCAN_CLASSIC_CAN;
  header->TxEventFifoControl = FDCAN_NO_TX_EVENTS;
  header->MessageMarker = 0;
}

HAL_StatusTypeDef can2_send(FDCAN_TxHeaderTypeDef *header, uint8_t *data) {

  return HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, header, data);
}

void can2_configure_filter() {
  FDCAN_FilterTypeDef filter_standard;
  filter_standard.FilterIndex = 0;
  filter_standard.FilterConfig = FDCAN_FILTER_TO_RXFIFO1; // Maybe rxfifo0
  filter_standard.FilterType = FDCAN_FILTER_MASK;
  filter_standard.IdType = FDCAN_STANDARD_ID; // IMD needs standard

  FDCAN_FilterTypeDef filter_extended;
  filter_extended.FilterIndex = 1;
  filter_extended.FilterConfig = FDCAN_FILTER_TO_RXFIFO1; // Maybe rxfifo0
  filter_extended.FilterType = FDCAN_FILTER_MASK;
  filter_extended.IdType = FDCAN_EXTENDED_ID; // ELCON needs extended

  const uint32_t target_mask = (0x1FU << 21); /* only care about target bits */
  const uint32_t bms_target = (0x1FU << 21);  /* BMS_DEVICE_ID == 0x1F */

  filter_standard.FilterID1 = bms_target;
  filter_standard.FilterID2 = target_mask;

  filter_extended.FilterID1 = bms_target;
  filter_extended.FilterID2 = target_mask;

  HAL_FDCAN_ConfigFilter(&hfdcan1, &filter_standard);
  HAL_FDCAN_ConfigFilter(&hfdcan1, &filter_extended);
}

void can2_hardware_init(FDCAN_TxHeaderTypeDef *header) {}