#include "elcon_comms.h"
#include "can.h"
#include "cmsis_os2.h"
#include "elcon_types.h"
#include "stm32g4xx_hal_fdcan.h"
#include <stdbool.h>
#include <string.h>

#define U16_TOP_HALF_8B(x) (uint8_t)((x) >> 8)
#define U16_BOT_HALF_8B(x) (uint8_t)((x) & 0x00FFU)
#define U16_FROM_BE(hi, lo) (uint16_t)(((uint16_t)(hi) << 8) | (uint16_t)(lo))

static const elcon_internal_t hard_stop = {
    .max_voltage_dv = 0,
    .max_current_da = 0,
    .charge_disable = ELCON_CONTROL_STOP,
};

void elcon_send_command(const elcon_internal_t *command_profile) {
  FDCAN_TxHeaderTypeDef header;
  configure_can2_tx_header(&header);
  header.IdType = FDCAN_EXTENDED_ID;
  header.Identifier = (ELCON_CHARGING_REQUEST_CAN_ID & 0x1FFFFFFFU);
  header.DataLength = FDCAN_DLC_BYTES_8;
  can2_send(&header, elcon_pack_can(command_profile).data);
}

void elcon_hard_stop() { elcon_send_command(&hard_stop); }

/**
 * @brief packs Message 1 (0x1806E5F4)
 *
 * byte 0..1 max charging voltage, 0.1 V/bit, big-endian
 * byte 2..3 max charging current, 0.1 A/bit, big-endian
 * byte 4    control, 0 = start charging, 1 = stop charging
 * byte 5..7 reserved, sent as 0
 */
can2_msg_t elcon_pack_can(const elcon_internal_t *command_profile) {
  can2_msg_t charge_request = {0};
  charge_request.id = ELCON_CHARGING_REQUEST_CAN_ID;
  charge_request.id_type = FDCAN_EXTENDED_ID;
  charge_request.len = ELCON_FRAME_DLC;

  // hardcoded safety clamps
  uint16_t max_volt_cmd = (command_profile->max_voltage_dv > ELCON_CLAMP_MAX_VOLTAGE_DV)
                              ? ELCON_CLAMP_MAX_VOLTAGE_DV
                              : command_profile->max_voltage_dv;
  uint16_t max_curr_cmd = (command_profile->max_current_da > ELCON_CLAMP_MAX_CURRENT_DA)
                              ? ELCON_CLAMP_MAX_CURRENT_DA
                              : command_profile->max_current_da;

  // anything other than an explicit start is a stop
  uint8_t control = (command_profile->charge_disable == ELCON_CONTROL_START)
                        ? ELCON_CONTROL_START
                        : ELCON_CONTROL_STOP;

  charge_request.data[0] = U16_TOP_HALF_8B(max_volt_cmd);
  charge_request.data[1] = U16_BOT_HALF_8B(max_volt_cmd);
  charge_request.data[2] = U16_TOP_HALF_8B(max_curr_cmd);
  charge_request.data[3] = U16_BOT_HALF_8B(max_curr_cmd);
  charge_request.data[4] = control;

  return charge_request;
}

static void elcon_unpack_status_byte(uint8_t stb, elcon_status_t *stat) {
  stat->hw             = !!(stb & ELCON_STATUS_HW_FAIL);
  stat->temp           = !!(stb & ELCON_STATUS_OVER_TEMP);
  stat->input_voltage  = !!(stb & ELCON_STATUS_INPUT_VOLTAGE);
  stat->starting_state = !!(stb & ELCON_STATUS_STARTING_STATE);
  stat->comm_state     = !!(stb & ELCON_STATUS_COMM_TIMEOUT);

  // if any 1s = error
  bool err = (stat->hw || stat->temp || stat->input_voltage ||
              stat->starting_state || stat->comm_state);
  if (!err) {
    stat->charger_OKAY = 1U;
  }
}

/**
 * @brief unpacks Message 2 (0x18FF50E5) into g_elcon
 *
 * byte 0..1 output voltage, 0.1 V/bit, big-endian
 * byte 2..3 output current, 0.1 A/bit, big-endian, bit 15 = 1 discharging
 * byte 4    status bits (see ELCON_STATUS_*)
 * byte 5..7 reserved
 *
 * Make sure to check for OKAY bit after calling this
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

  uint16_t volts_dv = U16_FROM_BE(msg->data[0], msg->data[1]);
  uint16_t curr_raw = U16_FROM_BE(msg->data[2], msg->data[3]);

  int32_t lock = osKernelLock();
  g_elcon.heartbeat_msg = status;
  g_elcon.heartbeat_tick = osKernelGetTickCount();
  g_elcon.output_voltage_dv = volts_dv;
  g_elcon.output_current_da = (uint16_t)(curr_raw & ELCON_CURRENT_MAG_MASK);
  g_elcon.output_discharging = (curr_raw & ELCON_CURRENT_DIR_BIT) ? 1U : 0U;
  (void)osKernelRestoreLock(lock);

  return status;
}
