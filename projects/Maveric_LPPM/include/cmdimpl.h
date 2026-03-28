#ifndef __CMDIMPL_H__
#define __CMDIMPL_H__

#include "cmdpkt.h"

// Register command implementations in cmdmgr hash table
void cmdimpl_init(void);

// COMMAND IMPLEMENTATIONS 

void cmdimpl_ping(cmdpkt_s* pkt);

void cmdimpl_ppm_reset(cmdpkt_s* pkt);
void cmdimpl_ppm_get_time(cmdpkt_s* pkt);
void cmdimpl_ppm_set_time(cmdpkt_s* pkt);
void cmdimpl_ppm_delay(cmdpkt_s* pkt);
void cmdimpl_ppm_clear_bufs(cmdpkt_s* pkt);
void cmdimpl_ppm_get_scheds(cmdpkt_s* pkt);
void cmdimpl_ppm_sched_cmd_in(cmdpkt_s* pkt);
void cmdimpl_ppm_deschedule(cmdpkt_s* pkt);

void cmdimpl_tlm_get_data(cmdpkt_s* pkt);

void cmdimpl_flash_get_cfg(cmdpkt_s* pkt);
void cmdimpl_flash_set_cfg(cmdpkt_s* pkt);

void cmdimpl_mtq_heartbeat(cmdpkt_s* pkt);
void cmdimpl_mtq_reset(cmdpkt_s* pkt);
void cmdimpl_mtq_read_1(cmdpkt_s* pkt);
void cmdimpl_mtq_get_1(cmdpkt_s* pkt);
void cmdimpl_mtq_set_1(cmdpkt_s* pkt);
void cmdimpl_mtq_read_fast(cmdpkt_s* pkt);
void cmdimpl_mtq_get_fast(cmdpkt_s* pkt);
void cmdimpl_mtq_read_ctrl(cmdpkt_s* pkt);
void cmdimpl_mtq_get_ctrl(cmdpkt_s* pkt);
void cmdimpl_mtq_read_all(cmdpkt_s* pkt);
void cmdimpl_mtq_get_all(cmdpkt_s* pkt);

void cmdimpl_nvg_heartbeat(cmdpkt_s* pkt);
void cmdimpl_nvg_reset(cmdpkt_s* pkt);
void cmdimpl_nvg_power(cmdpkt_s* pkt);
void cmdimpl_nvg_send_cmd(cmdpkt_s* pkt);
void cmdimpl_nvg_get_1(cmdpkt_s* pkt);
void cmdimpl_nvg_set_1(cmdpkt_s* pkt);
void cmdimpl_nvg_start_all(cmdpkt_s* pkt);
void cmdimpl_nvg_stop_all(cmdpkt_s* pkt);

#endif 
