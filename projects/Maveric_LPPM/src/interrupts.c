#include "interrupts.h"
#include "uart.c"
#include <stdint.h>
#include <stdio.h>

#module

// HELPERS

void default_rda_isr(uint8_t port) {
	disable_all_interrupts();
   	char c;
   	INTERRUPT_RCV_MSG_LEN = 0;
	if (start_flag) {
		while (uart_byte_avail(port) && c != 13) { 
			c = uart_read_byte(port);
	     	if (c == 8 && INTERRUPT_RCV_MSG_LEN > 0) {  // Backspace
	            INTERRUPT_RCV_MSG_LEN--;
	     	} else { 
				if (INTERRUPT_RCV_MSG_LEN >= MAX_BUF_LEN) break;
			 	if (c >= ' ' && c <= '~') INTERRUPT_RCV_BUF[INTERRUPT_RCV_MSG_LEN] = c; // Standard chars
                INTERRUPT_RCV_MSG_LEN++;
			}
		}
		INTERRUPT_RCV_BUF[INTERRUPT_RCV_MSG_LEN] = 0;
	}
	INTERRUPT_RCV_FLAG = TRUE;
	delay_ms(1);
	enable_all_interrupts();
}

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

void RDA1_ISR(void) {
    default_rda_isr(COM_A);
	fprintf(COM_A, "\033[33m[COM_A] RCV: %s\n\r", INTERRUPT_RCV_BUF);
}

void RDA2_ISR(void) {
    default_rda_isr(COM_B); 
    fprintf(COM_A, "\033[33m[COM_B] RCV: %s\n\r", INTERRUPT_RCV_BUF);
}

void RDA3_ISR(void) {
    default_rda_isr(COM_C);
    cmd_flag = TRUE;
	fprintf(COM_C, "\033[33m[COM_C] RCV: %s\n\r", INTERRUPT_RCV_BUF);
}

void RDA4_ISR(void) {
    default_rda_isr(COM_D);
	fprintf(COM_D, "\033[33m[COM_D] RCV: %s\n\r", INTERRUPT_RCV_BUF);
}
