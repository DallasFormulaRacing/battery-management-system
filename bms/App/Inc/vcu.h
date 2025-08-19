/**
 * @author Will Kim
 * @email <wkim@utdallas.edu>
 * @create date 2025-08-18 19:40:46
 * @modify date 2025-08-18 19:40:46
 * @desc Communications to talk to VCU over CAN bus
 */

#ifndef VCU_H
#define VCU_H

#include "stm32g4xx_hal_fdcan.h"
#define CAN_TX_PIN
#define CAN_RX_PIN

#include "main.h"
#include "fdcan.h"
#include <stdint.h>

FDCAN_HandleTypeDef hfdcan1;

FDCAN_TxHeaderTypeDef TxHeader;
uint8_t TxData[8];
uint32_t TxMailbox;

FDCAN_RxHeaderTypeDef RxHeader;
uint8_t RxData[8];
uint32_t RxMailbox;


#endif
