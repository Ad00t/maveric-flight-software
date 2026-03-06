#ifndef __CMDIMPL_H__
#define __CMDIMPL_H__

#include "cmdmgr.h"

// Register command implementations in cmdmgr hash table
void cmdimpl_init(void);

// COMMAND IMPLEMENTATIONS 

void cmdimpl_ppm_set_time(cmdpkt_s* pkt);
void cmdimpl_ppm_ftdi_log(cmdpkt_s* pkt);
void cmdimpl_ppm_ping(cmdpkt_s* pkt);

#endif 
