/*
 * BMS settings for the ev27-can node code (from the ev27-can template).
 * See bms/submodules/ev27-can/template/README.md.
 */
#ifndef EV27_DEVICE_CONFIG_H
#define EV27_DEVICE_CONFIG_H

#include "ev27_devices.h"
#include "ev27_main.h"
#include "stm32g4xx_hal.h"

/* Who this device is (can/devices.yaml). Status goes out on 0x700 + 0x03 = 0x703. */
#define EV27_MY_ADDR      EV27_ADDR_BMS
#define EV27_MY_ROLE      EV27_MAIN_STATUS_ROLE_NONE_CHOICE

/* Firmware version reported in Status. */
#define EV27_FW_MAJOR     0
#define EV27_FW_MINOR     1
#define EV27_FW_PATCH     0
#ifndef EV27_GIT_HASH
#define EV27_GIT_HASH     0x00000000u  /* TODO: set from the build: -DEV27_GIT_HASH=0x<first 8 hex digits> */
#endif

/* Hardware, as CubeMX names it (both defined in Core/Src/main.c). */
#define EV27_MAIN_FDCAN   hfdcan2   /* main bus, CAN FD 1M/1M, no BRS */
#define EV27_US_TIMER     htim2     /* 32-bit, 1 MHz, free running, started in main() */
extern FDCAN_HandleTypeDef hfdcan2;
extern TIM_HandleTypeDef htim2;

/* Main-bus 11-bit IDs the BMS receives. Everything else 11-bit is dropped in
 * hardware. Two IDs per filter, so FDCAN2 "Std Filters Nbr" (StdFiltersNbr) must
 * be at least (number of IDs + 1) / 2 = 2. The GUI's 29-bit protocol has its own
 * filter in core/fdcan/fdcan.c. */
#define EV27_RX_IDS { \
    EV27_MAIN_CM5_TIME_SYNC_FRAME_ID, \
    EV27_MAIN_CM5_COMMAND_FRAME_ID, \
    EV27_MAIN_CM5_CHARGE_CONTROL_FRAME_ID, \
}

#endif /* EV27_DEVICE_CONFIG_H */
