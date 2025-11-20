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
        circbuf_clear(&irqmgr->ports[p]);
    }
}

// Appropriately extract and call handler for frames on a given port
void process_frames_port(uint8_t port, void (*handle)(void* ctx, uint8_t* buf, uint8_t len)) {
    circbuf_s* b = &irqmgr->ports[port];
    size_t m;
    for (m = 0; m < MAX_PROC_FRAMES; m++) {
        uint8_t c;
        int1 found = FALSE;
        while (cb_peek(b, 0, &c)) {
            if (c == START_BYTE) { found = TRUE; break; }
            cb_pop(b, 1, NULL); // discard garbage
        }
        if (!found) return;

        if (cb_len(b) < HEADER_LEN)
            return;  // incomplete header

        uint8_t hdr[HEADER_LEN];
        for (int i = 0; i < HEADER_LEN; i++)
            cb_peek(b, i, &hdr[i]);

        uint16_t payload_len = get_packet_length(hdr);
        uint16_t total_len = HEADER_LEN + payload_len;

        if (total_len > MAX_FRAME_LEN) {
            cb_drop(b, 1);
            continue;
        }

        if (cb_len(b) < total_len)
            return; // incomplete frame

        uint8_t pkt[MAX_FRAME_LEN];
        for (int i = 0; i < total_len; i++)
            cb_get(b, &pkt[i]);

        switch (port) {
            case 0:
                handle(&tad102063, pkt, total_len); break;
            case 3:
                handle(&cmdmgr, pkt, total_len); break;
        }
    }
}



void irqmgr_process_frames(irqmgr_s* irqmgr) {
    isr_disable_all();
    
    process_frames_port(0, adcsmtq_proc_frame);
    process_frames_port(1, handle_

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
    circbuf_push(&irqmgr.ports[0], uart_read_byte(COM_A));
}

#INT_RDA2
void isr_rda2(void) {
    if (!irqmgr.started || !uart_byte_avail(COM_B)) return;
    circbuf_push(&irqmgr.ports[1], uart_read_byte(COM_B));
}

#INT_RDA3
void isr_rda3(void) {
    if (!irqmgr.started || !uart_byte_avail(COM_C)) return;
    circbuf_push(&irqmgr.ports[2], uart_read_byte(COM_C));
}

#INT_RDA4
void isr_rda4(void) {
    if (!irqmgr.started || !uart_byte_avail(COM_D)) return;
    circbuf_push(&irqmgr.ports[3], uart_read_byte(COM_D));
}

