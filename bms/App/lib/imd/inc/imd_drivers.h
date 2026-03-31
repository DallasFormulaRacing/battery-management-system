#ifndef IMD_DRIVER
#define IMD_DRIVER

#include "can.h"
#include "imd_types.h"
#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_def.h"
#include "stm32g4xx_hal_fdcan.h"

void imd_build_packet(IMD_CanId_t cmd, IMD_Packet_t *packet);

#endif