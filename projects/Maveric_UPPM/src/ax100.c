#include "ax100.h"
#include "uart.h"
#include "common.h"

// MAIN TRANSCEIVER INTERFACE

status_e ax100_init(ax100_s* a, uint8_t port) {
    a->is_init = TRUE;
    a->port = port;
    status_e s1 = ax100_set_power(a, TRUE);
    uint8_t power = 0;
    status_e s2 = ax100_get_power(a, &power);
    sprintf(LOGBUF, "ax100_init: port=%u power=%u", a->port, power); log_info();
    return (s1 == SUCCESS && s2 == SUCCESS && power == 1) ? SUCCESS : FAILURE;
}

status_e ax100_get_power(ax100_s* a, uint8_t* power) {
    if (!a->is_init) return FAILURE;
	*power = (uint8_t) input_state(AX100_PWR);
    return SUCCESS;
}


status_e ax100_set_power(ax100_s* a, uint8_t power) {
    if (!a->is_init) return FAILURE;
    uint8_t new_power = 0;
    if (power) {
        output_high(AX100_PWR);
    } else {
        output_low(AX100_PWR);
    }
    delay_ms(10);
    ax100_get_power(a, &new_power);
    return (new_power == power) ? SUCCESS : FAILURE;
}

status_e ax100_transmit_frame(ax100_s* a, uint8_t* frame, uint16_t len) {
    if (!a->is_init) return FAILURE;
    uart_write_buf(a->port, frame, len);
    uint16_t p = 0;
    uint16_t i;
    p += sprintf(LOGBUF, "ax100_tx_frame: len=%u [", frame); 
    for (i = 0; i < len; i++) 
        p += sprintf(&LOGBUF[p], " %02X", frame[i]); 
    p += sprintf(&LOGBUF[p], " ]"); 
    log_info();
    return SUCCESS;
}
