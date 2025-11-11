#include "cmd.h"

#module

void cmdmgr_init(cmdmgr_s* cmdmgr) {
    cmdmgr_clear(cmdmgr);
}

void cmdmgr_clear(cmdmgr_s* cmdmgr) {
    cmdmgr->active.busy = FALSE;
    cmdmgr->active.orgn = 0;
    cmdmgr->active.dest = 0;
    cmdmgr->active.id = 255;
    memset(cmdmgr->active.prmstr, 0, MAX_BUF_LEN);
}

void cmdmgr_handle_rcv(cmdmgr_s* cmdmgr, uint8_t* buf, uint8_t len) {
    if (len < 4) return;
    
    // TODO: crc check

    cmdmgr->active.busy = TRUE;
    // cmdmgr->active.orgn = buf[0] - 0x30; // '0' = 0x30
    cmdmgr->active.id = buf[2];
    cmdmgr->active.orgn = buf[3];
	cmdmgr->active.dest = buf[4];
	cmdmgr->active.echo = buf[5];
    cmdmgr->active.prmstr = &buf[6];
    
    if (cmdmgr->active.id > NUM_CMDS-1) return;
    const cmddef_s cmddef = CMD_TABLE[cmdmgr->active.id];

    // Giant top-level switch statement to run command handling routines
    switch (cmddef.id) {
        case 1:
            uint8_t month = buf[6];
            uint8_t day = buf[7];
            uint8_t yr = buf[8];
            uint8_t weekday = buf[9];
            uint8_t hr = buf[10];    
            uint8_t min = buf[11];    
            uint8_t sec = buf[12];    
            break;
    }

    cmdmgr_clear(cmdmgr);
}
