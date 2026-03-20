#include "spi.h"
#include "cmsis_os2.h"
#include "stm32g4xx_hal_spi.h"
#include <string.h>

// toggle if DMA on or off
extern osThreadId_t spi_thread_pid;

inline void delay(uint32_t ms) { HAL_Delay(ms); }

inline void asic_cs_low() {
  HAL_GPIO_WritePin(GPIO_PORT, CS_PIN, GPIO_PIN_RESET);
}

inline void asic_cs_hi() { HAL_GPIO_WritePin(GPIO_PORT, CS_PIN, GPIO_PIN_SET); }

inline void notify_SPI_task_on_DMA(SPI_HandleTypeDef *hspi) { (void)hspi; }

// we got data
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi) {
  notify_SPI_task_on_DMA(hspi);
}

// we sent and got data
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi) {
  notify_SPI_task_on_DMA(hspi);
}

// we sent data
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
  notify_SPI_task_on_DMA(hspi);
}

void spi_write(uint16_t size, uint8_t *tx_data) {
  asic_cs_low();
  HAL_SPI_Transmit_DMA(&hspi1, tx_data, size);
  // ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
  /* SPI1 , data, size, timeout */
  asic_cs_hi();
}

void spi_write_read(uint8_t *tx_data, uint8_t *rx_data, uint16_t size) {
  asic_cs_low();
  HAL_SPI_TransmitReceive_DMA(&hspi1, tx_data, rx_data, size);
  // ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
  asic_cs_hi();
}

void spi_read(uint16_t size, uint8_t *rx_data) {
  asic_cs_low();
  HAL_SPI_Receive_DMA(&hspi1, rx_data, size);
  // ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
  asic_cs_hi();
}

void print_over_uart(const char *str) {
  HAL_UART_Transmit(&hlpuart1, (uint8_t *)str, strlen(str), UART_TIME_OUT);
}

#if TIM_EN

void start_timer() { HAL_TIM_Base_Start_IT(&htim3); }

void stop_timer() { HAL_TIM_Base_Stop_IT(&htim3); }

uint32_t get_tim_count_with_reset() {
  uint32_t count = 0;
  count = __HAL_TIM_GetCounter(&htim3);
  __HAL_TIM_SetCounter(&htim3, 0);
  return (count);
}

#endif

/**
 * @brief Wake up IC chain
 * t ready 10 micro
 * t wake is 500 micro
 * @param total_ic
 */
void asic_wakeup(uint8_t total_ic) {
  for (uint8_t ic = 0; ic < total_ic; ic++) {
    asic_cs_low();
    delay(WAKEUP_DELAY);
    asic_cs_hi();
    delay(WAKEUP_DELAY);
  }
}
