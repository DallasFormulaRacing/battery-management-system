#ifndef IMD_H
#define IMD_H

#include "imd_drivers.h"
#include "imd_types.h"

// Required for repacking to FDCAN to send to GUI
#include "gui_drivers.h"
#include "gui_types.h"
#include <stdbool.h>

void imd_init(void); // calls configure_imd_params + configure_imd_cyclic

bool imd_is_healthy(void);
bool imd_is_safe_to_start(void);

uint16_t imd_get_isolation_resistance(void);
bool imd_isolation_alarm_active(void);

void imd_reset_fault(void); // calls reset_imd_alarm internally

// Responds to different cyclic incoming messages
void handle_general(IMD_Data_t data);
void handle_voltage(IMD_Data_t data);

void handle_error(IMD_Data_t data);

IMD_Config_Profile config = {0x1F4, 0xFF, 0x3E8, 0xFE, 0x00, 0xFD};

#endif