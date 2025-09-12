/**
 * @author Will Kim
 * @email <wkim@utdallas.edu>
 * @create date 2025-07-23 03:19:33
 * @modify date 2025-08-07 20:21:25
 * @desc Store safe operating limits, runtime configs, parameters and constants.
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <stdbool.h>

enum {
    NUM_PACKS_PER_SEGMENT = 23,
    NUM_SEGMENTS = 6,
    PLACEHOLDER_BCZ_IDK = 1,
    SPI_MAILBOX_LENGTH = 32,
    MAX_NOM_VOLT_mV = 4200,
    MIN_NOM_VOLT_mV = 2800,
    MAX_TEMP_DEG_C = 60,
    MAX_NOM_CURR_A = 180,
    BMS_SPI_TX_TIMEOUT = 100,
};

const double FULL_CHARGE_CAPACITY_AH = 0;


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

typedef enum {
    OPEN,
    CLOSED
} SHUTDOWN_CIRCUIT_STATE;

#endif // CONFIG_H