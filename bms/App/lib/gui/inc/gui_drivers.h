
#ifndef GUI_DRIVER
#define GUI_DRIVER

#include "gui_types.h"
#include "gui_data_handler.h"

#include "fdcan.h"
#include <string.h>

void process_can_command(uint32_t ext_id, uint8_t* data);

#endif