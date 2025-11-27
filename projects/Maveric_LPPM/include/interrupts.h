#ifndef __INTERRUPTS_H__
#define __INTERRUPTS_H__

#include "adcsmtq.h"
#include "cmd.h"
#include <stdint.h>

#define NUM_PORTS           4

// Interrupt request manager

typedef struct {
    int1 started;
    volatile circbuf_s irq_bufs[NUM_PORTS];
    // 0 = UART1    1 = UART2    2 = UART3   3 = UART4 
} irq_mgr_s;

// Initialize interrupt requests manager
void irq_mgr_init(irq_mgr_s* irq_mgr);

// Clear all interrupt rcv bufs
void irq_mgr_clear(irq_mgr_s* irq_mgr);

// Check interrupt buffers and advance port-specific FSM's and irq handling logic 
void irq_mgr_handle_rcv(irq_mgr_s* irq_mgr, cmdmgr_s* cmdmgr, adcsmtq_s* tad102063);

// Interrupt service routines

extern irq_mgr_s irq_mgr;

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
