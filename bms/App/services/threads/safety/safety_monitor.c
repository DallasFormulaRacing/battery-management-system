#include "safety_monitor.h"
#include "cmsis_os2.h"
#include <stdint.h>

const osThreadAttr_t bms_safety_task_attributes = {
    .name = "bms_safety_task",
    .priority = (osPriority_t)osPriorityNormal,
    .stack_size = 128 * 32};

const timing_parameters_t bms_safety_task_time =
    (timing_parameters_t){.frequency = 200};

void bms_safety_task(void *argument) {
  // bms_test_init();
  bms_fsm_init(&hbms); // uncomment this for the real thing
  const uint32_t task_freq = ((timing_parameters_t *)argument)->frequency;
  uint32_t tick = osKernelGetTickCount();
  for (;;) {
    // bms_test_run();
    bms_fsm_run(&hbms); // uncomment this for the real thing
    tick += task_freq;
    osDelayUntil(tick);
  }
}
