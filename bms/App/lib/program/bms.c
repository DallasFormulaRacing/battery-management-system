#include "bms.h"

typedef void (*state_handler_t)(bms_handler_t *hbms);

static const state_handler_t state_handlers[] = {
    [BMS_STATE_BOOT] = bms_state_entry,
    [BMS_STATE_INIT] = bms_state_init,
    [BMS_STATE_IDLE] = bms_state_idle,
    [BMS_STATE_MEASURE] = bms_state_measure,
    [BMS_STATE_CHARGING] = bms_state_charging,
    [BMS_STATE_BALANCING] = bms_state_balancing,
    [BMS_STATE_FAULT] = bms_state_fault,
    [BMS_STATE_SLEEP] = bms_state_sleep,
};

void bms_sm_init(bms_handler_t *hbms) {
  hbms->state.current_state = BMS_STATE_BOOT;
  hbms->state.previous_state = BMS_STATE_BOOT;
  hbms->state.error_code = BMS_ERR_NONE;
  hbms->state.state_entry_tick = 0;
  hbms->state.fault_flags = 0;
}

void bms_sm_run(bms_handler_t *hbms) {
  if (hbms->state.current_state != BMS_STATE_FAULT &&
      bms_check_for_fault(hbms)) {
    bms_sm_transition(hbms, BMS_STATE_FAULT);
    return;
  }

  state_handlers[hbms->state.current_state](hbms);
}

void bms_sm_transition(bms_handler_t *hbms, bms_state_t new_state) {
  hbms->state.previous_state = hbms->state.current_state;
  hbms->state.current_state = new_state;
  hbms->state.state_entry_tick = HAL_GetTick();

  if (new_state != BMS_STATE_MEASURE) {
    hbms->config->adc->continuous_measurement = SINGLE;
  }
  if (new_state != BMS_STATE_FAULT) {
    hbms->state.error_code = BMS_ERR_NONE;
    hbms->state.fault_flags = 0;
  }
}

void bms_state_entry(bms_handler_t *hbms) {
  /*
  this function needs to wake up all internal STM32 hardware peripherals


  mcu_peripheral_init() <- need to be implemented
  transition state: entry -> init

  why is this a separate state from init?
  we want to separate ADBMS and MCU initialization
  */
}

void bms_state_init(bms_handler_t *hbms) {
  /*
  this function needs to initialize the ADBMS
  */
}