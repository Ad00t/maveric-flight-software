#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

int1 start_flag, cmd_flag;
char rcv_cmd[256];
unsigned int max;
unsigned int len;

void enable_all_interrupts(void);
void disable_all_interrupts(void);

#INT_RDA
void RDA_ISR();
#INT_RDA2
void RDA2_ISR();
#INT_RDA3
void RDA3_ISR();
#INT_RDA4
void RDA4_ISR();

#endif
