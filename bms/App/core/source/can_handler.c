#include "can_handler.h"
#include "comms.h"
#include "can_gui_handler.h"
//#include "node_tasks.h"
//#include "cmsis_os2.h"

void Process_CAN_Command(uint32_t cmd_id, uint8_t* data) {
    switch(cmd_id) {
        case CAN_CMD_ID_READCAVG_ALL:
            can_readcavg_all(data);
            break;
        case CAN_CMD_ID_READCV_ALL:
            can_readcv_all(data);
            break;
        case CAN_CMD_ID_READSV_ALL:
            can_readsv_all(data);
            break;
        default:
            break;
    }
}
