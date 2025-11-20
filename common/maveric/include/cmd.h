#ifndef __CMD_H__
#define __CMD_H__

#include <stdint.h>

#define CMD_START_BYTE  0xCD
#define NUM_CMD_BUFS    3
#define MAX_CMD_ID_LEN  20

typedef struct {
    int1 busy;
    uint8_t orgn;
    uint8_t dest;
    uint8_t echo;
    char id[MAX_CMD_ID_LEN];
    uint8_t args_len;
    char args_str[MAX_BUF_LEN];
} cmdbuf_s;

// Initialize cmdbuf
void cmdbuf_init(cmdbuf_s* cmdbuf);

// Clear this cmdbuf
void cmdbuf_clear(cmdbuf_s* cmdbuf);

typedef struct {
    cmdbuf_s cmdbufs[NUM_CMD_BUFS];
} cmdmgr_s;

// Initialize cmdmgr
void cmdmgr_init(cmdmgr_s* cmdmgr);

// Clear all cmd bufs  
void cmdmgr_clear_all(cmdmgr_s* cmdmgr);

// Parse & handle cmd frame 
void cmdmgr_proc_frame(cmdmgr_s* cmdmgr, uint8_t* buf, uint8_t len);

#endif
