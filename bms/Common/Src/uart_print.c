#include "uart_print.h"

#define PRINT_OVER_UART_EN

#ifdef PRINT_OVER_UART_EN

// TODO: redirect printf to UART on Port 
// PC4.Mode=Asynchronous
// PC4.Signal=USART1_TX
// PC5.Mode=Asynchronous
// PC5.Signal=USART1_RX

void uart_print(const char *str) {

}

void uart_print_hex(uint32_t value) {

}

void uart_print_dec(uint32_t value) {

}


#endif