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

// ------------------- Command Code Definitions -------------------

/* How to read command codes from the datasheet and convert to C
    * 1. Use Clear Flags CLRFLAG [1 1 1 0 0 0 1 0 1 1 1] (11 bits) as example
    * 2. Write in binary: 11100010111 and pad: 0000 0111 0001 0111
    * 3. Convert to hex: 0x07, 0x17
    * 4. Write in C: extern uint8_t CLRFLAG[2] = {0x07, 0x17};
*/

// TODO define all the command codes as extern uint8_t arrays

// Configuration Register Commands : WRCFGx
const message_command_t WRCFGA = { .cmd = { 0x00, 0x01 } };
const message_command_t WRCFGB = { .cmd = { 0x00, 0x24 } };
const message_command_t RDCFGA = { .cmd = { 0x00, 0x02 } };
const message_command_t RDCFGB = { .cmd = { 0x00, 0x26 } };

// Read Status Register Commands : RDSTATx
const message_command_t RDSTATA    = { .cmd = { 0x00, 0x30 } };
const message_command_t RDSTATB    = { .cmd = { 0x00, 0x31 } };
const message_command_t RDSTATC    = { .cmd = { 0x00, 0x32 } };
const message_command_t RDSTATD    = { .cmd = { 0x00, 0x33 } };
const message_command_t RDSTATE    = { .cmd = { 0x00, 0x34 } };
const message_command_t RDSTATCERR = { .cmd = { 0x00, 0x72 } };

// Read serial ID command : RDSID
// 0000 0000 0010 1100
const message_command_t RDSID      = { .cmd = { 0x00, 0x30 } };

/* Read all AUX and all Status Registers */
const message_command_t RDASALL    = { .cmd = { 0x00, 0x35 } };

/* Poll adc command */
const message_command_t PLADC      = { .cmd = { 0x07, 0x18 } };
const message_command_t CLRFLAG    = { .cmd = { 0x07, 0x17 } };


// TODO rewrite these later
// 0b0000 0111 0010 0001 = 0x07 0x21
const message_command_t WRCOMM     = { .cmd = { 0x07, 0x21 } }; 

// 0b0000 0111 0010 0010 = 0x07 0x22
const message_command_t RDCOMM     = { .cmd = { 0x07, 0x22 } }; 

// 0b0000 0111 0010 0011 = 0x07 0x23
const message_command_t STCOMM     = { .cmd = { 0x07, 0x23 } }; 

// 0b0000 0000 0010 1101 = 0x00 0x2D
const message_command_t SNAP       = { .cmd = { 0x00, 0x2D } }; 

// 0b0000 0000 0010 1111 = 0x00 0x2F
const message_command_t UNSNAP     = { .cmd = { 0x00, 0x2F } }; 


// 0b0000 0000 0100 1100 = 0x00 0x4C
const message_command_t RDACALL    = { .cmd = { 0x00, 0x4C } }; 

// 0b0000 0000 0000 1100 = 0x00 0x0C
const message_command_t RDCVALL    = { .cmd = { 0x00, 0x0C } }; 


// 0b0000 0000 0100 0000 = 0x00 0x40
const message_command_t CMDIS      = { .cmd = { 0x00, 0x40 } }; 

// 0b0000 0000 0100 0001 = 0x00 0x41
const message_command_t CMEN       = { .cmd = { 0x00, 0x41 } }; 

// 0b0000 0000 1000 0011 = 0x00 0x83
const message_command_t CMHB       = { .cmd = { 0x00, 0x83 } }; 


// default adc command, can be modified with build_ADCV_command()

#endif // BMS_COMMANDS_H