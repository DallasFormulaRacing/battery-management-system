#include "csort.h"

// make sure sorting direction is valid (should be descending)
int comp(const void *CA, const void *CB) {
  const battery_cell_t *cellA = (const battery_cell_t *)CA;
  const battery_cell_t *cellB = (const battery_cell_t *)CB;

  if (cellA->cell_voltage > cellB->cell_voltage)
    return 1;

  if (cellA->cell_voltage < cellB->cell_voltage)
    return -1;

  return 0;
}

void qsort_cells_by_voltage(battery_cell_t *cells, uint16_t num_cells) {
  qsort(cells, num_cells, sizeof(battery_cell_t), comp);
}