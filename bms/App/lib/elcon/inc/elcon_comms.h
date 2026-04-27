#ifndef ELCON_COMMS_H
#define ELCON_COMMS_H

#include "can.h"
#include "elcon_types.h"

#define ELCON_CHARGING_REQUEST_CAN_ID 0x1806E5F4
#define ELCON_HEARTBEAT_CAN_ID 0x18FF50E5

void elcon_send_command(elcon_internal_t *command_profile);

can2_msg_t elcon_pack_can(elcon_internal_t *command_profile);

elcon_status_t elcon_handle_heartbeat(const can2_msg_t *msg);

#endif