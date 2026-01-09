#ifndef BMS_DRIVER_H
#define BMS_DRIVER_H

#include <stdbool.h>
#include <stdint.h>

#include "bms_enums.h"
#include "bms_types.h"
#include "command_list.h"
#include "stm32g4xx_hal.h"
#include "stm32g4xx_it.h"

extern ADC_HandleTypeDef hadc1;
extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c3;
extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi5;
extern UART_HandleTypeDef hlpuart1;

#define CS_PIN GPIO_PIN_6
#define GPIO_PORT GPIOB

void handle_init(void);

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
