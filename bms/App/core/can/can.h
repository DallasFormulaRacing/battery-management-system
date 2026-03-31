#ifndef CAN_H
#define CAN_H

#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_fdcan.h"
extern FDCAN_HandleTypeDef hfdcan1;

void configure_tx_header(FDCAN_TxHeaderTypeDef *header);
HAL_StatusTypeDef can2_send(FDCAN_TxHeaderTypeDef *header, uint8_t *data);
void can2_configure_filter();

void can2_hardware_init(FDCAN_TxHeaderTypeDef *header);

#endif