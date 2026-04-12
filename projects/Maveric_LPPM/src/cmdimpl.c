// LOWER PPM COMMAND IMPLEMENTATIONS

#include "cmdimpl.h"
#include "mcpmgr.h"
#include "systime.h"
#include "hashtable.h"
#include "interrupts.h"
#include "scheduler.h"
#include "flashmgr.h"
#include "adcsmtq.h"
#include "m41t81s.h"
#include "adis16260.h"
#include "naviguider.h"
#include "common.h"
#include "globals.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#module

void cmdimpl_init(void) {
    hashtable_s* ht = &g_mcpmgr.cmdimpls;

    ht_set(ht, "com_ping", (cmdimpl_f) cmdimpl_com_ping);
    
    ht_set(ht, "ppm_reset", (cmdimpl_f) cmdimpl_ppm_reset);
    ht_set(ht, "ppm_get_time", (cmdimpl_f) cmdimpl_ppm_get_time);
    ht_set(ht, "ppm_set_time", (cmdimpl_f) cmdimpl_ppm_set_time);
    ht_set(ht, "ppm_delay", (cmdimpl_f) cmdimpl_ppm_delay);
    ht_set(ht, "ppm_clear_bufs", (cmdimpl_f) cmdimpl_ppm_clear_bufs);

    ht_set(ht, "ppm_get_sched", (cmdimpl_f) cmdimpl_ppm_get_sched);
    ht_set(ht, "ppm_get_all_scheds", (cmdimpl_f) cmdimpl_ppm_get_all_scheds);
    ht_set(ht, "ppm_sched_cmd_in", (cmdimpl_f) cmdimpl_ppm_sched_cmd_in);
    ht_set(ht, "ppm_desched", (cmdimpl_f) cmdimpl_ppm_desched);
    ht_set(ht, "ppm_resched_in", (cmdimpl_f) cmdimpl_ppm_resched_in);
    ht_set(ht, "ppm_clear_sched", (cmdimpl_f) cmdimpl_ppm_clear_sched);
    ht_set(ht, "ppm_update_sched", (cmdimpl_f) cmdimpl_ppm_update_sched);

    ht_set(ht, "tlm_get_data", (cmdimpl_f) cmdimpl_tlm_get_data);
    
    ht_set(ht, "gnc_get_mode", (cmdimpl_f) cmdimpl_gnc_get_mode);
    ht_set(ht, "gnc_set_mode", (cmdimpl_f) cmdimpl_gnc_set_mode);
    ht_set(ht, "gnc_get_cnts", (cmdimpl_f) cmdimpl_gnc_get_cnts);
    
    ht_set(ht, "flash_read", (cmdimpl_f) cmdimpl_flash_read);
    ht_set(ht, "flash_write", (cmdimpl_f) cmdimpl_flash_write);
    ht_set(ht, "flash_erase", (cmdimpl_f) cmdimpl_flash_erase);
    ht_set(ht, "flash_unprot", (cmdimpl_f) cmdimpl_flash_unprot);
    ht_set(ht, "flash_read_prot", (cmdimpl_f) cmdimpl_flash_read_prot);
    
    ht_set(ht, "cfg_get", (cmdimpl_f) cmdimpl_cfg_get);
    ht_set(ht, "cfg_set", (cmdimpl_f) cmdimpl_cfg_set);
    ht_set(ht, "cfg_set_ll", (cmdimpl_f) cmdimpl_cfg_set_ll);
    ht_set(ht, "cfg_flush", (cmdimpl_f) cmdimpl_cfg_flush);

    ht_set(ht, "mtq_heartbeat", (cmdimpl_f) cmdimpl_mtq_heartbeat);
    ht_set(ht, "mtq_reset", (cmdimpl_f) cmdimpl_mtq_reset);
    ht_set(ht, "mtq_read_1", (cmdimpl_f) cmdimpl_mtq_read_1);
    ht_set(ht, "mtq_get_1", (cmdimpl_f) cmdimpl_mtq_get_1);
    ht_set(ht, "mtq_set_1", (cmdimpl_f) cmdimpl_mtq_set_1);
    ht_set(ht, "mtq_read_fast", (cmdimpl_f) cmdimpl_mtq_read_fast);
    ht_set(ht, "mtq_get_fast", (cmdimpl_f) cmdimpl_mtq_get_fast);
    ht_set(ht, "mtq_read_ctrl", (cmdimpl_f) cmdimpl_mtq_read_ctrl);
    ht_set(ht, "mtq_get_ctrl", (cmdimpl_f) cmdimpl_mtq_get_ctrl);
    ht_set(ht, "mtq_read_all", (cmdimpl_f) cmdimpl_mtq_read_all);
    ht_set(ht, "mtq_get_all", (cmdimpl_f) cmdimpl_mtq_get_all);
    
    ht_set(ht, "nvg_heartbeat", (cmdimpl_f) cmdimpl_nvg_heartbeat);
    ht_set(ht, "nvg_reset", (cmdimpl_f) cmdimpl_nvg_reset);
    ht_set(ht, "nvg_power", (cmdimpl_f) cmdimpl_nvg_power);
    ht_set(ht, "nvg_get_1", (cmdimpl_f) cmdimpl_nvg_get_1);
    ht_set(ht, "nvg_set_1", (cmdimpl_f) cmdimpl_nvg_set_1);
    ht_set(ht, "nvg_start_all", (cmdimpl_f) cmdimpl_nvg_start_all);
    ht_set(ht, "nvg_stop_all", (cmdimpl_f) cmdimpl_nvg_stop_all);
}

