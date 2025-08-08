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

#include <stdint.h>
#include "pec_table.h"

const uint16_t CMD_PEC15_LUT[256];
const uint16_t DATA_PEC10_LUT[256];

// ------------------- Command Codes -------------------
//extern uint8_t      [2];        // 

// Configuration Register Commands : WRCFGx
extern const uint8_t      WRCFGA[2];        // Write Configuration Register Group A
extern const uint8_t      WRCFGB[2];        // Write Configuration Register Group B
extern const uint8_t      RDCGFA[2];        // Read Configuration Register Group A
extern const uint8_t      RDCGFB[2];        // Read Configuration Register Group B

// Cell Voltage Register Commands : RDCVx
extern const uint8_t      RDCVA[2];        // Read Cell Voltage Register Group A
extern const uint8_t      RDCVB[2];        // Read Cell Voltage Register Group B
extern const uint8_t      RDCVC[2];        // Read Cell Voltage Register Group C
extern const uint8_t      RDCVD[2];        // Read Cell Voltage Register Group D
extern const uint8_t      RDCVE[2];        // Read Cell Voltage Register Group E
extern const uint8_t      RDCVF[2];        // Read Cell Voltage Register Group F
extern const uint8_t      RDCVALL[2];      // Read Cell Voltage ALL

// Read Status Register Commands : RDSTATx
extern const uint8_t      RDSTATA[2];     // Read Status Register Group A
extern const uint8_t      RDSTATB[2];     // Read Status Register Group B
extern const uint8_t      RDSTATC[2];     // Read Status Register Group C
extern const uint8_t      RDSTATD[2];     // Read Status Register Group D
extern const uint8_t      RDSTATE[2];     // Read Status Register Group E
extern const uint8_t      RDSTATCERR[2];  // Read Status Register Errors (toggle bit 6)

extern const uint8_t      PLADC[2];       // Poll for ADC Conversion Status


// Start Cell Voltage ADC Conversion and Poll Status
extern const uint8_t      ADCV[2];   // Start ADC Conversion


void build_ADCV_command(uint8_t OW, uint8_t RSTF, uint8_t DCP, uint8_t CONT, uint8_t RD);



uint16_t calc_PEC15(uint8_t *data, uint16_t len);
uint16_t calc_PEC10(uint8_t *data, uint16_t len);

#endif // BMS_COMMANDS_H