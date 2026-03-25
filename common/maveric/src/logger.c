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

extern flashmgr_s g_flashmgr;

void log_flush(log_level_e lvl) {
    static char* ll_to_text[] = { "TRACE", "DEBUG", "INFO", "WARN", "ERROR" };
    static char* ll_to_color[] = { KNRM, KWHT, KCYN, KYEL, KRED };
    static char* node_id_to_lbl[] = { "N/A", "LPPM", "EPS", "UPPM", "HN", "AB", "GS", "FTDI" };

    if (lvl >= g_flashmgr.config.log_level) {
        char logfmt[LOGBUF_MAX_LEN] = {0};
        sprintf(logfmt, "%s%Lu [%s] [%s] %s\n", ll_to_color[lvl], systime_epoch_ms(), ll_to_text[lvl], node_id_to_lbl[NODE], LOGBUF);
        cmd_dispatch(NODE, NODE_FTDI, 0, REQ, "ftdi_log", logfmt);
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
