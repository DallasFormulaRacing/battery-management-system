#include "can_job.h"
#include "cmsis_os2.h"
#include "gui_drivers.h"

// make sure this exists irl
extern osMessageQueueId_t canfd_rx_queueHandle;

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan,
                               uint32_t RxFifo0ITs) {
  if ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET) {
    FDCAN_RxHeaderTypeDef rxHeader;
    can_msg_t msg;

    if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &rxHeader, msg.data) ==
        HAL_OK) {
      msg.id = rxHeader.Identifier;
      osMessageQueuePut(canfd_rx_queueHandle, &msg, 0, 0);
    }
  }
}

void can_job_runner(void *argument) {
  can_msg_t msg;
  can_hardware_init();

  for (;;) {
    if (osMessageQueueGet(canfd_rx_queueHandle, &msg, NULL, osWaitForever) ==
        osOK) {
      process_can_command(msg.id, msg.data);
    }
  }
}