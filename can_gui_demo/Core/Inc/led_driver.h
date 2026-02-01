#ifndef LED_DRIVER_H
#define LED_DRIVER_H

typedef struct{

    //port
    //pin
    //on/off, status
    //blink mode on or off
    //blink on duration
    //blink off duration
    //last blink time

}led_typedef;

extern led_typedef red;
extern led_typedef green;

void drive_LED(led_typedef led);

#endif