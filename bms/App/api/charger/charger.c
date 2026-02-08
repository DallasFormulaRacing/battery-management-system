#include "charger.h"
#include "cb.h"
#include "config.h"
#include "segment.h"
#include <stdatomic.h>
#include <stdint.h>

volatile uint8_t discharge_timeout_flag = 0;

// ******************* CHARGING
/**
 * @brief cell balance super loop
 *
  - read all cell voltages
  - copy cell voltages
  - add deviant cells to pwm struct
  - send
 *
 * @param asic_ctx:
 * @return void
 */
void cell_delta_policy_enforcer(cell_asic_ctx_t *asic_ctx, pcb_ctx_t *pcb) {
  copy_cell_voltages(asic_ctx, pcb);
  /**
   * go through the battery cell array
   * add adbms_set_cell_pwm(asic, battery.cellnumber, battery.segmentnumber)
   *
   */
  find_cell_deltas(pcb);
  populate_pwm_register(asic_ctx, pcb);
  adbms_send_pwm_commands(asic_ctx);
  if (1 == discharge_timeout_flag) {
    adbms_clear_all_pwm(asic_ctx);
    discharge_timeout_flag = 0;
  }
}

/**
 * @brief then a second pass to calculate each cell
 * delta. during this second pass, if the cell delta is greater than the, add it
 * to the PWM struct (array)
 *
 * @param asic_ctx
 * @param pcb
 */
void populate_pwm_register(cell_asic_ctx_t *asic_ctx, pcb_ctx_t *pcb) {
  battery_cell_t *cell;
  // Second pass: find all cell deltas
  for (uint8_t cell_idx = 0; cell_idx < NUM_CELL_MAX; cell_idx++) {
    cell = &pcb->batteries[cell_idx];
    cell->delta = (voltage_readings_t)(cell->cell_voltage -
                                       pcb->lowest_cell.cell_voltage);

    adbms_set_cell_pwm(asic_ctx, cell->cell_number, cell->segment_number,
                       map_delta_to_pwm_discretize(pcb, cell->delta));
  }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  discharge_timeout_flag = 1;
}

// ! MAKE SURE THE DAISY CHAIN COMM PACKET ORDER IS RESPECTED HERE!
// ! THE FIRST PACKET NEEDS TO GO LAST, IS THIS HANDLED IN write_to_all_ics????