#ifndef __LOGGER_H__
#define __LOGGER_H__

#define LOGBUF_MAX_LEN          256 
char LOGBUF[LOGBUF_MAX_LEN];    // Global log buffer

typedef enum {
    LL_TRACE = 0,
    LL_INFO = 1,
    LL_WARN = 2,
    LL_ERROR = 3,
    LL_NONE = 4
} log_level_e;

// Logs whatever's in LOGBUF with the given color and a standard prefix
void log_flush(log_level_e lvl);

#endif // !__LOGGER_H__
