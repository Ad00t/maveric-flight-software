#include "cmd.h"

#module

// CMD Buffer

void cmdbuf_init(cmdbuf_s* cmdbuf) {
    cmdbuf_clear(cmdbuf);
}

void cmdbuf_clear(cmdbuf_s* cmdbuf) {
    cmdbuf->busy = FALSE;
    cmdbuf->orgn = 0;
    cmdbuf->dest = 0;
    cmdbuf->echo = 0;
    cmdbuf->id = 255;
    cmdbuf->argslen = 0;
    memset(cmdbuf->argstr, 0, MAX_BUF_LEN);
}

// CMD Manager

void cmdmgr_init(cmdmgr_s* cmdmgr) {
    cmdmgr_clear(cmdmgr);
}

void cmdmgr_clear_all(cmdmgr_s* cmdmgr) {
    size_t b;
    for (b = 0; b < NUM_CMD_BUFS; b++) {
        cmdbuf_clear(&cmdmgr->cmdbufs[b]);
    }
}

void cmdmgr_handle_rcv(cmdmgr_s* cmdmgr, uint8_t* buf, uint8_t len) {
    if (len < 4) return;
    
    //Lower PPM routes
    int8 node = 1;
    int8 route1[7]={0,2,3,3,3,3,7};
    int8 route2[3]={0,3,2};

    //Upper PPM routes
    //int node = 3;
    //int route1[7]={1,2,0,4,5,6,1};
    //int route2[3]={2,1,0};

    //EPS BBQ routes
    //int node = 2;
    //int route1[7]={1,0,3,3,3,3,1};
    //int route1[3]={3,0,1};

    unsigned int16 crc_value;

    //unsigned char fix_cmd[27];
    delay_ms(10);
    crc_init(255);
    delay_ms(100);			
    crc_value = crc_calc8(buf, len-1);
    //crc_value = crc_calc8(&fix_cmd,2,8);
    delay_ms(100);
    fprintf(COM_D,"%s[LPPM] solving cmd '%s' len=%u crc=%2u \r\n",
            KCYN, buf, len, crc_value);
    //Get the cmd components
    cmd_get_command(buf,&orgn,&dest,&ech, buf, prms);			
    fprintf(COM_D,"%s[LPPM] orig=%u dest=%u echo=%u\r\n", KCYN, orgn, dest, ech);
    //fprintf(COM_D,"[SYS] command: %s; params: %s\r\n",buf,prms);
    if (dest == node) {
        fprintf(COM_D,"%s[LPPM] decoding cmd '%s' params='%s' route=%u\r\n", KCYN, buf, prms, route1[dest-1]);
    } else {
        fprintf(COM_D,"%s[LPPM] forwarding cmd '%s' to route=%u\r\n", KCYN, buf, route1[dest-1]);
    }
    //delay_ms(1000);

    if (dest == node) {
        fprintf(COM_D,"%s[LPPM] rcv cmd '%s' params='%s' route=%u\r\n", KCYN, buf, prms, route1[dest-1]);
    } else {
        if (dest == route1[6]) {
            fprintf(COM_D,"%s%s\r", KCYN, buf);
        } else if ((dest == route1[2])||(dest == route1[3])||(dest == route1[4])||(dest == route1[5])) {
            fprintf(COM_C,"%s%s\r", KCYN, buf);
        }
    }

    // TODO: crc check

    cmdmgr->active.busy = TRUE;
    cmdmgr->active.orgn = buf[1];
	cmdmgr->active.dest = buf[2];
	cmdmgr->active.echo = buf[3];
    cmdmgr->active.args_len = buf[4];
    cmdmgr->active.id = buf[5];
    cmdmgr->active.args_str = &buf[6];
    
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
