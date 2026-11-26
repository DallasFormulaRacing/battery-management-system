
#ifndef FDCAN_H
#define FDCAN_H

#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_fdcan.h"
#include <stdint.h>
extern FDCAN_HandleTypeDef hfdcan2;

/* The BMS's address in the 29-bit DFR/ezfd ID scheme
 * ([prio 3][target 5][command 16][source 5]). Same as its ev27 node address
 * and the ezfd bootloader's THIS_NODE, so the dashboard's Ping and Reboot reach
 * the app as well as the bootloader. */
#define FDCAN_EZFD_NODE_ADDR 0x03U
#define FDCAN_DFR_TARGET(id) (((id) >> 21) & 0x1FU)

/**
 * @brief Queue one frame on FDCAN2. Every FDCAN2 sender goes through this
 * (the GUI's fdcan_send() and the ev27 main-bus code), so two tasks never
 * write the TX FIFO at once. If the 3-deep TX FIFO is full it waits 1 ms and
 * tries again, a few times. Task context only.
 */
HAL_StatusTypeDef fdcan2_tx(FDCAN_TxHeaderTypeDef *header, const uint8_t *data);

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
  uint32_t rx_tick;
} fdcan_msg_t;

/**
 * @brief Configure FDCAN filter(s) to accept the messages you want (hardcoded
 * for now). This is called inside fdcan_hardware_init().
 */
void fdcan_configure_filter(void);

/**
 * @brief Add the GUI filter, reject every frame no filter accepts, enable RX
 * FIFO0 notifications and start FDCAN2. Called once from ev27_can_start()
 * (services/ev27), after it has added the main-bus 11-bit filters.
 */
void fdcan_hardware_init(void);

#endif