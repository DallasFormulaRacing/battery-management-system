#include "cb.h"

// ! table 95, page 68
// ! needs to be refactored properly, just messy rn for testing purposes

voltage_readings_t find_lowest_cell_voltage(cell_asic_ctx_t *asic_ctx) {
  voltage_readings_t lowest = INT16_MAX;
  voltage_readings_t *array = NULL;
  for (uint8_t ic = 0; ic < IC_COUNT_CHAIN; ic++) {
    array = asic_ctx[ic].cell.cell_voltages_array;
    // handle chain asic so surround this in a for loop this is O(N^2) 12 x 12
    for (voltage_readings_t i = 0; i < ADBMS_NUM_CELLS_PER_IC; i++) {
      if (array[i] < lowest) {
        lowest = array[i];
      }
    }
  }
  return lowest;
}

/*implementation defined, we dont have enough details rn to implement this*/
// uint16_t find_cells_to_balance(cell_asic_ctx_t *asic_ctx,
//                                voltage_readings_t lowest, uint8_t queue[],
//                                voltage_readings_t delta) {
//   voltage_readings_t *array = asic_ctx->cell.cell_voltages_array;

//   voltage_readings_t upper_bound = (voltage_readings_t)(lowest + delta);

//   uint16_t num_queue_elems = 0;

//   for (voltage_readings_t i = 0; i < ADBMS_NUM_CELLS_PER_IC; i++) {
//     if (array[i] <= upper_bound) {
//       queue[num_queue_elems] = i;
//       num_queue_elems++;
//     }
//   }

//   return num_queue_elems;
// }

void send_pwm_to_cell(asic_ctx_t *asic_ctx, uint8_t cell_number,
                      pwm_duty_cycle_t duty_cycle) {
  // break this into two functions
  // the first one will map cell to pwm channel number
  // the second will send the actual command
  // we also want to take in pwm duty cycle as an argument

  // DCTO = discharge time out: basically, if this is 0 pwm will not stop
}

/*pwm_duty_cycle_t duty_cycle =
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
  }*/

/**
 * @brief cell balance super loop
 *
 *
 * @param asic_ctx: The context of the ASIC
 * @return void
 */
void cell_delta_policy_enforcer(cell_asic_ctx_t *asic_ctx) {};

void set_cell_pwm_duty_cycle(const asic_ctx_t *asic_ctx, uint8_t cell_number,
                             pwm_duty_cycle_t duty_cycle) {
  // set the config struct in the asic ctx
  // then pack it into the appropriate pwm mailbox
  if (cell_number < ADBMS_NUM_PWMA_CHANNELS) {
    asic_ctx->pwm_ctl_a.pwm_a_ctl_array[cell_number] = duty_cycle;
  } else {
    asic_ctx->pwm_ctl_b.pwm_b_ctl_array[cell_number - 12] = duty_cycle;
  }
}

void clear_cell_pwm_duty_cycle(const asic_ctx_t *asic_ctx,
                               uint8_t cell_number) {
  for (uint8_t ic = 0; ic < IC_COUNT_CHAIN; ic++) {
    if (cell_number < ADBMS_NUM_PWMA_CHANNELS) {
      asic_ctx[ic].pwm_ctl_a.pwm_a_ctl_array[cell_number] = 0;
    } else {
      asic_ctx[ic].pwm_ctl_b.pwm_b_ctl_array[cell_number - 12] = 0;
    }
  }
}

void pack_pwm_duty_cycle_into_mailbox(const asic_ctx_t *asic_ctx) {
  asic_mailbox_t *pwm_a_mailbox = &asic_ctx->pwm_a_mb;
  asic_mailbox_t *pwm_b_mailbox = &asic_ctx->pwm_b_mb;
  pwm_reg_a_t pwm_a_cfg = asic_ctx->pwm_ctl_a;
  pwm_reg_b_t pwm_b_cfg = asic_ctx->pwm_ctl_b;

  for (uint8_t cell = 0;
       cell < ADBMS_NUM_PWMA_CHANNELS + ADBMS_NUM_PWMB_CHANNELS; cell++) {
    if (cell < ADBMS_NUM_PWMA_CHANNELS) {
      pwm_a_mailbox->tx_data_array[cell] = pwm_a_cfg.pwm_a_ctl_array[cell];
    } else {
      pwm_b_mailbox->tx_data_array[cell - ADBMS_NUM_PWMA_CHANNELS] =
          pwm_b_cfg.pwm_b_ctl_array[cell - ADBMS_NUM_PWMA_CHANNELS];
    }
  }
}