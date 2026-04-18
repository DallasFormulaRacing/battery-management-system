#include "elcon_comms.h"
#include "can.h"
#include "elcon_types.h"
#include "stm32g4xx_hal_fdcan.h"
#include <stdbool.h>
#include <string.h>

#define U16_TOP_HALF_8B(x) (uint8_t)(x >> 8)
#define U16_BOT_HALF_8B(x) (uint8_t)(x & 0x00FF)

void elcon_send_command(elcon_command_t *command_profile) {
  //
  FDCAN_TxHeaderTypeDef header;
  configure_can2_tx_header(&header);
  header.IdType = FDCAN_EXTENDED_ID;
  header.Identifier = (ELCON_CHARGING_REQUEST_CAN_ID & 0x1FFFFFFFU);
  header.DataLength = FDCAN_DLC_BYTES_8;
  can2_send(&header, elcon_pack_can(command_profile).data);
}

can2_msg_t elcon_pack_can(elcon_command_t *command_profile) {
  can2_msg_t charge_request;
  memset(&charge_request, 0, sizeof(charge_request));
  uint16_t max_volt_cmd = command_profile->max_voltage;
  uint16_t max_curr_cmd = command_profile->max_current;

  max_volt_cmd *= 10;
  max_curr_cmd *= 10;

  charge_request.data[0] = U16_TOP_HALF_8B(max_volt_cmd);
  charge_request.data[1] = U16_BOT_HALF_8B(max_volt_cmd);
  charge_request.data[2] = U16_TOP_HALF_8B(max_curr_cmd);
  charge_request.data[3] = U16_BOT_HALF_8B(max_curr_cmd);
  charge_request.data[4] = command_profile->enable;

  return charge_request;
}

static void elcon_unpack_status_byte(uint8_t stb, elcon_status_t *stat) {
  //
  stat->hw = stb & (1 << 0);
  stat->temp = stb & (1 << 1);
  stat->input_voltage = stb & (1 << 2);
  stat->starting_state = stb & (1 << 3);
  stat->comm_state = stb & (1 << 4);

  // if any 1s = error
  bool err = (stat->hw || stat->temp || stat->input_voltage ||
              stat->starting_state || stat->comm_state);
  if (!err) {
    stat->charger_OKAY = 1U;
  }
}

/**
 * @brief Make sure to check for OKAY bit after calling this
 *
 * @param msg
 * @return elcon_status_t
 */
elcon_status_t elcon_handle_heartbeat(const can2_msg_t *msg) {
  elcon_status_t status = {0};

  if (msg == NULL || msg->len <= 4U) {
    return status;
  }

  elcon_unpack_status_byte(msg->data[4], &status);
  return status;
}