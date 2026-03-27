#include "logger.h"
#include "common.h"
#include "uart.h"
#include "systime.h"
#include "cmdpkt.h"
#include <stdint.h>
#include <string.h>

#module

void logger_init() {
    logger_clear();
}

void logger_clear() {
    memset(LOGBUF, 0, sizeof(LOGBUF));
}

void log_flush(log_level_e lvl) {
    static char* ll_to_text[] = { "TRACE", "DEBUG", "INFO", "WARN", "ERROR" };
    static char* ll_to_color[] = { KNRM, KWHT, KCYN, KYEL, KRED };

    if (lvl >= LOG_LEVEL) {
        char logfmt[LOGBUF_MAX_LEN] = {0};
        char cmd_id[] = "ftdi_log";
        sprintf(logfmt, "%s%Lu [%s] [%s] %s\n", ll_to_color[lvl], systime_epoch_ms(), ll_to_text[lvl], NODE_LBL, LOGBUF);
        //cmd_dispatch(NODE, NODE_FTDI, 0, REQ, "ftdi_log", logfmt);
        cmd_dispatch(NODE, NODE_FTDI, 0, REQ, cmd_id, logfmt);
// #if NODE == NODE_LPPM
//         uart_write_buf(FTDI_PORT, logfmt, strlen(logfmt));
// #elif NODE == NODE_UPPM
//         // uart_write_buf(COM_C, logfmt, strlen(logfmt));
//         cmd_dispatch(NODE, NODE_LPPM, 0, REQ, "ppm_ftdi_log", logfmt);
// #endif
    }

    logger_clear();
}

void log_trace() {
    log_flush(LL_TRACE);
}

void log_debug() {
    log_flush(LL_DEBUG);
}

void log_info() {
    log_flush(LL_INFO);
}

void log_warn() {
    log_flush(LL_WARN);    
}

void log_error() {
    log_flush(LL_ERROR);    
}
