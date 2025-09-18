#ifndef __BMS_DRIVER_H
#define __BMS_DRIVER_H

#include <stdbool.h>
#include <stdint.h>

#include "main.h"
#include "stm32g4xx_hal.h"
#include "stm32g4xx_it.h"

extern ADC_HandleTypeDef hadc1;
extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c3;
extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi5;
extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart5;

#define CS_PIN GPIO_PIN_6
#define GPIO_PORT GPIOB

inline void delay(uint32_t ms);

inline void asic_cs_low(void);

inline void asic_cs_hi(void);

inline void spi_write(uint16_t size, uint8_t *tx_data);

inline void spi_write_read(uint8_t *tx_data, uint8_t *rx_data, uint16_t size);

inline void spi_read(uint16_t size, uint8_t *rx_data);

inline void start_timer(void);

inline void stop_timer(void);

uint32_t get_tim_count(void);

void asic_wakeup(uint8_t total_ic);

#endif
/** @}*/
/** @}*/
