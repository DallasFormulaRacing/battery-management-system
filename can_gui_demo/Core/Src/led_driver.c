
#include "led_driver.h"
#include "stm32f4xx_hal_gpio.h"
#include "main.h"

led_typedef red = {0};
led_typedef green = {0};

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

void blink_led(led_typedef *led, uint32_t on_duration, uint32_t off_duration){
    led->on_duration = on_duration;
    led->off_duration = off_duration;
    led->is_blinking = true;
    led->last_toggle_time = HAL_GetTick();
}

void set_LED(led_typedef *led, GPIO_PinState state){
    HAL_GPIO_WritePin(led->port, led->pin, state);
    led->state = state;
    led->is_blinking = false;
}

void init_LEDs(){
    // Initialize Red LED
    red.port = LED_Red_GPIO_Port;
    red.pin = LED_Red_Pin;
    red.state = GPIO_PIN_SET; // Assuming active low
    red.is_blinking = false;
    red.on_duration = 0; // 500 ms
    red.off_duration = 0; // 500 ms
    red.last_toggle_time = 0;

    // Initialize Green LED
    green.port = LED_Green_GPIO_Port;
    green.pin = LED_Green_Pin;
    green.state = GPIO_PIN_SET;
    green.is_blinking = false;
    green.on_duration = 0;
    green.off_duration = 0;
    green.last_toggle_time = 0;

    set_LED(&red, GPIO_PIN_SET);
    set_LED(&green, GPIO_PIN_SET);
    return;
}