#include "bms_comms.h"
#include "command_list.h"
#include "spi.h"
#include <stdint.h>

static void build_command_buffer(const command_t command_bytes, uint8_t *cmd);

static void spi_read_all(uint8_t ic_count, command_msg_t cmd_msg,
                         uint8_t *rx_data, uint8_t bytes_per_asic_register);

static comm_status_t handle_single_asic(const uint8_t *rx_data,
                                        uint8_t bytes_in_reg,
                                        asic_status_buffers_t *status,
                                        uint8_t index);

static comm_status_t read_all_asics(uint8_t ic_count, uint8_t bytes_in_reg,
                                    const uint8_t *rx_data,
                                    asic_status_buffers_t *status);

static comm_status_t pec_check(const uint8_t *rx_data, uint16_t rx_pec,
                               uint8_t bytes_in_reg);

/******************************************************************************/

/**
 * @brief Send a command to the BMS
 *
 * @param tx_cmd
 */
void bms_send_command(const command_t tx_cmd) {
  command_msg_t cmd_msg;
  build_command_buffer(tx_cmd, cmd_msg);
  spi_write(4, cmd_msg);
}

/**
 * @brief Read data registers from the BMS
 *
 * @param ic_count
 * @param command_bytes
 * @param asic_status_buffers
 * @param bytes_per_asic_register
 * @return comm_status_t
 */
comm_status_t bms_read_register_spi(uint8_t ic_count,
                                    const command_t command_bytes,
                                    asic_status_buffers_t *asic_status_buffers,
                                    uint8_t bytes_per_asic_register) {

  if (ic_count == 0U)
    return COMM_INVALID_NUMBER_OF_ICS;

  if (bytes_per_asic_register == 0)
    return COMM_INVALID_PARAMETERS;

  if (bytes_per_asic_register < 3U)
    return COMM_INVALID_PARAMETERS;

  if ((uint16_t)ic_count * bytes_per_asic_register > BUF_MAX)
    return COMM_INVALID_PARAMETERS;

  if ((asic_status_buffers == NULL) ||
      (asic_status_buffers->register_data == NULL) ||
      (asic_status_buffers->pec_error_flags == NULL) ||
      (asic_status_buffers->command_counter == NULL)) {
    return COMM_ERROR;
  }

  command_msg_t cmd_msg = {0};

  build_command_buffer(command_bytes, cmd_msg);

  spi_read_all(ic_count, cmd_msg, asic_status_buffers->register_data,
               bytes_per_asic_register);

  return read_all_asics(ic_count, bytes_per_asic_register,
                        asic_status_buffers->register_data,
                        asic_status_buffers);
}

/**
 * @brief Write data registers to the BMS
 *
 * @param ic_count
 * @param command_bytes
 * @param per_asic_data
 * @param bytes_per_asic_data
 * @return comm_status_t
 */
comm_status_t bms_write_register_spi(uint8_t ic_count,
                                     const command_t command_bytes,
                                     const uint8_t *per_asic_data,
                                     uint8_t bytes_per_asic_data) {
  if (ic_count == 0U)
    return COMM_INVALID_NUMBER_OF_ICS;

  if ((per_asic_data == NULL) || (bytes_per_asic_data == 0U))
    return COMM_INVALID_PARAMETERS;

  const uint16_t per_asic_frame_len = (uint16_t)bytes_per_asic_data + 2U;

  const uint16_t total_frame_len =
      4U + (per_asic_frame_len * (uint16_t)ic_count);

  if (total_frame_len > BUF_MAX) {
    return COMM_INVALID_PARAMETERS;
  }

  uint8_t frame[total_frame_len];

  build_command_buffer(command_bytes, frame);

  uint8_t idx = 4U;

  for (uint8_t current_ic = ic_count; current_ic > 0U; --current_ic) {
    const uint8_t *src =
        &per_asic_data[(uint16_t)(current_ic - 1U) * bytes_per_asic_data];

    for (uint8_t byte_index = 0U; byte_index < bytes_per_asic_data;
         ++byte_index) {
      frame[idx++] = src[byte_index];
    }

    uint16_t data_pec = calc_PEC10(0U, bytes_per_asic_data, src);

    frame[idx++] = (uint8_t)(data_pec >> 8);
    frame[idx++] = (uint8_t)data_pec;
  } // end for

  spi_write((uint8_t)total_frame_len, frame);

  return COMM_OK;
}

/*************************************************************/

/**
 * @brief Append PEC bytes for a given command
 *
 * @param command_bytes
 * @param cmd
 */
static void build_command_buffer(const command_t command_bytes, uint8_t *cmd) {
  /*uint8_t cmd[4];
    cmd[0] = tx_cmd[0];
    cmd[1] = tx_cmd[1];
    cmd_pec = Pec15_Calc(2, cmd);
    cmd[2] = (uint8_t)(cmd_pec >> 8);
    cmd[3] = (uint8_t)(cmd_pec);*/
  uint16_t cmd_pec;
  cmd[0] = command_bytes[0];
  cmd[1] = command_bytes[1];
  cmd_pec = calc_PEC15(2, cmd);
  cmd[2] = (uint8_t)(cmd_pec >> 8);
  cmd[3] = (uint8_t)(cmd_pec);
}

