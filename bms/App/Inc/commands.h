/**
 * @author Will Kim
 * @email <wkim@utdallas.edu>
 * @create date 2025-08-04 10:17:02
 * @modify date 2025-08-04 10:17:02
 * @desc A header file for BMS commands. Please see adbms6830b.pdf 
         commands section Table 50 for details.
 */

 #ifndef BMS_COMMANDS_H
 #define BMS_COMMANDS_H

#include "pec_table.h"
#include <stdint.h>
//#include <stdlib.h>

const uint16_t CMD_PEC15_LUT[256];
const uint16_t DATA_PEC10_LUT[256];

// See table 50 of the ADBMS6830b ref man 

typedef enum {
        ADC_ADCV,
        ADC_ADSV,
        ADC_ADAX,
        ADC_ADAX2
} ADC_cmd_type_e;

typedef struct {
        uint8_t OW:2; // See bitfield
        uint8_t RSTF:1;
        uint8_t DCP:1;
        uint8_t CONT:1;
        uint8_t RD:1;
        uint8_t PUP:1;
        uint8_t CH:5;
        ADC_cmd_type_e command_type;
} ADC_cmd_cfg_t; // cmd = command, cfg = configuration

typedef struct {
        uint8_t cmd[2];
} message_command_t;

typedef struct {
        uint8_t pec[2];
} message_header_t;

typedef struct {
        message_command_t command;
        message_header_t message_header;
} message_t;

// See table 52 on how to use ADC Command flags

uint16_t calc_PEC15(const uint8_t *data, uint16_t len);
uint16_t calc_PEC10(const uint8_t *data, uint16_t len, const uint8_t *commandCounter);

// Command builder prototypes
message_command_t build_ADC_command(const ADC_cmd_cfg_t *config);


// PEC packer prototypes
message_header_t pack_PEC15(uint8_t *data);
message_header_t pack_PEC10(uint8_t *data, const uint8_t *commandCounter);

/*      
        See memory map (pg 61) on how to translate register reads
        into meaningful measurements
*/

#endif // BMS_COMMANDS_H