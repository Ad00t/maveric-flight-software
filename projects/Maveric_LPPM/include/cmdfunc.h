#ifndef __CMDFUNC_H__
#define __CMDFUNC_H__

#include "cmdmgr.h"

// Register command functions in cmdmgr hash table
void cmdmgr_register_funcs(cmdmgr* cmdmgr);

// COMMAND FUNCTIONS

void cmdfunc_set_time(cmdpkt_s* pkt);
void cmdfunc_ftdi_log(cmdpkt_s* pkt);

#endif 
