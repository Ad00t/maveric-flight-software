#include "interrupts.h"
#include "uart.h"
#include "cmd.h"
#include "adcsmtq.h"
#include <stdint.h>
#include <stdio.h>

#module

// Interrupt request buffer 

void irqbuf_init(irqbuf_s* irqbuf, char* type, uint8_t port) {
    memcpy(irqbuf->type, type, 16); 
    // irqbuf->type = type;
    irqbuf->port = port;
    irqbuf_clear(irqbuf);
}

void irqbuf_log(irqbuf_s* irqbuf) {
    fprintf(COM_D, "%s[LPPM] irqbuf port=%s%u flag=%u len=%u data=\"%s\" [ ", 
            KGRN, irqbuf->type, irqbuf->port, irqbuf->flag, irqbuf->len, irqbuf->data);
    size_t i;
    for (i = 0; i < irqbuf->len; i++) 
        fprintf(COM_D, "0x%02X ", irqbuf->data[i]);
    fprintf(COM_D, "]\r\n");
}

void irqbuf_clear(irqbuf_s* irqbuf) {
    irqbuf->flag = FALSE;
    irqbuf->busy = FALSE;
    memset(irqbuf->data, 0, sizeof(uint8_t) * MAX_BUF_LEN);
    irqbuf->len = 0;
}

// Interrupt request manager 

void irqmgr_init(irqmgr_s* irqmgr) {
    irqmgr->start_flag = FALSE;
    size_t b;
    for (b = 0; b < BUFS_PER_PORT; b++) {
        irqbuf_init(&irqmgr->uart1[b], "UART", 1);
        irqbuf_init(&irqmgr->uart2[b], "UART", 2);
        irqbuf_init(&irqmgr->uart3[b], "UART", 3);
        irqbuf_init(&irqmgr->uart4[b], "UART", 4);   
    }
}

void irqmgr_log_bufs(irqmgr_s* irqmgr) {
    size_t b;
    for (b = 0; b < BUFS_PER_PORT; b++) {
        irqbuf_log(&irqmgr->uart1[b]);
        irqbuf_log(&irqmgr->uart2[b]);
        irqbuf_log(&irqmgr->uart3[b]);
        irqbuf_log(&irqmgr->uart4[b]);
    }
}

void irqmgr_clear_bufs(irqmgr_s* irqmgr) {
    size_t b;
    for (b = 0; b < BUFS_PER_PORT; b++) {
        irqbuf_clear(&irqmgr->uart1[b]);
        irqbuf_clear(&irqmgr->uart2[b]);
        irqbuf_clear(&irqmgr->uart3[b]);
        irqbuf_clear(&irqmgr->uart4[b]);
    }
}

void irqmgr_handle_rcv(irqmgr_s* irqmgr, adcsmtq_s* a) {
    isr_disable_all();
    
    size_t b;
    for (b = 0; b < BUFS_PER_PORT; b++) {
        // ADCSMTQ
        if (irqmgr->uart1[b].flag) {
            irqbuf_s* rcv_buf = &irqmgr->uart1[b];
            switch (rcv_buf->data[0]) {
                case ADCSMTQ_HEAD_READ:
                    adcsmtq_read_complete(a, rcv_buf); break;
                case ADCSMTQ_HEAD_WRITE:
                    adcsmtq_write_complete(a, rcv_buf); break;
            }
        }

        if (irqmgr->uart2[b].flag) {
            // TODO: implement if needed
        }

        if (irqmgr->uart3[b].flag)  {
            // TODO: implement if needed
        }
        
        // Commands
        if (irqmgr->uart4[b].flag) {
            char* cmd = (char*) irqmgr->uart4[b].data;

            //Lower PPM routes
            int8 node = 1;
            int8 route1[7]={0,2,3,3,3,3,7};
            int8 route2[3]={0,3,2};

            //Upper PPM routes
            //int node = 3;
            //int route1[7]={1,2,0,4,5,6,1};
            //int route2[3]={2,1,0};

            //EPS BBQ routes
            //int node = 2;
            //int route1[7]={1,0,3,3,3,3,1};
            //int route1[3]={3,0,1};

            unsigned int16 crc_value;

            //unsigned char fix_cmd[27];
            delay_ms(10);
            crc_init(255);
            delay_ms(100);			
            crc_value = crc_calc8(cmd, irqmgr->uart4[b].len-1);
            //crc_value = crc_calc8(&fix_cmd,2,8);
            delay_ms(100);
            fprintf(COM_D,"%s[LPPM] solving cmd '%s' len=%u crc=%2u \r\n",
                    KCYN, cmd, irqmgr->uart4[b].len, crc_value);
            //Get the cmd components
            cmd_get_command(cmd,&orgn,&dest,&ech, cmd, prms);			
            fprintf(COM_D,"%s[LPPM] orig=%u dest=%u echo=%u\r\n", KCYN, orgn, dest, ech);
            //fprintf(COM_D,"[SYS] command: %s; params: %s\r\n",cmd,prms);
            if (dest == node) {
                fprintf(COM_D,"%s[LPPM] decoding cmd '%s' params='%s' route=%u\r\n", KCYN, cmd, prms, route1[dest-1]);
            } else {
                fprintf(COM_D,"%s[LPPM] forwarding cmd '%s' to route=%u\r\n", KCYN, cmd, route1[dest-1]);
            }
            //delay_ms(1000);

            if (dest == node) {
                fprintf(COM_D,"%s[LPPM] rcv cmd '%s' params='%s' route=%u\r\n", KCYN, cmd, prms, route1[dest-1]);
            } else {
                if (dest == route1[6]) {
                    fprintf(COM_D,"%s%s\r", KCYN, cmd);
                } else if ((dest == route1[2])||(dest == route1[3])||(dest == route1[4])||(dest == route1[5])) {
                    fprintf(COM_C,"%s%s\r", KCYN, cmd);
                }
            }
        }
    }

    irqmgr_clear_bufs(irqmgr);
    isr_enable_all();
}

