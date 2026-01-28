#include "queue.h"

void init_queue(queue_t *cq) {
  cq->front = 0;
  cq->back = -1;
}

void reset_queue(queue_t *cq) {

  cq->front = 0;
  cq->back = -1;
}

bool is_empty(queue_t *cq) { return (cq->front == cq->back - 1); }
bool is_full(queue_t *cq) { return (NUM_CELL_MAX == cq->back); }

int16_t enq(queue_t *cq, battery_cell_t bc) {
  if (is_full(cq))
    return -1;

  cq->pack[cq->back] = bc;
  return (int16_t)++cq->back;
}

int16_t deq(queue_t *cq) {
  if (is_empty(cq))
    return -1;

  cq->pack[cq->back] = (battery_cell_t){0};
  return (int16_t)++cq->front;
}

battery_cell_t *peek(queue_t *cq) {
  if (is_empty(cq))
    return NULL;

  return &cq->pack[cq->back];
}