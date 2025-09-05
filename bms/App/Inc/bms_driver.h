/**
 * @author Will Kim
 * @email <wkim@utdallas.edu>
 * @create date 2025-08-04 14:45:17
 * @modify date 2025-08-07 20:21:34
 * @desc AD BMS 6830 driver header file
 */

#ifndef BMS_DRIVER_H
#define BMS_DRIVER_H

#include "spi.h"
#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_spi.h"
#include "stm32g4xx_hal_tim.h"
#include "commands.h"
#include "config.h"

SPI_HandleTypeDef hspi2; 
SPI_HandleTypeDef hspi3;
TIM_HandleTypeDef htim2;

void bms_iso_wake_pin();
void bms_iso_wake_spi();

// See bus protocols

bms_core_state_t bms_init();
void bms_begin_task();

void bms_read();
void bms_write();

#endif // BMS_DRIVER_H