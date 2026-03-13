#include "emulator.h"
#include "parse.h"
#include "stm32g4xx_hal.h"
#include <cstdlib>
#include <stdint.h>

bms_handler_t fake_bms = {0};

int16_t get_random_cell() {
  // desired range of random cell: 2.9 - 4.1
  // window is 1200
  srand(HAL_GetTick());
  int32_t integral = rand() % 1200 + 2900;
  return convert_voltage_machine_readable((float)(integral / 1000));
}
uint8_t get_random_temp() {}
uint16_t get_random_packvoltage() {}
uint16_t get_random_currentreading() {}
uint16_t get_random_soc() {}
void get_cell_balancing_status() {}