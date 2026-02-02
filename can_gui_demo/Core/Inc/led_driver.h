#ifndef LED_DRIVER_H
#define LED_DRIVER_H

#include <stdint.h>
#include <sys/types.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"


typedef struct{
    GPIO_TypeDef *port;
    uint16_t pin;
    GPIO_PinState state;
    bool is_blinking;
    uint32_t on_duration; //blink on duration
    uint32_t off_duration; //blink off duration
    uint32_t last_toggle_time; //timestamp for last toggle

}led_typedef;

extern led_typedef red;
extern led_typedef green;

void drive_LED(led_typedef *led);
void set_LED(led_typedef *led, GPIO_PinState state);
void blink_led(led_typedef *led, uint32_t on_duration, uint32_t off_duration);
void init_LEDs(void);

#endif