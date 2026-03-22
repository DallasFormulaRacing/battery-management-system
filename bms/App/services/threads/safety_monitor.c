#include "safety_monitor.h"

const osThreadAttr_t bms_safety_task_attributes = {
    .name = "bms_safety_task",
    .priority = (osPriority_t)osPriorityNormal,
    .stack_size = 128 * 4};

void bms_safety_task(void *argument) {
  bms_test_init();
  for (;;) {
    bms_test_run();
    osDelay(1);
  }
}