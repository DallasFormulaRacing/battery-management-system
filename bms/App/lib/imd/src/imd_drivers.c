#include "imd_drivers.h"
#include "can.h"
#include "imd_types.h"
#include "stm32g474xx.h"
#include "stm32g4xx_hal_def.h"
#include "stm32g4xx_hal_fdcan.h"

static IMD_Packet_t packet;

static void configure_imd_header(FDCAN_TxHeaderTypeDef *header,
                                 uint8_t can_id) {
  configure_can2_tx_header(header);
  header->IdType = FDCAN_STANDARD_ID;
  header->Identifier = can_id;
}

static HAL_StatusTypeDef send_imd_buffer(uint8_t can_id, uint8_t *buf,
                                         uint8_t len) {
  FDCAN_TxHeaderTypeDef header;

  configure_imd_header(&header, can_id);
  header.DataLength = len;
  return can2_send(&header, buf);
}

HAL_StatusTypeDef imd_send_request(uint8_t can_id, uint8_t index,
                                   const uint8_t *payload, uint8_t len) {

  uint8_t tx_buf[IMD_MAX_FRAME_LEN] = {0};

  // First byte = command index
  tx_buf[0] = index;

  // Copy payload if present
  if (payload != NULL && len > 0) {
    memcpy(&tx_buf[1], payload, len);
  }

  return send_imd_buffer(can_id, tx_buf, len + 1);
}

void configure_imd_params(void) {
  uint8_t data[2] = {0, 0};

  imd_send_request(IMD_CAN_ID_REQUEST, IMD_THRESHOLD, data, 2);
  // Self-holding alarm, must be reset via command
  data[0] = 0xFD;
  imd_send_request(IMD_CAN_ID_REQUEST, IMD_ACTIVATION, data, 2);

  // Voltage mode to DC
  data[0] = 0xFE;
  imd_send_request(IMD_CAN_ID_REQUEST, IMD_VOLTAGE_MODE, data, 2);

  // ...
}

// By default, IMD_Info_General is sent every 100 ms
void configure_imd_cyclic() {
  // IMD_Info_IsolationDetail, 100 ms
  uint8_t data[2] = {0x01, 0xFA};
  imd_send_request(IMD_CAN_ID_REQUEST, 0x79, data, 2);
}

void reset_imd_alarm() {
  uint8_t data[1] = {0x01};
  imd_send_request(IMD_CAN_ID_REQUEST, IMD_RESET_ALARM, data, 1);
}

void parse_imd_msg(uint32_t can_id, const uint8_t *data) {
  memcpy(packet.data.raw, data, 8);
  packet.can_id = can_id;
}

IMD_Packet_t imd_get_data() { return packet; }