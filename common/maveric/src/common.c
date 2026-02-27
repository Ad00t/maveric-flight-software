#include "common.h"
#include "crcnew.h"
#include "kiss.h"
#include "uart.h"
#include "systime.h"
#include "cmdmgr.h"
#include <stdint.h>
#include <string.h>

#module

uint8_t create_cmd_frame(uint8_t orgn, uint8_t dest, uint8_t echo, cmdpkt_type_e ptype, char* id, char* args, uint8_t* out) {
    uint8_t len = 0;
    memset(out, 0, CMD_MAX_LEN);
    kiss_prepend_header(out, &len);
    uint8_t id_len = strlen(id);
    uint8_t args_len = strlen(id);
    
    out[len++] = orgn;
    out[len++] = dest;
    out[len++] = echo;
    out[len++] = ptype;
    uint16_t buf_len;
    out[len++] = id_len;
    out[len++] = args_len;

    memcpy(&out[len], id, id_len);
    len += id_len;
    out[len++] = '\0';
   
    memcpy(&out[len], args, args_len);
    len += args_len;
    out[len++] = '\0';
    
    // CRC should only be computed on message, not framing or anything else
    uint16_t crc = compute_crc16(&out[KISS_HEADER_SIZE], len - KISS_HEADER_SIZE); 
    out[len++] = crc & 0xFF; 
    out[len++] = (crc >> 8) & 0xFF; 
    kiss_append_footer(out, &len);
    return len;
}

void log_flush(log_level_e lvl) {
    static char* ll_to_text[] = { "TRACE", "INFO", "WARN", "ERROR" };
    static char* ll_to_color[] = { KWHT, KCYN, KYEL, KRED };

    if (lvl >= LOG_LEVEL) {
        char logfmt[LOGBUF_MAX_LEN];
        sprintf(logfmt, "%s%Lu [%s] [%s] %s\n", ll_to_color[lvl], systime_epoch_ms(), ll_to_text[lvl], NODE_LBL, LOGBUF);
        #if NODE_ID == NODE_ID_LPPM
            uart_write_buf(FTDI_PORT, logfmt, strlen(logfmt));
        #elif NODE_ID == NODE_ID_UPPM
            // uart_write_buf(COM_C, logfmt, strlen(logfmt));
            uint8_t cmd[CMD_MAX_LEN] = {0};
            uint8_t len = create_cmd_frame(NODE_ID, 1, 0, REQUEST, "cmd_ftdi_log", logfmt, cmd);
            uart_write_buf(LPPM_PORT, cmd, len);
        #endif
    }
   
    memset(LOGBUF, 0, sizeof(LOGBUF));
}
