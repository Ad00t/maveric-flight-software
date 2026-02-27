#ifndef __CMDMGR_H__
#define __CMDMGR_H__

#include "ringbuf.h"
#include "hashtable.h"
#include "cmdpkt.h"

// Command implementation function
typedef void (*cmdimpl_f)(cmdpkt_s* pkt);

typedef struct {
    cmdpkt_s rcvpkts[CMD_NUM_BUFS];
    hashtable_s cmdimpls;
} cmdmgr_s;

// Initialize cmdmgr
void cmdmgr_init(cmdmgr_s* cmdmgr);

// Clear all rcv pkts 
void cmdmgr_clear(cmdmgr_s* cmdmgr);

// Parse command frames from bytestream/buffer 
void cmdmgr_parse_stream(cmdmgr_s* cmdmgr, ringbuf_s* rcvbuf, cmdpkt_s* pkt);

// Checks link layer headers, CRC, and forwards/runs command appropriately
void cmdmgr_process_cmd(cmdmgr_s* cmdmgr, cmdpkt_s* pkt);

#endif
