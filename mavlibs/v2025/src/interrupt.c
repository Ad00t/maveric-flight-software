#include "interrupt.h"
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>

#module

// HELPERS

char fgetc_mux(uint8_t port) {
    switch (port) {
        case COM_A: return fgetc(COM_A);
        case COM_B: return fgetc(COM_B);
        case COM_C: return fgetc(COM_C);
        case COM_D: return fgetc(COM_D);
        default:    return 0;
    }
}

int1 kbhit_mux(uint8_t port) {
    switch (port) {
        case COM_A: return kbhit(COM_A);
        case COM_B: return kbhit(COM_B);
        case COM_C: return kbhit(COM_C);
        case COM_D: return kbhit(COM_D);
        default:    return 0;
    }
}

void default_rda_isr(uint8_t port) {
	disable_all_interrupts();
   	char c;
   	len = 0;
	if (kbhit_mux(port) && start_flag) {
		while (c != 13) { // Carriage return ends the message, use \n\r not \r\n
			c = fgetc_mux(port);
	     	if (c == 8 && len > 0) {  // Backspace
	            len--;
	     	} else { 
				if (len >= MAX_BUF_LEN) break;
			 	if (c >= ' ' && c <= '~') rcv_buf[len] = c; // Standard chars
                len++;
			}
		}
		rcv_buf[len] = 0;
	}
	rcv_flag = TRUE;
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
	fprintf(COM_A, "\033[33m[COM_A] RCV: %s\n\r", rcv_buf);
}

void RDA2_ISR(void) {
    default_rda_isr(COM_B); 
    fprintf(COM_A, "\033[33m[COM_B] RCV: %s\n\r", rcv_buf);
}

void RDA3_ISR(void) {
    default_rda_isr(COM_C);
    cmd_flag = TRUE;
	fprintf(COM_C, "\033[33m[COM_C] RCV: %s\n\r", rcv_buf);
}

void RDA4_ISR(void) {
    default_rda_isr(COM_D);
	fprintf(COM_D, "\033[33m[COM_D] RCV: %s\n\r", rcv_buf);
}
