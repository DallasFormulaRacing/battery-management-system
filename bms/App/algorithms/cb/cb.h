#ifndef CELL_BALANCING_H
#define CELL_BALANCING_H

#include "bms_enums.h"
#include "bms_types.h"
#include "config.h"
#include "parse.h"
#include "queue.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  voltage_readings_t maximum_cell_delta_allowed;
  battery_cell_t lowest_cell;
  battery_cell_t batteries[NUM_CELL_MAX];
  // queue_t balance_q;
} pcb_ctx_t;

void init_pcb(pcb_ctx_t *pcb);

/**
 * use the retention registers to store segment identifier for easy id during
 * cell balancing
 *
 */
void cell_delta_policy_enforcer(cell_asic_ctx_t *asic_ctx, pcb_ctx_t *pcb);

#endif
// void qsort_cells_by_voltage(battery_cell_t *cells, uint16_t num_cells);

// void sort_cell_queue(cell_asic_ctx_t *asic_ctx, pcb_ctx_t *pcb) {
//   qsort_cells_by_voltage(pcb->balance_q.pack, NUM_PARALLEL_JOBS);
// }

// int comp(const void *CA, const void *CB) {
//   const battery_cell_t *cellA = (const battery_cell_t *)CA;
//   const battery_cell_t *cellB = (const battery_cell_t *)CB;

//   if (cellA->cell_voltage > cellB->cell_voltage)
//     return 1;

//   if (cellA->cell_voltage < cellB->cell_voltage)
//     return -1;

//   return 0;
// }

// void qsort_cells_by_voltage(battery_cell_t *cells, uint16_t num_cells) {
//   qsort(cells, num_cells, sizeof(battery_cell_t), comp);
// }