#ifndef __CMDIMPL_H__
#define __CMDIMPL_H__

// Register command implementations in cmdmgr hash table
void cmdimpl_init();

// COMMAND IMPLEMENTATIONS 

void cmdimpl_ppm_set_time(cmdpkt_s* pkt);
void cmdimpl_ppm_get_time(cmdpkt_s* pkt);
void cmdimpl_ppm_ping(cmdpkt_s* pkt);
void cmdimpl_tlm_update(cmdpkt_s* pkt);

#endif 
