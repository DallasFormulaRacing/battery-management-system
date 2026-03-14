#ifndef FDCAN_H
#define FDCAN_H

#include <stdint.h>
#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_fdcan.h" 
#include "main.h"   // provides: extern FDCAN_HandleTypeDef hfdcan2

/* =========================================================
   Application RX handler registration
   ========================================================= */
typedef void (*fdcan_rx_handler_t)(const FDCAN_RxHeaderTypeDef *hdr,
                                  const uint8_t *data,
                                  void *ctx);

/**
 * @brief Register an application-level RX handler for FIFO0 frames.
 * Called by api (bms_can_init()) exactly once.
 */
void fdcan_register_rx_handler(fdcan_rx_handler_t handler, void *ctx);

/* =========================================================
   Transport primitive
   ========================================================= */
/**
 * @brief Send an extended-ID CAN FD data frame (FDCAN2).
 *
 * @param ext_id   29-bit extended identifier
 * @param data     payload buffer
 * @param dlc_code HAL DLC code
 */
HAL_StatusTypeDef fdcan_send(uint32_t ext_id, const uint8_t *data, uint32_t dlc_code);

/**
 * @brief Configure FDCAN filter(s) to accept the messages you want (hardcoded for now).
 * This is called inside can_hardware_init().
 */
void fdcan_configure_filter(void);

/**
 * @brief Start FDCAN2, configure filters, and enable RX FIFO0 notifications.
 * Called once from main.c before program/bms init runs.
 */
void can_hardware_init(void);

#endif