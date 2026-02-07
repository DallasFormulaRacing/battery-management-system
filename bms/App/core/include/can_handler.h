/* App/Inc/command_handler.h */
#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include "stm32h5xx_hal.h"
#include "types.h"

/**
 * @brief Dispatches incoming CAN messages to specific handling functions.
 * @param cmd_id The Identifier of the received CAN frame.
 * @param data   Pointer to the CAN frame data payload.
 */
void Process_CAN_Command(uint32_t cmd_id, uint8_t* data);

/* Internal command callbacks - useful if you want to call them manually for debugging */
//void handle_pi_request(uint8_t* data);

#endif /* COMMAND_HANDLER_H */