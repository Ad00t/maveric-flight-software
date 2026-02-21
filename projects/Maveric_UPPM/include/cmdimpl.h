#ifndef __CMDIMPL_H__
#define __CMDIMPL_H__

// Register command implementations in cmdmgr hash table
void cmdimpl_init();

// COMMAND IMPLEMENTATIONS 

void cmdimpl_cmd_set_time(cmdpkt_s* pkt);

#endif 
