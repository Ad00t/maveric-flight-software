#include "uart.h"
#include <stdint.h>
#include <stdio.h>

#module

int1 uart_byte_avail(uint8_t port) {
    switch (port) {
        case COM_A: return kbhit(COM_A);
        //case COM_B: return kbhit(COM_B);
        //case COM_C: return kbhit(COM_C);
        //case COM_D: return kbhit(COM_D);
        default:    return 0;
    }
}

uint8_t uart_read_byte(uint8_t port) {
    switch (port) {
        case COM_A: return fgetc(COM_A);
        //case COM_B: return fgetc(COM_B);
        //case COM_C: return fgetc(COM_C);
        //case COM_D: return fgetc(COM_D);
        default:    return 0;
    }
}

void uart_read_buf(uint8_t port, uint8_t* buf, uint16_t len) {
    uint16_t i;
    for (i = 0; i < len; i++) {
        buf[i] = uart_read_byte(port);
    }
}

void uart_write_byte(uint8_t port, uint8_t c) {
    switch (port) {
        case COM_A: fputc(c, COM_A); break;
        //case COM_B: fputc(c, COM_B); break;
        //case COM_C: fputc(c, COM_C); break;
        //case COM_D: fputc(c, COM_D); break;
        default:    break;
    }
}

void uart_write_buf(uint8_t port, uint8_t* buf, uint16_t len) {
    uint16_t i;
    for (i = 0; i < len; i++) {
        uart_write_byte(port, buf[i]);
    }
}

