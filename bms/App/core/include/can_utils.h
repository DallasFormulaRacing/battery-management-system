#ifndef CAN_UTILS_H
#define CAN_UTILS_H

#include "stm32h5xx_hal.h"
#include "types.h"

// ID Construction Macro
#define CAN_EXT_ID_MASK 0x1FFFFFFF
#define BUILD_CAN_ID(priority, cmd_type, device) \
    ((((uint32_t)(priority) << 26) | \
      ((uint32_t)(cmd_type) << 5) | \
      ((uint32_t)(device) & 0x1F)) & CAN_EXT_ID_MASK)

// Prototypes
void CAN_InitHeader(FDCAN_TxHeaderTypeDef *tx_header);
HAL_StatusTypeDef CAN_Transmit(uint8_t priority, uint32_t cmd_type, uint8_t* pData, uint32_t dlc_bytes);

#endif