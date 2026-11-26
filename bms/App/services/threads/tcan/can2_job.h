#ifndef CAN2JOB_H
#define CAN2JOB_H

#include "can.h"
#include "cmsis_os2.h"
#include <stdint.h>

/* Counters for the tb bus (FDCAN1) task, for the debugger. */
typedef struct {
  uint32_t rx_total;            /* frames taken from the queue */
  uint32_t forwarded_total;     /* copied to the main bus */
  uint32_t forward_drop_total;  /* main-bus TX FIFO stayed full */
  uint32_t forward_muted_total; /* not copied: the CM5 muted the BMS */
  uint32_t queue_overrun_total; /* RX interrupt found the queue full */
} can2_stats_t;

void can2_job_runner(void *argument);

/* From the FDCAN1 RX interrupt when can2_rx_dispatch_queueHandle is full. */
void can2_count_queue_overrun(void);

can2_stats_t can2_stats_get(void);

extern const osThreadAttr_t can2_job_runner_attributes;
#endif
