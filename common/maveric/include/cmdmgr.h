#ifndef __CMDMGR_H__
#define __CMDMGR_H__

#include "ringbuf.h"
#include "hashtable.h"
#include "cmdpkt.h"

#define CMDMGR_NUM_BUFS     4

// Command implementation function
typedef void (*cmdimpl_f)(cmdpkt_s* pkt);

typedef struct {
    cmdpkt_s rcvpkts[CMDMGR_NUM_BUFS];
    hashtable_s cmdimpls;
} cmdmgr_s;

// Initialize cmdmgr
void cmdmgr_init(cmdmgr_s* cmdmgr);

// Clear all rcv pkts 
void cmdmgr_clear(cmdmgr_s* cmdmgr);

// Parse command frames from bytestream/buffer 
void cmdmgr_parse_stream(cmdmgr_s* cmdmgr, ringbuf_s* rcvbuf, cmdpkt_s* pkt, int1 csp);

// Checks link layer headers, CRC, and forwards/runs command appropriately
void cmdmgr_process_cmd(cmdmgr_s* cmdmgr, cmdpkt_s* pkt);

// Send a command packet along its appropriate route. Assumes pkt buf field is populated correctly.
void cmd_dispatch(cmdpkt_s* pkt);
// Create then send command packet
void cmd_dispatch(uint8_t orgn, uint8_t dest, uint8_t echo, cmdpkt_type_e ptype, char* id, char* args);

#endif
