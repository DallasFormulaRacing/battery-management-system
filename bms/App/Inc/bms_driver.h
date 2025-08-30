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

void bms_iso_wake_pin();
void bms_iso_wake_spi();

void bms_request_();

// See bus protocols

void bms_read();
void bms_write();

#endif // BMS_DRIVER_H