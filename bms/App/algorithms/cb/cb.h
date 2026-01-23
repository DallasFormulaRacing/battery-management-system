#ifndef CELL_BALANCING_H
#define CELL_BALANCING_H

#include "config.h"
#include "parse.h"
#include "queue.h"

typedef struct {
  voltage_readings_t maximum_cell_delta_allowed;
  battery_cell_t lowest_cell;
  battery_cell_t batteries[NUM_CELL_MAX];
  // uint8_t pwm_timer_flag;
  // queue_t balance_q;
} pcb_ctx_t;

void find_cell_deltas(pcb_ctx_t *pcb);
void populate_pwm_register(cell_asic_ctx_t *asic_ctx, pcb_ctx_t *pcb);
void init_cell_balancing(pcb_ctx_t *pcb,
                         voltage_readings_t max_cell_delta_allowed);

void copy_cell_voltages(cell_asic_ctx_t *asic_ctx, pcb_ctx_t *pcb);

pwm_duty_cycle_t map_delta_to_pwm_discretize(pcb_ctx_t *pcb,
                                             voltage_readings_t delta);

#endif