// Interrupt service routines

void isr_enable_all(void) {
	enable_interrupts(INT_RDA);
	enable_interrupts(INT_RDA2);
	enable_interrupts(INT_RDA3);
	enable_interrupts(INT_RDA4);
}

void isr_disable_all(void) {
	disable_interrupts(INT_RDA);
	disable_interrupts(INT_RDA2);
	disable_interrupts(INT_RDA3);
	disable_interrupts(INT_RDA4);
}

// UART ISRs

#INT_RDA
void isr_rda1(void) {
    if (!irqmgr.start_flag || !uart_byte_avail(COM_A)) return;   
    size_t b = 0;
    irqbuf_s* rcv_buf = &irqmgr.uart1[b];
    while (rcv_buf->busy) {
        b++;
        if (b >= BUFS_PER_PORT) return;
        rcv_buf = &irqmgr.uart1[b];
    }
    rcv_buf->busy = TRUE;

    // TODO: perform max bounds checking? may be unnecessary due to small register sizes 

    size_t i;
    size_t max = 5;
    for (i = 0; i < max; i++) {
        rcv_buf->data[i] = uart_read_byte(COM_A);
        rcv_buf->len++;
        if (i == 2 && rcv_buf->data[0] == ADCSMTQ_HEAD_READ)
            max += 4*rcv_buf->data[i];
    }
    rcv_buf->data[rcv_buf->len] = 0;
    rcv_buf->flag = TRUE;
}

#INT_RDA2
void isr_rda2(void) {
    if (!irqmgr.start_flag || !uart_byte_avail(COM_B)) return;
    size_t b = 0;
    irqbuf_s* rcv_buf = &irqmgr.uart2[b];
    while (rcv_buf->busy) {
        b++;
        if (b >= BUFS_PER_PORT) return;
        rcv_buf = &irqmgr.uart2[b];
    }
    rcv_buf->busy = TRUE;
}

#INT_RDA3
void isr_rda3(void) {
    if (!irqmgr.start_flag || !uart_byte_avail(COM_C)) return;
    size_t b = 0;
    irqbuf_s* rcv_buf = &irqmgr.uart3[b];
    while (rcv_buf->busy) {
        b++;
        if (b >= BUFS_PER_PORT) return;
        rcv_buf = &irqmgr.uart3[b];
    }
    rcv_buf->busy = TRUE;
}

#INT_RDA4
void isr_rda4(void) {
    if (!irqmgr.start_flag || !uart_byte_avail(COM_D)) return;
    size_t b = 0;
    irqbuf_s* rcv_buf = &irqmgr.uart4[b];
    while (rcv_buf->busy) {
        b++;
        if (b >= BUFS_PER_PORT) return;
        rcv_buf = &irqmgr.uart4[b];
    }
    rcv_buf->busy = TRUE;

    char c = 0;
    while (c != 13) { // ASCII 13 = \r
        c = uart_read_byte(COM_D);
        if (c == 8 && rcv_buf->len > 0) {  // backspace
            rcv_buf->len--;
        } else { 
            if (rcv_buf->len >= MAX_BUF_LEN-1) break;
            if (c >= ' ' && c <= '~') 
                rcv_buf->data[rcv_buf->len] = c; // standard chars
            rcv_buf->len++;
        }
    }
    rcv_buf->data[rcv_buf->len] = 0;
	rcv_buf->flag = TRUE;
}

