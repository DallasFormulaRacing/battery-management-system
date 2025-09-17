/**
 * @author Will Kim
 * @email <wkim@utdallas.edu>
 * @create date 2025-08-18 19:40:46
 * @modify date 2025-08-18 19:40:46
 * @desc Communications to talk to VCU over CAN bus
 */

#ifndef VCU_H
#define VCU_H

#include "stm32g4xx_hal_fdcan.h"
#define CAN_TX_PIN
#define CAN_RX_PIN

#include "main.h"
#include "fdcan.h"
#include <stdint.h>



void VCU_CAN_Init(void);
void VCU_CAN_Send(uint8_t *data);
void VCU_CAN_Receive(void);

#endif
