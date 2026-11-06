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
#define ERROR_OCCURRED_BITMASK 1
#define CAB_ID 0x3C2
#define CAB_0_CURRENT_OFFSET 0x80000000

#define FRAME_PERIOD_SUB_FUNCTION 0xF013
#define WRITE_BY_DATA_IDENTIFIER 0x2E
#define FRAME_PERIOD_DATA_LENGTH 0x05

typedef enum
{
    NODE_UNKNOWN = 0x00,
    NODE_ALL = 0x01,
    NODE_FL = 0x02,
    NODE_FR = 0x03,
    NODE_RL = 0x04,
    NODE_RR = 0x05,
    NODE_NUCLEO_1 = 0x06,
    NODE_NUCLEO_2 = 0x07,
    NODE_VCU = 0x1B,
    NODE_BMS = 0x1C,
    NODE_DASH = 0x1D,
    NODE_RASPI = 0x1E,
} can_device_id_t;

cab_message_t parse_cab_message(const uint8_t *rx_data);
void process_cab_message(const uint8_t *data);
void set_cab_output_frequency(const uint16_t period_ms);

#endif
