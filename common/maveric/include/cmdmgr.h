#ifndef __CMDMGR_H__
#define __CMDMGR_H__

#include <stdint.h>
#include "ringbuf.h"
#include "hashtable.h"

#define CMD_MAX_LEN             127
#define CMD_MAX_ID_LEN          20
#define CMD_MAX_ARGSSTR_LEN     CMD_MAX_LEN - CMD_MAX_ID_LEN - 7
#define CMD_START_BYTE          0xCD
#define CMD_NUM_BUFS            3

// Command packet & reader FSM

typedef enum {
    CMDPKT_FSM_HEAD = 0,
    CMDPKT_FSM_ORGN,
    CMDPKT_FSM_DEST,
    CMDPKT_FSM_ECHO,
    CMDPKT_FSM_ARGSLEN,
    CMDPKT_FSM_ID,
    CMDPKT_FSM_ARGSSTR,
    CMDPKT_FSM_CRC1,
    CMDPKT_FSM_CRC2,
    CMDPKT_FSM_DONE,
    CMDPKT_FSM_ERROR
} cmdpkt_fsm_e;

typedef struct {
    // Packet parsing metadata
    cmdpkt_fsm_e fsm;
    uint8_t i_id, i_args;
    // Packet data
    uint8_t orgn;
    uint8_t dest;
    uint8_t echo;
    char id[CMD_MAX_ID_LEN];
    uint8_t args_len;
    char args_str[CMD_MAX_ARGSSTR_LEN];
    uint16_t crc;
    uint16_t running_crc;
} cmdpkt_s;

// Initialize cmdpkt
void cmdpkt_init(cmdpkt_s* pkt);

// Clear this cmdpkt
void cmdpkt_clear(cmdpkt_s* pkt);

// Command manager

typedef void (*cmdimpl_f)(cmdpkt_s* pkt);

typedef struct {
    cmdpkt_s rcvpkts[CMD_NUM_BUFS];
    hashtable_s cmdimpls;
} cmdmgr_s;

// Initialize cmdmgr
void cmdmgr_init(cmdmgr_s* cmdmgr);

// Clear all rcv pkts 
void cmdmgr_clear(cmdmgr_s* cmdmgr);

// Parse command from bytestream/buffer 
void cmdmgr_parse_stream(cmdmgr_s* cmdmgr, ringbuf_s* rcvbuf, cmdpkt_s* rcvpkt);

// Checks link layer headers, CRC, and forwards/runs command appropriately
void cmdmgr_process_cmd(cmdmgr_s* cmdmgr, cmdpkt_s* pkt);

#endif
