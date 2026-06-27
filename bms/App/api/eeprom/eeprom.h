#pragma once
#include "stm32g4xx_hal.h"
#include "main.h"

extern I2C_HandleTypeDef hi2c1;

int eeprom_init();

int eeprom_ready();
int eeprom_wait_until_ready(uint32_t timeout_ms);

int eeprom_write(uint16_t address, uint8_t* data, size_t size);
int eeprom_read(uint16_t address, uint8_t* buffer, size_t size);

int eeprom_write_dma(uint16_t address, uint8_t* data, size_t size);
int eeprom_read_dma(uint16_t address, uint8_t* buffer, size_t size);

void eeprom_TxCpltCallback();