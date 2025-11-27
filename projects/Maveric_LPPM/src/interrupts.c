#include "interrupts.h"
#include "circbuf.h"
#include "uart.h"
#include "cmd.h"
#include "adcsmtq.h"
#include <stdint.h>
#include <stdio.h>

#module

// Interrupt request manager 

void irq_mgr_init(irq_mgr_s* irq_mgr) {
    irq_mgr->started = FALSE;
    irq_mgr_clear(irq_mgr);
}

void irq_mgr_clear(irq_mgr_s* irq_mgr) {
    uint8_t p;
    for (p = 0; p < NUM_PORTS; p++) {
        cb_clear(&irq_mgr->irq_bufs[p]);
    }
}

void irq_mgr_handle_rcv(irq_mgr_s* irq_mgr, cmd_mgr_s* cmd_mgr, adcsmtq_s* tad102063) {
    isr_disable_all();
    
    adcsmtq_rcv_fsm(tad102063, &irq_mgr->irq_bufs[0]);
    cmd_mgr_rcv_fsm(cmd_mgr, &irq_mgr->irq_bufs[3], &cmd_mgr->rcvpkts[0]); // Handle FTDI commands on cmd rcvpkt 0

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
    if (!irq_mgr.started || !uart_byte_avail(COM_A)) return;   
    cb_push(&irq_mgr.irq_bufs[0], uart_read_byte(COM_A));
}

#INT_RDA2
void isr_rda2(void) {
    if (!irq_mgr.started || !uart_byte_avail(COM_B)) return;
    cb_push(&irq_mgr.irq_bufs[1], uart_read_byte(COM_B));
}

#INT_RDA3
void isr_rda3(void) {
    if (!irq_mgr.started || !uart_byte_avail(COM_C)) return;
    cb_push(&irq_mgr.irq_bufs[2], uart_read_byte(COM_C));
}

#INT_RDA4
void isr_rda4(void) {
    if (!irq_mgr.started || !uart_byte_avail(COM_D)) return;
    cb_push(&irq_mgr.irq_bufs[3], uart_read_byte(COM_D));
}

