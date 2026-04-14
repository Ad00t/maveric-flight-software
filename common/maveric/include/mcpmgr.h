#ifndef __MCPMGR_H__
#define __MCPMGR_H__

#include "ringbuf.h"
#include "hashtable.h"
#include "mcppkt.h"

#if NODE == NODE_LPPM
#define MCPMGR_NUM_BUFS     2
#elif NODE == NODE_UPPM
#define MCPMGR_NUM_BUFS     5
#endif

// Command implementation function
typedef void (*cmdimpl_f)(mcppkt_s* pkt);

typedef struct {
    mcppkt_s rcvpkts[MCPMGR_NUM_BUFS];
    hashtable_s cmdimpls;
    int1 is_init;
} mcpmgr_s;

// Initialize mcpmgr
void mcpmgr_init(mcpmgr_s* mcpmgr);

// Clear all rcv pkts 
void mcpmgr_clear(mcpmgr_s* mcpmgr);

// Parse command frames from bytestream/buffer 
void mcpmgr_parse_stream(mcpmgr_s* mcpmgr, ringbuf_s* rcvbuf, mcppkt_s* pkt, int1 csp);

// Checks link layer headers, CRC, and forwards/runs command appropriately
void mcpmgr_process_pkt(mcpmgr_s* mcpmgr, mcppkt_s* pkt);
// Easy use wrapper functions
void mcp_process(mcpmgr_s* mcpmgr, uint8_t orgn, uint8_t dest, uint8_t echo, mcppkt_type_e ptype, char* id, uint8_t* args, uint8_t args_len);
void mcp_process(mcpmgr_s* mcpmgr, uint8_t orgn, uint8_t dest, uint8_t echo, mcppkt_type_e ptype, char* id, char* args);

#endif
