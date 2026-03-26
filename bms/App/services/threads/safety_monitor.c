#include "safety_monitor.h"
#include "cmsis_os2.h"
#include <stdint.h>

const osThreadAttr_t bms_safety_task_attributes = {
    .name = "bms_safety_task",
    .priority = (osPriority_t)osPriorityNormal,
    .stack_size = 128 * 4};

void bms_safety_task(void *argument) {
  bms_test_init();
  const uint32_t task_freq = ((timing_parameters_t *)argument)->frequency;
  uint32_t tick = osKernelGetTickCount();
  for (;;) {
    bms_test_run();
    tick += task_freq;
    osDelayUntil(tick);
  }
}