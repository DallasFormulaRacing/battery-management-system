#ifndef ADBMS_H
#define ADBMS_H

#include <stdbool.h>
#include <stdint.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>

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

#define CS_PIN GPIO_PIN_6 /* Mcu dependent chip select */
#define GPIO_PORT GPIOB   /* Mcu dependent adc chip select port */

static inline void Delay_ms(uint32_t delay);

static inline void ADBMS_CS_Low(void);

static inline void ADBMS_CS_High(void);

static inline void spiWriteBytes(uint16_t size, uint8_t *tx_Data);

static inline void spiWriteReadBytes(uint8_t *tx_data, uint8_t *rx_data,
                                     uint16_t size);

static inline void spiReadBytes(uint16_t size, uint8_t *rx_data);

static inline void startTimer(void);

static inline void stopTimer(void);

uint32_t getTimCount(void);

void ADBMS_Wakeup(uint8_t total_ic);

#endif
/** @}*/
/** @}*/
