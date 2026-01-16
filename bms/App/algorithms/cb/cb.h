#ifndef CELL_BALANCING_H
#define CELL_BALANCING_H

#include "bms_types.h"
#include "config.h"
#include <stdint.h>

typedef struct {
  uint8_t segment_identifier;

} cb_ctx_t;

/**
 * use the retention registers to store segment identifier for easy id during
 * cell balancing
 *
 */

voltage_readings_t find_lowest_cell_voltage(cell_asic_ctx_t *asic_ctx);
uint16_t find_cells_to_balance(cell_asic_ctx_t *asic_ctx, uint8_t queue[],
                               voltage_readings_t delta);
void send_pwm_to_cell(uint16_t num_queue_elems, cell_asic_ctx_t *asic_ctx,
                      const uint8_t queue[], voltage_readings_t delta);

#endif
