#ifndef STM32G4XX_HAL_SPI_H
#define STM32G4XX_HAL_SPI_H

#include <stdint.h>

typedef struct
{
    void *Instance;
} SPI_HandleTypeDef;

typedef enum
{
    HAL_OK = 0,
    HAL_ERROR
} HAL_StatusTypeDef;

/* Dummy functions */
static inline HAL_StatusTypeDef HAL_SPI_Transmit(
    SPI_HandleTypeDef *hspi,
    uint8_t *pData,
    uint16_t Size,
    uint32_t Timeout)
{
    return HAL_OK;
}

static inline HAL_StatusTypeDef HAL_SPI_Receive(
    SPI_HandleTypeDef *hspi,
    uint8_t *pData,
    uint16_t Size,
    uint32_t Timeout)
{
    return HAL_OK;
}

static inline HAL_StatusTypeDef HAL_SPI_TransmitReceive(
    SPI_HandleTypeDef *hspi,
    uint8_t *tx,
    uint8_t *rx,
    uint16_t Size,
    uint32_t Timeout)
{
    return HAL_OK;
}

#endif