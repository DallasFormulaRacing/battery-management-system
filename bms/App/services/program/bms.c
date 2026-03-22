#include "bms.h"
#include "bms_comms.h"
#include "bms_enums.h"
#include "bms_types.h"
#include "charger.h"
#include "cmsis_os2.h"
#include "config.h"
#include "parse.h"
#include "segment.h"
#include "spi.h"
#include "stm32g4xx_hal.h"
#include "thermistor.h"
#include <stdbool.h>
#include <stdint.h>

volatile float look[4][13];
cell_asic_ctx_t asic[NUM_IC_COUNT_CHAIN];
uint8_t write_buffer[WRITE_SIZE];

static bms_cfg_t g_bms_cfg = {
    .adc = &g_cell_profile,
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

/*
 * @brief Check thermistor temperature and see if between range
 * @params None
 * @return Fault status message for whether thermistor is over or under temp.
 * Otherwise returns no error.
 */
bms_fault_t therm_temp_in_range_check() {
  // NOTE: Read might need to be outside
  // adbms_read_rdasall_voltage(hbms.asic);
  adbms_read_aux_voltages(hbms.asic);
  // adbms_read_s_voltages(hbms.asic);
  bool over_temp_flag = false;
  bool under_temp_flag = false;
  for (uint8_t seg_num = 0; seg_num < NUM_IC_COUNT_CHAIN; seg_num++) {
    for (uint16_t therm_num = 0; therm_num < NUM_THERM_PER_SEGMENT;
         therm_num++) {
      float temp = thermistor_from_adc(
          hbms.asic[seg_num].aux.aux_voltages_array[therm_num]);

      hbms.asic[seg_num].thermistor[therm_num] = temp;
      // NOTE: we should define max and min temp constant somewhere
      if (temp > 60.0F) {
        over_temp_flag = true;
        if (hbms.asic[seg_num].thermistor_fault_status[therm_num] !=
            OPEN_WIRE_FAULT) {
          hbms.asic[seg_num].thermistor_fault_status[therm_num] = OVER_FAULT;
        }
      }

      if (temp < -20.0F) {
        under_temp_flag = true;
        if (hbms.asic[seg_num].thermistor_fault_status[therm_num] !=
            OPEN_WIRE_FAULT) {
          hbms.asic[seg_num].thermistor_fault_status[therm_num] = UNDER_FAULT;
        }
      }
    }
  }

  // Over temp will take precedent over under temp in the near impossible event
  // that both will happen
  if (over_temp_flag) {
    return BMS_ERR_THERM_OVER_TEMP;
  }
  if (under_temp_flag) {
    return BMS_ERR_THERM_OVER_TEMP;
  }

  return BMS_ERR_NONE;
}

bms_fault_t therm_open_wire_check() {
  // adbms_read_rdasall_voltage(hbms.asic);

  adbms_read_aux_voltages(hbms.asic);
  // adbms_read_s_voltages(hbms.asic);
  bool open_wire_flag = false;
  for (uint8_t seg_num = 0; seg_num < NUM_IC_COUNT_CHAIN; seg_num++) {
    for (uint16_t i = 0; i < NUM_THERM_PER_SEGMENT; i++) {
      // if voltage is greater than 2.9 V, there is probably an OW or it's
      // really cold
      if (hbms.asic[seg_num].aux.aux_voltages_array[i] >
          g_voltage_cfg.openwire_aux_threshold_mv) {
        hbms.asic[seg_num].thermistor_fault_status[i] = OPEN_WIRE_FAULT;
        open_wire_flag = true;
      }
    }
  }

  if (open_wire_flag) {
    return BMS_ERR_AUX_OPENWIRE;
  }
  return BMS_ERR_NONE;
}

bms_fault_t cell_voltage_in_range_check() {
  // todo: test this and make sure it updates the fault struct

  adbms_read_fcell_voltages(hbms.asic);
  bool cell_over_flag = false;
  bool cell_under_flag = false;
  for (uint8_t seg_num = 0; seg_num < NUM_IC_COUNT_CHAIN; seg_num++) {

    for (uint16_t cell_num = 0; cell_num < NUM_CELLS_PER_SEGMENT; cell_num++) {
      float this_cell = convert_voltage_human_readable(
          hbms.asic[seg_num].filt_cell.filt_cell_voltages_array[cell_num]);
      if (this_cell > g_voltage_cfg.overvoltage_threshold_v) {
        cell_over_flag = true;
        hbms.asic[seg_num].cell_fault_status[cell_num] = OVER_FAULT;
      } // endif

      if (this_cell < g_voltage_cfg.undervoltage_threshold_v) {
        cell_under_flag = true;
        hbms.asic[seg_num].cell_fault_status[cell_num] = UNDER_FAULT;
      } // endif
    } // end inner fl
  }

  if (cell_over_flag) {
    return BMS_ERR_CELL_OV;
  }

  if (cell_under_flag) {
    return BMS_ERR_CELL_UV;
  }

  return BMS_ERR_NONE;
}

void adbms_set_watchdog() {}

bms_fault_t cell_open_wire_check_odd() {
  adbms_start_adc_s_voltage_measurement(hbms.asic,
                                        g_cell_open_wire_check_profile_odd);
  osDelay(10);
  spi_adc_snap_command();
  adbms_read_s_voltages(hbms.asic);
  spi_adc_unsnap_command();
  bool cell_open_wire_flag = false;

  //////////////

  // todo: how to detect? value reading is fine now
  // do odd cell taps (even indexs due to array indexing)
  for (uint8_t seg_num = 0; seg_num < NUM_IC_COUNT_CHAIN; seg_num++) {

    for (uint16_t cell_num = 0; cell_num < NUM_CELLS_PER_SEGMENT;
         cell_num += 2) {
      float this_cell = convert_voltage_human_readable(
          hbms.asic[seg_num].s_cell.s_cell_voltages_array[cell_num]);
      // look[2][cell_num + 1] = this_cell;

      if (this_cell < 1.0F) {
        hbms.asic[seg_num].cell_fault_status[cell_num] = OPEN_WIRE_FAULT;
        cell_open_wire_flag = true;
      } // endif
    } // end inner fl
  }
  if (cell_open_wire_flag)
    return BMS_ERR_CELL_OPENWIRE;

  return BMS_ERR_NONE;
}

bms_fault_t cell_open_wire_check_even() {
  adbms_start_adc_s_voltage_measurement(hbms.asic,
                                        g_cell_open_wire_check_profile_even);
  osDelay(10);
  spi_adc_snap_command();
  adbms_read_s_voltages(hbms.asic);
  spi_adc_unsnap_command();
  bool cell_open_wire_flag = false;

  // todo: how to detect? value reading is fine now
  // do even cell taps (odd indexs due to array indexing)
  for (uint8_t seg_num = 0; seg_num < NUM_IC_COUNT_CHAIN; seg_num++) {

    for (uint16_t cell_num = 1; cell_num < NUM_CELLS_PER_SEGMENT;
         cell_num += 2) {
      float this_cell = convert_voltage_human_readable(
          hbms.asic[seg_num].s_cell.s_cell_voltages_array[cell_num]);
      // look[3][cell_num] = this_cell;

      if (this_cell < 1.0F) {
        hbms.asic[seg_num].cell_fault_status[cell_num] = OPEN_WIRE_FAULT;
        cell_open_wire_flag = true;
      } // endif
    } // end inner fl
  }
  if (cell_open_wire_flag)
    return BMS_ERR_CELL_OPENWIRE;

  return BMS_ERR_NONE;
}

void force_sync_s_adc() {
  adbms_start_adc_s_voltage_measurement(hbms.asic, g_cell_force_sync_s_adc);
  osDelay(30);
}

void hard_fault_disable_openwire_on_profiles() {
  g_cell_profile.ow_mode = OW_OFF_ALL_CH;
  g_cell_profile.aux_ow_mode = AUX_OW_OFF;
  g_cell_profile.discharge_permit = DCP_OFF;

  g_thermistor_profile.ow_mode = OW_OFF_ALL_CH;
  g_thermistor_profile.aux_ow_mode = AUX_OW_OFF;
  g_thermistor_profile.discharge_permit = DCP_OFF;
}

/**
 * @brief this is a special mesurement loop designed to operate under fault
 * it should NOT: bleed cells or do open wire checks, since ow injects current
 */ // todo
void measure_during_fault() {
  hard_fault_disable_openwire_on_profiles();
  // measure voltages (top level task)
  // measure temps (top level task)
}

static void pop() {
  for (uint8_t i = 0; i < 12; i++) {
    look[0][i] = convert_voltage_human_readable(
        hbms.asic[0].filt_cell.filt_cell_voltages_array[i]);
  }

  for (uint8_t i = 0; i < 12; i++) {
    look[1][i] = convert_voltage_human_readable(
        hbms.asic[1].filt_cell.filt_cell_voltages_array[i]);
  }

  for (uint8_t i = 0; i < 12; i++) {
    look[2][i] = convert_voltage_human_readable(
        hbms.asic[0].cell.cell_voltages_array[i]);
  }

  for (uint8_t i = 0; i < 12; i++) {
    look[3][i] = convert_voltage_human_readable(
        hbms.asic[1].cell.cell_voltages_array[i]);
  }
}

/* ----------------------------------------------------- */
/* testing functions ------------------------------------ */

void bms_test_init() {
  hbms.config->adc = &g_cell_profile;
  hbms.config->voltage = &g_voltage_cfg;
  hbms.config->measurement = &g_meas_cfg;

  for (int i = 0; i < NUM_IC_COUNT_CHAIN; i++) {
    asic[i].ic_count = NUM_IC_COUNT_CHAIN;
  }

  adbms_init_config(hbms.asic);
  // adbms_start_aux_voltage_measurement(hbms.asic);
  adbms_clear_all_pwm(hbms.asic);
  adbms_start_cell_voltage_measurement(hbms.asic);
  // adbms_start_fcell_voltage_measurement(hbms.asic);
  // Needed for filtered cell readings
  // spi_adcv_command(g_cell_filtered_profile.redundant_measurement_mode,
  //                  g_cell_filtered_profile.continuous_measurement,
  //                  g_cell_filtered_profile.discharge_permit,
  //                  g_cell_filtered_profile.reset_filter,
  //                  g_cell_filtered_profile.ow_mode);
  osDelay(8);
}

void cell_open_wire_test() {
  cell_open_wire_check_even();
  cell_open_wire_check_odd();
  force_sync_s_adc();
}

void open_shutdown_circuit() {
  // todo
  // just a gpio flip
  // check rules
}

void bms_light() {
  // todo
  // just a gpio flip
  // check rules
}

void bms_test_run() {
  asic_wakeup(NUM_IC_COUNT_CHAIN);
  spi_adc_snap_command();
  adbms_read_fcell_voltages(hbms.asic);
  spi_adc_unsnap_command();

  asic_wakeup(NUM_IC_COUNT_CHAIN);
  spi_adc_snap_command();
  adbms_read_cell_voltages(hbms.asic);
  spi_adc_unsnap_command();
  pop();
  delay(1);
}