// COMMAND IMPLEMENTATIONS

void cmdimpl_com_ping(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            sprintf(LOGBUF, "cmdimpl_com_ping '%s'", pkt->args); log_info();
            mcp_respond(pkt, RES, "pong");
            break;
        }
    }
}

void cmdimpl_ppm_reset(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            sprintf(LOGBUF, "cmdimpl_ppm_reset"); log_info();
            g_superloop_running = FALSE;
            char res[8] = {0};
            sprintf(res, "%u", SUCCESS);
            mcp_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_ppm_get_time(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            sprintf(LOGBUF, "cmdimpl_ppm_get_time CMD"); log_info();
            char res[32] = {0};  
            systime_str(res);
            mcp_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_ppm_set_time(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            char* p = pkt->args;
            rtc_time_t time;
            time.tm_wday = strtoul(p, &p, 10); // Other options: strtok(), strtod(), strotol()
            time.tm_mon = strtoul(p, &p, 10); 
            time.tm_mday = strtoul(p, &p, 10);
            time.tm_year = strtoul(p, &p, 10);
            time.tm_hour = strtoul(p, &p, 10);
            time.tm_min = strtoul(p, &p, 10);
            time.tm_sec = strtoul(p, &p, 10);
            
            uint64_t oldtime = systime_epoch_ms();
            ertc_set_time(&g_ertc, time);
            systime_sync();
            scheduler_refresh_all(&g_scheduler, oldtime);

            mtq_set_datetime(&g_mtq, &time);
            mcp_dispatch(NODE, NODE_UPPM, 0, CMD, "ppm_set_time", pkt->args);

            rtc_time_t rtc;
            systime_rtc(&rtc);
            sprintf(LOGBUF, "cmdimpl_ppm_set_time '%s' [ %02u, %02u/%02u/20%02u %02u:%02u:%02u ]", pkt->args,
                    rtc.tm_wday, rtc.tm_mon, rtc.tm_mday, rtc.tm_year, 
                    rtc.tm_hour, rtc.tm_min, rtc.tm_sec); log_info();
            char res[8] = {0};
            sprintf(res, "%u", SUCCESS);
            mcp_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_ppm_delay(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            char* p = pkt->args;
            uint32_t delay = strtoul(p, &p, 10);
            sprintf(LOGBUF, "cmdimpl_ppm_delay d=%u", delay); log_info();
            char res[8] = {0};
            if (delay < 60000) {
                sprintf(res, "%u", SUCCESS);
                mcp_respond(pkt, RES, res);
                delay_ms(delay);
            } else {
                sprintf(res, "%u", FAILURE);
                mcp_respond(pkt, RES, res);
            }
            break;
        }
    }
}

void cmdimpl_ppm_clear_bufs(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            char* p = pkt->args;
            uint8_t mode = strtoul(p, &p, 10);
            sprintf(LOGBUF, "cmdimpl_ppm_clear_bufs m=%u", mode); log_info();
            char res[8] = {0};
            switch (mode) {
                case 0: 
                    irqmgr_clear(&g_irqmgr);
                    sprintf(res, "%u", SUCCESS);
                    mcp_respond(pkt, RES, res);
                    break;
                case 1:
                    mcpmgr_clear(&g_mcpmgr);
                    sprintf(res, "%u", SUCCESS);
                    mcp_respond(pkt, RES, res);
                    break;
                default:
                    sprintf(res, "%u", FAILURE);
                    mcp_respond(pkt, RES, res);
                    break;
            }
            break;
        }
    }
}

