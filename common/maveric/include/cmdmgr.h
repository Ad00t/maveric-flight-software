#ifndef __CMDMGR_H__
#define __CMDMGR_H__

#include <stdint.h>
#include "kiss.h"
#include "ringbuf.h"
#include "hashtable.h"

#define CMD_MAX_LEN             255 - KISS_HEADER_SIZE - KISS_FOOTER_SIZE
#define CMD_HEADER_SIZE         6
#define CMD_FOOTER_SIZE         2
#define CMD_MAX_ID_LEN          20
#define CMD_MAX_ARGSSTR_LEN     CMD_MAX_LEN - CMD_MAX_ID_LEN - CMD_HEADER_SIZE - CMD_FOOTER_SIZE
#define CMD_NUM_BUFS            3

// Command packet & reader FSM

typedef enum {
    REQUEST = 0,
    RESPONSE = 1
} cmdpkt_type_e;

typedef struct {
    // Packet parsing metadata
    char buf[CMD_MAX_LEN];
    uint8_t buf_len;
    uint8_t n_skip;
    int1 busy;
    // Packet data
    uint8_t orgn;
    uint8_t dest;
    uint8_t echo;
    cmdpkt_type_e ptype;
    uint8_t id_len;
    uint8_t args_len;
    char* id;
    char* args;
    uint16_t crc;
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
void cmdmgr_parse_stream(cmdmgr_s* cmdmgr, ringbuf_s* rcvbuf, cmdpkt_s* pkt);

// Checks link layer headers, CRC, and forwards/runs command appropriately
void cmdmgr_process_cmd(cmdmgr_s* cmdmgr, cmdpkt_s* pkt);

#endif
