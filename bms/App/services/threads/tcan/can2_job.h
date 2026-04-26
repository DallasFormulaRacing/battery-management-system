#ifndef CAN2JOB_H
#define CAN2JOB_H

#include "can.h"
#include "cmsis_os2.h"
#include <stdint.h>

typedef struct {
  uint32_t rx_total;
  uint32_t forwarded_total;
  uint32_t forward_drop_total;
  uint32_t queue_overrun_total;
} can2_forwarding_stats_t;

void can2_job_runner(void *argument);
void can2_forwarding_count_queue_overrun(void);
can2_forwarding_stats_t can2_forwarding_stats_get(void);

extern const osThreadAttr_t can2_job_runner_attributes;
#endif
