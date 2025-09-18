/**
 * @author Will Kim
 * @email <wkim@utdallas.edu>
 * @create date 2025-08-04 16:20:36
 * @modify date 2025-09-02 18:30:41
 * @desc Implementing PEC functions and command structs.
    The purpose of the CRC PEC functions is to ensure data integrity, 
    similar to how checksums are used in credit card numbers or hashes in passwords.
 */

#include "commands.h"
#include <stdint.h>
#include "pec_table.h"

#define BYTE_MASK 0xFFU
#define HALF_BYTE_MASK 0x0FU
#define TEN_BIT_MASK 0x03FFU

#define UPPER_8(x) ((x >> 8) & BYTE_MASK)
#define LOWER_8(x) (x & BYTE_MASK)

static inline message_command_t build_ADCV_command(const ADC_cmd_cfg_t *config);
static inline message_command_t build_ADSV_command(const ADC_cmd_cfg_t *config);
static inline message_command_t build_ADAX_command(const ADC_cmd_cfg_t *config);
static inline message_command_t build_ADAX2_command(const ADC_cmd_cfg_t *config);

/**
 * @brief Calculate the PEC15 for a given data frame.
 *  The command PEC is a 15-bit cyclic redundancy check (CRC) value 
    calculated for all 16 bits of a command, using the initial PEC value of
    b'000000000010000 and the following characteristic polynomial: 
    x^15 + x^14 + x^10 + x^8 + x^7 + x^4 + x^3 + 1.
    From nda
 * @param data pointer to the data frame to calculate the PEC for
 * @param len the length of the data frame
 * @return uint16_t the calculated PEC15 value
 */
