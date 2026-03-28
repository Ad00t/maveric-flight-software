// UPPER PPM COMMAND IMPLEMENTATIONS

#include "cmdimpl.h"
#include "cmdmgr.h"
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
    hashtable_s* ht = &g_cmdmgr.cmdimpls;
    
    ht_set(ht, "ping", (cmdimpl_f) cmdimpl_ping);
    
    ht_set(ht, "ppm_reset", (cmdimpl_f) cmdimpl_ppm_reset);
    ht_set(ht, "ppm_get_time", (cmdimpl_f) cmdimpl_ppm_get_time);
    ht_set(ht, "ppm_set_time", (cmdimpl_f) cmdimpl_ppm_set_time);
    ht_set(ht, "ppm_delay", (cmdimpl_f) cmdimpl_ppm_delay);
    ht_set(ht, "ppm_clear_bufs", (cmdimpl_f) cmdimpl_ppm_clear_bufs);
    ht_set(ht, "ppm_get_scheds", (cmdimpl_f) cmdimpl_ppm_get_scheds);
    ht_set(ht, "ppm_sched_cmd_in", (cmdimpl_f) cmdimpl_ppm_sched_cmd_in);
    ht_set(ht, "ppm_deschedule", (cmdimpl_f) cmdimpl_ppm_deschedule);
    
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

void cmdimpl_ping(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ: {
            cmd_respond(pkt, RES, "pong");
            break;
        }
    }
}

void cmdimpl_ppm_reset(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ: {
            sprintf(LOGBUF, "cmdimpl_ppm_reset"); log_info();
            g_superloop_running = FALSE;
            cmd_respond(pkt, ACK, "");
            break;
        }
    }
}

void cmdimpl_ppm_get_time(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ: {
            sprintf(LOGBUF, "cmdimpl_ppm_get_time REQ"); log_info();
            char res[CMD_MAX_ARGS_LEN] = {0};  
            systime_str(res);
            cmd_respond(pkt, RES, res);
            break;
        }
        case RES: {
            sprintf(LOGBUF, "cmdimpl_ppm_get_time RES"); log_info();
            ppm_set_time_from_str(pkt->args);
            break;
        }
    }
}

void cmdimpl_ppm_set_time(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ: {
            ppm_set_time_from_str(pkt->args);
            rtc_time_t rtc;
            systime_rtc(&rtc);
            sprintf(LOGBUF, "cmdimpl_ppm_set_time '%s' [ %02u, %02u/%02u/20%02u %02u:%02u:%02u ]", pkt->args,
                    rtc.tm_wday, rtc.tm_mon, rtc.tm_mday, rtc.tm_year, 
                    rtc.tm_hour, rtc.tm_min, rtc.tm_sec); log_info();
            break;
        }
    }
}

void cmdimpl_ppm_delay(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ: {
            char* p = pkt->args;
            uint32_t delay = strtoul(p, &p, 10);
            sprintf(LOGBUF, "cmdimpl_ppm_delay d=%u", delay); log_info();
            if (delay < 60000) {
                cmd_respond(pkt, ACK, "");
                delay_ms(delay);
            } else {
                cmd_respond(pkt, NACK, "");
            }
            break;
        }
    }
}

void cmdimpl_ppm_clear_bufs(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ: {
            char* p = pkt->args;
            uint8_t mode = strtoul(p, &p, 10);
            sprintf(LOGBUF, "cmdimpl_ppm_clear_bufs m=%u", mode); log_info();
            switch (mode) {
                case 0: 
                    irqmgr_clear(&g_irqmgr);
                    break;
                case 1:
                    cmdmgr_clear(&g_cmdmgr);
                    break;
            }
            break;
        }
    }
}

void cmdimpl_ppm_get_scheds(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ: {
            sprintf(LOGBUF, "cmdimpl_ppm_get_scheds"); log_info();
            char res[CMD_MAX_ARGS_LEN] = {0}; 
            uint8_t i;
            uint16_t j = 0;
            for (i = 0; i < SCHEDULER_MAX_TASKS; i++) {
                schedtask_s* st = &g_scheduler.tasks[i];
                j += sprintf(&res[j], "%u:%u,%u,%u ", i, st->id, st->active, st->type);
            }
            cmd_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_ppm_sched_cmd_in(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ: {
            char* p = pkt->args;
            uint8_t schedule_id = strtoul(p, &p, 10);
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
                    schedule_id, start_delay_ms, period_ms, reps, orgn, dest, echo, ptype, cmd_id, args); log_info();
            cmdpkt_s schedcmd;
            cmdpkt_create(&schedcmd, orgn, dest, echo, ptype, cmd_id, args);
            status_e s = scheduler_schedule_cmd_in(&g_scheduler, schedule_id, &schedcmd, start_delay_ms, period_ms, reps);
            char res[CMD_MAX_ARGS_LEN] = {0};
            sprintf(res, "%u", schedule_id);
            cmd_respond(pkt, stat2ack(s), res); 
            break;
        }
    }
}

void cmdimpl_ppm_deschedule(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ:
            char* p = pkt->args;
            uint8_t sched_id = strtoul(p, &p, 10);
            sprintf(LOGBUF, "cmdimpl_ppm_deschedule id=%u", sched_id); log_info();
            status_e s = scheduler_deschedule(&g_scheduler, sched_id);
            char res[CMD_MAX_ARGS_LEN] = {0};
            sprintf(res, "%u", sched_id);
            cmd_respond(pkt, stat2ack(s), res);
            break;
    }
}

void cmdimpl_tlm_get_data(cmdpkt_s* pkt) {
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

void cmdimpl_flash_get_cfg(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ: {
            char res[CMD_MAX_ARGS_LEN] = {0};
            config_s* cfg = &g_flashmgr.config;
            sprintf(res, "%u %u %u", cfg->log_level, cfg->last_holonav_seq, cfg->last_astroboard_seq);
            cmd_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_flash_set_cfg(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ: {
            char* p = pkt->args;
            config_s* cfg = &g_flashmgr.config;
            cfg->log_level = strtoul(p, &p, 10);
            cfg->last_holonav_seq = strtoul(p, &p, 10);
            cfg->last_astroboard_seq = strtoul(p, &p, 10);
            status_e s = flashmgr_config_flush(&g_flashmgr);
            cmd_respond(pkt, stat2ack(s), "");
            break;
        }
    }
}

void cmdimpl_ax100_get_power(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ: {
            uint8_t power = 0;
            if (ax100_get_power(&g_ax100, &power) == SUCCESS) {
                char res[CMD_MAX_ARGS_LEN] = {0};
                sprintf(res, "%u", power);
                cmd_respond(pkt, RES, res);
            } else {
                cmd_respond(pkt, NACK, "");
            }
            break;
        }
    }
}

void cmdimpl_ax100_set_power(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ: {
            char* p = pkt->args;
            uint8_t power = strtoul(p, &p, 10);
            status_e s = ax100_set_power(&g_ax100, power);
            cmd_respond(pkt, stat2ack(s), "");
            break;
        }
    }
}
