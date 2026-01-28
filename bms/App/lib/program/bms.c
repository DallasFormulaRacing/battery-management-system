#include "bms.h"
#include "api.h"
#include "charging.h"

cell_asic_ctx_t asic[IC_COUNT_CHAIN];
uint8_t write_buffer[WRITE_SIZE];
bms_handler_t hbms;

adc_config_t g_adc_cfg = {
    .redundant_measurement_mode = RD_OFF,
    .channel_to_convert = AUX_ALL,
    .continuous_measurement = SINGLE,
    .cell_open_wire_detection_mode = OW_OFF_ALL_CH,
    .AUX_OW_en = AUX_OW_OFF,
    .PUP_en = PUP_DOWN,
    .DCP_en = DCP_OFF,
    .RSTF_en = RSTF_OFF,
    .ERR_en = WITHOUT_ERR,
};

voltage_config_t g_voltage_cfg = {
    .overvoltage_threshold_v = 4.15F,
    .undervoltage_threshold_v = 2.51F,
    .openwire_cell_threshold_mv = 2000,
    .openwire_aux_threshold_mv = 50000,
    .loop_meas_count = 4,
    .meas_loop_time_ms = 1000,
};

measurement_config_t g_meas_cfg = {
    .measure_cell = ENABLED,
    .measure_avg_cell = ENABLED,
    .measure_f_cell = ENABLED,
    .measure_s_voltage = ENABLED,
    .measure_aux = ENABLED,
    .measure_raux = ENABLED,
    .measure_stat = ENABLED,
};

static bms_cfg_t g_bms_cfg = {
    .adc = &g_adc_cfg,
    .voltage = &g_voltage_cfg,
    .measurement = &g_meas_cfg,
};

bms_handler_t hbms = {
    .config = &g_bms_cfg,
    .state =
        {
            .current_state = BMS_STATE_BOOT,
            .previous_state = BMS_STATE_BOOT,
            .error_code = BMS_ERR_NONE,
            .state_entry_tick = 0,
            .fault_flags = 0,
        },
    .asic = asic,
};

typedef void (*state_handler_t)(bms_handler_t *hbms);

static const state_handler_t state_handlers[] = {
    [BMS_STATE_BOOT] = bms_state_entry,
    [BMS_STATE_INIT] = bms_state_init,
    [BMS_STATE_TRANSMIT_DATA] = bms_state_transmit_data,
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

  // todo: maybe reset flags here on transition
}

// state handlers

/**
 * @brief sm_run entry point and triggers the rest of the state machine
 *
 * @param hbms, bms handler struct
 */
void bms_state_entry(bms_handler_t *hbms) {
  // todo: maybe reset flags here on entry
  hbms->config->adc = &g_adc_cfg;
  hbms->config->voltage = &g_voltage_cfg;
  hbms->config->measurement = &g_meas_cfg;
}

/**
 * @brief initializes the bms config registers and starts the measurement loop
 *
 * @param hbms, bms handler struct
 */
void bms_state_init(bms_handler_t *hbms) {
  comm_status_t status = adbms_init_config(hbms->asic);

  if (status != COMM_OK) {
    bms_sm_transition(hbms, BMS_STATE_FAULT);
    return;
  }

  bms_sm_transition(hbms, BMS_STATE_MEASURE);
}

void bms_state_transmit_data(bms_handler_t *hbms) {
  /*
  - needs to transmit the following data to the host computer:
    - all 144 cell voltages
    - current
    - pack voltage
    - all 120 temperature readings
    - which cells are being balanced
    - any faults that have occurred
    - transmit the onboard SoC calculation
    -

  */
}

void bms_state_measure(bms_handler_t *hbms) {
  /*
  - this state must be able to:
    - start all adcs
    - measure from all adcs
    - pack into structs
    - do both
      - check for undeervoltage and overvoltage
      - check for openwire
      - check for over temp
      - check if real cell delta greater than target cell delta
    - if UV or OV or OW, set fault flag and transition to fault state
    - if cell delta greater than target cell delta, transition to balancin state
    - if nothing is wrong, stay in this state

    also needs to measure from the current sensor
    also needs to measure from the pack voltage sensor

  */
}

void bms_state_charging(bms_handler_t *hbms) {
  /*
  how do we want to handle this state?

  we need to interface with the charger here

  we need to alternate between charging and measuring states
  */
  cell_delta_policy_enforcer(hbms->asic, hbms->pcb);
  // need to handle errors
  // todo: handle safety as in check for UV and OV
  // todo: put this in a loop to stop when no cells need balancing

  bms_sm_transition(hbms, BMS_STATE_MEASURE);
}

void bms_state_balancing(bms_handler_t *hbms) {
  /*
  adbms_start_adc_cell_voltage_measurment(asic_ctx);
  adbms_read_cell_voltages(asic_ctx);

  --> if bad cell, stop execution and move to fault state

  cell_delta_policy_enforcer;

  */
}

void bms_state_fault(bms_handler_t *hbms) {
  /*
  - this state opens the shut down circuit and stays in fault state until the
  MCU is manually reset via NRST or Power cycled

  - this state cannot transition to any other state
  - once this state is reached it stays.
  */
}

void bms_state_sleep(bms_handler_t *hbms) {
  /*
  idk what this state does

  maybe triggers lpcm?
  */
}

/* ----------------------------------------------------- */
/* testing functions ------------------------------------ */

void bms_test_init() {
  hbms.config->adc = &g_adc_cfg;
  hbms.config->voltage = &g_voltage_cfg;
  hbms.config->measurement = &g_meas_cfg;

  for (int i = 0; i < IC_COUNT_CHAIN; i++) {
    asic[i].ic_count = IC_COUNT_CHAIN;
  }

  adbms_init_config(hbms.asic);
}

void bms_test_run() {
  adbms_write_read_config(hbms.asic);
  adbms_read_status_registers(hbms.asic);

  adbms_start_adc_cell_voltage_measurment(hbms.asic);
  // adbms_poll_for_conversion_adc(hbms.asic);
  adbms_read_cell_voltages(hbms.asic);

  adbms_set_cell_pwm(hbms.asic, 11, 0, PWM_19_8_PERCENT_DUTY_CYCLE);

  // reads 15 (14+1) on the scope
  // adbms_bleed_cell_pwm(hbms.asic, 14, 0, PWM_85_8_PERCENT_DUTY_CYCLE);
  HAL_Delay(20);
}
