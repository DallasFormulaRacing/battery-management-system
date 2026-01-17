#ifndef CELL_BALANCING_H
#define CELL_BALANCING_H

#include "bms_enums.h"
#include "bms_types.h"
#include "config.h"
#include <stddef.h>
#include <stdint.h>

typedef struct {
  uint8_t lowest_cell_index;
  voltage_readings_t lowest_cell_voltage;
  //???
} pcb_ctx_t;

typedef struct {
  voltage_readings_t cell_voltage;
  uint8_t cell_number;
  uint8_t segment_number;
} battery_cell_t;

/**
 * use the retention registers to store segment identifier for easy id during
 * cell balancing
 *
 */

void cell_delta_policy_enforcer(cell_asic_ctx_t *asic_ctx);

#endif
