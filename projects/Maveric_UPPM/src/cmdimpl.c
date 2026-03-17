// UPPER PPM COMMAND IMPLEMENTATIONS

#include "cmdimpl.h"
#include "cmdmgr.h"
#include "common.h"
#include "systime.h"
#include "ax100.h"
#include "hashtable.h"
#include "interrupts.h"
#include "scheduler.h"
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

extern irqmgr_s g_irqmgr;           // Interrupts manager
extern cmdmgr_s g_cmdmgr;           // Commands manager
extern scheduler_s g_scheduler;     // Schedules manager
extern rtc_time_t g_rtc_time;       // Global RTC time tracking instance (from lower PPM)       
extern ax100_s g_ax100;             // AX100 transceiver driver 
extern tlm_s g_tlm;                 // Global telemetry state / data store

void cmdimpl_init() {
    ht_set(&g_cmdmgr.cmdimpls, "ppm_set_time", (cmdimpl_f) cmdimpl_ppm_set_time);
    ht_set(&g_cmdmgr.cmdimpls, "ppm_get_time", (cmdimpl_f) cmdimpl_ppm_get_time);
    ht_set(&g_cmdmgr.cmdimpls, "ppm_ping", (cmdimpl_f) cmdimpl_ppm_ping);
    ht_set(&g_cmdmgr.cmdimpls, "tlm_get_data", (cmdimpl_f) cmdimpl_tlm_get_data);
}

void cmdimpl_ppm_set_time(cmdpkt_s* pkt) {
    char* p = pkt->args;
    rtc_time_t time;
    time.tm_wday = strtoul(p, &p, 10); // Other options: strtok(), strtod(), strotol()
    time.tm_mon = strtoul(p, &p, 10); 
    time.tm_mday = strtoul(p, &p, 10);
    time.tm_year = strtoul(p, &p, 10);
    time.tm_hour = strtoul(p, &p, 10);
    time.tm_min = strtoul(p, &p, 10);
    time.tm_sec = strtoul(p, &p, 10);
    // rtc_write(&time);
    // rtc_read(&g_rtc_time);
    
    uint64_t oldtime = systime_epoch_ms();
    memcpy(&g_rtc_time, &time, sizeof(rtc_time_t));
    systime_sync(); // Since Lower PPM is source of truth for timing, this sync should be the only sync in Upper PPM
    scheduler_refresh_all(&g_scheduler, oldtime);
    
    sprintf(LOGBUF, "cmdimpl_ppm_set_time '%s' [ %02u, %02u/%02u/20%02u %02u:%02u:%02u ]", pkt->args
            g_rtc_time.tm_wday, g_rtc_time.tm_mon, g_rtc_time.tm_mday, g_rtc_time.tm_year, 
            g_rtc_time.tm_hour, g_rtc_time.tm_min, g_rtc_time.tm_sec); log_info();
}

void cmdimpl_ppm_get_time(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ:
            char tm_str[32] = {0};  
            rtc_to_str(tm_str, &g_rtc_time);
            cmd_dispatch(NODE, pkt->orgn, pkt->echo, RES, "ppm_get_time", tm_str);
            sprintf(LOGBUF, "cmdimpl_ppm_get_time REQ '%s'", tm_str); log_info();
            break;
        case RES:
            sprintf(LOGBUF, "cmdimpl_ppm_get_time RES '%s'", pkt->args); log_info();
            cmdimpl_ppm_set_time(pkt);
            break;
    }
}

void cmdimpl_ppm_ping(cmdpkt_s* pkt) {
    if (pkt->ptype == REQ) {
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, RES, "ppm_ping", "pong");
    }
}

void cmdimpl_tlm_get_data(cmdpkt_s* pkt) {
    if (pkt->ptype != RES) return;
    sprintf(LOGBUF, "cmdimpl_tlm_get_data RES '%s'", pkt->args); log_info();
    char* p = pkt->args;
    switch (pkt->orgn) {
        case NODE_LPPM:
            g_tlm.lppm_rbt_cnt = strtoul(p, &p, 10);  
            g_tlm.lppm_rbt_cause = strtoul(p, &p, 10);  
            break;
        case NODE_EPS: break;
        case NODE_UPPM: break;
        case NODE_HOLONAV: break;
        case NODE_ASTROBOARD: break;
    }  
}
