
#ifndef FDCAN_H
#define FDCAN_H

//#include "can.h"
#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_fdcan.h"
#include <stdint.h>
extern FDCAN_HandleTypeDef hfdcan2;
extern FDCAN_HandleTypeDef hfdcan1;

/**
 * @brief Send an extended-ID CAN FD data frame (FDCAN2).
 *
 * @param ext_id   29-bit extended identifier
 * @param data     payload buffer
 * @param dlc_code HAL DLC code
 */
HAL_StatusTypeDef fdcan_send(uint32_t ext_id, const uint8_t *data,
                             uint32_t dlc_code);

typedef struct {
  uint32_t id;
  uint8_t data[64];
  uint32_t len;
} fdcan_msg_t;

/**
 * @brief Configure FDCAN filter(s) to accept the messages you want (hardcoded
 * for now). This is called inside fdcan_hardware_init().
 */
void fdcan_configure_filter(void);

/**
 * @brief Start FDCAN2, configure filters, and enable RX FIFO0 notifications.
 * Called once from main.c before program/bms init runs.
 */
void fdcan_hardware_init(void);

#endif