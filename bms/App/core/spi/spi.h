#ifndef SPI_H
#define SPI_H

#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_tim.h"
#include "stm32g4xx_it.h"
#include <stdbool.h>
#include <stdint.h>

#define WAKEUP_DELAY 1 /* BMS ic wakeup delay  */
#define TIM_EN 1

#define SPI_TIME_OUT HAL_MAX_DELAY
#define UART_TIME_OUT HAL_MAX_DELAY
#define I2C_TIME_OUT HAL_MAX_DELAY

extern ADC_HandleTypeDef hadc1;
extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c3;
extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi5;
extern UART_HandleTypeDef hlpuart1;
extern TIM_HandleTypeDef htim3;

#define CS_PIN GPIO_PIN_6
#define GPIO_PORT GPIOB

void delay(uint32_t ms);

void asic_cs_low(void);

void asic_cs_hi(void);

void spi_write(uint16_t size, uint8_t *tx_data);

void spi_write_read(uint8_t *tx_data, uint8_t *rx_data, uint16_t size);

void spi_read(uint16_t size, uint8_t *rx_data);

void start_timer(void);

void stop_timer(void);

uint32_t get_tim_count(void);

void asic_wakeup(uint8_t total_ic);

void print_over_uart(const char *str);

#endif
/** @}*/
/** @}*/
