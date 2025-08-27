/**
 * @author Will Kim
 * @email <wkim@utdallas.edu>
 * @create date 2025-08-04 14:45:17
 * @modify date 2025-08-07 20:21:34
 * @desc AD BMS 6830 driver header file
 */

#ifndef BMS_DRIVER_H
#define BMS_DRIVER_H

#include "main.h"
#include "spi.h"
#include "stm32g4xx_hal.h"
#include "commands.h"
#include "stm32g4xx_hal_def.h"
#include "stm32g4xx_hal_spi.h"
#include <stdio.h>

SPI_HandleTypeDef hspi2; 
SPI_HandleTypeDef hspi3;

void bms_iso_wake();
void bms_iso_wake_via_spi();

void bms_request_();

#endif // BMS_DRIVER_H