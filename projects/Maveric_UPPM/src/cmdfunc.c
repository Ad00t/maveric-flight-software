#include "cmdfunc.h"
#include "cmdmgr.h"
#include "systime.h"
#include "hashtable.h"
#include "interrupts.h"
#include "scheduler.h"
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

extern irqmgr_s irqmgr;             // Interrupts manager
extern cmdmgr_s cmdmgr;             // Commands manager
extern scheduler_s scheduler;       // Schedules manager
extern struct_tm g_rtc_time;        // Global RTC time tracking instance (from lower PPM)       

void cmdmgr_register_funcs(cmdmgr_s* cmdmgr) {
    ht_init(&cmdmgr->cmdfuncs); 
    ht_set(&cmdmgr->cmdfuncs, "cmd_set_time", (cmdfunc_f) cmdfunc_set_time);
}

void cmdfunc_set_time(cmdpkt_s* pkt) {
    // I know we're updating the original args string here. We should have all our args parsed out after this so it's ok.
    char* p = pkt->args_str;  
    struct_tm time;
    time.tm_wday = strtoul(p, &p, 10); // Other options: strtok(), strtod(), strotol()
    time.tm_mon = strtoul(p, &p, 10); 
    time.tm_mday = strtoul(p, &p, 10);
    time.tm_year = strtoul(p, &p, 10);
    time.tm_hour = strtoul(p, &p, 10);
    time.tm_min = strtoul(p, &p, 10);
    time.tm_sec = strtoul(p, &p, 10);

    memcpy(&g_rtc_time, &time, sizeof(struct_tm));
    systime_sync(); // Since Lower PPM is source of truth for timing, this sync should be the only sync in Upper PPM

    fprintf(FTDI_PORT, "%s[%s] cmdfunc_set_time [ %02u, %02u/%02u/20%02u %02u:%02u:%02u ]\n", KCYN, NODE_LBL, 
            g_rtc_time.tm_wday, g_rtc_time.tm_mon, g_rtc_time.tm_mday, g_rtc_time.tm_year, 
            g_rtc_time.tm_hour, g_rtc_time.tm_min, g_rtc_time.tm_sec);
}
