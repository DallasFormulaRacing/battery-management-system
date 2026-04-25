#include "precharge.h"
#include "amc_drivers.h"
#include "cmsis_os2.h"

static void close_ir_pos_relay(void) {
  HAL_GPIO_WritePin(PRECHARGE_CIRCUIT_GPIO_GROUP, IR_POS_GPIO, GPIO_PIN_SET);
}

static void open_ir_pos_relay(void) {
  HAL_GPIO_WritePin(PRECHARGE_CIRCUIT_GPIO_GROUP, IR_POS_GPIO, GPIO_PIN_RESET);
}

void run_precharge_task() {
  // make sure both open
  open_ir_pos_relay();
  osDelay(50);

  // detect if precharge capacitor
  while (1) {
    osDelay(50);
    if (get_hv_prchrg_cap_voltage() > PRECHARGE_DETECT_LINK_CAP_MAX_V)
      break;
  }

  while (1) {
    osDelay(100);
    if (get_hv_bus_voltage() - get_hv_prchrg_cap_voltage() <
        PRECHARGE_THRESHOLD_DELTA_V)
      break;
  }

  close_ir_pos_relay();
  osDelay(5);
}