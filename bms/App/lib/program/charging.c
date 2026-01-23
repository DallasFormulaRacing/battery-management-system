#include "charging.h"
#include "api.h"
#include "cb.h"
#include "config.h"
#include <stdatomic.h>
#include <stdint.h>

extern volatile uint8_t discharge_timeout_flag;

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

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  discharge_timeout_flag = 1;
}

// ! MAKE SURE THE DAISY CHAIN COMM PACKET ORDER IS RESPECTED HERE!
// ! THE FIRST PACKET NEEDS TO GO LAST, IS THIS HANDLED IN write_to_all_ics????