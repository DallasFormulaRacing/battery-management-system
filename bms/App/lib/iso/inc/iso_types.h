#ifndef ISOMETER_TYPES_H
#define ISOMETER_TYPES_H

#include <stdint.h>
extern isometer_t imd;

typedef enum {
  IMD_INFO_GENERAL = 0x37,
  IMD_INFO_ISOLATION_DETAIL = 0x38,
  IMD_INFO_VOLTAGE = 0x39,
  IMD_INFO_IT_SYSTEM = 0x3A
} imd_can_id_t;

typedef struct {
  uint16_t corrected_isolation;
  uint16_t isolation_status;
  uint16_t iso_measurement_cntr;
  uint16_t status_warnings;
  uint8_t status_device;
  uint16_t r_iso_neg;
  uint16_t r_iso_pos;
  uint16_t r_iso_org;
  uint8_t isolation_quality;
} imd_isolation_info_t;

typedef struct {
  uint16_t hv_system_voltage;
  uint16_t hv_neg_ground_voltage;
  uint16_t hv_pos_ground_voltage;
  uint8_t volt_measurement_cntr;
} imd_voltage_info_t;

typedef struct {
  uint16_t measured_capacity;
  uint8_t cap_measurement_cntr;
  uint8_t measured_unbalance;
  uint8_t unb_measurement_cntr;
} imd_capacity_info_t;

typedef struct {
  imd_isolation_info_t *imd_iso_info;
  imd_voltage_info_t *imd_volt_info;
  imd_capacity_info_t *imd_cap_info;
} isometer_t;

#endif