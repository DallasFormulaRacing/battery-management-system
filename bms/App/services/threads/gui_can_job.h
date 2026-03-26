#ifndef GUICANJOB_H
#define GUICANJOB_H

#include "cmsis_os2.h"
#include "fdcan.h"

void gui_can_job_runner(void *argument);

extern const osThreadAttr_t gui_can_job_runner_attributes;
#endif
