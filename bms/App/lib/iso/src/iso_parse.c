#include "iso_parse.h"
#include "iso_types.h"
#include <stdint.h>

/*
The ISO175 has a lot of commands, most of which we don't need.
assuming char (uint8_t) buffer for incoming CAN frame
Normal CAN is 8 byte data frame
*/

void imd_parse_general(isometer_t *imd, const uint8_t *buf) {

  imd->imd_iso_info->corrected_isolation = (buf[0] << 8) | buf[1];
  imd->imd_iso_info->isolation_status = buf[2];
  imd->imd_iso_info->iso_measurement_cntr = buf[3];
  imd->imd_iso_info->status_warnings = buf[4] << 8 | buf[5];
  imd->imd_iso_info->status_device = buf[6];
}

void imd_parse_isolation(isometer_t *imd, const uint8_t *buf) { imd-> }

void imd_parse_voltage(isometer_t *imd, const uint8_t *buf) {}

void imd_parse_IT_system(isometer_t *imd, const uint8_t *buf) {}

/*
In case of an unknown index, an error message will be send back by the device.
In case the device write lock is set (Index 0x6B), any set command except
write lock disable“ an error message will be send back.

Standard data format of an error meassage:
CAN-ID  | Byte 0    | Byte 1    | Byte 2 (requested index)
0x22    | 0xFF      | 0x23 unknown / invalid Request
                    | 0x24 Set command failed, parameter locked
                    | 0x25 - Data[1] range overflow 1
                    | 0x26 - Data[2] range overflow1
                    | 0x27 - CAN-ID already in use1
                    | 0x28 - Write failed1
                    | 0x29 - Read failed1
*/
void imd_parse_error() {}

/*
Commands to adjust IMD settings
*/
void imd_set_resistance_threshold() {}