/**
 * @brief
 *
 * @param ic_count
 * @param cmd_msg
 * @param rx_data
 * @param bytes_per_asic_register
 */
static void spi_read_all(uint8_t ic_count, command_msg_t cmd_msg,
                         uint8_t *rx_data, uint8_t bytes_per_asic_register) {
  uint8_t len = (uint8_t)(bytes_per_asic_register * ic_count);
  spi_write_read(cmd_msg, rx_data, len);
}

/**
 * @brief
 *
 * @param rx_data
 * @param bytes_in_reg
 * @param status
 * @param index
 * @return comm_status_t
 */
static comm_status_t handle_single_asic(const uint8_t *rx_data,
                                        uint8_t bytes_in_reg,
                                        asic_status_buffers_t *status,
                                        uint8_t index) {
  uint16_t rx_pec;

  uint8_t *cmd_counter = &status->command_counter[index];
  uint8_t *pec_err = &status->pec_error_flags[index];

  *cmd_counter = (uint8_t)(rx_data[bytes_in_reg - 2U] >> 2);

  rx_pec = (uint16_t)(((rx_data[bytes_in_reg - 2U] & 0x03U) << 8) |
                      rx_data[bytes_in_reg - 1U]);

  comm_status_t pec_status = pec_check(rx_data, rx_pec, bytes_in_reg);

  *pec_err = (pec_status == COMM_OK) ? 0U : 1U;
  return pec_status;
}

/**
 * @brief
 *
 * @param rx_data
 * @param rx_pec
 * @param bytes_in_reg
 * @return comm_status_t
 */
static comm_status_t pec_check(const uint8_t *rx_data, uint16_t rx_pec,
                               uint8_t bytes_in_reg) {
  uint16_t calculated_pec =
      calc_PEC10(1U, (uint8_t)(bytes_in_reg - 2U), rx_data);

  return (rx_pec == calculated_pec) ? COMM_OK : COMM_INVALID_PEC_ERROR_FLAG;
}

/**
 * @brief
 *
 * @param ic_count
 * @param bytes_in_reg
 * @param rx_data
 * @param status
 * @return comm_status_t
 */
static comm_status_t read_all_asics(uint8_t ic_count, uint8_t bytes_in_reg,
                                    const uint8_t *rx_data,
                                    asic_status_buffers_t *status) {

  comm_status_t command_status = COMM_OK;

  for (uint8_t current_ic = 0U; current_ic < ic_count; ++current_ic) {
    const uint8_t *asic_data = &rx_data[current_ic * bytes_in_reg];
    comm_status_t current_status =
        handle_single_asic(asic_data, bytes_in_reg, status, current_ic);

    if (current_status != COMM_OK)
      command_status = current_status;
  }

  return command_status;
}

void spi_adcv_command(redundant_enable_t rd, cont_measurement_mode_t cont,
                      discharge_permission_t dcp, reset_filter_mode_t rstf,
                      open_wire_detect_mode_t owcs) {
  uint8_t val = (cont << 7) | (dcp << 4) | (rstf << 2) | (owcs & 0x03) | 0x60;
  bms_send_command((command_t){0x02 | rd, val});
}

void spi_adsv_command(cont_measurement_mode_t cont, discharge_permission_t dcp,
                      open_wire_detect_mode_t owcs) {
  uint8_t val = (cont << 7) | (dcp << 4) | (owcs & 0x03) | 0x68;
  bms_send_command((command_t){0x01, val});
}

void spi_adax_command(aux_open_wire_detect_mode_t owaux,
                      pull_down_current_mode_t pup, aux_select_t ch) {
  uint8_t val = (pup << 7) | (((ch >> 4) & 0x01) << 6) | (ch & 0x0F) | 0x10;
  bms_send_command((command_t){0x04 + owaux, val});
}

void spi_adax2_command(aux_select_t ch) {
  uint8_t val = (ch & 0x0F);
  bms_send_command((command_t){0x04, val});
}

void spi_adc_snap_command(void) { bms_send_command((command_t){0x00, 0x2D}); }

void spi_adc_unsnap_command(void) { bms_send_command((command_t){0x00, 0x2F}); }

void spi_poll_command_raw(const command_t cmd_bytes, const uint8_t ic_count,
                          uint8_t *poll_bytes) {
  uint8_t dummy = 0xFF;

  asic_cs_low();

  command_msg_t cmd;
  build_command_buffer(cmd_bytes, cmd);
  spi_write(4, cmd);

  for (uint8_t i = 0; i < (2U * ic_count); i++) {
    HAL_SPI_TransmitReceive(&hspi1, &dummy, &poll_bytes[i], 1, SPI_TIME_OUT);
  }

  asic_cs_hi();
}
