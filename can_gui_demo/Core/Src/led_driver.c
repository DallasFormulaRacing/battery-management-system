
#include "stm32f4xx_hal_gpio.h"
void drive_LED(led_typedef *led){
    if(!led->is_blinking){
        return;
    }
    uint32_t current_time = HAL_GetTick();
    if(led->state == GPIO_PIN_SET && (current_time - led->last_toggle_time >= led->on_duration)){
        // Turn off the LED
        HAL_GPIO_WritePin(led->port, led->pin, GPIO_PIN_RESET);
        led->state = GPIO_PIN_RESET;
        led->last_toggle_time = current_time;
    } else if(led->state == GPIO_PIN_RESET && (current_time - led->last_toggle_time >= led->off_duration)){
        // Turn on the LED
        HAL_GPIO_WritePin(led->port, led->pin, GPIO_PIN_SET);
        led->state = GPIO_PIN_SET;
        led->last_toggle_time = current_time;
    }
}

void set_LED(led_typedef *led, GPIO_PinState state){
    HAL_GPIO_WritePin(led->port, led->pin, state);
    led->state = state;
}