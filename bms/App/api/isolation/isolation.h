#ifndef IMD_H
#define IMD_H

#include "imd_types.h"
#include "imd_drivers.h"
// Required for repacking to FDCAN to send to GUI
#include "gui_drivers.h"
#include "gui_types.h"
#include <stdbool.h>

void imd_init(void);         // calls configure_imd_params + configure_imd_cyclic

bool imd_is_healthy(void);
bool imd_is_safe_to_start(void);
IMD_Status_t imd_get_status(void);     // returns the current status flags

uint16_t imd_get_isolation_resistance(void);
bool imd_isolation_alarm_active(void);

void imd_reset_fault(void);   // calls reset_imd_alarm internally

// Responds to different cyclic incoming messages
void handle_general(IMD_Data_t data);
void handle_voltage(IMD_Data_t data);

#endif