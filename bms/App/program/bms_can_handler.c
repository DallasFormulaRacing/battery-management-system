#include "bms.h"
#include "charger.h"
#include "stm32g4xx_hal.h"
#include "fdcan.h"

// 64B CAN-FD payload, reserve 4 bytes for our mini-payload-header
#define CANFD_MAX_DATA_BYTES         64U
#define SV_HDR_BYTES                 4U
#define SV_MAX_READINGS_PER_FRAME    ((CANFD_MAX_DATA_BYTES - SV_HDR_BYTES) / 2U) // int16_t = 2B

static void BMS_CAN_RxHandler(const FDCAN_RxHeaderTypeDef *hdr, const uint8_t *data, void *ctx);
static void BMS_Send_SVoltages_All(bms_handler_t *bms);
static void BMS_Send_CVoltages_All(bms_handler_t *bms);

static void BMS_CAN_RxHandler(const FDCAN_RxHeaderTypeDef *hdr, const uint8_t *data, void *ctx)
{
    bms_handler_t *bms = (bms_handler_t*)ctx;

    uint32_t rx_id = hdr->Identifier;

    switch (CAN_ID_GET_CMD(rx_id)) {
        case CMD_ID_SVOLTAGE_ALL:
                BMS_Send_SVoltages_All(bms);
            break;
        case CMD_ID_CVOLTAGE_ALL:
                BMS_Send_CVoltages_All(bms);
            break;
        default:
            // handle unknown command or ignore
            break;
    }
}


static inline void write_i16_le(uint8_t *dst, int16_t v)
{
    dst[0] = (uint8_t)((uint16_t)v & 0xFFU);
    dst[1] = (uint8_t)(((uint16_t)v >> 8) & 0xFFU);
}

/*
Payload format (SVOLTAGE_ALL_RESP):
  byte0 = ic_index
  byte1 = start_cell_index
  byte2 = count (number of int16 readings in this frame)
  byte3 = chunk_index
  byte4.. = int16 readings (little-endian)
*/
static void BMS_Send_SVoltages_All(bms_handler_t *bms)
{
    if (bms == NULL || bms->asic == NULL) return;

    for (uint8_t ic = 0; ic < (uint8_t)IC_COUNT_CHAIN; ic++) {

        uint8_t start = 0;
        uint8_t chunk = 0;

        while (start < (uint8_t)ADBMS_NUM_CELLS_PER_IC) {

            uint8_t remaining = (uint8_t)ADBMS_NUM_CELLS_PER_IC - start;
            uint8_t count = (remaining > (uint8_t)SV_MAX_READINGS_PER_FRAME)
                                ? (uint8_t)SV_MAX_READINGS_PER_FRAME
                                : remaining;

            uint8_t tx[CANFD_MAX_DATA_BYTES] = {0};

            tx[0] = ic;        // ic_index
            tx[1] = start;     // start_cell_index
            tx[2] = count;     // number of readings
            tx[3] = chunk;     // chunk_index

            uint32_t off = SV_HDR_BYTES;
            for (uint8_t i = 0; i < count; i++) {
                int16_t v = (int16_t)bms->asic[ic]
                                .s_cell.s_cell_voltages_array[start + i];
                write_i16_le(&tx[off], v);
                off += 2U;
            }

            // Send one response frame (same CAN ID for all ICs/chunks)
            (void)CAN_Transmit(READ_SVOLTAGE_ALL_RESP_ID, tx, off);

            start = (uint8_t)(start + count);
            chunk++;
        }
    }
}
