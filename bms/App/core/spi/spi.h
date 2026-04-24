#ifndef STM32G4XX_HAL_SPI_H
#define STM32G4XX_HAL_SPI_H

#include "stm32g4xx_hal.h"
#include <stdint.h>

typedef struct
{
    void *Instance;
} SPI_HandleTypeDef;

static inline HAL_StatusTypeDef HAL_SPI_Transmit_DMA(
    SPI_HandleTypeDef *hspi,
    const uint8_t *pData,
    uint16_t Size)
{
    (void)hspi; (void)pData; (void)Size;
    return HAL_OK;
}

static inline HAL_StatusTypeDef HAL_SPI_Receive_DMA(
    SPI_HandleTypeDef *hspi,
    uint8_t *pData,
    uint16_t Size)
{
    (void)hspi; (void)pData; (void)Size;
    return HAL_OK;
}

static inline HAL_StatusTypeDef HAL_SPI_TransmitReceive_DMA(
    SPI_HandleTypeDef *hspi,
    const uint8_t *txData,
    uint8_t *rxData,
    uint16_t Size)
{
    (void)hspi; (void)txData; (void)rxData; (void)Size;
    return HAL_OK;
}

static inline HAL_StatusTypeDef HAL_SPI_Abort(SPI_HandleTypeDef *hspi)
{
    (void)hspi;
    return HAL_OK;
}

void spi_write(uint16_t size, uint8_t *tx_data);

void spi_write_read(uint8_t *tx_data, uint8_t *rx_data, uint16_t size);
void asic_wakeup(uint8_t ic_count);

#endif