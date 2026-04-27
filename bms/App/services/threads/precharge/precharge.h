#ifndef PRECHARGE_CIRCUIT_H
#define PRECHARGE_CIRCUIT_H

#include "amc_drivers.h"
#include "cmsis_os2.h"
#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_gpio.h"

#define PRECHARGE_CIRCUIT_GPIO_GROUP GPIOC
#define IR_POS_GPIO GPIO_PIN_1
#define PRECHARGE_THRESHOLD_DELTA_V 10U
#define PRECHARGE_DETECT_LINK_CAP_MAX_V 60U

void run_precharge_task();

#endif