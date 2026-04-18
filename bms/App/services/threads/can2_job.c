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