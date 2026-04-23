#include "isolation.h"

void imd_init() {
  configure_imd_params(config);
  configure_imd_cyclic();
}

void imd_update() {
  IMD_Packet_t packet = imd_get_data();
  switch (packet.can_id) {
  case (IMD_CAN_INFO_GENERAL):
    handle_general(packet.data);
    break;
  case (IMD_CAN_INFO_VOLTAGE):
    handle_voltage(packet.data);
    break;
  default:
    break;
  }
}

void handle_general(IMD_Data_t data) {
  // Can send data to gui as well
  // Check status
  if (data.general.status.raw != 0) {
    handle_error(data);
  }
}

void handle_voltage(IMD_Data_t data) {}

void handle_error(IMD_Data_t data) {}
