/**
 * @file emulator.h

 * @author Will Kim
 * @brief Creates fake BMS data to test CAN transmission to the GUI
          Fake BMS data assumes final battery pack design
 * @version 0.1
 * @date 2026-03-12
 *
 */

#ifndef EMULATOR_H
#define EMULATOR_H

#include "../../math/cb/cb.h"
#include "../../program/bms.h"
#include "bms_types.h"
#include "parse.h"
#include <stdint.h>
#include <stdlib.h>

extern bms_handler_t fake_bms;

void init_emulator();

int16_t get_random_cell();
uint8_t get_random_temp();
uint16_t get_random_packvoltage();
uint16_t get_random_currentreading();
uint16_t get_random_soc();
void get_cell_balancing_status();

#endif