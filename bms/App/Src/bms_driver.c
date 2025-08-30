/**
 * @author Will Kim
 * @email <wkim@utdallas.edu>
 * @create date 2025-08-04 15:19:12
 * @modify date 2025-08-04 15:19:12
 * @desc AD BMS 6830 driver definition file
 */

#include "bms_driver.h" // SPI handle for AD BMS 6830
#include <stdint.h>


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

// SPI 1 Peripheral for Uplink
SPI_HandleTypeDef hspi1;

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

void bms_iso_wake_spi() {
    const uint8_t DUMMY_BYTES[2] = {0xFF, 0xFF};

    HAL_SPI_Transmit(&hspi1, (uint8_t *)&DUMMY_BYTES, sizeof(DUMMY_BYTES), HAL_MAX_DELAY);
}


