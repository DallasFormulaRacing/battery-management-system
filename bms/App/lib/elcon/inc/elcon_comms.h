#ifndef ELCON_COMMS_H
#define ELCON_COMMS_H

#include "elcon_types.h"

#define ELCON_CMD_CAN_ID 0x1806E5F4
#define ELCON_HEARTBEAT_CAN_ID 0x18FF50E5

void elcon_send_command(elcon_command_t *command_profile);

static inline void elcon_pack_can(uint8_t *msg);

static inline void elcon_unpack_status_byte(uint8_t stb, elcon_status_t *stat);

#endif