#include "emulator.h"
#include "parse.h"
#include "stm32g4xx_hal.h"
#include <cstdlib>
#include <stdint.h>

bms_handler_t fake_bms = {0};

int16_t get_random_cell() {
  // desired range of random cell: 2.9 - 4.1
  // window is 1200
  srand(HAL_GetTick() << HAL_GetTick() & HAL_GetTick());
  int32_t integral = (rand() % 1200) + 2900;
  return convert_voltage_machine_readable(((float)integral / 1000));
}
uint8_t get_random_temp() {
  // desired range of random temp: 10C to 59C
  // window is 49
  srand(HAL_GetTick() << HAL_GetTick() & HAL_GetTick());

  return (uint8_t)((rand() % 49) + 10);
}
uint16_t get_random_packvoltage() {
  // desired range of random pack voltage: 450 V 600 V

  return ((uint16_t)get_random_cell() * 144);
}
uint16_t get_random_currentreading() {
  // 20-90
  srand(HAL_GetTick() << HAL_GetTick() & HAL_GetTick());
  return (rand() % 70) + 20;
}
uint16_t get_random_soc() {
  srand(HAL_GetTick() << HAL_GetTick() & HAL_GetTick());
  return rand() % 100;
}
void get_cell_balancing_status(pcb_ctx_t *pcb) {}