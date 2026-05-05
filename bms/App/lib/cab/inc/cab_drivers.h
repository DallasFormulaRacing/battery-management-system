#ifndef CAB_DRIVERS
#define CAB_DRIVERS

#include "bms.h"
#include "bms_types.h"
#include "cmsis_os2.h"
#include "config.h"
#include "fdcan.h"
#include "cab_types.h"
#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_fdcan.h"
#include <stdint.h>

#define ERROR_INFORMATION_BITMASK 0xFE
#define ERORR_OCCURED_BITMASK 1
#define CAB_ID 0x3C2
#define CAB_0_CURRENT_OFFSET 0x80000000

cab_message_t parse_cab_message(const uint8_t* data);
void process_cab_message(const uint8_t* data);

#endif
