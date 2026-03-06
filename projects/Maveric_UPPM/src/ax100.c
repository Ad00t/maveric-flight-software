#include "ax100.h"
#include "crcnew.h"
#include "uart.h"
#include "ringbuf.h"
#include "cmdpkt.h"
#include "common.h"
#include "cmdmgr.h"

// MAIN TRANSCEIVER INTERFACE

void ax100_init(ax100_s* a, uint8_t port) {
    a->port = port;
    ax100_set_power(a, TRUE);
    sprintf(LOGBUF, "ax100_init: port=%u", a->port); log_flush(LL_INFO);
}

void ax100_set_power(ax100_s* a, int1 on) {
    if (!a->is_init) return;
    if (on) {
        output_high(AX100_PWR);
    } else {
        output_low(AX100_PWR);
    }
}

int1 ax100_is_on(ax100_s* a) {
    if (!a->is_init) return 0;
	return (int1) input_state(AX100_PWR);
}

// You should not ever need to use this function. Just use cmd_dispatch() with dest as GS
void ax100_transmit_frame(ax100_s* a, uint8_t* frame, uint16_t len) {
    if (!a->is_init) return;
    uart_write_buf(a->port, frame, len);
    uint16_t p = 0;
    uint16_t i;
    p += sprintf(LOGBUF, "ax100_transmit_frame: len=%u [", frame); 
    for (i = 0; i < len; i++) 
        p += sprintf(&LOGBUF[p], " %02X", frame[i]); 
    p += sprintf(&LOGBUF[p], " ]"); 
    log_flush(LL_TRACE);
}
