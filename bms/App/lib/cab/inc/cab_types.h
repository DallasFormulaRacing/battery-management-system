#ifndef CAB_TYPES
#define CAB_TYPES

const uint32_t CAB_ID = 0x3C2;

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
    uint32_t current;
    cab_error_t error_status;
    uint8_t productName[2];
    uint8_t softwareRevision;
} cab_message_t;

#endif