#include "isolation.h"
#include <string.h>

static IMD_Msg_General_t last_general_msg;

void imd_init() {
  configure_imd_params(config);
  configure_imd_cyclic();
}

void imd_update() {
  IMD_Packet_t packet = imd_get_data();
  switch (packet.can_id) {
  case (IMD_CAN_ID_GENERAL):
    handle_general(packet.data);
    break;
  case (IMD_CAN_ID_VOLTAGE):
    handle_voltage(packet.data);
    break;
  default:
    break;
  }
}

void handle_general(IMD_Data_t data) {
  last_general_msg = data.general;

  handle_info_general(data);

  // Check status
  if (data.general.status.raw != 0) {
    handle_error(data);
  }

  uint8_t tx_frame[IMD_MAX_FRAME_LEN];
  memcpy(tx_frame, data.raw, IMD_MAX_FRAME_LEN);

  // Create fdcan packet and send to gui
  can_ext_id_t tx_id = can_id_build(CAN_PRIORITY_P0, GUI_DEVICE_ID,
                                    CMD_ID_IMD_DATA, BMS_DEVICE_ID);
  fdcan_send(tx_id, tx_frame, FDCAN_DLC_BYTES_8);
}

void handle_voltage(IMD_Data_t data) {}

void handle_error(IMD_Data_t data) {}

bool imd_is_healthy(void) {
  const IMD_Status_Flags_t *cur_msg_status_flags =
      &last_general_msg.status.flags;

  if (cur_msg_status_flags->device_error ||
      cur_msg_status_flags->hv_pos_conn_fail ||
      cur_msg_status_flags->hv_neg_conn_fail ||
      cur_msg_status_flags->earth_conn_fail ||
      cur_msg_status_flags->iso_alarm || cur_msg_status_flags->iso_warning ||
      cur_msg_status_flags->unsafe_to_start) {
    return false;
  }
  return true;
}
