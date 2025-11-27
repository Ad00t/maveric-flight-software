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
    CMD_PKT_FSM_HEAD = 0,
    CMD_PKT_FSM_ORGN,
    CMD_PKT_FSM_DEST,
    CMD_PKT_FSM_ECHO,
    CMD_PKT_FSM_ARGSLEN,
    CMD_PKT_FSM_ID,
    CMD_PKT_FSM_ARGSSTR,
    CMD_PKT_FSM_CRC1,
    CMD_PKT_FSM_CRC2,
    CMD_PKT_FSM_DONE,
    CMD_PKT_FSM_ERROR
} cmd_pkt_fsm_e;

typedef struct {
    // Packet parsing metadata
    cmd_pkt_fsm_e fsm;
    uint8_t i_id, i_args;
    // Packet data
    uint8_t orgn;
    uint8_t dest;
    uint8_t echo;
    char id[MAX_CMD_ID_LEN];
    uint8_t args_len;
    char args_str[MAX_BUF_LEN];
    uint16_t crc;
    uint16_t running_crc;
} cmd_pkt_s;

typedef void (*cmd_func_f)(cmd_pkt_s* pkt);

// Initialize cmd_pkt
void cmd_pkt_init(cmd_pkt_s* pkt);

// Clear this cmd_pkt
void cmd_pkt_clear(cmd_pkt_s* pkt);

// Command manager

typedef struct {
    cmd_pkt_s rcvpkts[NUM_CMD_BUFS];
    hashtable_s cmd_funcs;
} cmd_mgr_s;

// Initialize cmd_mgr
void cmd_mgr_init(cmd_mgr_s* cmd_mgr);

// Clear all rcv pkts 
void cmd_mgr_clear(cmd_mgr_s* cmd_mgr);

// Check interrupt buffer to advance packet reader FSM 
void cmd_mgr_rcv_fsm(cmd_mgr_s* cmd_mgr, circbuf_s* irqbuf, cmd_pkt_s* rcvpkt);

// Checks link layer headers and forwards/runs command appropriately
void cmd_mgr_process_cmd(cmd_mgr_s* cmd_mgr, cmd_pkt_s* pkt);

#endif
