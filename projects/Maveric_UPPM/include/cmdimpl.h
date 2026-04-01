#ifndef __CMDIMPL_H__
#define __CMDIMPL_H__

#include "mcppkt.h"

// Register command implementations in cmdmgr hash table
void cmdimpl_init();

// COMMAND IMPLEMENTATIONS 

void cmdimpl_com_ping(mcppkt_s* pkt);

void cmdimpl_ppm_reset(mcppkt_s* pkt);
void cmdimpl_ppm_get_time(mcppkt_s* pkt);
void cmdimpl_ppm_set_time(mcppkt_s* pkt);
void cmdimpl_ppm_delay(mcppkt_s* pkt);
void cmdimpl_ppm_clear_bufs(mcppkt_s* pkt);
void cmdimpl_ppm_get_scheds(mcppkt_s* pkt);
void cmdimpl_ppm_sched_cmd_in(mcppkt_s* pkt);
void cmdimpl_ppm_desched(mcppkt_s* pkt);
void cmdimpl_ppm_resched_in(mcppkt_s* pkt);
void cmdimpl_ppm_clear_sched(mcppkt_s* pkt);

void cmdimpl_tlm_get_data(mcppkt_s* pkt);

void cmdimpl_flash_get_cfg(mcppkt_s* pkt);
void cmdimpl_flash_set_cfg(mcppkt_s* pkt);

void cmdimpl_ax100_get_power(mcppkt_s* pkt);
void cmdimpl_ax100_set_power(mcppkt_s* pkt);

#endif 
