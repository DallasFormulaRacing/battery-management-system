#ifndef CAN_H
#define CAN_H

#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_fdcan.h"
extern FDCAN_HandleTypeDef hfdcan1;

void configure_can2_tx_header(FDCAN_TxHeaderTypeDef *header);
HAL_StatusTypeDef can2_send(FDCAN_TxHeaderTypeDef *header, uint8_t *data);
void can2_configure_filter();

typedef struct {
  uint32_t id;
  uint8_t data[8];
  uint32_t len;
} can2_msg_t;

void process_can2_message(can2_msg_t msg);

void can2_hardware_init();

#endif