// UPPER PPM COMMAND IMPLEMENTATIONS

#include "cmdimpl.h"
#include "mcpmgr.h"
#include "common.h"
#include "systime.h"
#include "ax100.h"
#include "telemetry.h"
#include "hashtable.h"
#include "interrupts.h"
#include "scheduler.h"
#include "globals.h"
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#module

void cmdimpl_init() {
    hashtable_s* ht = &g_mcpmgr.cmdimpls;
    
    ht_set(ht, "ping", (cmdimpl_f) cmdimpl_ping);
    
    ht_set(ht, "ppm_reset", (cmdimpl_f) cmdimpl_ppm_reset);
    ht_set(ht, "ppm_get_time", (cmdimpl_f) cmdimpl_ppm_get_time);
    ht_set(ht, "ppm_set_time", (cmdimpl_f) cmdimpl_ppm_set_time);
    ht_set(ht, "ppm_delay", (cmdimpl_f) cmdimpl_ppm_delay);
    ht_set(ht, "ppm_clear_bufs", (cmdimpl_f) cmdimpl_ppm_clear_bufs);
    ht_set(ht, "ppm_get_scheds", (cmdimpl_f) cmdimpl_ppm_get_scheds);
    ht_set(ht, "ppm_sched_cmd_in", (cmdimpl_f) cmdimpl_ppm_sched_cmd_in);
    ht_set(ht, "ppm_desched", (cmdimpl_f) cmdimpl_ppm_desched);
    ht_set(ht, "ppm_resched_in", (cmdimpl_f) cmdimpl_ppm_resched_in);
    ht_set(ht, "ppm_clear_sched", (cmdimpl_f) cmdimpl_ppm_clear_sched);
    
    ht_set(ht, "tlm_get_data", (cmdimpl_f) cmdimpl_tlm_get_data);

    ht_set(ht, "flash_get_cfg", (cmdimpl_f) cmdimpl_flash_get_cfg);
    ht_set(ht, "flash_set_cfg", (cmdimpl_f) cmdimpl_flash_set_cfg);
    
    ht_set(ht, "ax100_get_power", (cmdimpl_f) cmdimpl_ax100_get_power);
    ht_set(ht, "ax100_set_power", (cmdimpl_f) cmdimpl_ax100_set_power);
}

// HELPERS

void ppm_set_time_from_str(char* args) {
    char* p = args;
    rtc_time_t time = {0};
    time.tm_wday = strtoul(p, &p, 10);
    time.tm_mon = strtoul(p, &p, 10); 
    time.tm_mday = strtoul(p, &p, 10);
    time.tm_year = strtoul(p, &p, 10);
    time.tm_hour = strtoul(p, &p, 10);
    time.tm_min = strtoul(p, &p, 10);
    time.tm_sec = strtoul(p, &p, 10);

    uint64_t oldtime = systime_epoch_ms();
    memcpy(&g_rtc_time, &time, sizeof(rtc_time_t));
    systime_sync(); // Since Lower PPM is source of truth for timing, this sync should be the only sync in Upper PPM
    scheduler_refresh_all(&g_scheduler, oldtime);
}

// COMMAND IMPLEMENTATIONS

void cmdimpl_ping(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            sprintf(LOGBUF, "cmdimpl_ping '%s'", pkt->args); log_info();
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
        case RES: {
            sprintf(LOGBUF, "cmdimpl_ppm_get_time RES"); log_info();
            ppm_set_time_from_str(pkt->args);
            break;
        }
    }
}

