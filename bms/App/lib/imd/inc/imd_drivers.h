#ifndef IMD_DRIVER
#define IMD_DRIVER

#include "can.h"
#include "imd_types.h"
#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_def.h"
#include "stm32g4xx_hal_fdcan.h"
#include <string.h>

HAL_StatusTypeDef imd_send_request(uint8_t can_id, uint8_t index,
                                   const uint8_t *payload, uint8_t len);
static void configure_imd_header(FDCAN_TxHeaderTypeDef *header, uint8_t can_id);
static HAL_StatusTypeDef send_imd_buffer(uint8_t can_id, uint8_t *buf,
                                         uint8_t len);
void configure_imd_param();
void reset_imd_alarm();

#endif