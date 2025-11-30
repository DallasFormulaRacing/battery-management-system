#ifndef CONFIG_H
#define CONFIG_H

#include "bms_types.h"

#define IC_COUNT_CHAIN 1
#define WRITE_SIZE (ADBMS_TX_FRAME_BYTES * IC_COUNT_CHAIN)

cell_asic_ctx_t asic_ctx[IC_COUNT_CHAIN];
uint8_t write_buffer[WRITE_SIZE];

#endif