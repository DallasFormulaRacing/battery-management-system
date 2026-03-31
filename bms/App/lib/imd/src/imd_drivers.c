#include "imd_drivers.h"
#include "can.h"
#include "imd_types.h"
#include "stm32g474xx.h"
#include "stm32g4xx_hal_def.h"
#include "stm32g4xx_hal_fdcan.h"

void create_imd_packet(IMD_CanId_t cmd, IMD_Packet_t *packet,
                       FDCAN_TxHeaderTypeDef *header) {
  configure_tx_header(header);
  header->IdType = FDCAN_STANDARD_ID;
  header->Identifier = cmd;
  packet->can_id = cmd;
}
