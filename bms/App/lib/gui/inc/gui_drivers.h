#ifndef GUI_DRIVER
#define GUI_DRIVER

#include "bms.h"
#include "bms_types.h"
#include "cmsis_os2.h"
#include "config.h"
#include "fdcan.h"
#include "gui_types.h"
#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_fdcan.h"
#include <stdint.h>

void gui_process_can_command(uint32_t ext_id, uint8_t *data);

void send_can_error(can_error_id_t error_id);

void cell_voltage_readings(cell_asic_ctx_t *asic, uint8_t start_ic,
                           uint8_t end_ic, uint8_t *data_arr);

void therm_temp_readings(cell_asic_ctx_t *asic, uint8_t start_ic,
                         uint8_t end_ic, uint8_t *data_arr);

void metadata_readings(pack_data_t *pack, pcb_ctx_t *pcb, uint8_t *data_arr);

#endif