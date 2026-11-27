#ifndef BMS_BOARD_H
#define BMS_BOARD_H

/*
 * Board preset: which hardware this firmware is built for.
 *
 * bms.ioc and everything CubeMX generates always describe the CONTROLLER board.
 * Other boards are handled in code:
 *   - main.c bms_controller_config_apply() (USER CODE 4) re-maps pins and the SPI mode after MX_*_Init
 *   - the macros below give the 6822 chip select to the spi driver
 *
 * To switch boards change the default below, or pass -DBMS_BOARD=BMS_BOARD_CONTROLLER (or BMS_BOARD_NUCLEO) as a compile definition.
 *
 * Like bms_limits.h this header includes nothing; the GPIO names expand where they are used.
 */

#define BMS_BOARD_CONTROLLER 1 /* BMS controller board (STM32G474RET6 + ADBMS6822) */
#define BMS_BOARD_NUCLEO     2 /* Nucleo-G474RE + ADBMS6822 eval HAT (bench) */

#ifndef BMS_BOARD
#define BMS_BOARD BMS_BOARD_NUCLEO /* switch to BMS_BOARD_CONTROLLER as needed */
#endif

#if BMS_BOARD == BMS_BOARD_CONTROLLER
/*  6822 CS   : PA4 (SPI1_nCS in bms.ioc)
 *  SPI1      : mode 3 (6822 PHA/POL tied high)
 *  FDCAN1    : PA11 RX / PA12 TX, FDCAN2 : PB5 RX / PB6 TX
 *  PC7       : 6822 WAKE-1, input */
#define BMS_BOARD_NAME   "controller"
#define BMS_ASIC_CS_PORT SPI1_nCS_GPIO_Port
#define BMS_ASIC_CS_PIN  SPI1_nCS_Pin

#elif BMS_BOARD == BMS_BOARD_NUCLEO
/*  6822 CS   : PB6 (Arduino D10 on the eval HAT)
 *  SPI1      : mode 0 (HAT jumpers)
 *  FDCAN2    : PB12 RX / PB13 TX (only FDCAN2 is wired on the bench)
 *  PC7       : HAT CSB_2, driven high (inactive) */
#define BMS_BOARD_NAME   "nucleo"
#define BMS_ASIC_CS_PORT GPIOB
#define BMS_ASIC_CS_PIN  GPIO_PIN_6

#else
#error "BMS_BOARD must be BMS_BOARD_CONTROLLER or BMS_BOARD_NUCLEO"
#endif

#endif /* BMS_BOARD_H */
