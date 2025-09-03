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
#include <stdint.h>

SPI_HandleTypeDef hspi2; 
SPI_HandleTypeDef hspi3;

/**
 * @brief Enumeration of measurement states
 * OK = all good
 * OVUV_ERR = over/under voltage error
 * OC_ERR = overcurrent error
 */
typedef enum {
    BMS_OK,
    BMS_OVUV_ERR,
    BMS_OC_ERR,
    BMS_OPEN_WIRE_ERR
} bms_measure_status_t;

/**
 * @brief Enumeration of ADBMS6830 chip states
 */
typedef enum {
    BMS_SLEEP,
    BMS_STDBY,
    BMS_EXT_BAL,
    BMS_MEASURE,
    BMS_REFUP,
    BMS_DTM_MEASURE
} bms_core_state_t;

/**
 * @brief Enumeration of ADBMS6822 isoSPI Transciever states
 * 
 */
typedef enum {
    ISOSPI_IDLE,
    ISOSPI_READY,
    ISOSPI_ACTIVE
} isoSPI_state_t;

void bms_iso_wake_pin();
void bms_iso_wake_spi();

// See bus protocols

void bms_read();
void bms_write();

#endif // BMS_DRIVER_H