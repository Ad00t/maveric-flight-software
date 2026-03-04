#include "logger.h"
#include "common.h"
#include "uart.h"
#include "systime.h"
#include "cmdpkt.h"
#include "cmdmgr.h"
#include <stdint.h>
#include <string.h>

#module

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
        cmd_dispatch(NODE_ID, NODE_ID_LPPM, 0, REQUEST, "ppm_ftdi_log", logfmt);
#endif
    }
   
    memset(LOGBUF, 0, sizeof(LOGBUF));
}
