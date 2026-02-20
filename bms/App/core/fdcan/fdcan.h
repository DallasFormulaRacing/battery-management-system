
#ifndef FDCAN_H
#define FDCAN_H

#include "stm32g4xx_hal.h"

// ID Construction Macro
#define CAN_EXT_ID_MASK 0x1FFFFFFF
#define BUILD_CAN_ID(priority, target, cmd, source) \
((((uint32_t)(priority) & 0x07) << 26) | \
(((uint32_t)(target) & 0x1F) << 21) | \
(((uint32_t)(cmd) & 0xFFFF) << 5) | \
(((uint32_t)(source) & 0x1F)))

// Can Segment Enums (following CANpute CANFD convention)
// Priority Levels
typedef enum{
    CAN_PRIORITY_CRITICAL = 0x0,
    CAN_PRIORITY_VERY_HIGH = 0x1,
    CAN_PRIORITY_HIGH = 0x2,
    CAN_PRIORITY_MEDIUM = 0x3,
    CAN_PRIORITY_LOW = 0x4,
    CAN_PRIORITY_VERY_LOW = 0x5
} PriorityID_t;

// Device IDs
#define BMS_DEVICE_ID 0x1F
#define GUI_DEVICE_ID 0x00

// Command Enums
typedef enum {
    CMD_ID_SVOLTAGE_ALL = 0xA0,
    CMD_ID_SVOLTAGE_ALL_RESP = 0xA1,
    CMD_ID_CVOLTAGE_ALL = 0xA2,
    CMD_ID_CVOLTAGE_ALL_RESP = 0xA3
} CommandID_t;

// Complete FDCAN Headers
#define READ_SVOLTAGE_ALL_ID BUILD_CAN_ID(CAN_PRIORITY_MEDIUM, BMS_DEVICE_ID, CMD_ID_SVOLTAGE_ALL, GUI_DEVICE_ID)
#define READ_SVOLTAGE_ALL_RESP_ID BUILD_CAN_ID(CAN_PRIORITY_MEDIUM, GUI_DEVICE_ID, CMD_ID_SVOLTAGE_ALL_RESP, BMS_DEVICE_ID)
#define READ_CVOLTAGE_ALL_ID BUILD_CAN_ID(CAN_PRIORITY_MEDIUM, BMS_DEVICE_ID, CMD_ID_CVOLTAGE_ALL, GUI_DEVICE_ID)
#define READ_CVOLTAGE_ALL_RESP_ID BUILD_CAN_ID(CAN_PRIORITY_MEDIUM, GUI_DEVICE_ID, CMD_ID_CVOLTAGE_ALL_RESP, BMS_DEVICE_ID)

// Prototypes
void CAN_InitHeader(FDCAN_TxHeaderTypeDef *tx_header);
HAL_StatusTypeDef CAN_Transmit(uint32_t commandHeader, uint8_t* pData, uint32_t dlc_bytes);
void Process_CAN_Command(uint32_t rx_id, uint8_t* data);

#endif