#include "command_handler.h"
#include "commands.h"
#include "node_tasks.h"
#include "cmsis_os2.h"



void Process_CAN_Command(uint32_t cmd_id, uint8_t* data) {
    switch(cmd_id) {
        case CMD_ID_REQ_DATA:
            break;
        default:
            break;
    }
}