#include "cmdimpl.h"
#include "cmdmgr.h"
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
extern struct_tm g_rtc_time;        // Global RTC time tracking instance (from lower PPM)       
extern ax100_s g_ax100;             // AX100 transceiver driver 

void cmdimpl_init() {
    ht_set(&g_cmdmgr.cmdimpls, "ppm_set_time", (cmdimpl_f) cmdimpl_ppm_set_time);
    ht_set(&g_cmdmgr.cmdimpls, "ppm_ping", (cmdimpl_f) cmdimpl_ppm_ping);
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
    rtc_write(&time);
    rtc_read(&g_rtc_time);

    systime_sync(); // Since Lower PPM is source of truth for timing, this sync should be the only sync in Upper PPM

    sprintf(LOGBUF, "cmdimpl_ppm_set_time '%s' [ %02u, %02u/%02u/20%02u %02u:%02u:%02u ]", pkt->args
            g_rtc_time.tm_wday, g_rtc_time.tm_mon, g_rtc_time.tm_mday, g_rtc_time.tm_year, 
            g_rtc_time.tm_hour, g_rtc_time.tm_min, g_rtc_time.tm_sec); log_flush(LL_INFO);
}

void cmdimpl_ppm_ping(cmdpkt_s* pkt) {
    if (pkt->ptype == REQ) {
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, RES, "ppm_ping", "pong");
    }
}
