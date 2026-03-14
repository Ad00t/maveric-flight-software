// LOWER PPM COMMAND IMPLEMENTATIONS

#include "cmdimpl.h"
#include "cmdmgr.h"
#include "systime.h"
#include "hashtable.h"
#include "interrupts.h"
#include "scheduler.h"
#include "adcsmtq.h"
#include "m41t81s.h"
#include "adis16260.h"
#include "naviguider.h"
#include "housekeeping.h"
#include "common.h"
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

extern irqmgr_s g_irqmgr;             // Interrupts manager
extern cmdmgr_s g_cmdmgr;             // Commands manager
extern scheduler_s g_scheduler;       // Schedules manager
extern ertc_s g_ertc;                 // External RTC (on motherboard)
extern mtq_s g_mtq;                   // Magnetorquer
extern gyro_s g_gyro;                 // Gyroscope (x3)
extern nvg_s g_nvg;                   // Naviguider

void cmdimpl_init(void) {
    ht_set(&g_cmdmgr.cmdimpls, "ppm_set_time", (cmdimpl_f) cmdimpl_ppm_set_time);
    ht_set(&g_cmdmgr.cmdimpls, "ppm_get_time", (cmdimpl_f) cmdimpl_ppm_get_time);
    ht_set(&g_cmdmgr.cmdimpls, "ppm_ping", (cmdimpl_f) cmdimpl_ppm_ping);
    ht_set(&g_cmdmgr.cmdimpls, "ppm_delay", (cmdimpl_f) cmdimpl_ppm_delay);
    ht_set(&g_cmdmgr.cmdimpls, "ppm_clear_bufs", (cmdimpl_f) cmdimpl_ppm_clear_bufs);
}

// COMMAND IMPLEMENTATIONS

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

    ertc_set_time(&g_ertc, &time);
    systime_sync();
    mtq_set_date_time(&g_mtq, &time); 
    cmd_dispatch(NODE, NODE_UPPM, 0, REQ, "ppm_set_time", pkt->args);

    sprintf(LOGBUF, "cmdimpl_ppm_set_time '%s' [ %02u, %02u/%02u/20%02u %02u:%02u:%02u ]", pkt->args
            g_ertc.time.tm_wday, g_ertc.time.tm_mon, g_ertc.time.tm_mday, g_ertc.time.tm_year, 
            g_ertc.time.tm_hour, g_ertc.time.tm_min, g_ertc.time.tm_sec); log_info();
}

void cmdimpl_ppm_get_time(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ: // Only implement REQ here because LPPM should never be replacing its time from another subsystem
            char tm_str[32] = {0};  
            rtc_to_str(tm_str, &g_ertc.time);
            cmd_dispatch(NODE, pkt->orgn, pkt->echo, RES, "ppm_get_time", tm_str);
            sprintf(LOGBUF, "cmdimpl_ppm_get_time REQ '%s'", tm_str); log_info();
            break;
    }
}

void cmdimpl_ppm_ping(cmdpkt_s* pkt) {
    sprintf(LOGBUF, "cmdimpl_ppm_ping '%s'", pkt->args); log_info();
    if (pkt->ptype == REQ) {
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, RES, "ppm_ping", "pong");
    }
}

void cmdimpl_ppm_delay(cmdpkt_s* pkt) {
    char* p = pkt->args;
    uint32_t delay = strtoul(p, &p, 10);
    sprintf(LOGBUF, "cmdimpl_ppm_delay d=%u", delay); log_info();
    if (delay < 60000) {
        delay_ms(delay);
    }
}

void cmdimpl_ppm_clear_bufs(cmdpkt_s* pkt) {
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
}
