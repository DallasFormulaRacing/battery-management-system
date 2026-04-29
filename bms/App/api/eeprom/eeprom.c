#include "eeprom.h"

#define EEPROM_ADDRESS 0x50

void setWriteProtect(int enable) {
    if (enable) {
        HAL_GPIO_WritePin(WriteProtect_EEPROM_GPIO_Port, WriteProtect_EEPROM_Pin, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(WriteProtect_EEPROM_GPIO_Port, WriteProtect_EEPROM_Pin, GPIO_PIN_RESET);
    }
}

int eeprom_init()
{
    setWriteProtect(1);
    return 0;
}

int eeprom_ready()
{
    HAL_StatusTypeDef status = HAL_I2C_IsDeviceReady(&hi2c1, (EEPROM_ADDRESS << 1), 3, 1);

    if (status == HAL_OK)
    {
        return 1;
    }
    else if (status == HAL_BUSY || status == HAL_TIMEOUT)
    {
        return 0;
    }

    return -1;
}

int eeprom_wait_until_ready(uint32_t timeout_ms)
{
    uint32_t tickstart = HAL_GetTick();
    while ((HAL_GetTick() - tickstart) < timeout_ms)
    {
        int ready = eeprom_ready();
        if (ready == 1) return 0;
        else if (ready == -1) return -1;
        HAL_Delay(5);
    }
    return -1;
}

int eeprom_write(uint16_t address, uint8_t *data, size_t size)
{
    if (size > 128)
        return -1;

    setWriteProtect(0);    

    HAL_StatusTypeDef status = HAL_I2C_Mem_Write(
        &hi2c1,
        (EEPROM_ADDRESS << 1),
        address,
        I2C_MEMADD_SIZE_16BIT,
        data,
        size,
        10);

    setWriteProtect(1);
        
    if (status != HAL_OK)
        return -1;

    return 0;
}

int eeprom_write_dma(uint16_t address, uint8_t *data, size_t size) {
    if (hi2c1.State != HAL_I2C_STATE_READY) return -1;

    setWriteProtect(0);

    HAL_StatusTypeDef status = HAL_I2C_Mem_Write_DMA(
        &hi2c1,
        (EEPROM_ADDRESS << 1),
        address,
        I2C_MEMADD_SIZE_16BIT,
        data,
        size
    );

    if (status != HAL_OK) {
        setWriteProtect(1);
        return -1;
    }

    return 0;
}

void eeprom_TxCpltCallback() {
    setWriteProtect(1);
}

int eeprom_read(uint16_t address, uint8_t *buffer, size_t size)
{
    if (buffer == NULL || size == 0)
        return -1;

    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(
        &hi2c1,
        (EEPROM_ADDRESS << 1),
        address,
        I2C_MEMADD_SIZE_16BIT,
        buffer,
        size,
        10);

    if (status != HAL_OK)
        return -1;

    return 0;
}

int eeprom_read_dma(uint16_t address, uint8_t *buffer, size_t size) {
    if (hi2c1.State != HAL_I2C_STATE_READY) return -1;

    HAL_StatusTypeDef status = HAL_I2C_Mem_Read_DMA(
        &hi2c1,
        (EEPROM_ADDRESS << 1),
        address,
        I2C_MEMADD_SIZE_16BIT,
        buffer,
        size
    );

    if (status != HAL_OK) {
        return -1;
    }

    return 0;
}