#ifndef CAB_TYPES
#define CAB_TYPES

const uint32_t CAB_ID = 0x3C2;

typedef struct  {
int32_t current;
int8_t error;
uint8_t productName[2];
int8_t softwareRevision;
} CABMessage;

typedef enum : int8_t {
OK = 0x00,
MEMORY_ERROR = 0x40,
OVER_CURRENT = 0x41,//lets just hope this doesnt happen lol... with a 600v battery that would be a cool..348kW
FLUXGATE_NO_OSCILLATION = 0x42,
WRONG_SUPPLY_VOLTAGE = 0x46,
DEFAULT_ADC_CHANNEL = 0x47,
NO_NEW_DATA = 0x49,
DEFAULT_DAC_THRESHOLD = 0x4A,
DEFAULT_REFERENCE_VOLTAGE = 0x4B
} CAB_ERRORS;

#endif