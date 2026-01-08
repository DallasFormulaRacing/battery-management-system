#ifndef CELL_BALANCING_H
#define CELL_BALANCING_H

typedef struct {
  uint8_t ic_count;
  uint8_t *cell_voltages;
  uint8_t *cell_voltages_raw;
  uint8_t *cell_voltages_filtered;
  uint8_t *cell_voltages_filtered_raw;
  uint8_t *cell_voltages_filtered_raw_filtered;
  uint8_t *cell_voltages_filtered_raw_filtered_filtered;
  uint8_t *cell_voltages_filtered_raw_filtered_filtered_filtered;
  uint8_t *cell_voltages_filtered_raw_filtered_filtered_filtered_filtered;
} cb_ctx_t;

#endif