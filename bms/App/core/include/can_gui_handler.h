#ifndef CAN_GUI_HANDLER_H
#define CAN_GUI_HANDLER_H

//#include "stm32h5xx_hal.h"
//#include "types.h"

void can_readcavg_all(uint8_t* data);
void can_readcv_all(uint8_t* data);
void can_readsv_all(uint8_t* data);

#endif /* CAN_GUI_HANDLER_H */