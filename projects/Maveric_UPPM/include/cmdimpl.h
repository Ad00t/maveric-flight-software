#ifndef __CMDIMPL_H__
#define __CMDIMPL_H__

#include "mcppkt.h"

// Register command implementations in cmdmgr hash table
void cmdimpl_init();

// COMMAND IMPLEMENTATIONS 

void cmdimpl_ppm_reset(mcppkt_s* pkt);
void cmdimpl_ppm_get_time(mcppkt_s* pkt);
void cmdimpl_ppm_set_time(mcppkt_s* pkt);
void cmdimpl_ppm_delay(mcppkt_s* pkt);
void cmdimpl_ppm_clear_bufs(mcppkt_s* pkt);
void cmdimpl_ppm_get_sched(mcppkt_s* pkt);
void cmdimpl_ppm_get_all_scheds(mcppkt_s* pkt);
void cmdimpl_ppm_sched_cmd(mcppkt_s* pkt);
void cmdimpl_ppm_desched(mcppkt_s* pkt);
void cmdimpl_ppm_resched(mcppkt_s* pkt);
void cmdimpl_ppm_clear_sched(mcppkt_s* pkt);
void cmdimpl_ppm_update_sched(mcppkt_s* pkt);

void cmdimpl_tlm_get_data(mcppkt_s* pkt);

void cmdimpl_flash_read(mcppkt_s* pkt);
void cmdimpl_flash_write(mcppkt_s* pkt);
void cmdimpl_flash_erase(mcppkt_s* pkt);
void cmdimpl_flash_unprot(mcppkt_s* pkt);
void cmdimpl_flash_read_prot(mcppkt_s* pkt);

void cmdimpl_cfg_get(mcppkt_s* pkt);
void cmdimpl_cfg_set_ll(mcppkt_s* pkt);
void cmdimpl_cfg_set_ops(mcppkt_s* pkt);
void cmdimpl_cfg_set_gsdelay(mcppkt_s* pkt);
void cmdimpl_cfg_set_bcnper(mcppkt_s* pkt);
void cmdimpl_cfg_load_dfl(mcppkt_s* pkt);
void cmdimpl_cfg_load_flash(mcppkt_s* pkt);
void cmdimpl_cfg_flush(mcppkt_s* pkt);

void cmdimpl_ax100_get_power(mcppkt_s* pkt);
void cmdimpl_ax100_set_power(mcppkt_s* pkt);

void cmdimpl_rpi_disp_cap(mcppkt_s* pkt);
void cmdimpl_com_ping(mcppkt_s* pkt);
void cmdimpl_lcd_display(mcppkt_s* pkt);
void cmdimpl_cam_capture(mcppkt_s* pkt);
void cmdimpl_lcd_off(mcppkt_s* pkt);
void cmdimpl_cam_off(mcppkt_s* pkt);

#endif 
