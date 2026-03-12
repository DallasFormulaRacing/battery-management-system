// api/bms_can.c

// #include "bms_can.h"
// #include "bms_can_protocol.h" // CAN_ID_GET_CMD, CMD_ID_*, READ_*_RESP_ID
#include "bms_types.h"
#include "config.h"
#include "fdcan.h" // core: fdcan_register_rx_handler(), fdcan_send()
#include <stdint.h>

#define CANFD_MAX_DATA_BYTES 64U

typedef enum { CELL, S_CELL, AVG_CELL, FILTERED_CELL } cell_voltage_type_t;

static void bms_can_rx_handler(const FDCAN_RxHeaderTypeDef *hdr,
                               const uint8_t *data, void *ctx);

static void bms_transmit_voltage_can_frame(cell_voltage_type_t voltage_type,
                                           const uint8_t *tx);

static void bms_send_voltages_all(cell_asic_ctx_t *asic,
                                  cell_voltage_type_t voltage_type);

/**
 * @brief Called by program (bms.c). Registers the API RX handler with core.
 */
void bms_can_init(cell_asic_ctx_t *asic) {
  fdcan_register_rx_handler(bms_can_rx_handler, (void *)asic);
}

/**
 * @brief ACTUAL application-level RX handler. Runs when RX FIFO0 gets a new
 * frame. This is the function Core dispatches to via the registered function
 * pointer.
 */
static void bms_can_rx_handler(const FDCAN_RxHeaderTypeDef *hdr,
                               const uint8_t *data, void *ctx) {
  (void)data; // currently unused (dispatching solely on cmd)

  if (hdr == NULL || ctx == NULL) {
    return;
  }

  cell_asic_ctx_t *asic = (cell_asic_ctx_t *)ctx;
  const uint32_t rx_id = hdr->Identifier;

  // switch (can_id_get_cmd(rx_id)) {
  //   // case CMD_ID_SVOLTAGE_ALL:
  //   bms_send_voltages_all(asic, S_CELL);
  //   break;

  //   // case CMD_ID_CVOLTAGE_ALL:
  //   bms_send_voltages_all(asic, CELL);
  //   break;

  // default:
  //   // ignore unknown commands
  //   break;
  // }
}

/**
 * @brief Transmit a voltage response frame for the requested voltage type.
 */
static void bms_transmit_voltage_can_frame(cell_voltage_type_t voltage_type,
                                           const uint8_t *data_frame) {
  uint32_t can_id;

  switch (voltage_type) {
  case CELL:
    // can_id = CMD_ID_CVOLTAGE_ALL_RESP;
    break;

  case S_CELL:
    // can_id = CMD_ID_SVOLTAGE_ALL_RESP;
    break;

  case AVG_CELL:
    // unimplemented
    return;

  case FILTERED_CELL:
    // unimplemented
    return;

  default:
    return;
  }

  // Your payload is 33 bytes, but rounding up to 48 to fit a CANFD DLC.
  (void)fdcan_send(can_id, (uint8_t *)data_frame, FDCAN_DLC_BYTES_48);
}

/**
 * @brief Sends all cell voltages (for each IC) in the requested voltage domain.
 *
 * Payload format:
 *   byte0  = IC index
 *   bytes1..32 = 16 cell voltages, 2 bytes each, little-endian (total 32 bytes)
 * Total logical bytes = 33
 */
// void bms_send_voltages_all(cell_asic_ctx_t *asic,
//                            cell_voltage_type_t voltage_type) {
//   if (asic == NULL) {
//     return;
//   }

//   for (uint8_t ic = 0; ic < (uint8_t)IC_COUNT_CHAIN; ic++) {

//     uint8_t tx[CANFD_MAX_DATA_BYTES] = {0};
//     tx[0] = ic;

//     uint32_t off = 1U;

//     for (uint8_t current_cell = 0;
//          current_cell < (uint8_t)ADBMS_NUM_CELLS_PER_IC; current_cell++) {
//       int16_t voltage;

//       switch (voltage_type) {
//       case CELL:
//         voltage = (int16_t)asic->cell.cell_voltages_array[current_cell];
//         break;

//       case S_CELL:
//         voltage = (int16_t)asic->s_cell.s_cell_voltages_array[current_cell];
//         break;

//       case AVG_CELL:
//         voltage =
//         (int16_t)asic->avg_cell.avg_cell_voltages_array[current_cell]; break;

//       case FILTERED_CELL:
//         voltage =
//             (int16_t)
//                 asic->filtered_cell.filtered_cell_voltages_array[current_cell];
//         break;

//       default:
//         voltage = 0;
//         break;
//       }

//       tx[off] = (uint8_t)((uint16_t)voltage & 0xFFU);
//       tx[off + 1] = (uint8_t)(((uint16_t)voltage >> 8) & 0xFFU);
//       off += 2U;
//     }

//     bms_transmit_voltage_can_frame(voltage_type, tx);
//   }
// }