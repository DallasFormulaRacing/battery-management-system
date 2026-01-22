#include "charging.h"

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
  adbms_start_adc_cell_voltage_measurment(asic_ctx);
  adbms_read_cell_voltages(asic_ctx);
  copy_cell_voltages(asic_ctx, pcb);
}