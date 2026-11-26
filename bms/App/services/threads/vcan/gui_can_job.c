#include "gui_can_job.h"
#include "can.h"
#include "can2_job.h"
#include "cmsis_os2.h"
#include "ev27_node.h"
#include "ev27_ezfd.h"
#include "ev27_port.h"
#include "gui_drivers.h"
#include "gui_types.h"
#include "stm32g474xx.h"
#include <string.h>

// make sure this exists irl
extern osMessageQueueId_t fdcan_rx_dispatch_queueHandle;
extern osMessageQueueId_t can2_rx_dispatch_queueHandle;
extern osMessageQueueId_t charger_power_setpoint_queueHandle;

const osThreadAttr_t gui_can_job_runner_attributes = {
    .name = "gui_can_job_runner",
    .priority = (osPriority_t)osPriorityHigh,
    .stack_size = 128 * 8};

/**
 * @brief
 * FDCAN1 frames go to can2_job. FDCAN2 (main bus) frames are sorted by ID
 * type: 11-bit ev27 main-bus frames go to the ev27 RX task, 29-bit GUI frames
 * to the GUI handler.
 * @param hfdcan
 * @param RxFifo0ITs
 */
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan,
                               uint32_t RxFifo0ITs) {
  /* first thing: ev27 TimeSync needs the instant the frame arrived */
  const uint32_t now_us = ev27_port_time_us();

  if ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) == 0U) {
    return;
  }

  FDCAN_RxHeaderTypeDef rxHeader;
  uint8_t rx_payload[64];
  static const uint8_t dlc_to_len[] = {0, 1,  2,  3,  4,  5,  6,  7,
                                       8, 12, 16, 20, 24, 32, 48, 64};

  while (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &rxHeader,
                                rx_payload) == HAL_OK) {
    const uint8_t len = dlc_to_len[rxHeader.DataLength & 0x0FU];
    const uint32_t tick = osKernelGetTickCount();

    if (hfdcan->Instance == FDCAN1) {
      can2_msg_t msg = {
          .id = rxHeader.Identifier,
          .id_type = rxHeader.IdType,
          .len = (len > 8U) ? 8U : len, // clamp to 8 (can2 max)
          .rx_tick = tick,
      };
      memcpy(msg.data, rx_payload, msg.len);

      if (osMessageQueuePut(can2_rx_dispatch_queueHandle, &msg, 0, 0) !=
          osOK) {
        can2_count_queue_overrun();
      }
    } else if (hfdcan->Instance == FDCAN2 &&
               rxHeader.IdType == FDCAN_STANDARD_ID) {
      ev27_can_queue_rx_from_isr(rxHeader.Identifier, rx_payload, len, now_us);
    } else if (hfdcan->Instance == FDCAN2) {
      fdcan_msg_t msg = {
          .id = rxHeader.Identifier,
          .len = len,
          .rx_tick = tick,
      };
      memcpy(msg.data, rx_payload, len);
      (void)osMessageQueuePut(fdcan_rx_dispatch_queueHandle, &msg, 0, 0);
    }
  }
}

// **************************************************
// actual thread function
void gui_can_job_runner(void *argument) {
  fdcan_msg_t msg;
  /* FDCAN2 is set up and started by ev27_can_start() in main.c */

  for (;;) {
    if (osMessageQueueGet(fdcan_rx_dispatch_queueHandle, &msg, NULL,
                          osWaitForever) == osOK) {
      if (FDCAN_DFR_TARGET(msg.id) == FDCAN_EZFD_NODE_ADDR) {
        ev27_ezfd_rx(msg.id, msg.data, (uint8_t)msg.len);
      } else {
        gui_process_can_command(msg.id, msg.data, msg.len);
      }
    }
  }
}
