#ifndef SAFETY_MONITOR_H
#define SAFETY_MONITOR_H

#include "bms.h"
#include "cmsis_os2.h"
#include "state.h"

void bms_safety_task(void *argument);

extern const osThreadAttr_t bms_safety_task_attributes;

#endif