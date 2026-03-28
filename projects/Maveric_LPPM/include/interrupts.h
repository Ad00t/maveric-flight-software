#ifndef __INTERRUPTS_H__
#define __INTERRUPTS_H__

#include <stdint.h>

// LOWER PPM INTERRUPTS CONFIG

#define IRQ_NUM_PORTS       5
#define I2C_NUM_RX_BUFS     1
#define I2C_MAX_SIZE        128

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

// I2C Buffer Structure. Not really used in interrupts but whatever
typedef struct {
    int1 started;
    volatile ringbuf_s rxbufs[I2C_NUM_RX_BUFS];
} i2cmgr_s;

void i2cmgr_init(i2cmgr_s* i2cmgr);

void i2cmgr_clear(i2cmgr_s* i2cmgr);

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
void isr_i2c1(void);
void isr_timer1(void);

#endif
