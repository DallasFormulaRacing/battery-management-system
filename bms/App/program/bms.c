#include "bms.h"
#include "bms_enums.h"
#include "bms_types.h"
#include "charger.h"
#include "config.h"
#include "segment.h"
#include <stdbool.h>
#include <stdint.h>

cell_asic_ctx_t asic[IC_COUNT_CHAIN];
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

bms_fault_t therm_over_temp_check() {
  // use
  return BMS_ERR_NONE;
}
bms_fault_t cell_voltage_in_range_check() {
  // use
  return BMS_ERR_NONE;
}

bms_fault_t cell_open_wire_check_odd() {
  // todo:
  // read S-ADC
  adbms_read_rdcsall_voltage(hbms.asic);
  // if less than 1V call openwire check
  // does not have to use C-ADC at all
  for (uint16_t i = 0; i < NUM_CELL_MAX; i++) {
    if (convert_voltage_human_readable(
            hbms.asic->s_cell.s_cell_voltages_array[i]) < 1.0F) {
      return BMS_ERR_CELL_OPENWIRE;
    }
  }
  return BMS_ERR_NONE;
}

bms_fault_t cell_open_wire_check_even() {
  // todo:
  // read S-ADC
  adbms_read_rdcsall_voltage(hbms.asic);
  // if less than 1V call openwire check
  // does not have to use C-ADC at all
  for (uint16_t i = 0; i < NUM_CELL_MAX; i++) {
    if (convert_voltage_human_readable(
            hbms.asic->s_cell.s_cell_voltages_array[i]) < 1.0F) {
      return BMS_ERR_CELL_OPENWIRE;
    }
  }
  return BMS_ERR_NONE;
}

bms_fault_t therm_open_wire_check() {
  adbms_read_rdasall_voltage(hbms.asic);
  bool open_wire_flag = false;
  for (uint16_t i = 0; i < ADBMS_NUM_AUX_CHANNELS - 2; i++) {
    if (hbms.asic->aux.aux_voltages_array[i] >
        g_voltage_cfg.openwire_aux_threshold_mv) {
      hbms.asic->gpio.therm_ow_pull_up_array[i] = true;
      open_wire_flag = true;
    }
  }

  if (open_wire_flag) {
    return BMS_ERR_AUX_OPENWIRE;
  }
  return BMS_ERR_NONE;
}

/* ----------------------------------------------------- */
/* testing functions ------------------------------------ */

void bms_test_init() {
  hbms.config->adc = &g_cell_profile;
  hbms.config->voltage = &g_voltage_cfg;
  hbms.config->measurement = &g_meas_cfg;

  for (int i = 0; i < IC_COUNT_CHAIN; i++) {
    asic[i].ic_count = IC_COUNT_CHAIN;
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

// static void volt_ow_loop() {}

static inline float f2v(int16_t xin) {
  float volt = (0.00015F * (float)xin) + 1.5F;
  return volt;
}

static inline float thermpoly(float xin) {
  // 2.3487131 * v * * 8 - 35.359734 * v * * 7 + 218.27577 * v * * 6 -
  //     724.54830 * v * * 5 + 1417.8324 * v * * 4 - 1687.9102 * v * * 3 +
  //     1225.0384 * v * * 2 - 565.64244 * v +
  //     209.04676

  return (2.3487131F * xin * xin * xin * xin * xin * xin * xin * xin) -
         (35.359734F * xin * xin * xin * xin * xin * xin * xin) +
         (218.27577F * xin * xin * xin * xin * xin * xin) -
         (724.54830F * xin * xin * xin * xin * xin) +
         (1417.8324F * xin * xin * xin * xin) - (1687.9102F * xin * xin * xin) +
         (1225.0384F * xin * xin) - (565.64244F * xin) + (209.04676F);
}

static float VOLTAGE[12];
// static float g_thermtesterTEMPERATURE;
// static float g_thermistorVOLTAGE;

static void thermtestvoltage() {
  for (int i = 0; i <= 11; i++) {
    VOLTAGE[i] = f2v(hbms.asic->aux.aux_voltages_array[i]);
  }
}

// static void thermtestvoltage() {
//   for (int i = 0; i <= 11; i++) {
//     VOLTAGE[i] = f2v(hbms.asic->cell.cell_voltages_array[i]);
//   }
// }

static float vref2;

void bms_test_run() {
  adbms_write_read_config(hbms.asic);
  adbms_start_aux_voltage_measurement(hbms.asic);

  // adbms_read_cell_voltages(hbms.asic);
  // adbms_read_status_registers(hbms.asic);
  // adbms_read_aux_voltages(hbms.asic);

  // adbms_read_status_registers(hbms.asic);

  // HAL_Delay(20);
  // therm_open_wire_test();
  // g_thermistorVOLTAGE = g_thermtesterVOLTAGE[9];
  // g_thermtesterTEMPERATURE = thermpoly(g_thermtesterVOLTAGE[9]);

  // spi_adc_snap_command();
  // HAL_Delay(8);

  // adbms_read_rdcvall_voltage(hbms.asic);
  // spi_adc_snap_command();
  // adbms_read_cell_voltages(hbms.asic);
  // Testing
  // adbms_read_status_registers(hbms.asic);
  // adbms_read_aux_voltages(hbms.asic);
  // adbms_read_raux_voltages(hbms.asic);
  //  adbms_read_filtered_cell_voltages(hbms.asic);
  // spi_adc_unsnap_command();

  adbms_read_rdasall_voltage(hbms.asic);

  vref2 = f2v(hbms.asic->stat_a.VREF2);
  therm_open_wire_check();
  HAL_Delay(8);
  thermtestvoltage();
}

/*
  adbms_start_adc_cell_voltage_measurment(hbms.asic);
  adbms_poll_for_conversion_adc(hbms.asic);
  adbms_read_cell_voltages(hbms.asic);

  adbms_set_cell_pwm(hbms.asic, 11, 0, PWM_19_8_PERCENT_DUTY_CYCLE);
  adbms_read_aux_voltages(hbms.asic);

  // reads 15 (14+1) on the scope
  adbms_bleed_cell_pwm(hbms.asic, 14, 0, PWM_85_8_PERCENT_DUTY_CYCLE);
*/
