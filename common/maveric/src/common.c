#include "common.h"
#include "crcnew.h"
#include "kiss.h"
#include "uart.h"
#include "systime.h"
#include "cmdpkt.h"
#include <stdint.h>
#include <string.h>

#module

void send_cmd(uint8_t port, uint8_t orgn, uint8_t dest, uint8_t echo, cmdpkt_type_e ptype, char* id, char* args) {
    cmdpkt_s pkt;
    cmdpkt_create(&pkt, orgn, dest, echo, ptype, id, args);
    uint8_t frame[CMD_MAX_FRAME_SIZE] = {0};
    uint8_t len = 0;
    kiss_prepend_header(frame, &len);
    memcpy(&frame[len], pkt.buf, pkt.buf_len);
    len += pkt.buf_len;
    kiss_append_footer(frame, &len);
    uart_write_buf(port, frame, len);
}

void log_flush(log_level_e lvl) {
    static char* ll_to_text[] = { "TRACE", "INFO", "WARN", "ERROR" };
    static char* ll_to_color[] = { KWHT, KCYN, KYEL, KRED };

    if (lvl >= LOG_LEVEL) {
        char logfmt[LOGBUF_MAX_LEN] = {0};
        sprintf(logfmt, "%s%Lu [%s] [%s] %s\n", ll_to_color[lvl], systime_epoch_ms(), ll_to_text[lvl], NODE_LBL, LOGBUF);
        #if NODE_ID == NODE_ID_LPPM
            uart_write_buf(FTDI_PORT, logfmt, strlen(logfmt));
        #elif NODE_ID == NODE_ID_UPPM
            // uart_write_buf(COM_C, logfmt, strlen(logfmt));
            send_cmd(LPPM_PORT, NODE_ID, NODE_ID_LPPM, 0, REQUEST, "cmd_ftdi_log", logfmt);
        #endif
    }
   
    memset(LOGBUF, 0, sizeof(LOGBUF));
}
