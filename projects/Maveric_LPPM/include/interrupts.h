#ifndef __INTERRUPTS_H__
#define __INTERRUPTS_H__

#include <stdint.h>

// LOWER PPM INTERRUPTS CONFIG

#define IRQ_NUM_PORTS       4

// Interrupt request manager

typedef struct {
    int1 started;
    volatile ringbuf_s irqbufs[IRQ_NUM_PORTS];
    volatile uint64_t ms;
} irqmgr_s;

// Initialize interrupt requests manager
void irqmgr_init(irqmgr_s* irqmgr);

// Clear all interrupt rcv bufs
void irqmgr_clear(irqmgr_s* irqmgr);

// Interrupt service routines

// Enable all interrupts
void isr_enable_all(void);

// Disable all interrupts
void isr_disable_all(void);

// Interrupt Service Routines 
void isr_uart1(void);
void isr_uart2(void);
void isr_uart3(void);
void isr_uart4(void);
void isr_timer1(void);

#endif
