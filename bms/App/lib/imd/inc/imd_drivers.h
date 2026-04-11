
#ifndef IMD_DRIVERS_H
#define IMD_DRIVERS_H

#include "can.h"
#include "imd_types.h"
#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_def.h"
#include "stm32g4xx_hal_fdcan.h"
#include <string.h>
#include <stdint.h>
 
/* -----------------------------------------------------------------------
 * Configuration
 * --------------------------------------------------------------------- */
HAL_StatusTypeDef imd_send_request(uint8_t can_id, uint8_t index,
                                   const uint8_t *payload, uint8_t len);
 
void configure_imd_params(void);
void configure_imd_cyclic(void);
 
/* -----------------------------------------------------------------------
 * Control
 * --------------------------------------------------------------------- */
void reset_imd_alarm(void);
 
/* -----------------------------------------------------------------------
 * Message Handling
 * --------------------------------------------------------------------- */
void parse_imd_msg(uint8_t can_id, uint8_t *data); 
IMD_Packet_t imd_get_data(void); // Called by api to get current data
void handle_info_general(IMD_Data_t msg);
 
#endif /* IMD_DRIVERS_H */