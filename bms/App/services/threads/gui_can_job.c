#include "gui_can_job.h"
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
    .stack_size = 128 * 4};

/**
 * @brief
 * if MSG comes from GUI, send to GUI can handle
 * @param hfdcan
 * @param RxFifo0ITs
 */
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan,
                               uint32_t RxFifo0ITs) {
  if ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET) {
    FDCAN_RxHeaderTypeDef rxHeader;
    fdcan_msg_t msg;

    if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &rxHeader, msg.data) ==
        HAL_OK) {
      msg.id = rxHeader.Identifier;
      // this can2
      if (hfdcan->Instance == FDCAN1) {
        osMessageQueuePut(can2_rx_dispatch_queueHandle, &msg, 0, 0);
      }

      // this is fdcan
      if (hfdcan->Instance == FDCAN2) {
        osMessageQueuePut(fdcan_rx_dispatch_queueHandle, &msg, 0, 0);
      }
      /*If msg comes from something else, send it to its own message queue*/
    }
  }
}

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