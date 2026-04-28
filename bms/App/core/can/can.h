#ifndef CAN_H
#define CAN_H

#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_fdcan.h"
extern FDCAN_HandleTypeDef hfdcan1;

// Current Sensor reading, DLC: 48, (Motorola) Current exiting the pack
#define DFR_CAN_BMS_CURRENT_SENSOR 0xBEEF
// IMD Request, DLC: 8, (Intel)
#define DFR_CAN_BMS_IMD_REQUEST 0xBF22
// IMD Response, DLC: 8, (Intel)
#define DFR_CAN_BMS_IMD_RESPONSE 0xBF23
// IMD General, DLC: 8, (Intel)
#define DFR_CAN_BMS_IMD_GENERAL 0xBF37
// IMD Isolation detail, DLC: 8, (Intel)
#define DFR_CAN_BMS_IMD_ISO_DETAIL 0xBF38
// IMD Voltage, DLC: 8, (Intel)
#define DFR_CAN_BMS_IMD_VOLTAGE 0xBF39
// IMD IT system, DLC: 8, (Intel)
#define DFR_CAN_BMS_IMD_IT_SYSTEM 0xBF3A

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