#include "charging.h"
#include "cb.h"

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
}

// ! MAKE SURE THE DAISY CHAIN COMM PACKET ORDER IS RESPECTED HERE!
// ! THE FIRST PACKET NEEDS TO GO LAST, IS THIS HANDLED IN write_to_all_ics????