#ifndef __COMMON_H__
#define __COMMON_H__

#include "cmdpkt.h"
#include <stdint.h>

#define STATUS_OK               1
#define STATUS_ERR              0

#define NODE_ID_LPPM            1
#define NODE_ID_UPPM            3

#define LOGBUF_MAX_LEN          256 
char LOGBUF[LOGBUF_MAX_LEN];    // Global log buffer

typedef enum {
    LL_TRACE = 0,
    LL_INFO = 1,
    LL_WARN = 2,
    LL_ERROR = 3,
    LL_NONE = 4
} log_level_e;

// Creates a packet KISS frame with the specified parameters and sends it to port
void send_cmd(uint8_t port, uint8_t orgn, uint8_t dest, uint8_t echo, cmdpkt_type_e ptype, char* id, char* args); 

// Logs whatever's in LOGBUF with the given color and a standard prefix
void log_flush(log_level_e lvl);

#endif // !__COMMON_H__
