#ifndef __LOGGER_H__
#define __LOGGER_H__

#include "cmdpkt.h"

#define LOGBUF_MAX_LEN          CMD_MAX_ARGS_LEN 
char LOGBUF[LOGBUF_MAX_LEN] = {0};    // Global log buffer

typedef enum {
    LL_TRACE = 0,
    LL_DEBUG = 1,
    LL_INFO = 2,
    LL_WARN = 3,
    LL_ERROR = 4,
    LL_NONE = 5
} log_level_e;

void logger_init();

void logger_clear();

// Logs whatever's in LOGBUF with the given color and a standard prefix
void log_flush(log_level_e lvl);

// Log flush helper methods
void log_trace();
void log_debug();
void log_info();
void log_warn();
void log_error();

#endif // !__LOGGER_H__
