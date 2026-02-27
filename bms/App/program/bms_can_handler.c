#include "bms.h"
#include "bms_types.h"
//#include "charger.h"
//#include "stm32g4xx_hal.h"
#include "fdcan.h"
#include <stdint.h>

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
//static void BMS_Send_CVoltages_All(bms_handler_t *bms);

static void BMS_CAN_RxHandler(const FDCAN_RxHeaderTypeDef *hdr, const uint8_t *data, void *ctx)
{
    bms_handler_t *bms = (bms_handler_t*)ctx;

    uint32_t rx_id = hdr->Identifier;

    switch (CAN_ID_GET_CMD(rx_id)) {
        case CMD_ID_SVOLTAGE_ALL:
                BMS_Send_Voltages_All(bms, S_CELL);
            break;
        case CMD_ID_CVOLTAGE_ALL:
                BMS_Send_Voltages_All(bms, CELL);
            break;
        default:
            // ignore
            break;
    }
}

static void BMS_SendVoltageFrame(cell_voltage_type_t voltage_type, uint8_t *tx)
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

    (void)CAN_Transmit(can_id, tx, FDCAN_DLC_BYTES_48);
}

/*
Payload format:
    byte0 = ic_index
    byte1...32 = cell voltages
*/
static void BMS_Send_Voltages_All(bms_handler_t *bms, cell_voltage_type_t voltage_type)
{
    if (bms == NULL || bms->asic == NULL) return;

    for (uint8_t ic = 0; ic < (uint8_t)IC_COUNT_CHAIN; ic++) {

            uint8_t tx[CANFD_MAX_DATA_BYTES] = {0};
            tx[0] = ic;        // ic_index
            uint32_t off = 1U; // offset in tx buffer, start after ic identifier byte

        for (uint8_t current_cell = 0; current_cell < (uint8_t)ADBMS_NUM_CELLS_PER_IC; current_cell++) {

            int16_t voltage;;

            switch(voltage_type) {
                case CELL:
                    voltage = (int16_t)bms->asic[ic].cell.cell_voltages_array[current_cell];
                    break;
                case S_CELL:
                    voltage = (int16_t)bms->asic[ic].s_cell.s_cell_voltages_array[current_cell];
                    break;
                
                case AVG_CELL: //unused for now but can be added as a command later if needed
                    voltage = (int16_t)bms->asic[ic].avg_cell.avg_cell_voltages_array[current_cell];
                    break;
                case FILTERED_CELL: //unused for now but can be added as a command later if needed
                    voltage = (int16_t)bms->asic[ic].filtered_cell.filtered_cell_voltages_array[current_cell];
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

        BMS_SendVoltageFrame(voltage_type, tx);
    }
}

void CAN_Hardware_Init(){
    Configure_FDCAN_Filter();

    if (HAL_FDCAN_Start(&hfdcan2) != HAL_OK) {
        // Handle error 
    }

    //FDCAN_RegisterRXHandler(BMS_CAN_RxHandler, NULL); // Register the CAN RX handler

    if(HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK) {
        // Handle error
    }

}