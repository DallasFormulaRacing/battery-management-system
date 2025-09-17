/**
 * @author Will Kim
 * @email <wkim@utdallas.edu>
 * @create date 2025-08-04 14:45:17
 * @modify date 2025-08-07 20:21:34
 * @desc AD BMS 6830 driver header file
 */

#ifndef BMS_DRIVER_H
#define BMS_DRIVER_H

#include "current.h"
#include "spi.h"
#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_def.h"
#include "stm32g4xx_hal_spi.h"
#include "stm32g4xx_hal_tim.h"
#include "commands.h"
#include "config.h"
#include "stm32g4xx_hal_uart.h"

SPI_HandleTypeDef hspi2; 
SPI_HandleTypeDef hspi3;
TIM_HandleTypeDef htim2;
UART_HandleTypeDef huart1;
FDCAN_HandleTypeDef hfdcan2;

typedef struct {
    bms_core_state_t current_state;
    bms_measure_status_t last_measure_status;
    coulomb_counter_t state_of_charge;

    uint32_t    t_now_ms;
    uint32_t    t_next_action_ms;

    uint8_t     spi_rx[SPI_MAILBOX_LENGTH];
    uint8_t     spi_tx[SPI_MAILBOX_LENGTH];

    uint32_t    pec_errors_count;
    uint32_t    comm_retries_count;

    
    uint16_t     high_current_mA;

    uint32_t    last_sample_ms;
    uint32_t    last_report_ms;
    uint32_t    fault_code;
} bms_system_context_block_t;

typedef struct {
    float raw_voltage;
    float avg_voltage;
    float filtered_voltage;
    float temperature;
} cell_info_t;

void bms_iso_wake_pin();
void bms_iso_wake_spi();

bms_core_state_t bms_init();
void bms_begin_task();

// void bms_read();
// void bms_write();
HAL_StatusTypeDef bms_transmit(SPI_HandleTypeDef hspi, message_t tx_msg);

message_t pack_message(const message_command_t *msg_c, const message_header_t *msg_h);
message_t unpack_message(const uint8_t *data);

void set_shutdown_circuit(SHUTDOWN_CIRCUIT_STATE line);

// See bus protocols

#endif // BMS_DRIVER_H