uint16_t calc_PEC15(const uint8_t *data, uint16_t len) {
    uint16_t remainder = 16; 
    
    uint16_t addr = 0U; /* initialize the PEC */

    for (uint8_t i = 0; i < len; i++) /* loops for each byte in data array */
    {
        addr = (((remainder >> 7) ^ data[i]) & BYTE_MASK); /* calculate PEC table address */
        remainder = ((remainder << 8) ^ CMD_PEC15_LUT[addr]);
    }

    /* The CRC15 has a 0 in the LSB so the remainder must be multiplied by 2 */
    return (remainder << 1); 

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
uint16_t calc_PEC10(const uint8_t *data, uint16_t len, const uint8_t *commandCounter) {
    uint16_t nRemainder = 16u; /* PEC_SEED */
    /* x10 + x7 + x3 + x2 + x + 1 <- the CRC10 polynomial 100 1000 1111 */
    uint16_t nPolynomial = 0x8Fu;
    uint8_t nByteIndex = 0;
    uint8_t nBitIndex = 0;
    uint8_t nTableAddr = 0;

    for (nByteIndex = 0u; nByteIndex < len; ++nByteIndex)
    {
        /* calculate PEC table address */
        nTableAddr = (uint16_t)(((uint16_t)(nRemainder >> 2) ^ (uint8_t)data[nByteIndex]) & (uint8_t)0xff);
        nRemainder = (uint16_t)(((uint16_t)(nRemainder << 8)) ^ DATA_PEC10_LUT[nTableAddr]);
    }
    /* If array is from received buffer add command counter to crc calculation */
    if (commandCounter != NULL)
    {
        nRemainder ^= (uint16_t)(*commandCounter << 4U);
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
    return (uint16_t)(nRemainder & TEN_BIT_MASK);
}



/**
 * @brief Pack the PEC15 value into a message header.
 * Split the PEC15 value into two bytes and store them in the message header.
 * @param data 
 * @return message_header_t 
 */
message_header_t pack_PEC15(uint8_t *data) {
    // TODO
    uint16_t pec_calc = calc_PEC15(data, 2);

    return (message_header_t){
        .pec[0] = UPPER_8(pec_calc),
        .pec[1] = LOWER_8(pec_calc)
    };
}

/**
 * @brief Pack the PEC10 value into a message header.
 * Split the PEC10 value into two bytes and store them in the message header.
 * @param data 
 * @param commandCounter 
 * @return message_header_t 
 */
message_header_t pack_PEC10(uint8_t *data, const uint8_t *commandCounter) {
    // TODO
    uint16_t pec_calc = calc_PEC10(data, sizeof(data), commandCounter);

    return (message_header_t){
        .pec[0] = UPPER_8(pec_calc),
        .pec[1] = LOWER_8(pec_calc)
    };
}

/**
 * @brief Top level ADC command builder function
 * 
 * @param config ADC command config struct
 * @return message_command_t the command built based on which type
 */
message_command_t build_ADC_command(const ADC_cmd_cfg_t *config) {
    switch (config->command_type) {
        case ADC_ADCV:
            return build_ADCV_command(config);
        case ADC_ADSV:
            return build_ADSV_command(config);
        case ADC_ADAX:
            return build_ADAX_command(config);
        case ADC_ADAX2:
            return build_ADAX2_command(config);
        default:
            // Return a default command 
            return (message_command_t){ .cmd = {0} };
    }
}

/**
 * @brief Build ADCV command (cell voltage readings)
 * | 10 | 9 | 8  |  7   | 6 | 5 |  4  | 3 |  2   |  1  |  0  |
 * |----|---|--- |------|---|---|-----|---|----- |-----|-----|
 * |  0 | 1 | RD | CONT | 1 | 1 | DCP | 0 | RSTF | OW1 | OW0 |
 * @param config ADC command config struct
 * @return message_command_t 
 */
static message_command_t build_ADCV_command(const ADC_cmd_cfg_t *config) {
    uint16_t command_builder = 0;

    command_builder |= (config->RD << 8) | (config->CONT << 7);
    command_builder |= (config->DCP << 4) | (config->RSTF << 2) | config->OW;
    command_builder |= (0x01 << 9) | (0x03 << 5);

    return (message_command_t) {
        .cmd[0] = UPPER_8(command_builder),
        .cmd[1] = LOWER_8(command_builder)
    };
}

/**
 * @brief Build ADSV command (S-ADC)
 * | 10 | 9 | 8 |  7   | 6 | 5 |  4  | 3 | 2 |  1  |  0  |
 * |----|---|---|------|---|---|-----|---|---|-----|-----|
 * |  0 | 0 | 1 | CONT | 1 | 1 | DCP | 1 | 0 | OW1 | OW0 |
 * @param config 
 * @return message_command_t 
 */
static message_command_t build_ADSV_command(const ADC_cmd_cfg_t *config) {
    uint16_t command_builder = 0;

    command_builder |= (config->CONT << 7) | (config->DCP << 4) | config->OW;
    command_builder |= (0x01 << 8) | (0x11 << 5) | (0x01 << 3);

    return (message_command_t) {
        .cmd[0] = UPPER_8(command_builder),
        .cmd[1] = LOWER_8(command_builder)
    };
}

/**
 * @brief Build ADAX command (auxiliary ADC status)
 * | 10 | 9 | 8  |  7  |  6  | 5 | 4 |  3  |  2  |  1  |  0  |
 * |----|---|--- |-----|-----|---|---|-----|-----|-----|-----|
 * |  1 | 0 | OW | PUP | CH4 | 0 | 1 | CH3 | CH2 | CH1 | CH0 |
 * @param config 
 * @return message_command_t 
 */
static message_command_t build_ADAX_command(const ADC_cmd_cfg_t *config) {
    uint16_t command_builder = 0;

    uint8_t CH4 = (config->CH >> 4);

    command_builder |= (config->OW << 8) | (config->PUP << 7) | (CH4 << 6);
    command_builder |= (0x01 << 10) | (0x01 << 4);
    command_builder |= (config->CH); 

    return (message_command_t) {
        .cmd[0] = UPPER_8(command_builder),
        .cmd[1] = LOWER_8(command_builder)
    };
}

/**
 * @brief Build ADAX2 command (auxiliary 2 ADC status)
 * | 10 | 9 | 8 | 7 | 6 | 5 | 4 |  3  |  2  |  1  |  0  |
 * |----|---|---|---|---|---|---|-----|-----|-----|-----|
 * | 1  | 0 | 0 | 0 | 0 | 0 | 0 | CH3 | CH2 | CH1 | CH0 |
 * @param config 
 * @return message_command_t 
 */
static message_command_t build_ADAX2_command(const ADC_cmd_cfg_t *config) {
    uint16_t command_builder = 0;

    uint8_t CH = (config->CH & HALF_BYTE_MASK);

    command_builder |= (0x01 << 10) | (config->CH);

    return (message_command_t) {
        .cmd[0] = UPPER_8(command_builder),
        .cmd[1] = LOWER_8(command_builder) 
    };
}
