#ifndef SORT_H
#define SORT_H

#include "bms_types.h"
#include "cb.h"
#include <stdint.h>
#include <stdlib.h>

void qsort_cells_by_voltage(battery_cell_t *cells, uint16_t num_cells);

int comp(const void *CA, const void *CB);

#endif