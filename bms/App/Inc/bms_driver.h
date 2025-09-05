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
#include "commands.h"
#include "stm32g4xx_hal_spi.h"
#include <stdint.h>
#include <stdbool.h>

SPI_HandleTypeDef hspi2; 
SPI_HandleTypeDef hspi3;

enum {
NUM_PACKS_PER_SEGMENT = 23,
NUM_SEGMENTS = 6,
PLACEHOLDER_BCZ_IDK = 1,
SPI_MAILBOX_LENGTH = 32
};

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
    BMS_DTM_MEASURE,
    BMS_ERR,
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


typedef struct {
    bms_core_state_t current_state;
    bms_measure_status_t last_measure_status;
    uint32_t    t_now_ms;
    uint32_t    t_next_action_ms;
    uint8_t     spi_rx[SPI_MAILBOX_LENGTH];
    uint8_t     spi_tx[SPI_MAILBOX_LENGTH];
    uint32_t    pec_errors_count;
    uint32_t    comm_retries_count;
    bool        intr1_flag;
    bool        intr2_flag;

    uint16_t     pack_volt_mV[PLACEHOLDER_BCZ_IDK];
    uint16_t     pack_temp_degC[PLACEHOLDER_BCZ_IDK];  
    uint16_t     pack_curr_mA[PLACEHOLDER_BCZ_IDK];
    // status
    uint32_t    last_sample_ms;
    uint32_t    last_report_ms;
    uint32_t    fault_code;
} bms_system_context_block_t;

void bms_iso_wake_pin();
void bms_iso_wake_spi();

// See bus protocols

bms_core_state_t bms_init();
void bms_begin_task();

void bms_read();
void bms_write();

#endif // BMS_DRIVER_H