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


CABMessage ProcessCABMessage();

#endif
