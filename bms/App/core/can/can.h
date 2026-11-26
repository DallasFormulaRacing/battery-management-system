#ifndef CAN_H
#define CAN_H

/*
 * FDCAN1: the tb bus (classic CAN 2.0B, 500 kbit/s). CAB500, IMD and the Elcon
 * charger live here; the BMS forwards some of their frames to the main bus.
 */

#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_fdcan.h"
#include <stdbool.h>
#include <stdint.h>
extern FDCAN_HandleTypeDef hfdcan1;

// todo: this is not being implemented, but im keeping it here in case
#define DFR_CAN_BMS_PRECHARGE_FAILURE 0xBFAA

void configure_can2_tx_header(FDCAN_TxHeaderTypeDef *header);

/**
 * @brief Queue one frame on FDCAN1. Every FDCAN1 sender (Elcon, IMD) goes
 * through this, so two tasks never write the TX FIFO at once. If the 3-deep TX
 * FIFO is full it waits 1 ms and tries again, a few times. Task context only.
 */
HAL_StatusTypeDef can2_send(FDCAN_TxHeaderTypeDef *header, uint8_t *data);

/** @brief Send a classic data frame with a 29-bit ID on FDCAN1 (len 0..8). */
HAL_StatusTypeDef can2_send_ext(uint32_t ext_id, const uint8_t *data,
                                uint8_t len);

void can2_configure_filter(void);

typedef struct {
  uint32_t id;
  uint32_t id_type;
  uint8_t data[8];
  uint32_t len;
  uint32_t rx_tick;
} can2_msg_t;

void can2_hardware_init(void);

/** @brief Bus-off recovery for FDCAN1. Call it periodically from task context. */
void can2_service(void);

#endif
