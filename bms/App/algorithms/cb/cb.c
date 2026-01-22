#include "cb.h"
#include "bms_enums.h"
#include "bms_types.h"

// ! table 95, page 68

/**
 * @brief
 * Some things to think about:
 * how do we want to collect the cells that need to be balanced?
 * queue? all at once?
 *
 * collate all cell votlages from the asic array 192 element (asic) to 144
 * element (pcb) -- is this necessary? -- not really but could make life easier
 * we have a lot of memory to spare.
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */

/**
 * @brief cell balance super loop
 *
 *
 * @param asic_ctx:
 * @return void
 */
void cell_delta_policy_enforcer(cell_asic_ctx_t *asic_ctx, pcb_ctx_t *pcb) {
  // todo
}

void copy_cell_voltages(cell_asic_ctx_t *asic_ctx, pcb_ctx_t *pcb) {
  // copy cells from asic
}

/**
 * @brief maps a certain cell delta to an appropriate duty cycle (since time is
 * fixed). if saturated (i.e. above a certain value), clamp to 100%. intervals
 * and corresponding pwm values are listed in descending order.
 *
 * a good starting point would be a 1-to-1 (mV difference maps to ceil(PWM))
 *
 * @param delta -- cell delta in signed int16
 * @return pwm_duty_cycle_t -- interpolated PWM Duty Cycle
 */
pwm_duty_cycle_t map_delta_to_pwm_discretize(voltage_readings_t delta) {
  float delta_v = convert_voltage_human_readable(delta);
  if (delta_v >= 0.100F /*is above X = 100 mV */)
    return PWM_100_0_PERCENT_DUTY_CYCLE;
  if (delta_v < 0.100F && delta_v >= 0.0924F /*is in between X and Y */)
    return PWM_92_4_PERCENT_DUTY_CYCLE;
  if (delta_v < 0.0924F && delta_v >= 0.0858F /*is in between X and Y */)
    return PWM_85_8_PERCENT_DUTY_CYCLE;
  if (delta_v < 0.0858F && delta_v >= 0.0792F /*is in between X and Y */)
    return PWM_79_2_PERCENT_DUTY_CYCLE;
  if (delta_v < 0.0792F && delta_v >= 0.0726F /*is in between X and Y */)
    return PWM_72_6_PERCENT_DUTY_CYCLE;
  if (delta_v < 0.0726F && delta_v >= 0.0660F /*is in between X and Y */)
    return PWM_66_0_PERCENT_DUTY_CYCLE;
  if (delta_v < 0.0660F && delta_v >= 0.0594F /*is in between X and Y */)
    return PWM_59_4_PERCENT_DUTY_CYCLE;
  if (delta_v < 0.0594F && delta_v >= 0.0528F /*is in between X and Y */)
    return PWM_52_8_PERCENT_DUTY_CYCLE;
  if (delta_v < 0.0528F && delta_v >= 0.0462F /*is in between X and Y */)
    return PWM_46_2_PERCENT_DUTY_CYCLE;
  if (delta_v < 0.0462F && delta_v >= 0.0396F /*is in between X and Y */)
    return PWM_39_6_PERCENT_DUTY_CYCLE;
  if (delta_v < 0.0396F && delta_v >= 0.0330F /*is in between X and Y */)
    return PWM_33_0_PERCENT_DUTY_CYCLE;
  if (delta_v < 0.0330F && delta_v >= 0.0264F /*is in between X and Y */)
    return PWM_26_4_PERCENT_DUTY_CYCLE;
  if (delta_v < 0.0264F && delta_v >= 0.0198F /*is in between X and Y */)
    return PWM_19_8_PERCENT_DUTY_CYCLE;
  if (delta_v < 0.0198F && delta_v >= 0.0132F /*is in between X and Y */)
    return PWM_13_2_PERCENT_DUTY_CYCLE;
  if (delta_v < 0.0132F && delta_v >= 0.0066F /*is in between X and Y */)
    return PWM_6_6_PERCENT_DUTY_CYCLE;
  return PWM_0_0_PERCENT_DUTY_CYCLE;
}
