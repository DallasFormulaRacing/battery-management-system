#include "spi.h"
#include "cmsis_os2.h"
#include "stm32g4xx_hal_def.h"
#include "stm32g4xx_hal_spi.h"
#include <string.h>

extern osThreadId_t spi_thread_pid;
extern osMutexId_t spi_mutex_id;

const osMutexAttr_t spi_mutex_attr = {
    "spi1-mutex", osMutexRecursive | osMutexPrioInherit, NULL, 0U};

inline void delay(uint32_t ms) { HAL_Delay(ms); }

inline void asic_cs_low() {
  while (__HAL_SPI_GET_FLAG(&hspi1, SPI_FLAG_BSY))
    ;
  HAL_GPIO_WritePin(GPIO_PORT, CS_PIN, GPIO_PIN_RESET);
}

inline void asic_cs_hi() {
  while (__HAL_SPI_GET_FLAG(&hspi1, SPI_FLAG_BSY))
    ;
  HAL_GPIO_WritePin(GPIO_PORT, CS_PIN, GPIO_PIN_SET);
}

void notify_SPI_task_on_DMA(SPI_HandleTypeDef *hspi) {
  if (SPI1 == hspi->Instance && spi_thread_pid != NULL)
    osThreadFlagsSet(spi_thread_pid, SPI_THREAD_READY_FLAG);
}

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
  if (HAL_OK == HAL_SPI_Transmit_DMA(&hspi1, tx_data, size)) {

    uint32_t flags = osThreadFlagsWait(SPI_THREAD_READY_FLAG, osFlagsWaitAny,
                                       SPI_DMA_FLAG_WAIT);

    if (flags == (uint32_t)osErrorTimeout) {
      HAL_SPI_Abort(&hspi1);
    }
  }
  asic_cs_hi();
}

void spi_write_read(uint8_t *tx_data, uint8_t *rx_data, uint16_t size) {
  asic_cs_low();
  if (HAL_OK == HAL_SPI_TransmitReceive_DMA(&hspi1, tx_data, rx_data, size)) {
    uint32_t flags = osThreadFlagsWait(SPI_THREAD_READY_FLAG, osFlagsWaitAny,
                                       SPI_DMA_FLAG_WAIT);

    if (flags == (uint32_t)osErrorTimeout)
      HAL_SPI_Abort(&hspi1);
  }
  asic_cs_hi();
}

void spi_read(uint16_t size, uint8_t *rx_data) {
  asic_cs_low();
  if (HAL_OK == HAL_SPI_Receive_DMA(&hspi1, rx_data, size)) {
    uint32_t flags = osThreadFlagsWait(SPI_THREAD_READY_FLAG, osFlagsWaitAny,
                                       SPI_DMA_FLAG_WAIT);

    if (flags == (uint32_t)osErrorTimeout)
      HAL_SPI_Abort(&hspi1);
  }
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
    osDelay(WAKEUP_DELAY);
    asic_cs_hi();
    osDelay(WAKEUP_DELAY);
  }
}
