#ifndef SAFETY_MONITOR_H
#define SAFETY_MONITOR_H

#include "bms.h"
#include "cmsis_os2.h"
#include "state.h"
#include <stdint.h>

void bms_safety_task(void *argument);

typedef struct {
  uint32_t frequency;
} timing_parameters_t;

extern const osThreadAttr_t bms_safety_task_attributes;
extern const timing_parameters_t bms_safety_task_time;

#endif