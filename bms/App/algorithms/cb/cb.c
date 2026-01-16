#include "bms_enums.h"
#include "bms_types.h"
#include "config.h"
#include "parse.h"
#include <stddef.h>
#include <stdint.h>

voltage_readings_t find_lowest_cell_voltage(cell_asic_ctx_t *asic_ctx) {
  voltage_readings_t *array = asic_ctx->cell.cell_voltages_array;
  voltage_readings_t lowest = array[0];
  for (voltage_readings_t i = 0; i < ADBMS_NUM_CELLS_PER_IC; i++) {
    if (array[i] < lowest) {
      lowest = array[i];
    }
  }
  return lowest;
}

uint16_t find_cells_to_balance(cell_asic_ctx_t *asic_ctx,
                               voltage_readings_t lowest, uint8_t queue[],
                               voltage_readings_t delta) {
  voltage_readings_t *array = asic_ctx->cell.cell_voltages_array;

  voltage_readings_t upper_bound =
      (voltage_readings_t)(lowest + (lowest * delta));
  voltage_readings_t lower_bound =
      (voltage_readings_t)(lowest - (lowest * delta));

  uint16_t num_queue_elems = 0;

  for (voltage_readings_t i = 0; i < ADBMS_NUM_CELLS_PER_IC; i++) {
    if (array[i] >= lower_bound && array[i] <= upper_bound) {
      queue[num_queue_elems] = i;
      num_queue_elems++;
    }
  }

  return num_queue_elems;
}

void send_pwm_to_cell(uint16_t num_queue_elems, cell_asic_ctx_t *asic_ctx,
                      const uint8_t queue[], voltage_readings_t delta) {
  pwm_duty_cycle_t duty_cycle =
      PWM_33_0_PERCENT_DUTY_CYCLE; // NOTE: Keeping the duty cycle fixed for now

  for (size_t i = 0; i < num_queue_elems; i++) {
    if (queue[i] < ADBMS_NUM_PWMA_CHANNELS) {
      set_pwm_duty_cycle_target_single(asic_ctx, 0, duty_cycle, PWM_REG_GROUP_A,
                                       queue[i]);
      // asic_ctx->pwm_ctl_a.pwm_a_ctl_array[i] = duty_cycle;
    } else if (queue[i] >= ADBMS_NUM_PWMA_CHANNELS) {
      set_pwm_duty_cycle_target_single(asic_ctx, 0, duty_cycle, PWM_REG_GROUP_B,
                                       queue[i]);
      // asic_ctx->pwm_ctl_b.pwm_b_ctl_array[i] = duty_cycle;
    }
  }
}
