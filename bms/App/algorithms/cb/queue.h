#ifndef QUEUE_H
#define QUEUE_H

#include "bms_enums.h"
#include "bms_types.h"
#include "config.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define NUM_CELLS_PER_SEGMENT 12
#define NUM_CELL_MAX (IC_COUNT_CHAIN * NUM_CELLS_PER_SEGMENT)
#define NUM_PARALLEL_JOBS NUM_CELL_MAX

typedef struct {
  voltage_readings_t cell_voltage;
  uint8_t cell_number;
  uint8_t segment_number;
  int16_t delta;
} battery_cell_t;

typedef struct {
  uint8_t num_parallel_balancing_allowed;
  uint8_t back, front;
  battery_cell_t pack[NUM_CELL_MAX];
} queue_t;

void init_queue(queue_t *cq);
void reset_queue(queue_t *cq);
bool is_empty(queue_t *cq);
bool is_full(queue_t *cq);

int16_t enq(queue_t *cq, battery_cell_t);
int16_t deq(queue_t *cq);

battery_cell_t *peek(queue_t *cq);

#endif // QUEUE_H