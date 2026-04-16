#include "can2_job.h"
#include "can.h"
#include "cmsis_os2.h"
#include "imd_drivers.h"
#include "imd_types.h"
#include "stm32g474xx.h"
#include <string.h>

// make sure this exists irl
extern osMessageQueueId_t can2_rx_processing_queueHandle;

const osThreadAttr_t can2_job_runner_attributes = {
    .name = "can2_job_runner",
    .priority = (osPriority_t)osPriorityHigh,
    .stack_size = 128 * 4};

// /**
//  * @brief
//  * if MSG comes from GUI, send to GUI can handle
//  * @param hfdcan
//  * @param RxFifo0ITs
//  */
// void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan,
//                                uint32_t RxFifo0ITs) {
//   if ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET) {
//     FDCAN_RxHeaderTypeDef rxHeader;
//     can2_msg_t msg;

//     if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &rxHeader, msg.data)
//     ==
//         HAL_OK) {
//       msg.id = rxHeader.Identifier;
//       if (hfdcan->Instance == FDCAN1) {
//         osMessageQueuePut(fdcan_rx_dispatch_queueHandle, &msg, 0, 0);
//       }

//       if (hfdcan->Instance == FDCAN2) {
//         osMessageQueuePut(can2_rx_dispatch_queueHandle, &msg, 0, 0);
//       }

//       /*If msg comes from something else, send it to its own message queue*/
//     }
//   }
// }

void can2_job_runner(void *argument) {
  can2_msg_t msg;
  can2_hardware_init();

  for (;;) {
    if (osMessageQueueGet(can2_rx_processing_queueHandle, &msg, NULL,
                          osWaitForever) == osOK) {
      // if message is from IMD
      if (msg.id == IMD_CAN_ID_REQUEST) {
        parse_imd_msg(msg.id, msg.data);
      }

      if (/*msg.id == ELCON_CHARGER_CAN_ID_REQUEST*/ 1) {
        // parse_charger_msg(msg.id, msg.data);
      }

      if (/*msg.id == CAB_CAN_ID_REQUEST*/ 1) {
        // parse_current_sensor_msg(msg.id, msg.data);
      }
    }
  }
}