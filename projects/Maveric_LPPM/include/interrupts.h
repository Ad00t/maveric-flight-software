#ifndef __INTERRUPTS_H__
#define __INTERRUPTS_H__

#include <stdint.h>

// LOWER PPM INTERRUPTS CONFIG

#define NUM_PORTS           2

// Interrupt request manager

typedef struct {
    int1 started;
    volatile circbuf_s irqbufs[NUM_PORTS];
    // 0 = UART1    1 = UART4   
} irqmgr_s;

// Initialize interrupt requests manager
void irqmgr_init(irqmgr_s* irqmgr);

// Clear all interrupt rcv bufs
void irqmgr_clear(irqmgr_s* irqmgr);

// Interrupt service routines

extern irqmgr_s irqmgr;

// Enable all interrupts
void isr_enable_all(void);

// Disable all interrupts
void isr_disable_all(void);

// Interrupt Service Routines 
void isr_uart1(void);
void isr_uart2(void);
void isr_uart3(void);
void isr_uart4(void);

#endif