void cmdimpl_ppm_get_sched(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            char* p = pkt->args;
            uint8_t sched_id = strtoul(p, &p, 10);
            sprintf(LOGBUF, "cmdimpl_ppm_get_sched id=%u", sched_id); log_info();
            char res[MCP_MAX_ARGS_LEN] = {0};
            schedtask_s* st = g_scheduler.id_map[sched_id]; 
            if (st->id == sched_id) {
                sprintf(res, "%u %u %u %u %u %u %u", 
                        SUCCESS, st->id, st->active, st->type, st->period_ms, st->remaining_reps, st->next_release);
            } else {
                sprintf(res, "%u %u", FAILURE, sched_id);
            }
            mcp_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_ppm_get_all_scheds(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            sprintf(LOGBUF, "cmdimpl_ppm_get_all_scheds"); log_info();
            char res[MCP_MAX_ARGS_LEN] = {0}; 
            uint8_t i;
            uint16_t j = 0;
            for (i = 0; i < SCHEDULER_MAX_TASKS; i++) {
                schedtask_s* st = &g_scheduler.tasks[i];
                j += sprintf(&res[j], "%u:%u,%u,%u ", i, st->id, st->active, st->type);
            }
            mcp_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_ppm_sched_cmd_in(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            char* p = pkt->args;
            uint8_t sched_id = strtoul(p, &p, 10);
            uint32_t start_delay_ms = strtoul(p, &p, 10);
            uint32_t period_ms = strtoul(p, &p, 10);
            uint16_t reps = strtoul(p, &p, 10);
            uint8_t orgn = strtoul(p, &p, 10);
            uint8_t dest = strtoul(p, &p, 10);
            uint8_t echo = strtoul(p, &p, 10);
            uint8_t ptype = strtoul(p, &p, 10);
            char* mcp_id = strtok(p+1, " ");
            char* args = strtok(0, "");
            sprintf(LOGBUF, "cmdimpl_ppm_sched_cmd_in sid=%u del=%u per=%u rep=%u o=%u d=%u e=%u p=%u id='%s' args='%s'",
                    sched_id, start_delay_ms, period_ms, reps, orgn, dest, echo, ptype, mcp_id, args); log_info();
            mcppkt_s schedcmd;
            mcppkt_create(&schedcmd, orgn, dest, echo, ptype, mcp_id, args);
            status_e s = scheduler_schedule_cmd_in(&g_scheduler, sched_id, &schedcmd, start_delay_ms, period_ms, reps);
            char res[32] = {0};
            uint8_t j = sprintf(res, "%u %u", s, sched_id);
            if (s == SUCCESS) {
                j += sprintf(&res[j], " %u", g_scheduler.id_map[sched_id]->next_release);
            }
            mcp_respond(pkt, RES, res); 
            break;
        }
    }
}

