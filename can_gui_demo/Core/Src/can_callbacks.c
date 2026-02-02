#include "stm32f4xx_hal_can.h"
#include "led_driver.h"
#include "main.h"

#define LED_RED 0x01
#define LED_GREEN 0x00

#define CMD_LED_INIT 0x100
#define CMD_SET_LED 0x101
#define CMD_BLINK_LED 0x102

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan){

    while (HAL_CAN_GetRxFifoFillLevel(hcan, CAN_RX_FIFO0) > 0) {
        CAN_RxHeaderTypeDef rxHeader;
        uint8_t rxData[8];

        // Receive the message
        if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rxHeader, rxData) == HAL_OK) {
            switch (rxHeader.StdId) {
                case CMD_SET_LED:
                    if (rxData[0] == LED_RED) {
                        if (rxData[1] == 0x01)
                            set_LED(&red, GPIO_PIN_RESET); // Turn on (assuming active low)
                        else
                            set_LED(&red, GPIO_PIN_SET); // Turn off
                    } else if (rxData[0] == LED_GREEN) {
                        if (rxData[1] == 0x01)
                            set_LED(&green, GPIO_PIN_RESET); // Turn on
                        else
                            set_LED(&green, GPIO_PIN_SET); // Turn off
                    break;
                case CMD_BLINK_LED:
                if (rxData[0] == LED_RED) {
                        blink_led(&red, rxData[1], rxData[2]);
                    } else if (rxData[0] == LED_GREEN) {
                        blink_led(&green, rxData[1], rxData[2]);
                    break;
                case CMD_LED_INIT:
                    init_LEDs();
                    break;
                default:
                    // Unknown command, ignore
                    break;
            }
        }
    }
}

void HAL_CAN_RxFifo0FullCallback(CAN_HandleTypeDef *hcan);

void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan);

