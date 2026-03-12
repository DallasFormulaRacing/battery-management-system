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

#include "../../program/bms.h"
#include "bms_types.h"

extern bms_handler_t fake_bms;

#endif