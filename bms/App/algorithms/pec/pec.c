#include "pec.h"

/**
 * @brief Calculate the PEC15 for a given data frame.
 * x15 + x14 + x10 + x8 + x7 + x4 + x3 + 1 <- the CRC15 polynomial 1100 0101
 * 1001 1001
 * @param len the length of the data frame
 * @param data the data frame to calculate the PEC15 for
 * @return uint16_t the calculated PEC15 value
 */
uint16_t calc_PEC15(uint8_t len, const uint8_t *data) {
  uint16_t remainder = 16;
  uint16_t addr = 0U;

  for (uint8_t i = 0; i < len; i++) {
    addr = (((remainder >> 7) ^ data[i]) & 0xFFU);
    remainder = ((remainder << 8) ^ CMD_PEC15_LUT[addr]);
  }

  return (remainder << 1);
}

/**
 * @brief Calculate the PEC10 for a given data frame.
 * x10 + x7 + x3 + x2 + x + 1 <- the CRC10 polynomial 100 1000 1111
 * @param reciever
 * @param len
 * @param data
 * @return uint16_t
 */
uint16_t calc_PEC10(bool reciever, uint8_t len, const uint8_t *data) {
  uint16_t remainder = 16U; /* PEC_SEED */
  uint16_t polynomial = 0x8FU;
  uint8_t byte_index = 0U;
  uint8_t bit_index = 0U;

  for (byte_index = 0U; byte_index < len; ++byte_index) {
    /* Bring the next byte into the remainder. */
    remainder ^= (uint16_t)((uint16_t)data[byte_index] << 2U);

    /* Perform modulo-2 division, a bit at a time.*/
    for (bit_index = 8U; bit_index > 0U; --bit_index) {
      /* Try to divide the current data bit. */
      if ((remainder & 0x200U) > 0U) {
        remainder = (uint16_t)((remainder << 1U));
        remainder = (uint16_t)(remainder ^ polynomial);
      } else {
        remainder = (uint16_t)(remainder << 1U);
      }
    }
  }

  /* If array is from received buffer add command counter to crc calculation */
  if (reciever == true) {
    remainder ^= (uint16_t)(((uint16_t)data[len] & (uint8_t)0xFC) << 2U);
  }
  /* Perform modulo-2 division, a bit at a time */
  for (bit_index = 6U; bit_index > 0U; --bit_index) {

    /* Try to divide the current data bit */
    if ((remainder & 0x200U) > 0U) {
      remainder = (uint16_t)((remainder << 1U));
      remainder = (uint16_t)(remainder ^ polynomial);
    } else {
      remainder = (uint16_t)((remainder << 1U));
    }
  }
  return ((uint16_t)(remainder & 0x3FFU));
}

uint16_t calc_PEC10_table(bool reciever, uint8_t len, const uint8_t *data) {
  uint16_t remainder = 16U; /* PEC_SEED */
  uint16_t polynomial = 0x8FU;
  uint8_t byte_index;
  uint8_t bit_index;
  uint16_t table_addr;

  for (byte_index = 0U; byte_index < len; ++byte_index) {
    /* calculate PEC table address */
    table_addr =
        (uint16_t)(((uint16_t)(remainder >> 2U) ^ (uint8_t)data[byte_index]) &
                   (uint8_t)0xFFU);
    remainder =
        (uint16_t)(((uint16_t)(remainder << 8)) ^ DATA_PEC10_LUT[table_addr]);
  }

  if (reciever == true) {
    remainder ^= (uint16_t)(((uint16_t)data[len] & (uint8_t)0xFC) << 2U);
  }

  /* Perform modulo-2 division, a bit at a time */
  for (bit_index = 6U; bit_index > 0U; --bit_index) {
    if ((remainder & 0x200U) > 0U) {
      remainder = (uint16_t)((remainder << 1U));
      remainder = (uint16_t)(remainder ^ polynomial);
    } else {
      remainder = (uint16_t)((remainder << 1U));
    }
  }

  return ((uint16_t)(remainder & 0x3FFU));
}
