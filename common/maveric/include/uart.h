#ifndef __UART__H__ 
#define __UART__H__

#include <stdint.h>

// kbhit wrapper
int1 uart_byte_avail(uint8_t port);
// fgetc wrapper
uint8_t uart_read_byte(uint8_t port);
// uart_read_byte into a given buffer
void uart_read_buf(uint8_t port, uint8_t* buf, uint16_t len);
// fputc wrapper
void uart_write_byte(uint8_t port);
// uart_write_byte sequentially on a given buffer
void uart_write_buf(uint8_t port, uint8_t* buf, uint16_t len);

#endif
