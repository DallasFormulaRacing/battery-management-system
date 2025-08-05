/**
 * @author Will Kim
 * @email <wkim@utdallas.edu>
 * @create date 2025-08-04 16:20:36
 * @modify date 2025-08-04 16:20:36
 * @desc Implementing PEC functions and command structs.
    The purpose of the CRC PEC functions is to ensure data integrity, 
    similar to how checksums are used in credit card numbers or hashes in passwords.
 */

#include "commands.h"

/**
 * @brief Calculate the PEC15 for a given data frame.
 *  The command PEC is a 15-bit cyclic redundancy check (CRC) value 
    calculated for all 16 bits of a command, using the initial PEC value of
    b'000000000010000 and the following characteristic polynomial: 
    x^15 + x^14 + x^10 + x^8 + x^7 + x^4 + x^3 + 1.
 * @param data pointer to the data frame to calculate the PEC for
 * @param len the length of the data frame
 * @return uint16_t the calculated PEC15 value
 */
uint16_t calc_PEC15(uint8_t *data, uint16_t len) {
    uint16_t rem, addr;
    rem = 16; /* initialize the PEC */
    for (uint8_t i = 0; i < len; i++)
    {
        /* calculate PEC table address */
        addr = ( ((rem >> 7) ^ data[i]) & 0xff);
        rem = ( (rem << 8) ^ CMD_PEC15_LUT[addr]);
    }
    /* The CRC15 has a 0 in the LSB so the rem must be multiplied by 2 */
    return (rem << 1);
}

/**
 * @brief Calculate the PEC10 for a given data frame.
 *  The data PEC is a 10-bit CRC value calculated for all bits read from a register 
    and the command counter bits. The initial PEC value is
    b'0000010000 and the following characteristic polynomial: 
    x^10 + x^7 + x^3 + x^2 + x + 1. 
    Data is regarded as valid only if the data PEC matches.
 * @param data pointer to the data frame to calculate the PEC for
 * @param len the length of the data frame
 * @return uint16_t the calculated PEC10 value
 */
uint16_t calc_PEC10(uint8_t *data, uint16_t len) {
    //todo
}

// ------------------- Command Code Definitions -------------------

/* How to read command codes from the datasheet and convert to C
    * 1. Use Clear Flags CLRFLAG [1 1 1 0 0 0 1 0 1 1 1] (11 bits) as example
    * 2. Write in binary: 11100010111 and pad: 0000 0111 0001 0111
    * 3. Convert to hex: 0x07, 0x17
    * 4. Write in C: extern uint8_t CLRFLAG[2] = {0x07, 0x17};
*/

// TODO define all the command codes as extern uint8_t arrays