#include "interrupts.h"
#include "circbuf.h"
#include "uart.h"
#include <stdint.h>
#include <stdio.h>

#module

// Interrupt request manager 

void irqmgr_init(irqmgr_s* irqmgr) {
    irqmgr->started = FALSE;
    irqmgr->ms = 0;
    irqmgr_clear(irqmgr);
    isr_disable_all();
}

void irqmgr_clear(irqmgr_s* irqmgr) {
    uint8_t p;
    for (p = 0; p < NUM_PORTS; p++) {
        cb_clear(&irqmgr->irqbufs[p]);
    }
}

// Interrupt service routines

void isr_enable_all(void) {
	enable_interrupts(INT_RDA);
	enable_interrupts(INT_RDA2);
	enable_interrupts(INT_RDA3);
	enable_interrupts(INT_RDA4);
    enable_interrupts(INT_TIMER1);
}

void isr_disable_all(void) {
	disable_interrupts(INT_RDA);
	disable_interrupts(INT_RDA2);
	disable_interrupts(INT_RDA3);
	disable_interrupts(INT_RDA4);
    disable_interrupts(INT_TIMER1);
}

// Interrupt Service Routines 

extern irqmgr_s g_irqmgr;

#INT_RDA // Magnetorquer
void isr_uart1(void) {
    if (!g_irqmgr.started || !uart_byte_avail(COM_A)) return;   
    cb_push(&g_irqmgr.irqbufs[0], uart_read_byte(COM_A));
}

#INT_RDA2 // Naviguider 
void isr_uart2(void) {
    if (!g_irqmgr.started || !uart_byte_avail(COM_B)) return;
    cb_push(&g_irqmgr.irqbufs[1], uart_read_byte(COM_B));
}

#INT_RDA3 // Upper PPM 
void isr_uart3(void) {
    if (!g_irqmgr.started || !uart_byte_avail(COM_C)) return;   
    cb_push(&g_irqmgr.irqbufs[2], uart_read_byte(COM_C));
}

#INT_RDA4 // FTDI
void isr_uart4(void) {
    if (!g_irqmgr.started || !uart_byte_avail(COM_D)) return;
    cb_push(&g_irqmgr.irqbufs[3], uart_read_byte(COM_D));
}

#INT_TIMER1 // MS TIMER
void isr_timer1(void) {
    g_irqmgr.ms++;
}
