#include "common.h"
#include "crcnew.h"
#include "uart.h"
#include "systime.h"
#include <stdint.h>
#include <string.h>

#module

uint8_t create_cmd(uint8_t src, uint8_t dest, uint8_t echo, char* id, char* args, uint8_t* out) {
    uint8_t len_id = strlen(id);
    uint8_t len_args = strlen(args);
    out[0] = 0xCD;    
    out[1] = src;
    out[2] = dest;
    out[3] = echo;
    out[4] = len_args;
    memcpy(&out[5], id, len_id);
    out[5 + len_id] = ' ';
    memcpy(&out[5 + len_id + 1], args, len_args);
    uint16_t crc = compute_crc16(out, 5 + len_id + 1 + len_args);   
    out[5 + len_id + 1 + len_args + 1] = crc & 0xFF; 
    out[5 + len_id + 1 + len_args + 2] = (crc >> 8) & 0xFF; 
    out[5 + len_id + 1 + len_args + 3] = '\0';
    return 5 + len_id + 1 + len_args + 2;
}

void log_flush(log_level_e lvl) {
    static char* ll_to_text[] = { "TRACE", "INFO", "WARN", "ERROR" };
    static char* ll_to_color[] = { KWHT, KCYN, KYEL, KRED };

    if (lvl >= LOG_LEVEL) {
        char logfmt[LOGBUF_MAX_LEN];
        sprintf(logfmt, "%s%Lu [%s] [%s] %s\n", ll_to_color[lvl], systime_epoch_ms(), ll_to_text[lvl], NODE_LBL, LOGBUF);
        #if NODE_ID == 1
            uart_write_buf(FTDI_PORT, logfmt, strlen(logfmt));
        #else
            uint8_t cmd[LOGBUF_MAX_LEN] = {0};
            uint8_t len = create_cmd(NODE_ID, 1, 0, "cmd_ftdi_log", logfmt, cmd);
            uart_write_buf(LPPM_PORT, cmd, len);
        #endif
    }
   
    memset(LOGBUF, 0, sizeof(LOGBUF));
}
