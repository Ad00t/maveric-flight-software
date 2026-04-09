#ifndef __CMDIMPL_H__
#define __CMDIMPL_H__

#include "mcppkt.h"

// Register command implementations in cmdmgr hash table
void cmdimpl_init(void);

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

void cmdimpl_gnc_get_mode(mcppkt_s* pkt);
void cmdimpl_gnc_set_mode(mcppkt_s* pkt);
void cmdimpl_gnc_get_cnts(mcppkt_s* pkt);

void cmdimpl_flash_read(mcppkt_s* pkt);
void cmdimpl_flash_write(mcppkt_s* pkt);
void cmdimpl_flash_erase(mcppkt_s* pkt);
void cmdimpl_flash_unprot(mcppkt_s* pkt);
void cmdimpl_flash_read_prot(mcppkt_s* pkt);

void cmdimpl_cfg_get(mcppkt_s* pkt);
void cmdimpl_cfg_set(mcppkt_s* pkt);
void cmdimpl_cfg_set_ll(mcppkt_s* pkt);
void cmdimpl_cfg_flush(mcppkt_s* pkt);

void cmdimpl_mtq_heartbeat(mcppkt_s* pkt);
void cmdimpl_mtq_reset(mcppkt_s* pkt);
void cmdimpl_mtq_read_1(mcppkt_s* pkt);
void cmdimpl_mtq_get_1(mcppkt_s* pkt);
void cmdimpl_mtq_set_1(mcppkt_s* pkt);
void cmdimpl_mtq_read_fast(mcppkt_s* pkt);
void cmdimpl_mtq_get_fast(mcppkt_s* pkt);
void cmdimpl_mtq_read_ctrl(mcppkt_s* pkt);
void cmdimpl_mtq_get_ctrl(mcppkt_s* pkt);
void cmdimpl_mtq_read_all(mcppkt_s* pkt);
void cmdimpl_mtq_get_all(mcppkt_s* pkt);

void cmdimpl_nvg_heartbeat(mcppkt_s* pkt);
void cmdimpl_nvg_reset(mcppkt_s* pkt);
void cmdimpl_nvg_power(mcppkt_s* pkt);
void cmdimpl_nvg_send_cmd(mcppkt_s* pkt);
void cmdimpl_nvg_get_1(mcppkt_s* pkt);
void cmdimpl_nvg_set_1(mcppkt_s* pkt);
void cmdimpl_nvg_start_all(mcppkt_s* pkt);
void cmdimpl_nvg_stop_all(mcppkt_s* pkt);

#endif 
