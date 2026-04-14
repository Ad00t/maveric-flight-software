#include "logger.h"
#include "common.h"
#include "flashmgr.h"
#include "uart.h"
#include "systime.h"
#include "mcppkt.h"
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
        char logfmt[MCP_MAX_ARGS_LEN] = {0};
        char timebuf[32] = {0};
        sprintf(timebuf, "%Lu", systime_epoch_ms());
        uint16_t j = sprintf(logfmt, "%s%s [%s] [%s] ", ll_to_color[lvl], timebuf, ll_to_text[lvl], node_id_to_lbl[NODE]);
        uint16_t msg_cap = MCP_MAX_ARGS_LEN - j - 3;
        if (strlen(LOGBUF) >= msg_cap) {
            LOGBUF[msg_cap] = '\0';
            LOGBUF[msg_cap - 1] = '.';
            LOGBUF[msg_cap - 2] = '.';
            LOGBUF[msg_cap - 3] = '.';
        }
        sprintf(&logfmt[j], "%s\n", LOGBUF);
        mcp_dispatch(NODE, NODE_FTDI, 0, CMD, "ftdi_log", (char*) logfmt);
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
