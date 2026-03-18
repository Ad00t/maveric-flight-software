#ifndef __CMDIMPL_H__
#define __CMDIMPL_H__

#include "cmdmgr.h"

// Register command implementations in cmdmgr hash table
void cmdimpl_init(void);

// COMMAND IMPLEMENTATIONS 

void cmdimpl_ppm_set_time(cmdpkt_s* pkt);
void cmdimpl_ppm_get_time(cmdpkt_s* pkt);
void cmdimpl_ppm_ping(cmdpkt_s* pkt);
void cmdimpl_ppm_delay(cmdpkt_s* pkt);
void cmdimpl_ppm_clear_bufs(cmdpkt_s* pkt);

void cmdimpl_tlm_get_data(cmdpkt_s* pkt);

void cmdimpl_mtq_get_conf(cmdpkt_s* pkt);
void cmdimpl_mtq_set_conf(cmdpkt_s* pkt);
void cmdimpl_mtq_get_datetime(cmdpkt_s* pkt);
void cmdimpl_mtq_set_datetime(cmdpkt_s* pkt);
void cmdimpl_mtq_get_tle(cmdpkt_s* pkt);
void cmdimpl_mtq_set_tle(cmdpkt_s* pkt);
void cmdimpl_mtq_get_paxs(cmdpkt_s* pkt);
void cmdimpl_mtq_set_paxs(cmdpkt_s* pkt);
void cmdimpl_mtq_get_mtquser(cmdpkt_s* pkt);
void cmdimpl_mtq_set_mtquser(cmdpkt_s* pkt);
void cmdimpl_mtq_reset(cmdpkt_s* pkt);
void cmdimpl_mtq_get_stat(cmdpkt_s* pkt);

#endif 
