#ifndef BALANCING
#define BALANCING

#include "bms_driver.h"
#include <stdint.h>

typedef struct {
    uint8_t duty_cycle:4;
} pwm_t;

typedef struct {
    cell_info_t cells[NUM_SEGMENTS * NUM_PACKS_PER_SEGMENT];
    uint16_t size;   
} call_priority_queue_t;


#endif 