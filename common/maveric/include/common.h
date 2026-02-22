#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdint.h>

#define STATUS_OK               1
#define STATUS_ERR              0

#define LOGBUF_MAX_LEN          256 
char LOGBUF[LOGBUF_MAX_LEN];    // Global log buffer

typedef enum {
    LL_TRACE = 0,
    LL_INFO = 1,
    LL_WARN = 2,
    LL_ERROR = 3,
    LL_NONE = 4
} log_level_e;

// Creates a command string with the given parameters
uint8_t create_cmd(uint8_t src, uint8_t dest, uint8_t echo, char* id, char* args, uint8_t* out);

// Logs whatever's in LOGBUF with the given color and a standard prefix
void log_flush(log_level_e lvl);

#endif // !__COMMON_H__
