#ifndef QUEUE_H
#define QUEUE_H

#include "cb.h"
#include <stdbool.h>
#include <stdint.h>

#define NUM_CELL_MAX 144
#define NUM_PARALLEL_JOBS 8 // balance 8 cells at a time

typedef struct {
  uint8_t num_parallel_balancing_allowed;
  uint8_t back, front;
  battery_cell_t pack[NUM_CELL_MAX];
} queue_t;

void init_queue(queue_t *cq);
bool is_empty(queue_t *cq);
bool is_full(queue_t *cq);

int16_t enq(queue_t *cq, battery_cell_t);
int16_t deq(queue_t *cq);

battery_cell_t *peek(queue_t *cq);

#endif // QUEUE_H