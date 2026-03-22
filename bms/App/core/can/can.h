#ifndef MY_CAN_H
#define MY_CAN_H

#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_fdcan.h"

#define BUILD_CAN_ID(priority, cmd_type, device)                               \
  ((((uint32_t)(priority) << 26) | ((uint32_t)(cmd_type) << 5) |               \
    ((uint32_t)(device) & 0x1F)) &                                             \
   CAN_EXT_ID_MASK)




#endif