void cmdimpl_ppm_set_time(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            ppm_set_time_from_str(pkt->args);
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
                case 2: 
                    i2cmgr_clear(&g_i2cmgr);
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

void cmdimpl_ppm_get_scheds(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            sprintf(LOGBUF, "cmdimpl_ppm_get_scheds"); log_info();
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
            char sep[] = " ";
            char* cmd_id = strtok(++p, sep);
            char* args = strtok(0, sep);
            sprintf(LOGBUF, "cmdimpl_ppm_sched_cmd_in sid=%u del=%u per=%u rep=%u o=%u d=%u e=%u p=%u id='%s' args='%s'",
                    sched_id, start_delay_ms, period_ms, reps, orgn, dest, echo, ptype, cmd_id, args); log_info();
            mcppkt_s schedcmd;
            mcppkt_create(&schedcmd, orgn, dest, echo, ptype, cmd_id, args);
            status_e s = scheduler_schedule_cmd_in(&g_scheduler, sched_id, &schedcmd, start_delay_ms, period_ms, reps);
            char res[32] = {0};
            uint8_t j = sprintf(res, "%u %u", s, sched_id);
            if (s == SUCCESS) {
                j += sprintf(&res[j], " %u", &g_scheduler.id_map[sched_id]->next_release);
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
            sprintf(LOGBUF, "cmdimpl_ppm_resched_in id=%u", sched_id); log_info();
            status_e s = scheduler_reschedule_in(&g_scheduler, sched_id, start_delay_ms);
            char res[32] = {0};
            uint8_t j = sprintf(res, "%u %u", s, sched_id);
            if (s == SUCCESS) {
                j += sprintf(&res[j], " %u", &g_scheduler.id_map[sched_id]->next_release);
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

void cmdimpl_tlm_get_data(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case RES: {
            sprintf(LOGBUF, "cmdimpl_tlm_get_data RES o=%u al=%u", pkt->orgn, pkt->args_len); log_info();
            char* p = pkt->args;
            switch (pkt->orgn) {
                case NODE_LPPM:
                    g_tlm.lppm_rbt_cnt = strtoul(p, &p, 10);  
                    g_tlm.lppm_rbt_cause = strtoul(p, &p, 10);  
                    g_tlm.ertc_heartbeat = strtoul(p, &p, 10);
                    g_tlm.mtq_heartbeat = strtoul(p, &p, 10);
                    g_tlm.nvg_heartbeat = strtoul(p, &p, 10);
                    g_tlm.mtq_stat = strtoul(p, &p, 10);
                    g_tlm.gyro_rate[0] = strtof(p, &p);
                    g_tlm.gyro_rate[1] = strtof(p, &p);
                    g_tlm.gyro_rate[2] = strtof(p, &p);
                    g_tlm.attitude[0] = strtof(p, &p);
                    g_tlm.attitude[1] = strtof(p, &p);
                    g_tlm.attitude[2] = strtof(p, &p);
                    g_tlm.attitude[3] = strtof(p, &p);
                    break;
                case NODE_EPS:
                    break;
                case NODE_HOLONAV: 
                    break;
                case NODE_ASTROBOARD:
                    break;
            }  
            break;
        }
    }
}

void cmdimpl_flash_get_cfg(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            char res[MCP_MAX_ARGS_LEN] = {0};
            config_s* cfg = &g_flashmgr.config;
            sprintf(res, "%u", cfg->log_level);
            mcp_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_flash_set_cfg(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            char* p = pkt->args;
            config_s* cfg = &g_flashmgr.config;
            cfg->log_level = strtoul(p, &p, 10);
            status_e s = flashmgr_config_flush(&g_flashmgr);
            char res[8] = {0};
            sprintf(res, "%u", s);
            mcp_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_ax100_get_power(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            uint8_t power = 0;
            status_e s = ax100_get_power(&g_ax100, &power);
            char res[8] = {0};
            sprintf(res, "%u", s);
            mcp_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_ax100_set_power(mcppkt_s* pkt) {
    switch (pkt->ptype) {
        case CMD: {
            char* p = pkt->args;
            uint8_t power = strtoul(p, &p, 10);
            status_e s = ax100_set_power(&g_ax100, power);
            char res[8] = {0};
            sprintf(res, "%u", s);
            mcp_respond(pkt, RES, res);
            break;
        }
    }
}
