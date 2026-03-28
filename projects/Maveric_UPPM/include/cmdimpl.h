#ifndef __CMDIMPL_H__
#define __CMDIMPL_H__

// Register command implementations in cmdmgr hash table
void cmdimpl_init();

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

void cmdimpl_ax100_get_power(cmdpkt_s* pkt);
void cmdimpl_ax100_set_power(cmdpkt_s* pkt);

#endif 
