#ifndef __CMD_H__
#define __CMD_H__

#include <stdint.h>

#define MAX_CMD_PARAMS  16
#define NUM_CMDS        1
#define NUM_CMD_BUFS    3

typedef struct {
    uint8_t id;
    uint8_t n_prms;
    char fmt[MAX_CMD_PARAMS]; // U = uint8_t, X = hex string, S = char string 
} cmddef_s;

typedef struct {
    int1 busy;
    uint8_t orgn;
    uint8_t dest;
    uint8_t echo;
    uint8_t id;
    char prmstr[MAX_BUF_LEN];
} cmdbuf_s;

typedef struct {
    cmdbuf_s active;
} cmdmgr_s;

// Initialize cmdmgr
void cmdmgr_init(cmdmgr_s* cmdmgr);

// Clear active cmd data  
void cmdmgr_clear(cmdmgr_s* cmdmgr);

// Parse & handle cmd from recv'd byte buffer 
void cmdmgr_handle_rcv(cmdmgr_s* cmdmgr, uint8_t* buf, uint8_t len);

// Table of available cmd definitions in ROM; id should match actual index in table
static const cmddef_s CMD_TABLE[] = {
    { 0, 0, NULL },
    { 1, 8, "UUUUUUUU" }
}; 

#endif
