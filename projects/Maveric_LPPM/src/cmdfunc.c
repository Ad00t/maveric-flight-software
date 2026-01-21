#include "cmdfunc.h"
#include "cmdmgr.h"
#include "hashtable.h"
#include "adcsmtq.h"
#include <stdint.h>
#include <stdlib.h>

void cmdfunc_register_all(cmdmgr_s* cmdmgr) {
    ht_init(&cmdmgr->cmdfuncs); 
    ht_set(&cmdmgr->cmdfuncs, "cmd_set_time", (cmdfunc_f) cmdfunc_set_time);
}

extern mtq_s mtq;

void cmdfunc_set_time(cmdpkt_s* pkt) {
    // I know we're updating the original args string here. We should have all our args parsed out after this so it's ok.
    char* p = pkt->args_str;   
    rtc_time_t t;
    t.tm_mon = strtoul(p, &p, 10); // Other options: strtok(), strtod(), strotol()
    t.tm_mday = strtoul(p, &p, 10);
    t.tm_year = strtoul(p, &p, 10);
    t.tm_hour = strtoul(p, &p, 10);
    t.tm_min = strtoul(p, &p, 10);
    t.tm_sec = strtoul(p, &p, 10);
    rtc_write(&t);

    mtq_set_date_time(&mtq, t.tm_mon, t.tm_mday, t.tm_year, t.tm_hour, t.tm_min, t.tm_sec); 

    fprintf(COM_D, "%s[%s] cmdfunc_set_time [ %02u/%02u/20%u %02u:%02u:%02u ]\n", KCYN, NODE_LBL, 
            t.tm_mon, t.tm_mday, t.tm_year, t.tm_hour, t.tm_min, t.tm_sec);
}
