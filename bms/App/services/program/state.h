#ifndef STATE_H
#define STATE_H

#include "bms.h"
#include "bms_enums.h"
#include "bms_types.h"
#include "cb.h"
#include "charger.h"
#include "config.h"
#include "segment.h"
#include "spi.h"
#include "stm32g4xx_hal.h"
#include "cmsis_os2.h"

extern osMutexAttr_t spi_mutex_attr;

void bms_sm_init(bms_handler_t *hbms);
void bms_sm_run(bms_handler_t *hbms);

void bms_state_entry(bms_handler_t *hbms);
void bms_state_init(bms_handler_t *hbms);
void bms_state_transmit_data(bms_handler_t *hbms);
void bms_state_measure(bms_handler_t *hbms);
void bms_state_charging(bms_handler_t *hbms);
void bms_state_balancing(bms_handler_t *hbms);
void bms_state_fault(bms_handler_t *hbms);
void bms_state_sleep(bms_handler_t *hbms);

void bms_sm_transition(bms_handler_t *hbms, bms_state_t new_state);
bool bms_check_for_fault(bms_handler_t *hbms);
#endif