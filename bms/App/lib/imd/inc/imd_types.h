#ifndef IMD_TYPES
#define IMD_TYPES

#include <stdint.h>

#define IMD_MAX_FRAME_LEN 8

// Index enum for commands, make up the CAN header
typedef enum {
  IMD_CAN_ID_REQUEST = 0x22,
  IMD_CAN_ID_RESPONSE = 0x23,
  IMD_CAN_ID_GENERAL = 0x37,
  IMD_CAN_ID_ISO_DETAIL = 0x38,
  IMD_CAN_ID_VOLTAGE = 0x39,
  IMD_CAN_ID_IT_SYSTEM = 0x3A
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
  THRESHOLD = 0x2F,
  ACTIVATION = 0x31,
  POWER_ON = 0x3B,

} IMD_CanIndexSet_t;

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

// Request and response struct organization
typedef struct {
  uint8_t index; // Index
  uint8_t *data; // Bytes 1-7
} IMD_Msg_RequestResponse_t;

// Union for message, all are 8 bytes but packed differently
typedef union {
  uint8_t raw[8];
  IMD_Msg_General_t general;
  IMD_Msg_Isolation_t isolation;
  IMD_Msg_Voltage_t voltage;
  IMD_Msg_RequestResponse_t req_res;
} IMD_Data_t;

#endif