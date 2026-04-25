#include "precharge.h"
#include "cmsis_os2.h"

static void close_precharge_relay(void) {
  HAL_GPIO_WritePin(PRECHAGE_CIRCUIT_GPIO_GROUP, PRECHARGE_RELAY_GPIO,
                    GPIO_PIN_SET);
}

static void open_precharge_relay(void) {
  HAL_GPIO_WritePin(PRECHAGE_CIRCUIT_GPIO_GROUP, PRECHARGE_RELAY_GPIO,
                    GPIO_PIN_RESET);
}

static void close_ir_pos_relay(void) {
  HAL_GPIO_WritePin(PRECHAGE_CIRCUIT_GPIO_GROUP, IR_POS_GPIO, GPIO_PIN_SET);
}

static void open_ir_pos_relay(void) {
  HAL_GPIO_WritePin(PRECHAGE_CIRCUIT_GPIO_GROUP, IR_POS_GPIO, GPIO_PIN_RESET);
}

void run_precharge_task() {
  // make sure both open
  open_precharge_relay();
  open_ir_pos_relay();
  osDelay(50);

  // start prechargign capacitor
  close_precharge_relay();
}