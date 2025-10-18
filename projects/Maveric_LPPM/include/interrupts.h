#ifndef __INTERRUPTS_H__
#define __INTERRUPTS_H__

#include <stdint.h>

#define MAX_BUF_LEN     256

int1 INTERRUPT_START_FLAG, INTERRUPT_RCV_FLAG, INTERRUPT_CMD_FLAG;
uint8_t INTERRUPT_RCV_BUF[MAX_BUF_LEN];
uint8_t INTERRUPT_RCV_MSG_LEN;
uint8_t INTERRUPT_RCV_PORT;

void enable_all_interrupts(void);
void disable_all_interrupts(void);
void log_interrupts_rcv(void);
void cleanup_interrupts_rcv(void);

#endif
