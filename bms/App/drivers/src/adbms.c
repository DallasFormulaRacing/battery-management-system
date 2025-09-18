#include "adbms.h"
#define WAKEUP_DELAY 1 /* BMS ic wakeup delay  */
#define TIM_EN 0

#define SPI_TIME_OUT HAL_MAX_DELAY
#define UART_TIME_OUT HAL_MAX_DELAY
#define I2C_TIME_OUT HAL_MAX_DELAY

SPI_HandleTypeDef *hspi_bms;
UART_HandleTypeDef *huart_bms;
I2C_HandleTypeDef *hi2c_bms;

void handle_init() {
  hspi_bms = &hspi1;
  huart_bms = &huart5;
  hi2c_bms = &hi2c1;
}

static inline void Delay_ms(uint32_t delay) { HAL_Delay(delay); }

static inline void adBmsCsLow() {
  HAL_GPIO_WritePin(GPIO_PORT, CS_PIN, GPIO_PIN_RESET);
}

void adBmsCsHigh() { HAL_GPIO_WritePin(GPIO_PORT, CS_PIN, GPIO_PIN_SET); }

static inline void spiWriteBytes(uint16_t size, uint8_t *tx_Data) {
  HAL_SPI_Transmit(hspi_bms, tx_Data, size, SPI_TIME_OUT);
  /* SPI1 , data, size, timeout */
}

static inline void spiWriteReadBytes(uint8_t *tx_data, uint8_t *rx_data,
                                     uint16_t size) {
  HAL_SPI_Transmit(hspi_bms, tx_data, 4, SPI_TIME_OUT);
  HAL_SPI_Receive(hspi_bms, rx_data, size, SPI_TIME_OUT);
}

static inline void spiReadBytes(uint16_t size, uint8_t *rx_data) {
  HAL_SPI_Receive(hspi_bms, rx_data, size, SPI_TIME_OUT);
}

#if TIM_EN

void startTimer() { HAL_TIM_Base_Start(htim); }

void stopTimer() { HAL_TIM_Base_Stop(htim); }

uint32_t getTimCount() {
  uint32_t count = 0;
  count = __HAL_TIM_GetCounter(htim);
  __HAL_TIM_SetCounter(htim, 0);
  return (count);
}

#endif

void adBmsWakeupIc(uint8_t total_ic) {
  for (uint8_t ic = 0; ic < total_ic; ic++) {
    adBmsCsLow();
    Delay_ms(WAKEUP_DELAY);
    adBmsCsHigh();
    Delay_ms(WAKEUP_DELAY);
  }
}
