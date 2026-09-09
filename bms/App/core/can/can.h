#ifndef CAN_H
#define CAN_H

#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_fdcan.h"
extern FDCAN_HandleTypeDef hfdcan1;

#define DFR_CAN_BMS_CURRENT_SENSOR 0xBEEF
#define DFR_CAN_BMS_IMD_REQUEST 0xBF22
#define DFR_CAN_BMS_IMD_RESPONSE 0xBF23
#define DFR_CAN_BMS_IMD_GENERAL 0xBF37
#define DFR_CAN_BMS_IMD_ISO_DETAIL 0xBF38
#define DFR_CAN_BMS_IMD_VOLTAGE 0xBF39
#define DFR_CAN_BMS_IMD_IT_SYSTEM 0xBF3A
// todo: this is not being implemented, but im keeping it here in case
#define DFR_CAN_BMS_PRECHARGE_FAILURE 0xBFAA

void configure_can2_tx_header(FDCAN_TxHeaderTypeDef *header);
HAL_StatusTypeDef can2_send(FDCAN_TxHeaderTypeDef *header, uint8_t *data);
void can2_configure_filter();

typedef struct {
  uint32_t id;
  uint32_t id_type;
  uint8_t data[8];
  uint32_t len;
  uint32_t rx_tick;
} can2_msg_t;

void process_can2_message(can2_msg_t msg);

void can2_hardware_init();

#endif
