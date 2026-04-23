#include "imd_drivers.h"
#include "can.h"
#include "fdcan.h"
#include "gui_types.h"
#include "imd_types.h"
#include "stm32g474xx.h"
#include "stm32g4xx_hal_def.h"
#include "stm32g4xx_hal_fdcan.h"

static IMD_Packet_t packet;

static void configure_imd_header(FDCAN_TxHeaderTypeDef *header,
                                 uint8_t can_id) {
  configure_can2_tx_header(header);
  header->IdType = FDCAN_EXTENDED_ID;
  header->Identifier = can_id;
}

static HAL_StatusTypeDef send_imd_buffer(uint8_t can_id, uint8_t *buf,
                                         uint8_t len) {
  FDCAN_TxHeaderTypeDef header;

  configure_imd_header(&header, can_id);
  header.DataLength = len;
  return can2_send(&header, buf);
}

static HAL_StatusTypeDef imd_send(uint8_t can_id, uint8_t index,
                                  const uint8_t *payload, uint8_t len) {
  uint8_t tx_buf[IMD_MAX_FRAME_LEN] = {0};

  // command index
  tx_buf[0] = index;

  // Request must be 3 bytes, packed in front if needed, little-endian
  if (payload != NULL && len > 0) {
    uint8_t encoded[2] = {0xFF, 0xFF};
    uint8_t copy_len = (len > 3) ? 3 : len;

    for (uint8_t i = 0; i < copy_len; i++) {
      encoded[i] = payload[copy_len - 1 - i];
    }

    memcpy(&tx_buf[1], encoded, 2);
  } else {
    tx_buf[1] = 0xFF;
    tx_buf[2] = 0xFF;
  }

  return send_imd_buffer(can_id, tx_buf, 4);
}

void configure_imd_params(IMD_Config_Profile prof) {
  const imd_param_t params[] = {
      {THRESHOLD_ERROR, prof.threshold_error, 3},
      {THRESHOLD_TIMEOUT, prof.threshold_timeout, 3},
      {THRESHOLD_WARNING, prof.threshold_warning, 3},
      {VOLTAGE_MODE, prof.voltage_mode, 2},
      {ALARM_THRESHOLD, prof.unbal_alarm_threshold, 2},
      {ALARM_MODE, prof.self_holding_alarm, 2},
      {ACTIVE_PROFILE, prof.active_profile, 2},
      {POWER_PROFILE, prof.power_on_profile, 2},
  };

  for (uint8_t i = 0; i < sizeof(params) / sizeof(params[0]); i++) {
    imd_send(ID_CAN_SOURCE, params[i].cmd, params[i].value, params[i].len);
  }
}

// By default, IMD_Info_General is sent every 100 ms
void configure_imd_cyclic() {}

void reset_imd_alarm() {
  uint8_t data[1] = {0x01};
  imd_send_request(ID_CAN_SOURCE, IMD_RESET_ALARM, data, 1);
}

void parse_imd_msg(uint32_t can_id, uint8_t *data) {
  memcpy(packet.data.raw, data, 8);
  packet.can_id = can_id;
}

// Need to rewrite, directly copied from gui
void can2_to_fdcan(uint16_t resp_id, uint8_t *tx_frame) {
  can_ext_id_t tx_header = can_id_build(CAN_PRIORITY_P0, GUI_DEVICE_ID,
                                        (uint16_t)resp_id, BMS_DEVICE_ID);
  fdcan_send(tx_header, tx_frame, FDCAN_DLC_BYTES_48);
}

IMD_Packet_t imd_get_data() { return packet; }