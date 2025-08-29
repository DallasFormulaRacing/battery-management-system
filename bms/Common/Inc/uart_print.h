#ifndef UART_PRINT_H
#define UART_PRINT_H

#include <stdint.h>

#define BAUD_RATE 115200

void uart_print(const char *str);
void uart_print_hex(uint32_t value);
void uart_print_dec(uint32_t value);

#endif // UART_PRINT_H