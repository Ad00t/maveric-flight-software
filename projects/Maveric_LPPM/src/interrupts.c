#include "interrupts.h"
#include "uart.h"
#include "adcsmtq.h"
#include <stdint.h>
#include <stdio.h>

#module

// HELPERS

// API

void enable_all_interrupts(void) {
	enable_interrupts(INT_RDA);
	enable_interrupts(INT_RDA2);
	enable_interrupts(INT_RDA3);
	enable_interrupts(INT_RDA4);
}

void disable_all_interrupts(void) {
	disable_interrupts(INT_RDA);
	disable_interrupts(INT_RDA2);
	disable_interrupts(INT_RDA3);
	disable_interrupts(INT_RDA4);
}

void log_interrupts_rcv(void) {
    if (INTERRUPT_RCV_FLAG) {
        fprintf(COM_D, "%s[LPPM] RCV:%u CMD:%u port:%u msg_len:%u str:\"%s\" [ ", 
                KGRN, INTERRUPT_RCV_FLAG, INTERRUPT_CMD_FLAG, INTERRUPT_RCV_PORT, INTERRUPT_RCV_MSG_LEN, INTERRUPT_RCV_BUF);
        size_t i;
        for (i = 0; i < INTERRUPT_RCV_MSG_LEN; i++) 
            fprintf(COM_D, "0x%02X ", INTERRUPT_RCV_BUF[i]);
        fprintf(COM_D, "]\r\n");
    }
}

void cleanup_interrupts_rcv(void) {
    memset(INTERRUPT_RCV_BUF, 0, MAX_BUF_LEN * sizeof(uint8_t));
    INTERRUPT_RCV_FLAG = 0;
    INTERRUPT_CMD_FLAG = 0;
    INTERRUPT_RCV_PORT = 255;
    INTERRUPT_RCV_MSG_LEN = 0;
}

// UART ISRs

void default_rda_isr(uint8_t port) {
    if (!INTERRUPT_START_FLAG || !uart_byte_avail(port)) return;
    char c = 0;
    INTERRUPT_RCV_MSG_LEN = 0;
    while (c != 13) { // ASCII 13 = \r
        c = uart_read_byte(port);
        if (c == 8 && INTERRUPT_RCV_MSG_LEN > 0) {  // Backspace
            INTERRUPT_RCV_MSG_LEN--;
        } else { 
            if (INTERRUPT_RCV_MSG_LEN >= MAX_BUF_LEN-1) break;
            if (c >= ' ' && c <= '~') INTERRUPT_RCV_BUF[INTERRUPT_RCV_MSG_LEN] = c; // Standard chars
               INTERRUPT_RCV_MSG_LEN++;
        }
    }
    INTERRUPT_RCV_BUF[INTERRUPT_RCV_MSG_LEN] = 0;
	INTERRUPT_RCV_FLAG = TRUE;
    INTERRUPT_RCV_PORT = port;
}

#INT_RDA
void RDA1_ISR(void) {
    if (!INTERRUPT_START_FLAG || !uart_byte_avail(COM_A)) return;
    // size_t i;
    // size_t max = 5;
    // for (i = 0; i < max; i++) {
    //     INTERRUPT_RCV_BUF[i] = uart_read_byte(COM_A);
    //     INTERRUPT_RCV_MSG_LEN++;
    //     if (i == 2 && INTERRUPT_RCV_BUF[i] == ADCSMTQ_HEAD_READ)
    //         max += 4*INTERRUPT_RCV_BUF[i];
    // }
    // INTERRUPT_RCV_BUF[INTERRUPT_RCV_MSG_LEN] = 0;
    // INTERRUPT_RCV_FLAG = TRUE;
    // INTERRUPT_RCV_PORT = COM_A;
}

#INT_RDA2
void RDA2_ISR(void) {
    // default_rda_isr(COM_B); 
	//    INTERRUPT_CMD_FLAG = TRUE;
}

#INT_RDA3
void RDA3_ISR(void) {
    // default_rda_isr(COM_C);
    // INTERRUPT_CMD_FLAG = TRUE;
}

#INT_RDA4
void RDA4_ISR(void) {
	default_rda_isr(COM_D);
    INTERRUPT_CMD_FLAG = TRUE;
}
