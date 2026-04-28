#include "gui_can_job.h"
#include "can.h"
#include "can2_job.h"
#include "cmsis_os2.h"
#include "gui_drivers.h"
#include "gui_types.h"
#include "imd_drivers.h"
#include "stm32g474xx.h"
#include <string.h>

// make sure this exists irl
extern osMessageQueueId_t fdcan_rx_dispatch_queueHandle;
extern osMessageQueueId_t can2_rx_dispatch_queueHandle;

const osThreadAttr_t gui_can_job_runner_attributes = {
    .name = "gui_can_job_runner",
    .priority = (osPriority_t)osPriorityHigh,
    .stack_size = 128 * 8};

/**
 * @brief
 * if MSG comes from GUI, send to GUI can handle
 * @param hfdcan
 * @param RxFifo0ITs
 */
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan,
                               uint32_t RxFifo0ITs) {
  if ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) == 0U) {
    return;
  }

  FDCAN_RxHeaderTypeDef rxHeader;
  uint8_t rx_payload[64];
  static const uint8_t dlc_to_len[] = {0, 1,  2,  3,  4,  5,  6,  7,
                                       8, 12, 16, 20, 24, 32, 48, 64};

  if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &rxHeader, rx_payload) !=
      HAL_OK) {
    return;
  }

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

    if (osMessageQueuePut(can2_rx_dispatch_queueHandle, &msg, 0, 0) != osOK) {
      can2_forwarding_count_queue_overrun();
    }
  }

  else if (hfdcan->Instance == FDCAN2) {
    fdcan_msg_t msg = {
        .id = rxHeader.Identifier,
        .len = len,
        .rx_tick = tick,
    };
    memcpy(msg.data, rx_payload, len);
    (void)osMessageQueuePut(fdcan_rx_dispatch_queueHandle, &msg, 0, 0);
  }
}

// **************************************************

void gui_can_job_runner(void *argument) {
  fdcan_msg_t msg;
  fdcan_hardware_init();

  for (;;) {
    if (osMessageQueueGet(fdcan_rx_dispatch_queueHandle, &msg, NULL,
                          osWaitForever) == osOK) {
      gui_process_can_command(msg.id, msg.data);
    }
  }
}
