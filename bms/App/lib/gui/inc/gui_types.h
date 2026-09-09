#ifndef BMS_CAN_PROTOCOL_H
#define BMS_CAN_PROTOCOL_H

#include <stdbool.h>
#include <stdint.h>

typedef uint32_t can_ext_id_t;

#define THERM_CAN_OFFSET                                                       \
  4 // Start index of can frame so that 60 thermistors can be sent in one frame

enum { CAN_EXT_ID_MASK = 0x1FFFFFFFU };

/*
 * DFR CAN Standard — 29-bit extended ID layout:
 *   [28:26] Priority (3) | [25:21] Target (5) | [20:5] Command (16) | [4:0] Source (5)
 */
typedef enum {
  CAN_PRIORITY_P0 = 0x0,
  CAN_PRIORITY_P1 = 0x1,
  CAN_PRIORITY_P2 = 0x2,
  CAN_PRIORITY_P3 = 0x3,
  CAN_PRIORITY_P4 = 0x4,
  CAN_PRIORITY_P5 = 0x5,
  CAN_PRIORITY_P6 = 0x6,
  CAN_PRIORITY_P7 = 0x7,
} can_priority_t;

typedef enum {
  NODE_UNKNOWN = 0x00,
  NODE_ALL = 0x01,
  NODE_FL = 0x02,
  NODE_FR = 0x03,
  NODE_RL = 0x04,
  NODE_RR = 0x05,
  NODE_NUCLEO_1 = 0x06,
  NODE_NUCLEO_2 = 0x07,
  NODE_VCU = 0x1B,
  NODE_BMS = 0x1C,
  NODE_DASH = 0x1D,
  NODE_RASPI = 0x1E,
} can_device_id_t;

/* Compatibility aliases used by existing BMS GUI code paths */
#define BMS_DEVICE_ID NODE_BMS
#define GUI_DEVICE_ID NODE_RASPI

typedef uint16_t can_command_id_t;

/* BMS GUI Application Commands */
enum {
  BMS_BATTERY_PACK_DATA = 0xB000,
  BMS_CELL_VOLTAGES_PACK_1 = 0xB101,
  BMS_CELL_VOLTAGES_PACK_2 = 0xB102,
  BMS_CELL_VOLTAGES_PACK_3 = 0xB103,
  BMS_CELL_VOLTAGES_PACK_4 = 0xB104,
  BMS_CELL_VOLTAGES_PACK_5 = 0xB105,
  BMS_CELL_VOLTAGES_PACK_6 = 0xB106,
  BMS_SEGMENT_TEMPS_HALF_1 = 0xB111,
  BMS_SEGMENT_TEMPS_HALF_2 = 0xB112,
  BMS_IMD_DATA = 0xBA01,
  BMS_GUI_CONNECT = 0xBB01,
  BMS_GUI_CHARGING_REQUEST = 0xBB02,
  BMS_GUI_DISCONNECT = 0xBB03,
  BMS_GUI_CHARGING_SOFTSTOP = 0xBBFF,
};

/* BMS Normal Commands (TCAN → VCAN forwarding / onboard devices) */
enum {
  BMS_CURRENT_SENSOR = 0xBEEF,
  BMS_IMD_REQUEST = 0xBF22,
  BMS_IMD_RESPONSE = 0xBF23,
  BMS_IMD_GENERAL = 0xBF37,
  BMS_IMD_ISO_DETAIL = 0xBF38,
  BMS_IMD_VOLTAGE = 0xBF39,
  BMS_IMD_IT_SYSTEM = 0xBF3A,
  BMS_PRECHARGE_FAILURE = 0xBFAA,
};

/*
 * Protocol error IDs (local to BMS GUI). Kept out of 0xF0xx so they do not
 * collide with DFR bootloader commands (BL_CMD_ERASE = 0xF001, …).
 */
typedef enum {
  ERROR_ID_INVALID_CMD = 0xBE01,
  ERROR_ID_INVALID_TARGET = 0xBE02,
  ERROR_ID_INVALID_ID = 0xBE03,
} can_error_id_t;

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
  return (id & ~CAN_EXT_ID_MASK) == 0U;
}

#endif
