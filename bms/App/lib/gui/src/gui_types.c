#include "gui_types.h"

static inline can_priority_t can_id_get_priority(can_ext_id_t id) {
  return (can_priority_t)((id >> 26) & 0x07U);
}

static inline can_device_id_t can_id_get_target(can_ext_id_t id) {
  return (can_device_id_t)((id >> 21) & 0x1FU);
}

static inline can_command_id_t can_id_get_cmd(can_ext_id_t id) {
  return (can_command_id_t)((id >> 5) & 0xFFFFU);
}

static inline can_device_id_t can_id_get_source(can_ext_id_t id) {
  return (can_device_id_t)(id & 0x1FU);
}

static inline bool can_id_is_valid(can_ext_id_t id) {
  // Fits in 29 bits
  return (id & ~CAN_EXT_ID_MASK) == 0U;
}

// static inline can_ext_id_t can_id_read_svoltage_all(void) {
//   return can_id_build(CAN_PRIORITY_P3, BMS_DEVICE_ID, CMD_ID_SVOLTAGE_ALL,
//                       GUI_DEVICE_ID);
// }

// static inline can_ext_id_t can_id_read_svoltage_all_resp(void) {
//   return can_id_build(CAN_PRIORITY_P3, GUI_DEVICE_ID, CMD_ID_SVOLTAGE_ALL_RESP,
//                       BMS_DEVICE_ID);
// }

// static inline can_ext_id_t can_id_read_cvoltage_all(void) {
//   return can_id_build(CAN_PRIORITY_P3, BMS_DEVICE_ID, CMD_ID_CVOLTAGE_ALL,
//                       GUI_DEVICE_ID);
// }

// static inline can_ext_id_t can_id_read_cvoltage_all_resp(void) {
//   return can_id_build(CAN_PRIORITY_P3, GUI_DEVICE_ID, CMD_ID_CVOLTAGE_ALL_RESP,
//                       BMS_DEVICE_ID);
// }