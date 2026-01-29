#include "cb.h"
// #include "api.h"
#include "bms_enums.h"
#include "bms_types.h"
#include "queue.h"
#include <stdint.h>

// ! table 95, page 68

/**
 ******************************************************************************
 * @file           : cb.c
 * @brief          : Header for cb.c file. This file contains platform
 *                   independent helper functions for EV charging functionality.
 ******************************************************************************
 * @attention
 *
 * Some things to think about:
 * handling bad cells: make sure under voltage and over voltage cases are
 * handled in the case where a cell is not accepting charge, and the algo might
 * bleed every cell to a value under the undervoltage threshold
 *
 * collate all cell votlages from the asic array 192 element (asic) to 144
 * element (pcb) -- is this necessary? -- not really but could make life easier
 * we have a lot of memory to spare.
 *
 *
 ******************************************************************************
 */

void copy_cell_voltages(cell_asic_ctx_t *asic_ctx, pcb_ctx_t *pcb) {
  // copy cells from asic
  uint8_t segment_idx = 0;
  uint8_t battery_idx = 0;
  for (battery_idx = 0; battery_idx < NUM_CELL_MAX; battery_idx++) {
    // TODO: Find out what this magic number in "cell_idx < 12" means,
    // NUM_CELLS_PER_SEGMENT?
    for (uint8_t cell_idx = 0; cell_idx < 12; cell_idx++) {
      pcb->batteries[battery_idx].cell_voltage =
          asic_ctx[segment_idx].cell.cell_voltages_array[cell_idx];
    }
    segment_idx++;
  }
}

/** // TODO is there a better way to write this?
 * @brief maps a certain cell delta to an appropriate duty cycle (since time is
 * fixed). if saturated (i.e. above a certain value), clamp to 100%. intervals
 * and corresponding pwm values are listed in descending order.
 *
 * a good starting point would be a 1-to-1 (mV difference maps to ceil(PWM))
 *
 * @param delta -- cell delta in signed int16
 * @return pwm_duty_cycle_t -- interpolated PWM Duty Cycle
 */
pwm_duty_cycle_t map_delta_to_pwm_discretize(pcb_ctx_t *pcb,
                                             voltage_readings_t delta) {
  float delta_v = convert_voltage_human_readable(delta);
  if (delta_v <=
      convert_voltage_human_readable(pcb->maximum_cell_delta_allowed))
    return PWM_0_0_PERCENT_DUTY_CYCLE;
  if (delta_v >= 0.100F /*is above X = 100 mV */)
    return PWM_100_0_PERCENT_DUTY_CYCLE;
  if (delta_v < 0.100F && delta_v >= 0.0924F)
    return PWM_92_4_PERCENT_DUTY_CYCLE;
  if (delta_v < 0.0924F && delta_v >= 0.0858F)
    return PWM_85_8_PERCENT_DUTY_CYCLE;
  if (delta_v < 0.0858F && delta_v >= 0.0792F)
    return PWM_79_2_PERCENT_DUTY_CYCLE;
  if (delta_v < 0.0792F && delta_v >= 0.0726F)
    return PWM_72_6_PERCENT_DUTY_CYCLE;
  if (delta_v < 0.0726F && delta_v >= 0.0660F)
    return PWM_66_0_PERCENT_DUTY_CYCLE;
  if (delta_v < 0.0660F && delta_v >= 0.0594F)
    return PWM_59_4_PERCENT_DUTY_CYCLE;
  if (delta_v < 0.0594F && delta_v >= 0.0528F)
    return PWM_52_8_PERCENT_DUTY_CYCLE;
  if (delta_v < 0.0528F && delta_v >= 0.0462F)
    return PWM_46_2_PERCENT_DUTY_CYCLE;
  if (delta_v < 0.0462F && delta_v >= 0.0396F)
    return PWM_39_6_PERCENT_DUTY_CYCLE;
  if (delta_v < 0.0396F && delta_v >= 0.0330F)
    return PWM_33_0_PERCENT_DUTY_CYCLE;
  if (delta_v < 0.0330F && delta_v >= 0.0264F)
    return PWM_26_4_PERCENT_DUTY_CYCLE;
  if (delta_v < 0.0264F && delta_v >= 0.0198F)
    return PWM_19_8_PERCENT_DUTY_CYCLE;
  if (delta_v < 0.0198F && delta_v >= 0.0132F)
    return PWM_13_2_PERCENT_DUTY_CYCLE;
  if (delta_v < 0.0132F && delta_v >= 0.0066F)
    return PWM_6_6_PERCENT_DUTY_CYCLE;
  return PWM_0_0_PERCENT_DUTY_CYCLE;
}

/**
 * @brief Populate the PCB struct by doing a first pass through the battery
 * array to find the lowest cell
 *
 * @param pcb
 */
void find_cell_deltas(pcb_ctx_t *pcb) {
  // First pass: find minimum cell
  voltage_readings_t min_voltage = INT16_MAX;
  battery_cell_t base;
  for (uint8_t cell_idx = 0; cell_idx < NUM_CELL_MAX; cell_idx++) {
    if (pcb->batteries[cell_idx].cell_voltage < min_voltage) {
      min_voltage = pcb->batteries[cell_idx].cell_voltage;
      base = pcb->batteries[cell_idx];
    }
  }

  pcb->lowest_cell = base;
}

/**
 * @brief then a second pass to calculate each cell
 * delta. during this second pass, if the cell delta is greater than the, add it
 * to the PWM struct (array)
 *
 * @param asic_ctx
 * @param pcb
 */
// void populate_pwm_register(cell_asic_ctx_t *asic_ctx, pcb_ctx_t *pcb) {
//   battery_cell_t *cell;
//   // Second pass: find all cell deltas
//   for (uint8_t cell_idx = 0; cell_idx < NUM_CELL_MAX; cell_idx++) {
//     cell = &pcb->batteries[cell_idx];
//     cell->delta = (voltage_readings_t)(cell->cell_voltage -
//                                        pcb->lowest_cell.cell_voltage);
//
//     adbms_set_cell_pwm(asic_ctx, cell->cell_number, cell->segment_number,
//                        map_delta_to_pwm_discretize(pcb, cell->delta));
//   }
// }

/**
 * @brief init the PCB struct with the user input: max cell delta
 *
 * @param pcb
 * @param max_cell_delta_allowed (target cell delta)
 */
void init_cell_balancing(pcb_ctx_t *pcb,
                         voltage_readings_t max_cell_delta_allowed) {
  pcb->maximum_cell_delta_allowed = max_cell_delta_allowed;
}
