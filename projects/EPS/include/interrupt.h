#ifndef __INTERRUPT__
#define __INTERRUPT__

#include <stdint.h>

// LOWER PPM INTERRUPTS CONFIG

#define IRQ_NUM_PORTS   4
#define I2C_NUM_PORTS   4

// Interrupt request manager

// UART Buffer Structure
typedef struct {
    int1 started;
    volatile ringbuf_s irqbufs[IRQ_NUM_PORTS];
    volatile uint64_t ms;
} irqmgr_s;

// I2C Buffer Structure
typedef struct {
    int1 started;
    volatile ringbuf_s i2cbufs[I2C_NUM_PORTS];
    volatile uint64_t ms;
} i2cmgr_s;

// Initialize uart interrupt requests manager
void irqmgr_init(irqmgr_s* irqmgr);

// Clear all uart interrupt rcv bufs
void irqmgr_clear(irqmgr_s* irqmgr);

// Initialize i2c interrupt requests manager
void i2cmgr_init(i2cmgr_s* i2cmgr);

// Clear all i2c interrupt rcv bufs
void i2cmgr_clear(i2cmgr_s* i2cmgr);

// Interrupt service routines
void isr_enable_all(void);
void isr_disable_all(void);
void RDA_isr(void);
void i2c2_isr(void);
void i2c3_isr(void);

#endif