void cmdimpl_ppm_desched(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            char* p = pkt->args;
            uint8_t sched_id = strtoul(p, &p, 10);
            sprintf(LOGBUF, "cmdimpl_ppm_desched id=%u", sched_id); log_info();
            status_e s = scheduler_deschedule(&g_scheduler, sched_id);
            char res[8] = {0};
            sprintf(res, "%u %u", s, sched_id);
            mcp_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_ppm_resched_in(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            char* p = pkt->args;
            uint8_t sched_id = strtoul(p, &p, 10);
            uint32_t start_delay_ms = strtoul(p, &p, 10);
            sprintf(LOGBUF, "cmdimpl_ppm_resched_in id=%u d=%u", sched_id, start_delay_ms); log_info();
            status_e s = scheduler_reschedule_in(&g_scheduler, sched_id, start_delay_ms);
            char res[32] = {0};
            uint8_t j = sprintf(res, "%u %u", s, sched_id);
            if (s == SUCCESS) {
                j += sprintf(&res[j], " %u", g_scheduler.id_map[sched_id]->next_release);
            }
            mcp_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_ppm_clear_sched(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            char* p = pkt->args;
            uint8_t sched_id = strtoul(p, &p, 10);
            sprintf(LOGBUF, "cmdimpl_ppm_clear_sched id=%u", sched_id); log_info();
            status_e s = scheduler_clear_task(&g_scheduler, sched_id);
            char res[8] = {0};
            sprintf(res, "%u %u", s, sched_id);
            mcp_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_ppm_update_sched(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            char* p = pkt->args;
            uint8_t sched_id = strtoul(p, &p, 10);
            uint32_t period_ms = strtoul(p, &p, 10);
            uint16_t remaining_reps = strtoul(p, &p, 10);
            sprintf(LOGBUF, "cmdimpl_ppm_update_sched id=%u p=%u r=%u", sched_id, period_ms, remaining_reps); log_info();
            char res[MCP_MAX_ARGS_LEN] = {0};
            schedtask_s* st = g_scheduler.id_map[sched_id]; 
            if (st->id == sched_id) {
                st->period_ms = period_ms;
                st->remaining_reps = remaining_reps;
                sprintf(res, "%u %u %Lu %u", SUCCESS, sched_id, period_ms, remaining_reps);
            } else {
                sprintf(res, "%u %u %Lu %u", FAILURE, sched_id, period_ms, remaining_reps);
            }
            mcp_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_gnc_get_mode(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            sprintf(LOGBUF, "cmdimpl_gnc_get_mode mode=%u", g_gnc.gnc_mode); log_info();
            char res[8] = {0};
            sprintf(res, "%u", g_gnc.gnc_mode);
            mcp_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_gnc_set_mode(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            char* p = pkt->args;
            uint8_t mode = strtoul(p, &p, 10);
            sprintf(LOGBUF, "cmdimpl_gnc_set_mode mode=%u", mode); log_info();
            g_gnc.gnc_mode = mode;
            char res[8] = {0};
            sprintf(res, "%u", (g_gnc.gnc_mode == mode) ? SUCCESS : FAILURE);
            mcp_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_gnc_get_cnts(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            sprintf(LOGBUF, "cmdimpl_gnc_get_cnts"); log_info();
            char res[32] = {0};
            sprintf(res, "%u %u %u", g_gnc.unexpected_safe_count, g_gnc.unexpected_detumble_count, g_gnc.sunspin_count);
            mcp_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_tlm_get_data(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            sprintf(LOGBUF, "cmdimpl_tlm_get_data CMD"); log_info();
            char res[MCP_MAX_ARGS_LEN] = {0};

            uint32_t mtq_stat = 0;
            mtq_get_data(&g_mtq, MTQ_STAT, &mtq_stat);

            config_s* cfg = &g_flashmgr.config;
            uint8_t i;
            uint16_t j = sprintf(res, "%u %u %u %u %u %u %u %u %u %Lu %u %u ", 
                                 g_flashmgr.rbt_cnt, g_rbt_cause, g_ertc.heartbeat, g_mtq.heartbeat, g_nvg.heartbeat,
                                 g_gnc.gnc_mode, g_gnc.unexpected_safe_count, g_gnc.unexpected_detumble_count, g_gnc.sunspin_count,
                                 mtq_stat, cfg->gyro_rate_src, cfg->mag_src);

            float gyro_rate[4] = {0}; // Make sure these buffers have enough space for the extra value(s) the nvg reports
            switch (cfg->gyro_rate_src) {
                case DATASRC_MTQ:
                    mtq_get_data(&g_mtq, MTQ_RATE, gyro_rate); // rad/s
                    break;
                case DATASRC_NVG:
                    nvg_get_sensor_data(&g_nvg, NVG_GYROSCOPE_CAL, gyro_rate); // rad/s
                    break;
            }
            for (i = 0; i < 3; i++) {
                j += sprintf(&res[j], " ");
                j += ftoa(gyro_rate[i], &res[j], 3, 'f');
            }

            float mag[4] = {0};
            switch (cfg->mag_src) {
                case DATASRC_MTQ:
                    mtq_get_data(&g_mtq, MTQ_MAG, mag); // nT
                    break;
                case DATASRC_NVG:
                    nvg_get_sensor_data(&g_nvg, NVG_MAGNETOMETER_CAL, mag); // uT
                    break;
            }
            for (i = 0; i < 3; i++) {
                j += sprintf(&res[j], " ");
                j += ftoa(mag[i], &res[j], 3, 'f');
            }

            float mtq_dipole[3] = {0};
            mtq_get_data(&g_mtq, MTQ_MTQ, mtq_dipole);
            for (i = 0; i < 3; i++) {
                j += sprintf(&res[j], " ");
                j += ftoa(mtq_dipole[i], &res[j], 3, 'f');
            }

            uint16_t mtq_adcs_tmp[2] = {0};
            mtq_get_data(&g_mtq, MTQ_ADCS_TMP, mtq_adcs_tmp);
            float adcs_temp = (float) mtq_adcs_tmp[0] * 150 / 32768;
            j += ftoa(adcs_temp, &res[j], 3, 'f');

            mcp_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_flash_read(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            char* p = pkt->args;
            uint32_t addr = strtoul(p, &p, 16);
            uint8_t len = minu8(strtoul(p, &p, 10), 150);
            sprintf(LOGBUF, "cmdimpl_flash_read addr=0x%02X%02X%02X len=%u", 
                    make8(addr, 2), make8(addr, 1), make8(addr, 0), len); log_info();
            char res[MCP_MAX_ARGS_LEN] = {0};
            uint8_t j = sprintf(res, "%u 0x%02X%02X%02X %u ", 
                                SUCCESS, make8(addr, 2), make8(addr, 1), make8(addr, 0), len);
            flashRead(addr, len, &res[j]); 
            mcp_respond(pkt, RES, res, j+len);
            break;
        }
    }
}

void cmdimpl_flash_write(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            char* p = pkt->args;
            uint32_t addr = strtoul(p, &p, 16);
            uint8_t len = minu8(strtoul(p, &p, 10), 150);
            sprintf(LOGBUF, "cmdimpl_flash_write addr=0x%02X%02X%02X len=%u", 
                    make8(addr, 2), make8(addr, 1), make8(addr, 0), len); log_info();
            status_e s = flashWriteSafe(addr, len, p+1, addr, addr + FLASH_BLOCK_SIZE - 1); 
            char res[32] = {0};
            sprintf(res, "%u 0x%02X%02X%02X %u", 
                    s, make8(addr, 2), make8(addr, 1), make8(addr, 0), len);
            mcp_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_flash_erase(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            char* p = pkt->args;
            uint32_t addr = strtoul(p, &p, 16);
            sprintf(LOGBUF, "cmdimpl_flash_erase addr=0x%02X%02X%02X", 
                    make8(addr, 2), make8(addr, 1), make8(addr, 0)); log_info();
            flashEraseBlockByAddr(addr); 
            char res[32] = {0};
            sprintf(res, "%u 0x%02X%02X%02X", 
                    SUCCESS, make8(addr, 2), make8(addr, 1), make8(addr, 0));
            mcp_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_flash_unprot(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            char* p = pkt->args;
            uint32_t addr = strtoul(p, &p, 16);
            sprintf(LOGBUF, "cmdimpl_flash_unprot addr=0x%02X%02X%02X", 
                    make8(addr, 2), make8(addr, 1), make8(addr, 0)); log_info();
            flashSectorProtectDisable(addr); 
            char res[32] = {0};
            sprintf(res, "%u 0x%02X%02X%02X", 
                    SUCCESS, make8(addr, 2), make8(addr, 1), make8(addr, 0));
            mcp_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_flash_read_prot(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            char* p = pkt->args;
            uint32_t addr = strtoul(p, &p, 16);
            sprintf(LOGBUF, "cmdimpl_flash_read_prot addr=0x%02X%02X%02X", 
                    make8(addr, 2), make8(addr, 1), make8(addr, 0)); log_info();
            uint8_t sp = flashReadSectorProtection(addr); 
            char res[32] = {0};
            sprintf(res, "%u 0x%02X%02X%02X %u", 
                    SUCCESS, make8(addr, 2), make8(addr, 1), make8(addr, 0), sp);
            mcp_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_cfg_get(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            config_s* cfg = &g_flashmgr.config;
            char res[MCP_MAX_ARGS_LEN] = {0};
            uint16_t j = sprintf(res, "%u %u %u", cfg->log_level, cfg->gyro_rate_src, cfg->mag_src);
            uint8_t i;
            for (i = 0; i < 3; i++) {
                j += sprintf(&res[j], " ");
                j += ftoa(cfg->paxs[i], &res[j], 3, 'f');
            }
            char tlebuf[140] = {0};
            memcpy(tlebuf, cfg->tle, sizeof(tlebuf));
            j += sprintf(&res[j], " %s", tlebuf); // Doesn't compile with just cfg->tle for some reason
            mcp_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_cfg_set(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            char* p = pkt->args;
            config_s* cfg = &g_flashmgr.config;
            cfg->log_level = strtoul(p, &p, 10);
            cfg->gyro_rate_src = strtoul(p, &p, 10);
            cfg->mag_src = strtoul(p, &p, 10);
            cfg->paxs[0] = strtof(p, &p);
            cfg->paxs[1] = strtof(p, &p);
            cfg->paxs[2] = strtof(p, &p);
            memset(cfg->tle, 0, sizeof(cfg->tle));
            memcpy(cfg->tle, p+1, strlen(p+1) + 1);
            char res[8] = {0};
            sprintf(res, "%u", SUCCESS);
            mcp_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_cfg_set_ll(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            char* p = pkt->args;
            config_s* cfg = &g_flashmgr.config;
            cfg->log_level = strtoul(p, &p, 10);
            char res[8] = {0};
            sprintf(res, "%u", SUCCESS);
            mcp_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_cfg_flush(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            status_e s = flashmgr_config_flush(&g_flashmgr);
            char res[8] = {0};
            sprintf(res, "%u", s);
            mcp_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_mtq_heartbeat(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            mtq_check_heartbeat(&g_mtq);
            char res[8] = {0};
            sprintf(res, "%u", g_mtq.heartbeat);
            mcp_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_mtq_reset(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            status_e s = mtq_reset(&g_mtq);
            char res[8] = {0};
            sprintf(res, "%u", s);
            mcp_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_mtq_read_1(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            char* p = pkt->args;
            uint8_t midx = strtoul(p, &p, 10);
            uint8_t idx = strtoul(p, &p, 10);
            sprintf(LOGBUF, "cmdimpl_mtq_read_1 midx=%u idx=%u", midx, idx); log_info();
            uint16_t key = (uint16_t) midx << 8 | idx;
            status_e s = mtq_read_start(&g_mtq, key);
            char res[16] = {0};
            sprintf(res, "%u %u %u", s, midx, idx);
            mcp_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_mtq_get_1(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            char* p = pkt->args;
            uint8_t midx = strtoul(p, &p, 10);
            uint8_t idx = strtoul(p, &p, 10);
            sprintf(LOGBUF, "cmdimpl_mtq_get_1 midx=%u idx=%u", midx, idx); log_info();
            char res[MCP_MAX_ARGS_LEN] = {0};
            uint16_t j = sprintf(res, "%u %u", midx, idx);
            uint16_t key = ((uint16_t)midx << 8) | idx;
            mtq_print_reg_data(&g_mtq, key, (char*)res, &j);
            mcp_respond(pkt, RES, (char*)res);
            break;
        }
    }
}

void cmdimpl_mtq_set_1(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            char* p = pkt->args;
            uint8_t midx = strtoul(p, &p, 10);
            uint8_t idx = strtoul(p, &p, 10);
            sprintf(LOGBUF, "cmdimpl_mtq_set_1 midx=%u idx=%u", midx, idx); log_info();
            uint16_t key = ((uint16_t) midx << 8) | idx;
            mtq_reg_s* reg = mtq_get_reg(&g_mtq, key);
            char res[16] = {0};
            if (reg == NULL) {
                sprintf(res, "%u %u %u", FAILURE, midx, idx);
                mcp_respond(pkt, RES, res);
                break;
            }
            uint8_t i;
            uint8_t data[MTQ_MAX_PAYLOAD_LEN] = {0};
            uint8_t l = MTQ_REG_TYPE_SIZES[reg->type];
            switch (reg->type) {
                case T_UINT8:
                    for (i = 0; i < reg->value_len; i++) {
                        uint8_t val = strtoul(p, &p, 10);
                        memcpy(&data[i*l], &val, l);
                    }
                    break;
                case T_INT8: 
                    for (i = 0; i < reg->value_len; i++) {
                        int8_t val = strtol(p, &p, 10);
                        memcpy(&data[i*l], &val, l);
                    }
                    break;
                case T_UINT16:
                    for (i = 0; i < reg->value_len; i++) {
                        uint16_t val = strtoul(p, &p, 10);
                        memcpy(&data[i*l], &val, l);
                    }
                    break;
                case T_INT16:
                    for (i = 0; i < reg->value_len; i++) {
                        int16_t val = strtol(p, &p, 10);
                        memcpy(&data[i*l], &val, l);
                    }
                    break;
                case T_FLOAT:
                    for (i = 0; i < reg->value_len; i++) {
                        float val = strtof(p, &p);
                        memcpy(&data[i*l], &val, l);
                    }
                    break;
                case T_CHAR:
                    memcpy(data, p+1, strlen(p+1)); // Skip 1 space
                    break;
            }
            status_e s = mtq_write_start(&g_mtq, reg, data);
            sprintf(res, "%u %u %u", s, midx, idx);
            mcp_respond(pkt, RES, res); 
            break;
        }
    }
}

void cmdimpl_mtq_read_fast(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            sprintf(LOGBUF, "cmdimpl_mtq_read_fast"); log_info();
            status_e s = mtq_read_fast(&g_mtq); 
            char res[8] = {0};
            sprintf(res, "%u", s);
            mcp_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_mtq_get_fast(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            char* p = pkt->args;
            uint8_t page = strtoul(p, &p, 10);
            char res[MCP_MAX_ARGS_LEN] = {0};
            if (page > MTQ_NUM_FAST_REGS / MTQ_PAGE_SIZE) {
                sprintf(res, "%u %u", FAILURE, page);
                mcp_respond(pkt, RES, res);
                break;
            }
            uint8_t i1 = MTQ_PAGE_SIZE * page;
            uint8_t i2 = minu8(i1 + MTQ_PAGE_SIZE, MTQ_NUM_FAST_REGS);
            sprintf(LOGBUF, "cmdimpl_mtq_get_fast pg=%u i1=%u i2=%u", page, i1, i2); log_info();
            uint16_t j = sprintf(res, "%u %u", SUCCESS, page);
            uint8_t i;
            for (i = i1; i < i2; i++) {
                uint16_t key = MTQ_FAST_FRAME_REGS[i];
                j += sprintf(&res[j], " %u,%u", key >> 8, key & 0xFF);
                mtq_print_reg_data(&g_mtq, key, res, &j);
            }
            mcp_respond(pkt, RES, res); 
            break;
        }
    }
}

void cmdimpl_mtq_read_ctrl(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            sprintf(LOGBUF, "cmdimpl_mtq_read_ctrl"); log_info();
            status_e s = mtq_read_ctrl(&g_mtq);
            char res[8] = {0};
            sprintf(res, "%u", s);
            mcp_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_mtq_get_ctrl(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            char* p = pkt->args;
            uint8_t page = strtoul(p, &p, 10);
            char res[MCP_MAX_ARGS_LEN] = {0};
            if (page > MTQ_NUM_CTRL_REGS / MTQ_PAGE_SIZE) {
                sprintf(res, "%u %u", FAILURE, page);
                mcp_respond(pkt, RES, res);
                break;
            }
            uint8_t i1 = MTQ_PAGE_SIZE * page;
            uint8_t i2 = minu8(i1 + MTQ_PAGE_SIZE, MTQ_NUM_CTRL_REGS);
            sprintf(LOGBUF, "cmdimpl_mtq_get_ctrl pg=%u i1=%u i2=%u", page, i1, i2); log_info();
            uint16_t j = sprintf(res, "%u %u", SUCCESS, page);
            uint8_t i;
            for (i = i1; i < i2; i++) {
                uint16_t key = MTQ_CTRL_FRAME_REGS[i];
                j += sprintf(&res[j], " %u,%u", key >> 8, key & 0xFF);
                mtq_print_reg_data(&g_mtq, key, res, &j);
            }
            mcp_respond(pkt, RES, res); 
            break;
        }
    }
}

void cmdimpl_mtq_read_all(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            sprintf(LOGBUF, "cmdimpl_mtq_read_all"); log_info();
            status_e s = mtq_read_all(&g_mtq); 
            char res[8] = {0};
            sprintf(res, "%u", s);
            mcp_respond(pkt, RES, res); 
            break;
        }
    }
}

void cmdimpl_mtq_get_all(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            char* p = pkt->args;
            uint8_t page = strtoul(p, &p, 10);
            char res[MCP_MAX_ARGS_LEN] = {0};
            if (page > MTQ_REG_TABLE_LEN / MTQ_PAGE_SIZE) {
                sprintf(res, "%u %u", FAILURE, page);
                mcp_respond(pkt, RES, res);
                break;
            }
            uint8_t i1 = MTQ_PAGE_SIZE * page;
            uint8_t i2 = minu8(i1 + MTQ_PAGE_SIZE, MTQ_REG_TABLE_LEN);
            sprintf(LOGBUF, "cmdimpl_mtq_get_all pg=%u i1=%u i2=%u", page, i1, i2); log_info();
            uint16_t j = sprintf(res, "%u %u", SUCCESS, page);
            uint8_t i;
            for (i = i1; i < i2; i++) {
                mtq_reg_s* reg = &g_mtq.reg_table[i];
                j += sprintf(&res[j], " %u,%u", reg->midx, reg->idx);
                mtq_print_reg_data(&g_mtq, reg, res, &j);
            }
            mcp_respond(pkt, RES, res); 
            break;
        }
    }
}

void cmdimpl_nvg_heartbeat(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            nvg_check_heartbeat(&g_nvg);
            char res[8] = {0};
            sprintf(res, "%u", g_nvg.heartbeat);
            mcp_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_nvg_reset(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            sprintf(LOGBUF, "cmdimpl_nvg_reset"); log_info();
            status_e s = nvg_reset(&g_nvg); 
            char res[8] = {0};
            sprintf(res, "%u", s);
            mcp_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_nvg_power(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            sprintf(LOGBUF, "cmdimpl_nvg_power"); log_info();
            status_e s = nvg_power(&g_nvg); 
            char res[8] = {0};
            sprintf(res, "%u", s);
            mcp_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_nvg_send_cmd(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            char* p = pkt->args;
            sprintf(LOGBUF, "cmdimpl_nvg_send_cmd '%s'", p); log_info();
            status_e s = nvg_send_cmd(&g_nvg, p); 
            char res[16] = {0};
            sprintf(res, "%u %s", s, p);
            mcp_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_nvg_get_1(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            char* p = pkt->args;
            uint8_t sensor_id = strtoul(p, &p, 10);  
            sprintf(LOGBUF, "cmdimpl_nvg_get_1 sid=%u", sensor_id); log_info();
            float data[8] = {0}; 
            status_e s = nvg_get_sensor_data(&g_nvg, sensor_id, data); 
            if (s == SUCCESS) {
                char res[MCP_MAX_ARGS_LEN] = {0};
                nvg_sensor_s* sensor = &g_nvg.sensors[sensor_id];
                uint16_t j = sprintf(res, "%u %u ", s, sensor_id);
                j += ftoa(sensor->ts, &res[j], 6, 'f');
                uint8_t i;
                for (i = 0; i < sensor->len; i++) {
                    j += sprintf(&res[j], " ");
                    j += ftoa(data[i], &res[j], 6, 'f');
                }
                mcp_respond(pkt, RES, res); 
            } else {
                char res[8] = {0};
                sprintf(res, "%u", FAILURE);
                mcp_respond(pkt, RES, res); 
            }
            break;
        }
    }
}

void cmdimpl_nvg_set_1(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            char* p = pkt->args;
            uint8_t sensor_id = strtoul(p, &p, 10);  
            uint8_t rate = strtoul(p, &p, 10);  
            sprintf(LOGBUF, "cmdimpl_nvg_set_1 sid=%u rate=%u", sensor_id, rate); log_info();
            status_e s = nvg_set_sensor(&g_nvg, sensor_id, rate); 
            char res[16] = {0};
            sprintf(res, "%u %u %u", s, sensor_id, rate);
            mcp_respond(pkt, RES, res); 
            break;
        }
    }
}

void cmdimpl_nvg_start_all(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            sprintf(LOGBUF, "cmdimpl_nvg_start_all"); log_info();
            status_e s = nvg_start_all_sensors(&g_nvg); 
            char res[8] = {0};
            sprintf(res, "%u", s);
            mcp_respond(pkt, RES, res); 
            break;
        }
    }
}

void cmdimpl_nvg_stop_all(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            sprintf(LOGBUF, "cmdimpl_nvg_stop_all"); log_info();
            status_e s = nvg_stop_all_sensors(&g_nvg); 
            char res[8] = {0};
            sprintf(res, "%u", s);
            mcp_respond(pkt, RES, res); 
            break;
        }
    }
}
