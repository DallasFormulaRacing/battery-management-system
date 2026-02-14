/* App/Inc/commands.h */
#ifndef TYPES_H
#define TYPES_H
#include <stdint.h>

/*
typedef struct __attribute__((packed)){
    uint32_t linPotData;
    uint32_t wheelSpeed;
    uint32_t fillerData4bytes;
    uint16_t fillerData2bytes;
    uint8_t brakeTemperature;
    uint8_t tireTemperature;
} NodeDataTypeDef;
*/


/*
typedef enum {
    NODE_ID_FRONT_LEFT  = 0x01, // 00001
    NODE_ID_FRONT_RIGHT = 0x02, // 00010
    NODE_ID_REAR_LEFT   = 0x03, // 00011
    NODE_ID_REAR_RIGHT  = 0x04, // 00100

    NODE_ID_NUCLEO_1    = 0x0A, // 01010
    NODE_ID_NUCLEO_2    = 0x0B, // 01011
    
    NODE_ID_DASH        = 0x1F, // 11111 (Node 31)
    NODE_ID_UNKNOWN     = 0x00
} NodeHardwareID_t;
*/

/*
typedef struct {
    uint32_t uid[3];            // STM32 96-bit UID
    NodeHardwareID_t nodeType;  // Assigned CAN ID
} UID_Mapping_t;
*/


//extern NodeHardwareID_t self_node_id;
//extern NodeDataTypeDef nodeData;

#endif