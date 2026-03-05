#include "bms.h"
#include "bms_types.h"
#include "fdcan.h"
#include <stdint.h>
#include "main.h"

#define CANFD_MAX_DATA_BYTES         64U

typedef enum{
    CELL,
    S_CELL,
    AVG_CELL,
    FILTERED_CELL
} cell_voltage_type_t;

static void BMS_CAN_RxHandler(const FDCAN_RxHeaderTypeDef *hdr, const uint8_t *data, void *ctx);
static void BMS_SendVoltageFrame(cell_voltage_type_t voltage_type, uint8_t *tx);
static void BMS_Send_Voltages_All(bms_handler_t *bms, cell_voltage_type_t voltage_type);

/**
 * @brief RX handler for incoming CAN messages, registered with FDCAN driver
 */
static void BMS_CAN_RxHandler(const FDCAN_RxHeaderTypeDef *hdr, const uint8_t *data, void *ctx)
{
    cell_asic_ctx_t *asic = (cell_asic_ctx_t*)ctx; // Cast context to ASIC context pointer, can be used to access BMS state if needed
    //bms_handler_t *bms = (bms_handler_t*)ctx;

    uint32_t rx_id = hdr->Identifier;

    switch (CAN_ID_GET_CMD(rx_id)) {
        case CMD_ID_SVOLTAGE_ALL:
                BMS_Send_Voltages_All(asic, S_CELL);
            break;
        case CMD_ID_CVOLTAGE_ALL:
                BMS_Send_Voltages_All(asic, CELL);
            break;
        default:
            // ignore
            break;
    }
}

/**
 * @brief Cell voltage can transmission wrapper
 */
static void BMS_Transmit_Voltage_CAN_Frame(cell_voltage_type_t voltage_type, uint8_t *tx)
{
    uint32_t can_id;

    switch (voltage_type)
    {
        case CELL:
            can_id = READ_CVOLTAGE_ALL_RESP_ID;
            break;

        case S_CELL:
            can_id = READ_SVOLTAGE_ALL_RESP_ID;
            break;

        case AVG_CELL:
            //can_id = READ_AVGVOLTAGE_ALL_RESP_ID; unimplemented in fdcan.h
            break;

        case FILTERED_CELL:
            //can_id = READ_FILTEREDVOLTAGE_ALL_RESP_ID; unimplemented in fdcan.h
            break;

        default:
            return;  // invalid type → do nothing
    }

    //round up to 48 bytes to keep CANFD macro, max is actually 33
    (void)CAN_Transmit(can_id, tx, FDCAN_DLC_BYTES_48);
}

/**
 * @brief Sends all cell voltages of the specified type (raw, s_cell, avg, or filtered) for all ICs in the chain over CAN
    * @param bms Pointer to the BMS handler struct containing ASIC contexts
    * @param voltage_type Enum indicating which type of cell voltage to send (CELL, S_CELL, AVG_CELL, FILTERED_CELL)
    * payload format has first byte as IC index
    * followed by 16 bytes of cell voltages for that IC (little endian, 2 bytes per voltage)
    * total payload is 1 + (16*2) = 33 bytes per IC
 */
static void BMS_Send_Voltages_All(cell_asic_ctx_t *asic, cell_voltage_type_t voltage_type)
{
    if (asic == NULL) return;

    for (uint8_t ic = 0; ic < (uint8_t)IC_COUNT_CHAIN; ic++) {

            uint8_t tx[CANFD_MAX_DATA_BYTES] = {0};
            tx[0] = ic;        // ic_index
            uint32_t off = 1U; // offset in tx buffer, start after ic identifier byte

        for (uint8_t current_cell = 0; current_cell < (uint8_t)ADBMS_NUM_CELLS_PER_IC; current_cell++) {

            int16_t voltage;;

            switch(voltage_type) {
                case CELL:
                    voltage = (int16_t)asic->cell.cell_voltages_array[current_cell];
                    break;
                case S_CELL:
                    voltage = (int16_t)asic->s_cell.s_cell_voltages_array[current_cell];
                    break;
                
                case AVG_CELL: //unused for now but can be added as a command later if needed
                    voltage = (int16_t)asic->avg_cell.avg_cell_voltages_array[current_cell];
                    break;
                case FILTERED_CELL: //unused for now but can be added as a command later if needed
                    voltage = (int16_t)asic->filtered_cell.filtered_cell_voltages_array[current_cell];
                    break;
                default:
                    voltage = 0;
                    break;
                }

                //split 16 bit voltages into bytes and store in tx buffer, little endian
                tx[off] = (uint8_t)((uint16_t)voltage & 0xFFU);
                tx[off+1] = (uint8_t)(((uint16_t)voltage >> 8) & 0xFFU);

                off += 2U;
        }

        BMS_Transmit_Voltage_CAN_Frame(voltage_type, tx);
    }
}

/**
 * @brief Initializes FDCAN peripheral, configures filters, and registers application-level RX handler
 */
void CAN_Hardware_Init(){
    Configure_FDCAN_Filter();

    if (HAL_FDCAN_Start(&hfdcan2) != HAL_OK) {
        // Handle error 
    }


    if(HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK) {
        // Handle error
    }

}