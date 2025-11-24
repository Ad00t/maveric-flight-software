#include "interrupts.h"
#include "circbuf.h"
#include "uart.h"
#include "cmd.h"
#include "adcsmtq.h"
#include <stdint.h>
#include <stdio.h>

#module

// Interrupt request manager 

void irqmgr_init(irqmgr_s* irqmgr) {
    irqmgr->started = FALSE;
    irqmgr_clear(irqmgr);
}

void irqmgr_clear(irqmgr_s* irqmgr) {
    size_t p;
    for (p = 0; p < NUM_PORTS; p++) {
        circbuf_clear(&irqmgr->irqbufs[p]);
    }
}

void irqmgr_handle_rcv(irqmgr_s* irqmgr) {
    isr_disable_all();
    
    adcsmtq_rcv_fsm(&tad102063, &irqmgr.irqbufs[0]);
    cmdmgr_rcv_fsm(&tad102063, &irqmgr.irqbufs[3], &cmdmgr.rcvpkts[0]); // Handle FTDI commands on cmd rcvpkt 0

    isr_enable_all();
}

// Interrupt service routines

void isr_enable_all(void) {
	enable_interrupts(INT_RDA);
	// enable_interrupts(INT_RDA2);
	// enable_interrupts(INT_RDA3);
	enable_interrupts(INT_RDA4);
}

void isr_disable_all(void) {
	disable_interrupts(INT_RDA);
	// disable_interrupts(INT_RDA2);
	// disable_interrupts(INT_RDA3);
	disable_interrupts(INT_RDA4);
}

// UART ISRs

#INT_RDA
void isr_rda1(void) {
    if (!irqmgr.started || !uart_byte_avail(COM_A)) return;   
    cb_push(&irqmgr.irqbufs[0], uart_read_byte(COM_A));
}

#INT_RDA2
void isr_rda2(void) {
    if (!irqmgr.started || !uart_byte_avail(COM_B)) return;
    cb_push(&irqmgr.irqbufs[1], uart_read_byte(COM_B));
}

#INT_RDA3
void isr_rda3(void) {
    if (!irqmgr.started || !uart_byte_avail(COM_C)) return;
    cb_push(&irqmgr.irqbufs[2], uart_read_byte(COM_C));
}

#INT_RDA4
void isr_rda4(void) {
    if (!irqmgr.started || !uart_byte_avail(COM_D)) return;
    cb_push(&irqmgr.irqbufs[3], uart_read_byte(COM_D));
}

