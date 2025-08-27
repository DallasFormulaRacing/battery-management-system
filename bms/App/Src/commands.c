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
#include <stdint.h>

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
    //0b000 0000 0001 0000; from datasheet pg 53
    const uint16_t initial_PEC = 0x0010;
    
    //0b 0100 0101 1001 1001; x^15 + x^14 + x^10 + x^8 + x^7 + x^4 + x^3 + 1
    const uint16_t polynomial = 0x4599;

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
uint16_t calc_PEC10(uint8_t *data, uint16_t len, uint8_t *commandCounter) {
    uint16_t nRemainder = 16u; /* PEC_SEED */
    /* x10 + x7 + x3 + x2 + x + 1 <- the CRC10 polynomial 100 1000 1111 */
    uint16_t nPolynomial = 0x8Fu;
    uint8_t nByteIndex, nBitIndex;
    uint16_t nTableAddr;

    for (nByteIndex = 0u; nByteIndex < len; ++nByteIndex)
    {
        /* calculate PEC table address */
        nTableAddr = (uint16_t)(((uint16_t)(nRemainder >> 2) ^ (uint8_t)data[nByteIndex]) & (uint8_t)0xff);
        nRemainder = (uint16_t)(((uint16_t)(nRemainder << 8)) ^ DATA_PEC10_LUT[nTableAddr]);
    }
    /* If array is from received buffer add command counter to crc calculation */
    if (commandCounter != NULL)
    {
        nRemainder ^= (uint16_t)(*commandCounter << 4u);
    }
    /* Perform modulo-2 division, a bit at a time */
    for (nBitIndex = 6U; nBitIndex > 0U; --nBitIndex)
    {
        /* Try to divide the current data bit */
        if ((nRemainder & 0x200U) > 0U)
        {
            nRemainder = (uint16_t)((nRemainder << 1U));
            nRemainder = (uint16_t)(nRemainder ^ nPolynomial);
        }
        else
        {
            nRemainder = (uint16_t)((nRemainder << 1U));
        }
    }
    return ((uint16_t)(nRemainder & 0x3FFU));
}

// ------------------- Command Code Definitions -------------------

/* How to read command codes from the datasheet and convert to C
    * 1. Use Clear Flags CLRFLAG [1 1 1 0 0 0 1 0 1 1 1] (11 bits) as example
    * 2. Write in binary: 11100010111 and pad: 0000 0111 0001 0111
    * 3. Convert to hex: 0x07, 0x17
    * 4. Write in C: extern uint8_t CLRFLAG[2] = {0x07, 0x17};
*/

// TODO define all the command codes as extern uint8_t arrays

// Configuration Register Commands : WRCFGx
const uint8_t WRCFGA[2]        = { 0x00, 0x01 };
const uint8_t WRCFGB[2]        = { 0x00, 0x24 };
const uint8_t RDCFGA[2]        = { 0x00, 0x02 };
const uint8_t RDCFGB[2]        = { 0x00, 0x26 };

// Read Status Register Commands : RDSTATx
const uint8_t RDSTATA[2]       = { 0x00, 0x30 };
const uint8_t RDSTATB[2]       = { 0x00, 0x31 };
const uint8_t RDSTATC[2]       = { 0x00, 0x32 };
const uint8_t RDSTATD[2]       = { 0x00, 0x33 };
const uint8_t RDSTATE[2]       = { 0x00, 0x34 };
const uint8_t RDSTATCERR[2]    = { 0x00, 0x72 };  

/* Read all AUX and all Status Registers */
const uint8_t RDASALL[2]       = { 0x00, 0x35 };

/* Poll adc command */
const uint8_t PLADC[2]         = { 0x07, 0x18 };
const uint8_t CLRFLAG[2]       = { 0x07, 0x17 };