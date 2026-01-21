#ifndef __CMDFUNC_H__
#define __CMDFUNC_H__

#include "cmdmgr.h"

typedef void (*cmdfunc_f)(cmdpkt_s* pkt);

// Register command functions in cmdmgr hash table
void cmdfunc_register_all(cmdmgr_s* cmdmgr);

// COMMAND FUNCTIONS

void cmdfunc_set_time(cmdpkt_s* pkt);

#endif 
