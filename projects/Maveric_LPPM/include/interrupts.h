#ifndef __INTERRUPTS_H__
#define __INTERRUPTS_H__

#include <stdint.h>

#define MAX_BUF_LEN     256

int1 INTERRUPT_START_FLAG, INTERRUPT_RCV_FLAG, INTERRUPT_CMD_FLAG;
uint8_t INTERRUPT_RCV_BUF[MAX_BUF_LEN];
uint8_t INTERRUPT_RCV_MSG_LEN;

void enable_all_interrupts(void);
void disable_all_interrupts(void);

#INT_RDA
void RDA1_ISR();
#INT_RDA2
void RDA2_ISR();
#INT_RDA3
void RDA3_ISR();
#INT_RDA4
void RDA4_ISR();

#endif
