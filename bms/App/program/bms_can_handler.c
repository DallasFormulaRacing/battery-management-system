#include "bms.h"
#include "charger.h"
#include "stm32g4xx_hal.h"
#include "fdcan.h"
#include <stdint.h>

#define CANFD_MAX_DATA_BYTES         64U

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

/*
Payload format (SVOLTAGE_ALL_RESP):
  byte0 = ic_index
    byte1...32 = cell voltages
*/
static void BMS_Send_SVoltages_All(bms_handler_t *bms)
{
    if (bms == NULL || bms->asic == NULL) return;

    for (uint8_t ic = 0; ic < (uint8_t)IC_COUNT_CHAIN; ic++) {

            uint8_t tx[CANFD_MAX_DATA_BYTES] = {0};
            tx[0] = ic;        // ic_index
            uint32_t off = 1U; // offset in tx buffer, start after ic identifier byte

        for (uint8_t current_cell = 0; current_cell < (uint8_t)ADBMS_NUM_CELLS_PER_IC; current_cell++) {
                int16_t voltage = (int16_t)bms->asic[ic].s_cell.s_cell_voltages_array[current_cell];                
                /*
                cell_voltage_t cell;
                avg_cell_voltage_t avg_cell;
                s_cell_voltage_t s_cell;
                filtered_cell_voltage_t filtered_cell;
                */

                //split 16 bit voltages into bytes and store in tx buffer, little endian
                tx[off] = (uint8_t)((uint16_t)voltage & 0xFFU);
                tx[off+1] = (uint8_t)(((uint16_t)voltage >> 8) & 0xFFU);

                off += 2U;
        }

        // Send one response frame (same CAN ID for all ICs), round up to 48 bytes for CAN-FD DLC
        (void)CAN_Transmit(READ_SVOLTAGE_ALL_RESP_ID, tx, FDCAN_DLC_BYTES_48);

    }
}
