#include "cmdfunc.h"
#include "cmdmgr.h"
#include "systime.h"
#include "hashtable.h"
#include "interrupts.h"
#include "scheduler.h"
#include "adcsmtq.h"
#include "m41t81s.h"
#include "adis16260.h"
#include "naviguider.h"
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

    ertc_set_time(&g_ertc, &time);
    systime_sync();
    mtq_set_date_time(&g_mtq, &time); 

    fprintf(FTDI_PORT, "%s[%s] cmdfunc_set_time [ %02u, %02u/%02u/20%02u %02u:%02u:%02u ]\n", KCYN, NODE_LBL, 
            g_ertc.time.tm_wday, g_ertc.time.tm_mon, g_ertc.time.tm_mday, g_ertc.time.tm_year, 
            g_ertc.time.tm_hour, g_ertc.time.tm_min, g_ertc.time.tm_sec);
}
