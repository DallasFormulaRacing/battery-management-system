#ifndef IMD_TYPES
#define IMD_TYPES

#include <stdint.h>

#define IMD_MAX_FRAME_LEN 8

#define ID_CAN_SOURCE 0xEFF4

// Index enum for messages, make up the CAN header
typedef enum {
  IMD_CAN_ID_REQUEST = 0x18EAFF1C,
  IMD_CAN_INFO_GENERAL = 0xFF01,
  IMD_CAN_INFO_ISOLATION = 0xFF02,
  IMD_CAN_INFO_VOLTAGE = 0xFF03,
  IMD_CAN_INFO_IT = 0xFF04
} IMD_CanId_t;

// Index enum for commands, parsed by the IMD to determine specific function
typedef enum {
  IMD_RESET_ALARM = 0x33,
  IMD_TRIGGER_TEST = 0x57,
  IMD_FACTORY_RESET = 0x6F,
  IMD_STATUS = 0x71,
  IMD_THRESHOLD = 0x2F,
  IMD_ACTIVATION = 0x31,
  IMD_ACTIVE_PROFILE = 0x39,
  IMD_VOLTAGE_MODE = 0x65
} IMD_CanIndex_t;

typedef enum {
  THRESHOLD_ERROR = 0x47,
  THRESHOLD_TIMEOUT = 0x49,
  THRESHOLD_WARNING = 0x4B,
  VOLTAGE_MODE = 0x65,
  ALARM_THRESHOLD = 0x2F,
  ALARM_MODE = 0x31,
  ACTIVE_PROFILE = 0x39,
  POWER_PROFILE = 0x3B
} IMD_SetId_t;

// Bitfield for status'
typedef struct {
  uint16_t device_error : 1; // Bit 0
  uint16_t hv_pos_conn_fail : 1;
  uint16_t hv_neg_conn_fail : 1;
  uint16_t earth_conn_fail : 1;
  uint16_t iso_alarm : 1;
  uint16_t iso_warning : 1;
  uint16_t iso_outdated : 1;
  uint16_t unbalance_alarm : 1;
  uint16_t undervoltage_alarm : 1;
  uint16_t unsafe_to_start : 1;
  uint16_t earthlift_open : 1; // Bit 10
  uint16_t reserved : 5;
} IMD_Status_Flags_t;

typedef union {
  uint16_t raw;
  IMD_Status_Flags_t flags;
} IMD_Status;

// Message structs
typedef struct {
  uint16_t r_iso_corrected;
  uint8_t r_iso_status;
  uint8_t counter;
  IMD_Status status;
  uint8_t activity;
  uint8_t reserved_0xFF;
} IMD_Msg_General_t;

typedef struct {
  uint16_t r_iso_neg;
  uint16_t r_iso_pos;
  uint16_t r_iso_original;
  uint8_t counter;
  uint8_t quality; // %
} IMD_Msg_Isolation_t;

typedef struct {
  uint16_t hv_system;
  uint16_t hv_neg_to_earth;
  uint16_t hv_pos_to_earth;
  uint8_t counter;
  uint8_t reserved_0xFF;
} IMD_Msg_Voltage_t;

typedef struct {
  uint8_t index;
  uint8_t data[7];
} IMD_Msg_RequestResponse_t;

// Union for message, all are 8 bytes but packed differently
typedef union {
  uint8_t raw[8];
  IMD_Msg_General_t general;
  IMD_Msg_Isolation_t isolation;
  IMD_Msg_Voltage_t voltage;
  IMD_Msg_RequestResponse_t req_res;
} IMD_Data_t;

typedef struct {
  IMD_Data_t data;
  IMD_CanId_t can_id;
} IMD_Packet_t;

typedef enum : uint8_t {
  standard_fast = 0x01,
  standard = 0x02,
  high_cap_fast = 0x03,
  high_cap = 0x04,
  disturbed = 0x05,
  service = 0x06

} IMD_Active_Prof;

typedef struct {
  uint8_t threshold_error[2];       // Value in kOhm
  uint8_t threshold_timeout[2];     // Value in s
  uint8_t threshold_warning[2];     // Value in kOhm
  uint8_t voltage_mode[1];          // 0xFD = AC, 0xFE = DC
  uint8_t unbal_alarm_threshold[1]; // 15-45 as a %
  uint8_t self_holding_alarm[1]; // 0xFC = False, 0xFD = true (reset by command)
  uint8_t active_profile[1];
  uint8_t power_on_profile[1];
} IMD_Config_Profile;

typedef struct {
  uint8_t cmd;
  uint8_t *value;
  uint8_t len;
} imd_param_t;

#endif