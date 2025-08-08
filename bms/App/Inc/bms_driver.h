/**
 * @author Will Kim
 * @email <wkim@utdallas.edu>
 * @create date 2025-08-04 14:45:17
 * @modify date 2025-08-07 20:21:34
 * @desc AD BMS 6830 driver header file
 */

#ifndef BMS_DRIVER_H
#define BMS_DRIVER_H

#include <stdint.h>

#include "stm32g4xx_hal.h" 
#include "spi.h"
#include "commands.h"

void bms_wakeup();

#endif // BMS_DRIVER_H