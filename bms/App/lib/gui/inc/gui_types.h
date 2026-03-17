#ifndef BMS_CAN_PROTOCOL_H
#define BMS_CAN_PROTOCOL_H

#include <stdbool.h>
#include <stdint.h>

typedef uint32_t can_ext_id_t;

enum { CAN_EXT_ID_MASK = 0x1FFFFFFFU };

typedef enum {
  CAN_PRIORITY_P0 = 0x0,
  CAN_PRIORITY_P1 = 0x1,
  CAN_PRIORITY_P2 = 0x2,
  CAN_PRIORITY_P3 = 0x3,
} can_priority_t;

typedef enum { 
  BMS_DEVICE_ID = 0x1F, 
  GUI_DEVICE_ID = 0x1E 
} can_device_id_t;

typedef enum {
  CMD_ID_FIRST_24_CELLS = 0x00A0,
  CMD_ID_SECOND_24_CELLS = 0x00A1,
  CMD_ID_THIRD_24_CELLS = 0x00A2,
  CMD_ID_FOURTH_24_CELLS = 0x00A3,
  CMD_ID_FIFTH_24_CELLS = 0x00A4,
  CMD_ID_SIXTH_24_CELLS = 0x00A5,
  CMD_ID_FIRST_60_TEMPS = 0x00B0,
  CMD_ID_LAST_60_TEMPS = 0x00B1,
  CMD_ID_PACK_METADATA = 0x00C0,
  CMD_ID_IMD_DATA = 0x00D0,
} can_command_id_t;

typedef enum{
  CMD_ID_FIRST_24_CELLS_RESP = 0x01A0,
  CMD_ID_SECOND_24_CELLS_RESP = 0x01A1,
  CMD_ID_THIRD_24_CELLS_RESP = 0x01A2,
  CMD_ID_FOURTH_24_CELLS_RESP = 0x01A3,
  CMD_ID_FIFTH_24_CELLS_RESP = 0x01A4,
  CMD_ID_SIXTH_24_CELLS_RESP = 0x01A5,
  CMD_ID_FIRST_60_TEMPS_RESP = 0x01B0,
  CMD_ID_LAST_60_TEMPS_RESP = 0x01B1,
  CMD_ID_PACK_METADATA_RESP = 0x01C0,
  CMD_ID_IMD_DATA_RESP = 0x00D0,
}can_resp_id_t;

static inline can_ext_id_t can_id_build(can_priority_t priority,
                                        can_device_id_t target,
                                        can_command_id_t cmd,
                                        can_device_id_t source) {
  uint32_t priority_field = ((uint32_t)priority & 0x07U) << 26;
  uint32_t target_field = ((uint32_t)target & 0x1FU) << 21;
  uint32_t cmd_field = ((uint32_t)cmd & 0xFFFFU) << 5;
  uint32_t source_field = ((uint32_t)source & 0x1FU);

  return ((priority_field | target_field | cmd_field | source_field) &
          CAN_EXT_ID_MASK);
}

inline can_priority_t can_id_get_priority(can_ext_id_t id) {
  return (can_priority_t)((id >> 26) & 0x07U);
}

inline can_device_id_t can_id_get_target(can_ext_id_t id) {
  return (can_device_id_t)((id >> 21) & 0x1FU);
}

inline can_command_id_t can_id_get_cmd(can_ext_id_t id) {
  return (can_command_id_t)((id >> 5) & 0xFFFFU);
}

inline can_device_id_t can_id_get_source(can_ext_id_t id) {
  return (can_device_id_t)(id & 0x1FU);
}

inline bool can_id_is_valid(can_ext_id_t id) {
  return (id & ~CAN_EXT_ID_MASK) == 0U;
}

#endif