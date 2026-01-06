#ifndef COMMS_H
#define COMMS_H

#include "bms_driver.h"
#include "bms_enums.h"
#include "bms_types.h"
#include "command_list.h"
#include "pec.h"
#include <stdint.h>

typedef uint8_t command_msg_t[4];

// typedef struct {
//   uint8_t *register_data;
//   uint8_t *pec_error_flags;
//   uint8_t *command_counter;
// } asic_status_buffers_t;

// typedef enum {
//   COMM_OK,
//   COMM_ERROR,
//   COMM_TIMEOUT,
//   COMM_INVALID_NUMBER_OF_ICS,
//   COMM_INVALID_COMMAND,
//   COMM_INVALID_COMMAND_COUNTER,
//   COMM_INVALID_PEC_ERROR_FLAG,
//   COMM_INVALID_PARAMETERS,
// } comm_status_t;

void bms_send_command(const command_t tx_cmd);

comm_status_t bms_read_register_spi(uint8_t ic_count, command_t command_bytes,
                                    asic_status_buffers_t *asic_status_buffers,
                                    uint8_t bytes_per_asic_register);

comm_status_t bms_write_register_spi(uint8_t ic_count,
                                     const command_t command_bytes,
                                     const uint8_t *per_asic_data,
                                     uint8_t bytes_per_asic_data);

#endif
