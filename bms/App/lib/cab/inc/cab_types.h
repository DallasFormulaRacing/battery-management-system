#ifndef CAB_TYPES
#define CAB_TYPES

typedef enum
{
    OK = 0x00,
    MEMORY_ERROR = 0x40,
    OVER_CURRENT = 0x41,
    FLUXGATE_NO_OSCILLATION = 0x42,
    WRONG_SUPPLY_VOLTAGE = 0x46,
    DEFAULT_ADC_CHANNEL = 0x47,
    NO_NEW_DATA = 0x49,
    DEFAULT_DAC_THRESHOLD = 0x4A,
    DEFAULT_REFERENCE_VOLTAGE = 0x4B
} cab_error_t;

typedef struct
{
    int32_t current;
    cab_error_t error_status;
    uint8_t productName[2];
    uint8_t softwareRevision;
} cab_message_t;

typedef enum
{ // borrowed from gui_types.h
    CAN_PRIORITY_P0 = 0x0,
    CAN_PRIORITY_P1 = 0x1,
    CAN_PRIORITY_P2 = 0x2,
    CAN_PRIORITY_P3 = 0x3,
    CAN_PRIORITY_P4 = 0x4,
    CAN_PRIORITY_P5 = 0x5,
    CAN_PRIORITY_P6 = 0x6,
    CAN_PRIORITY_P7 = 0x7,
} can_priority_t;

#define CAN_EXT_ID_MASK 0x1FFFFFFFU

// probably a better way than copy and pasting?
static inline uint32_t can_id_build(can_priority_t priority, // borrowed from gui_types.h
                                    uint8_t target,
                                    uint16_t cmd,
                                    uint8_t source)
{
    uint32_t priority_field = ((uint32_t)priority & 0x07U) << 26;
    uint32_t target_field = ((uint32_t)target & 0x1FU) << 21;
    uint32_t cmd_field = ((uint32_t)cmd & 0xFFFFU) << 5;
    uint32_t source_field = ((uint32_t)source & 0x1FU);

    return ((priority_field | target_field | cmd_field | source_field) &
            CAN_EXT_ID_MASK);
}

#endif