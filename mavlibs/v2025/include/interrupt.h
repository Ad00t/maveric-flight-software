#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#define MAX_BUF_LEN     256

int1 start_flag, rcv_flag, cmd_flag;
char rcv_buf[MAX_BUF_LEN];
unsigned int max;
unsigned int len;

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
