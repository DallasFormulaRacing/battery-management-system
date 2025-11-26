#include "bms_driver.h"
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

void delay(uint32_t ms) { HAL_Delay(ms); }

void asic_cs_low() { HAL_GPIO_WritePin(GPIO_PORT, CS_PIN, GPIO_PIN_RESET); }

void asic_cs_hi() { HAL_GPIO_WritePin(GPIO_PORT, CS_PIN, GPIO_PIN_SET); }

void spi_write(uint16_t size, uint8_t *tx_data) {
  asic_cs_low();
  HAL_SPI_Transmit(hspi_bms, tx_data, size, SPI_TIME_OUT);
  /* SPI1 , data, size, timeout */
  asic_cs_hi();
}

void spi_write_read(uint8_t *tx_data, uint8_t *rx_data, uint16_t size) {
  asic_cs_low();
  HAL_SPI_Transmit(hspi_bms, tx_data, 4, SPI_TIME_OUT);
  HAL_SPI_Receive(hspi_bms, rx_data, size, SPI_TIME_OUT);
  asic_cs_hi();
}

void spi_read(uint16_t size, uint8_t *rx_data) {
  asic_cs_low();
  HAL_SPI_Receive(hspi_bms, rx_data, size, SPI_TIME_OUT);
  asic_cs_hi();
}

#if TIM_EN

void start_timer() { HAL_TIM_Base_Start(htim); }

void stop_timer() { HAL_TIM_Base_Stop(htim); }

uint32_t get_tim_count() {
  uint32_t count = 0;
  count = __HAL_TIM_GetCounter(htim);
  __HAL_TIM_SetCounter(htim, 0);
  return (count);
}

#endif

void asic_wakeup(uint8_t total_ic) {
  for (uint8_t ic = 0; ic < total_ic; ic++) {
    asic_cs_low();
    delay(WAKEUP_DELAY);
    asic_cs_hi();
    delay(WAKEUP_DELAY);
  }
}
