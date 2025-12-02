#include "interrupts.h"
#include "circbuf.h"
#include "uart.h"
#include <stdint.h>
#include <stdio.h>

#module

// Interrupt request manager 

void irqmgr_init(irqmgr_s* irqmgr) {
    irqmgr->started = FALSE;
    irqmgr_clear(irqmgr);
    setup_spi(SPI_MASTER | SPI_H_TO_L | SPI_CLK_DIV_16)
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
	enable_interrupts(INT_RDA4);
    enable_interrupts(INT_SSPA);
}

void isr_disable_all(void) {
	disable_interrupts(INT_RDA);
	disable_interrupts(INT_RDA4);
	disable_interrupts(INT_SSPA);
}

// Interrupt Service Routines 

#INT_RDA
void isr_uart1(void) {
    if (!irqmgr.started || !uart_byte_avail(COM_A)) return;   
    cb_push(&irqmgr.irqbufs[0], uart_read_byte(COM_A));
}

#INT_RDA4
void isr_uart4(void) {
    if (!irqmgr.started || !uart_byte_avail(COM_D)) return;
    cb_push(&irqmgr.irqbufs[2], uart_read_byte(COM_D));
}

#INT_SSPA
void isr_spi1(void) {
    if (!irqmgr.started || !spi_data_is_in()) return;   
    cb_push(&irqmgr.irqbufs[1], spi_read(0));
}

