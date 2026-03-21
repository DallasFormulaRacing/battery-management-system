
#ifndef GUI_DRIVER
#define GUI_DRIVER

#include "gui_types.h"
#include "gui_data_handler.h"
#include "bms.h"
#include "fdcan.h"

#include "stm32g4xx_hal_fdcan.h"
#include "bms_types.h"

#include <stdint.h>

void process_can_command(uint32_t ext_id, uint8_t* data);

void send_can_error(can_error_id_t error_id);

#endif