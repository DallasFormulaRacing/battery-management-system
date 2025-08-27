/**
 * @author Will Kim
 * @email <wkim@utdallas.edu>
 * @create date 2025-08-18 19:40:46
 * @modify date 2025-08-18 19:40:46
 * @desc Communications to talk to VCU over CAN bus
 */

#include "vcu.h"
#include "stm32g4xx_hal_fdcan.h"
#include <stdint.h>

FDCAN_HandleTypeDef hfdcan1;

FDCAN_TxHeaderTypeDef TxHeader;
uint8_t TxData[64];
uint32_t TxMailbox;

FDCAN_RxHeaderTypeDef RxHeader;
uint8_t RxData[64];
uint32_t RxMailbox;

uint32_t message_id;

#define BMS_CAN_ID 0x7FF;

void VCU_CAN_Init(void) {

    // TODO: Configure CAN for DFR EV std
    // hfdcan2.Instance = FDCAN2;
    // hfdcan2.Init.ClockDivider = FDCAN_CLOCK_DIV1;
    // hfdcan2.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
    // hfdcan2.Init.Mode = FDCAN_MODE_NORMAL;
    // hfdcan2.Init.AutoRetransmission = DISABLE;
    // hfdcan2.Init.TransmitPause = DISABLE;
    // hfdcan2.Init.ProtocolException = DISABLE;
    // hfdcan2.Init.NominalPrescaler = 4;
    // hfdcan2.Init.NominalSyncJumpWidth = 2;
    // hfdcan2.Init.NominalTimeSeg1 = 7;
    // hfdcan2.Init.NominalTimeSeg2 = 2;
    // hfdcan2.Init.DataPrescaler = 1;
    // hfdcan2.Init.DataSyncJumpWidth = 2;
    // hfdcan2.Init.DataTimeSeg1 = 7;
    // hfdcan2.Init.DataTimeSeg2 = 2;
    // hfdcan2.Init.StdFiltersNbr = 0;
    // hfdcan2.Init.ExtFiltersNbr = 0;
    // hfdcan2.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
    if (HAL_FDCAN_Init(&hfdcan2) != HAL_OK)
    {
      Error_Handler();
    }
    TxHeader.Identifier = BMS_CAN_ID;
}

void VCU_CAN_Packetize() {

}

void VCU_CAN_Send(uint8_t *data) {

}

void VCU_CAN_Receive(void) {
    HAL_FDCAN_GetRxMessage(&hfdcan1, 0, &RxHeader, RxData);
}


