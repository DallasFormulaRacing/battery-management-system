#include "bms.h"
#include "bms_enums.h"
#include "bms_types.h"
#include "charger.h"
#include "config.h"
#include "segment.h"
#include "thermistor.h"
#include <stdbool.h>
#include <stdint.h>

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
  adbms_read_rdasall_voltage(hbms.asic);
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
        if (hbms.asic->thermistor_fault_status[therm_num] != OPEN_WIRE_FAULT) {
          hbms.asic->thermistor_fault_status[therm_num] = OVER_FAULT;
        }
      }

      if (temp < -20.0F) {
        under_temp_flag = true;
        if (hbms.asic->thermistor_fault_status[therm_num] != OPEN_WIRE_FAULT) {
          hbms.asic->thermistor_fault_status[therm_num] = UNDER_FAULT;
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
  adbms_read_rdasall_voltage(hbms.asic);
  bool open_wire_flag = false;
  for (uint8_t seg_num = 0; seg_num < NUM_IC_COUNT_CHAIN; seg_num++) {
    for (uint16_t i = 0; i < NUM_THERM_PER_SEGMENT; i++) {
      // if voltage is greater than 2.9 V, there is probably an OW or it's
      // really cold
      if (hbms.asic->aux.aux_voltages_array[i] >
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

  adbms_read_rdfcall_voltage(hbms.asic);
  bool cell_over_flag = false;
  bool cell_under_flag = false;
  for (uint8_t seg_num = 0; seg_num < NUM_IC_COUNT_CHAIN; seg_num++) {

    for (uint16_t cell_num = 0; cell_num < NUM_CELLS_PER_SEGMENT; cell_num++) {
      float this_cell = convert_voltage_human_readable(
          hbms.asic[seg_num].filt_cell.filt_cell_voltages_array[cell_num]);

      if (this_cell > g_voltage_cfg.overvoltage_threshold_v) {
        cell_over_flag = true;
        hbms.asic->cell_fault_status[cell_num] = OVER_FAULT;
      } // endif

      if (this_cell < g_voltage_cfg.undervoltage_threshold_v) {
        cell_under_flag = true;
        hbms.asic->cell_fault_status[cell_num] = UNDER_FAULT;
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

bms_fault_t cell_open_wire_check_odd() {
  // TODO: test this & make sure odd/even is right
  // TODO: add: this function also updates the fault enum array
  // read S-ADC
  adbms_read_rdsall_voltage(hbms.asic, OW_ON_ODD_CH);
  // if less than 1V call openwire check
  // does not have to use C-ADC at all
  bool cell_open_wire_flag = false;

  // do odd cell taps (even indexs due to array indexing)
  for (uint8_t seg_num = 0; seg_num < NUM_IC_COUNT_CHAIN; seg_num++) {

    for (uint16_t cell_num = 0; cell_num < NUM_CELLS_PER_SEGMENT;
         cell_num += 2) {
      float this_cell = convert_voltage_human_readable(
          hbms.asic[seg_num].s_cell.s_cell_voltages_array[cell_num]);

      if (1000 * this_cell < (float)g_voltage_cfg.openwire_cell_threshold_mv) {
        hbms.asic->cell_fault_status[cell_num] = OPEN_WIRE_FAULT;
        cell_open_wire_flag = true;
      } // endif
    } // end inner fl
  }
  if (cell_open_wire_flag)
    return BMS_ERR_CELL_OPENWIRE;

  return BMS_ERR_NONE;
}

bms_fault_t cell_open_wire_check_even() {
  // todo: test this & make sure odd/even is right
  // todo: add: this function also updates the fault enum array
  // read S-ADC
  adbms_read_rdsall_voltage(hbms.asic, OW_ON_EVEN_CH);
  // if less than 1V call openwire check
  // does not have to use C-ADC at all
  bool cell_open_wire_flag = false;

  // do even cell taps (odd indexs due to array indexing)
  for (uint8_t seg_num = 0; seg_num < NUM_IC_COUNT_CHAIN; seg_num++) {

    for (uint16_t cell_num = 1; cell_num < NUM_CELLS_PER_SEGMENT;
         cell_num += 2) {
      float this_cell = convert_voltage_human_readable(
          hbms.asic[seg_num].s_cell.s_cell_voltages_array[cell_num]);

      if (1000 * this_cell < (float)g_voltage_cfg.openwire_cell_threshold_mv) {
        hbms.asic->cell_fault_status[cell_num] = OPEN_WIRE_FAULT;
        cell_open_wire_flag = true;
      } // endif
    } // end inner fl
  }
  if (cell_open_wire_flag)
    return BMS_ERR_CELL_OPENWIRE;

  return BMS_ERR_NONE;
}

void hard_fault_disable_openwire_on_profiles() {
  g_cell_profile.ow_mode = OW_OFF_ALL_CH;
  g_cell_profile.AUX_OW_en = AUX_OW_OFF;
  g_cell_profile.DCP_en = DCP_OFF;

  g_thermistor_profile.ow_mode = OW_OFF_ALL_CH;
  g_thermistor_profile.AUX_OW_en = AUX_OW_OFF;
  g_thermistor_profile.DCP_en = DCP_OFF;
}

/**
 * @brief this is a special mesurement loop designed to operate under fault
 * it should NOT: bleed cells or do open wire checks, since ow injects current
 */
void measure_during_fault() {
  hard_fault_disable_openwire_on_profiles();
  adbms_read_rdfcall_voltage(hbms.asic);
  adbms_read_rdasall_voltage(hbms.asic);
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
  adbms_start_aux_voltage_measurement(hbms.asic);
  adbms_start_adc_cell_voltage_measurment(hbms.asic);
  // Needed for filtered cell readings
  spi_adcv_command(g_cell_filtered_profile.redundant_measurement_mode,
                   g_cell_filtered_profile.continuous_measurement,
                   g_cell_filtered_profile.DCP_en,
                   g_cell_filtered_profile.RSTF_en,
                   g_cell_filtered_profile.ow_mode);
  HAL_Delay(8);
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

// static float TEST_VOLTAGE[12];

void bms_test_run() {
  // keep
}
