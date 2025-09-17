/**
 * @author Will Kim
 * @email <wkim@utdallas.edu>
 * @create date 2025-08-04 15:19:12
 * @modify date 2025-08-04 15:19:12
 * @desc AD BMS 6830 driver definition file
 */

#include "bms_driver.h" // SPI handle for AD BMS 6830
#include "commands.h"
#include "main.h"
#include "stm32g4xx_hal_def.h"
#include "stm32g4xx_hal_fdcan.h"
#include "stm32g4xx_hal_gpio.h"
#include "stm32g4xx_hal_spi.h"
#include <string.h>

// Pin PA10
#define BMS_INTR1_GPIO_Port GPIOA
#define BMS_INTR1_Pin GPIO_PIN_10

// Pin PC9
#define BMS_INTR2_GPIO_Port GPIOC
#define BMS_INTR2_Pin GPIO_PIN_9

// Pin PA9
#define BMS_WAKE1_GPIO_Port GPIOA
#define BMS_WAKE1_Pin GPIO_PIN_9

// Pin PC8
#define BMS_WAKE2_GPIO_Port GPIOC
#define BMS_WAKE2_Pin GPIO_PIN_8

#define WAKE_TX_DELAY_MS

typedef enum {
    MESSAGE_COPY_TO_BYTE,
    MESSAGE_COPY_FROM_BYTE
} memcpy_direction;

/**
 * @brief Helper function to copy messages 
 * between different representations
 * @param src 
 * @param direction of translation
 * @param dest 
 */
static inline void message_memcpy(void *src, memcpy_direction direction, void *dest) {
    if (!src || !dest) {
        return;
    }

    if (direction == MESSAGE_COPY_TO_BYTE) {
        const message_t *msg = (const message_t *)src;
        uint8_t *out = (uint8_t *)dest;
        memcpy(&out[0], msg->command.cmd, 2);
        memcpy(&out[2], msg->message_header.pec, 2);
    } 
    
    else { 
        message_t *msg = (message_t *)dest;
        const uint8_t *in = (const uint8_t *)src;
        memcpy(msg->command.cmd, &in[0], 2);
        memcpy(msg->message_header.pec, &in[2], 2);
    }
}

/**
 * @brief 
 * 
 */
 void bms_iso_wake_pin() {
    // Set the wake pins to high
    HAL_GPIO_WritePin(BMS_WAKE1_GPIO_Port, BMS_WAKE1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(BMS_WAKE2_GPIO_Port, BMS_WAKE2_Pin, GPIO_PIN_SET);

    // 5 micro delay
    // __asm__("nop");
    // __asm__("nop");
    // __asm__("nop");
    // __asm__("nop");
    // __asm__("nop");

    // Set the wake pins to low
    HAL_GPIO_WritePin(BMS_WAKE1_GPIO_Port, BMS_WAKE1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(BMS_WAKE2_GPIO_Port, BMS_WAKE2_Pin, GPIO_PIN_RESET);
}

/**
 * @brief 
 * 
 */
void bms_iso_wake_spi() {
    const uint8_t DUMMY_BYTES[2] = {0xFF, 0xFF};

    HAL_SPI_Transmit(&hspi2, (uint8_t *)&DUMMY_BYTES, sizeof(DUMMY_BYTES), HAL_MAX_DELAY);
    HAL_SPI_Transmit(&hspi3, (uint8_t *)&DUMMY_BYTES, sizeof(DUMMY_BYTES), HAL_MAX_DELAY);
}

/**
 * @brief 
 * 
 * @return bms_core_state_t 
 */
bms_core_state_t bms_init() {
    // Initialize the SPI peripherals
    (void)HAL_SPI_Init(&hspi2);
    (void)HAL_SPI_Init(&hspi3);

    // Initialize timer peripheral
    (void)HAL_TIM_Base_Init(&htim2);
    (void)HAL_TIM_Base_Start_IT(&htim2);

    // Start timer tick
    (void)HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);

    // Initialize UART peripheral #ifdef DEBUG
    (void)HAL_UART_Init(&huart1);

    // Initialize CAN peripheral
    (void)HAL_FDCAN_Init(&hfdcan2);

    // Drive SHUTDOWN SIGNAL HIGH
    set_shutdown_circuit(CLOSED);

    // Wake BMS chain
    bms_iso_wake_pin();

    // Start ADCs -- command

    // Write to a configuration register to start ADCs -- command

    // Read from a status register confirming OK status -- command

    // Do other stuff before task loop idk
    return BMS_STDBY;
}

/**
 * @brief 
 * 
 */
void bms_service_taskd() {
    // loop register read commands 
        // Cell voltages
        // open wire (OW) detection
    // pack can message
    // report to CAN line 

}

HAL_StatusTypeDef bms_transmit(SPI_HandleTypeDef hspi, message_t tx_msg) {
    //return HAL_SPI_Transmit(&hspi, (uint8_t) ???, 16, HAL_MAX_DELAY);
    return HAL_OK;
}

void set_shutdown_circuit(SHUTDOWN_CIRCUIT_STATE line) {
    switch (line) {
        case OPEN: 
            HAL_GPIO_WritePin(SHUTDOWN_GPIO_Port, SHUTDOWN_Pin , GPIO_PIN_RESET);
            return;
        case CLOSED: 
            HAL_GPIO_WritePin(SHUTDOWN_GPIO_Port, SHUTDOWN_Pin , GPIO_PIN_SET);
            return;
    }
}

/*
implement HAL_TIM_Base_Start_IT

__HAL_TIM_GET_COUNTER

HAL_TIM_IC_Start_IT, HAL_TIM_ReadCapturedValue

HAL_TIM_OC_Start_IT, HAL_TIM_OC_Stop_IT, __HAL_TIM_SET_COMPARE

HAL_TIM_PeriodElapsedCallback

HAL_TIM_IC_CaptureCallback

HAL_TIM_OC_DelayElapsedCallback
*/


