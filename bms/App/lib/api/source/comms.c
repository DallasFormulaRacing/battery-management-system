#include "comms.h"
#include "command_list.h"
#include <stdint.h>

static void build_command_buffer(const command_t command_bytes, uint8_t *cmd);

static void spi_read_all(uint8_t ic_count, command_msg_t cmd_msg,
                         uint8_t *rx_data, uint8_t bytes_per_asic_register);

static void spi_write_all(uint8_t ic_count, command_msg_t cmd_msg,
                          uint8_t *tx_data, uint8_t bytes_per_asic_register);

static comm_status_t handle_single_asic(const uint8_t *rx_data,
                                        uint8_t bytes_in_reg,
                                        asic_status_buffers_t *status,
                                        uint8_t index);

static comm_status_t read_all_asics(uint8_t ic_count, uint8_t bytes_in_reg,
                                    uint8_t *rx_data,
                                    asic_status_buffers_t *status);

static comm_status_t pec_check(const uint8_t *rx_data, uint16_t rx_pec,
                               uint8_t bytes_in_reg);

/******************************************************************************/

void bms_send_command(const command_t tx_cmd) {
  command_msg_t cmd_msg;
  build_command_buffer(tx_cmd, cmd_msg);
  spi_write(4, cmd_msg);
}

comm_status_t bms_read_data_register(uint8_t ic_count, command_t command_bytes,
                                     asic_status_buffers_t *asic_status_buffers,
                                     uint8_t bytes_per_asic_register) {
  command_msg_t cmd_msg;
  comm_status_t status;

  if (ic_count > ADBMS_NUM_CELLS_PER_IC || ic_count == 0)
    return COMM_INVALID_NUMBER_OF_ICS;

  if (bytes_per_asic_register == 0)
    return COMM_INVALID_PARAMETERS;

  if ((asic_status_buffers == NULL) ||
      (asic_status_buffers->register_data == NULL) ||
      (asic_status_buffers->pec_error_flags == NULL) ||
      (asic_status_buffers->command_counter == NULL)) {
    return COMM_ERROR;
  }

  if (bytes_per_asic_register < 3U) {
    return COMM_ERROR;
  }

  if ((uint16_t)ic_count * bytes_per_asic_register > UINT8_MAX) {
    return COMM_ERROR;
  }

  build_command_buffer(command_bytes, cmd_msg);

  spi_read_all(ic_count, cmd_msg, asic_status_buffers->register_data,
               bytes_per_asic_register);

  for (uint8_t i = 0; i < ic_count; i++) {
    status =
        handle_single_asic(asic_status_buffers->register_data,
                           bytes_per_asic_register, asic_status_buffers, i);
  }

  return status;
}

comm_status_t bms_write_data_register(uint8_t ic_count, command_t command_bytes,
                                      uint8_t *data) {
  // todo: placeholder
}

/*************************************************************/

static void build_command_buffer(const command_t command_bytes, uint8_t *cmd) {
  uint16_t cmd_pec;
  cmd[0] = command_bytes[0];
  cmd[1] = command_bytes[1];
  cmd_pec = calc_PEC15(2, cmd);
  cmd[2] = (uint8_t)(cmd_pec >> 8);
  cmd[3] = (uint8_t)(cmd_pec);
}

static void spi_read_all(uint8_t ic_count, command_msg_t cmd_msg,
                         uint8_t *rx_data, uint8_t bytes_per_asic_register) {
  uint8_t len = (uint8_t)(bytes_per_asic_register * ic_count);
  spi_write_read(cmd_msg, rx_data, len);
}

static void spi_write_all(uint8_t ic_count, command_msg_t cmd_msg,
                          uint8_t *tx_data, uint8_t bytes_per_asic_register) {
  uint8_t len = (uint8_t)(bytes_per_asic_register * ic_count);

  spi_write(4U, cmd_msg);
  spi_write(len, tx_data);
}

static comm_status_t handle_single_asic(const uint8_t *rx_data,
                                        uint8_t bytes_in_reg,
                                        asic_status_buffers_t *status,
                                        uint8_t index) {
  uint16_t rx_pec;

  uint8_t *cmd_counter = &status->command_counter[index];
  uint8_t *pec_err = &status->pec_error_flags[index];

  /* Extract command counter from second-to-last byte (upper 6 bits) */
  *cmd_counter = (uint8_t)(rx_data[bytes_in_reg - 2U] >> 2);

  /* Reconstruct 10-bit PEC from last two bytes */
  rx_pec = (uint16_t)(((rx_data[bytes_in_reg - 2U] & 0x03U) << 8) |
                      rx_data[bytes_in_reg - 1U]);

  /* Check PEC */
  comm_status_t pec_status = pec_check(rx_data, rx_pec, bytes_in_reg);

  /* 0 = OK, 1 = PEC error (your existing convention) */
  *pec_err = (pec_status == COMM_OK) ? 0U : 1U;
  return pec_status;
}

static comm_status_t pec_check(const uint8_t *rx_data, uint16_t rx_pec,
                               uint8_t bytes_in_reg) {
  uint16_t calculated_pec =
      calc_PEC10(1U, (uint8_t)(bytes_in_reg - 2U), rx_data);
  return (rx_pec == calculated_pec) ? COMM_OK : COMM_INVALID_PEC_ERROR_FLAG;
}

static comm_status_t read_all_asics(uint8_t ic_count, uint8_t bytes_in_reg,
                                    uint8_t *rx_data,
                                    asic_status_buffers_t *status) {
  comm_status_t overall = COMM_OK;

  for (uint8_t current_ic = 0U; current_ic < ic_count; ++current_ic) {
    const uint8_t *asic_data = &rx_data[current_ic * bytes_in_reg];
    comm_status_t s =
        handle_single_asic(asic_data, bytes_in_reg, status, current_ic);

    if ((s != COMM_OK) && (overall == COMM_OK)) {
      overall = s; /* preserve first error */
    }
  }

  return overall;
}