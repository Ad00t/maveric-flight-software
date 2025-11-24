#ifndef __INTERRUPTS_H__
#define __INTERRUPTS_H__

#include "adcsmtq.h"
#include <stdint.h>

#define NUM_PORTS           4
#define MAX_PROC_FRAMES     2

// Importing global managers
extern irqmgr_s irqmgr;
extern cmdmgr_s cmdmgr;
extern adcsmtq_s tad102063;

// Interrupt request manager

typedef struct {
    int1 started;
    volatile circbuf_s irqbufs[NUM_PORTS];
    // 0 = UART1    1 = UART2    2 = UART3   3 = UART4 
} irqmgr_s;

// Initialize interrupt requests manager
void irqmgr_init(irqmgr_s* irqmgr);

// Clear all interrupt rcv bufs
void irqmgr_clear(irqmgr_s* irqmgr);

// Check interrupt buffers and advance port-specific FSM's and irq handling logic 
void irqmgr_handle_rcv(irqmgr_s* irqmgr);

// Interrupt service routines

// Enable all interrupts
void isr_enable_all(void);

// Disable all interrupts
void isr_disable_all(void);

// UART ISRs
void isr_rda1(void);
void isr_rda2(void);
void isr_rda3(void);
void isr_rda4(void);

#endif
