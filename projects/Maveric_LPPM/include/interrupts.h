#ifndef __INTERRUPTS_H__
#define __INTERRUPTS_H__

#include "adcsmtq.h"
#include <stdint.h>

#define MAX_BUF_LEN     256
#define BUFS_PER_PORT   3

// Interrupt request buffer

typedef struct {
    volatile int1 flag, busy;
    volatile uint8_t data[MAX_BUF_LEN];
    volatile uint8_t len;
    char type[16]; // "UART" or "I2C"
    uint8_t port;
} irqbuf_s; 

// Initialize an interrupt buf
void irqbuf_init(irqbuf_s* irqbuf);

// Log a single interrupt buf
void irqbuf_log(irqbuf_s* irqbuf);

// Clear a single interrupt buf
void irqbuf_clear(irqbuf_s* irqbuf);

// Interrupt request manager

typedef struct {
    int1 start_flag;
    irqbuf_s uart1[BUFS_PER_PORT];
    irqbuf_s uart2[BUFS_PER_PORT];
    irqbuf_s uart3[BUFS_PER_PORT];
    irqbuf_s uart4[BUFS_PER_PORT];
} irqmgr_s; 

// Initialize interrupt requests manager
void irqmgr_init(irqmgr_s* irqmgr);

// Log all interrupt rcv bufs
void irqmgr_log_bufs(irqmgr_s* irqmgr);

// Clear all interrupt rcv bufs
void irqmgr_clear_bufs(irqmgr_s* irqmgr);

// Handle all rcv'd interrupts
void irqmgr_handle_rcv(irqmgr_s* irqmgr, adcsmtq_s* a);

// Interrupt service routines

// Importing global just for ISRs
extern irqmgr_s irqmgr;

// Enable all interrupts
void isr_enable_all(void);

// Disable all interrupts
void isr_disable_all(void);

void isr_rda1(void);
void isr_rda2(void);
void isr_rda3(void);
void isr_rda4(void);

#endif
