/**
 * @author Will Kim
 * @email <wkim@utdallas.edu>
 * @create date 2025-07-23 03:19:33
 * @modify date 2025-08-07 20:21:25
 * @desc Store safe operating limits, runtime configs, parameters and constants.
 */

#ifndef CONFIG_H
#define CONFIG_H

// Safe operating limits
#define TEMP_LIMIT_HIGH_F 1111
#define TEMP_LIMIT_LOW_F
#define VOLT_LIMIT_HIGH_V
#define VOLT_LIMIT_LOW_V
#define CURR_LIMIT_HIGH_A
#define CURR_LIMIT_LOW_A

// Runtime configurations
#define ADC_RESOLUTION
#define ADC_VREF 3.

// Parameters
#define NUM_CELLS
#define CELL_CAPACITY

// Constants
#define PI 3.14159

#endif // CONFIG_H