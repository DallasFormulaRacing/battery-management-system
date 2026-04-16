#ifndef CAN2JOB_H
#define CAN2JOB_H

#include "can.h"
#include "cmsis_os2.h"

void can2_job_runner(void *argument);

extern const osThreadAttr_t can2_job_runner_attributes;
#endif
