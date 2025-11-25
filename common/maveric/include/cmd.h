#ifndef __CMD_H__
#define __CMD_H__

#include <stdint.h>
#include "circbuf.h"
#include "hashtable.h"

#define CMD_START_BYTE  0xCD
#define NUM_CMD_BUFS    1
#define MAX_CMD_ID_LEN  20

// Command packet & reader FSM

typedef enum {
    CMDPKT_FSM_HEAD = 0,
    CMDPKT_FSM_ORGN,
    CMDPKT_FSM_DEST,
    CMDPKT_FSM_ECHO,
    CMDPKT_FSM_ARGSLEN,
    CMDPKT_FSM_ID,
    CMDPKT_FSM_ARGSSTR,
    CMDPKT_FSM_CRC,
    CMDPKT_FSM_DONE,
    CMDPKT_FSM_ERROR
} cmdpkt_fsm_e;

typedef struct {
    // Interrupt parsing metadata
    cmdpkt_fsm_e fsm;
    uint8_t i_id, i_args;
    // Command data
    uint8_t orgn;
    uint8_t dest;
    uint8_t echo;
    char id[MAX_CMD_ID_LEN];
    uint8_t args_len;
    char args_str[MAX_BUF_LEN];
    uint8_t crc;
} cmdpkt_s;

typedef void (*cmdfunc_f)(cmdpkt_s* pkt);

// Initialize cmdpkt
void cmdpkt_init(cmdpkt_s* cmdpkt);

// Clear this cmdpkt
void cmdpkt_clear(cmdpkt_s* cmdpkt);

// Command manager

typedef struct {
    cmdpkt_s rcvpkts[NUM_CMD_BUFS];
    hashtable_s cmdfuncs;
} cmdmgr_s;

// Initialize cmdmgr
void cmdmgr_init(cmdmgr_s* cmdmgr);

// Clear all rcv pkts 
void cmdmgr_clear(cmdmgr_s* cmdmgr);

// Check interrupt buffer to advance packet reader FSM 
void cmdmgr_rcv_fsm(cmdmgr_s* cmdmgr, circbuf_s* irqbuf, cmdpkt_s* rcvpkt);

// Execute the actions associated with the cmd in buf 
void cmdmgr_run_cmd(cmdmgr_s* cmdmgr, cmdpkt_s* pkt);

#endif
