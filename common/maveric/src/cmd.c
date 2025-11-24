#include "cmd.h"
#include "circbuf.h"
#include <cstddef>

#module

// CMD Buffer

void cmdpkt_init(cmdpkt_s* cmdpkt) {
    cmdpkt_clear(cmdpkt);
}

void cmdpkt_clear(cmdpkt_s* cmdpkt) {
    cmdpkt->fsm = CMDPKT_FSM_HEAD;
    cmdpkt->i_id = 0;
    cmdpkt->i_args = 0;
    cmdpkt->orgn = 0;
    cmdpkt->dest = 0;
    cmdpkt->echo = 0;
    memset(cmdpkt->id, 0, MAX_CMD_ID_LEN);
    cmdpkt->argslen = 0;
    memset(cmdpkt->argstr, 0, MAX_BUF_LEN);
    cmdpkt->crc = 0;
}

// CMD Manager

void cmdmgr_init(cmdmgr_s* cmdmgr) {
    cmdmgr_clear(cmdmgr);
}

void cmdmgr_clear(cmdmgr_s* cmdmgr) {
    size_t b;
    for (b = 0; b < NUM_CMD_BUFS; b++) {
        cmdpkt_clear(&cmdmgr->rcvbufs[b]);
    }
}

void cmdmgr_rcv_fsm(cmdmgr_s* cmdmgr, circbuf_s* irqbuf, cmdpkt_s* rcvpkt) {
    uint8_t b;
    if (!cb_pop(irqbuf, 1, &b)) return;

    switch (rcvpkt->fsm) {
        case CMDPKT_FSM_HEAD:
            if (b == 0xCD) {
                rcvpkt->fsm = CMDPKT_FSM_ORGN;
            }
            break;
            
        case CMDPKT_FSM_ORGN:
            rcvpkt->orgn = b;
            rcvpkt->fsm = CMDPKT_FSM_DEST;
            break;

        case CMDPKT_FSM_DEST:
            rcvpkt->dest = b;
            rcvpkt->fsm = CMDPKT_FSM_ECHO;
            break;

        case CMDPKT_FSM_ECHO:
            rcvpkt->echo = b;
            rcvpkt->fsm = CMDPKT_FSM_ID;
            break;

        case CMDPKT_FSM_ID:
            rcvpkt->id[rcvpkt->i_id++] = b;
            if (rcvpkt->i_id >= MAX_CMD_ID_LEN || b == ' ') {
                rcvpkt->id[rcvpkt->i_id-1] = '\0';
                rcvpkt->fsm = CMDPKT_FSM_ARGSLEN;
            }
            break;

        case CMDPKT_FSM_ARGSLEN:
            rcvpkt->argslen = b;
            rcvpkt->fsm = CMDPKT_FSM_ARGSLEN;
            break;

        case CMDPKT_FSM_ARGSSTR:
            rcvpkt->argsstr[rcvpkt->i_args++] = b;
            if (rcvpkt->i_args >= MAX_BUF_LEN) {
                rcvpkt->fsm = CMDPKT_FSM_ERROR;
            } else if (rcvpkt->i_args >= rcvpkt->argslen) {
                rcvpkt->fsm = CMDPKT_FSM_CRC;
            }
            break;

        case CMDPKT_FSM_CRC:
            rcvpkt->crc = b;
            rcvpkt->fsm = CMDPKT_FSM_DONE;
            break;
    }

    switch (rcvpkt->fsm) {
        case CMDPKT_FSM_DONE:
            cmdmgr_run_cmd(cmdmgr, rcvpkt);
        case CMDPKT_FSM_ERROR:
            rcvpkt_clear(rcvpkt);
            break;
    }
}

void cmdmgr_run_cmd(cmdmgr_s* cmdmgr, cmdpkt_s* pkt) {
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
    crc_value = crc_calc8(pkt, len-1);
    //crc_value = crc_calc8(&fix_cmd,2,8);
    delay_ms(100);
    fprintf(COM_D,"%s[LPPM] solving cmd '%s' len=%u crc=%2u \r\n",
            KCYN, pkt, len, crc_value);
    //Get the cmd components
    cmd_get_command(pkt,&orgn,&dest,&ech, pkt, prms);			
    fprintf(COM_D,"%s[LPPM] orig=%u dest=%u echo=%u\r\n", KCYN, orgn, dest, ech);
    //fprintf(COM_D,"[SYS] command: %s; params: %s\r\n",pkt,prms);
    if (dest == node) {
        fprintf(COM_D,"%s[LPPM] decoding cmd '%s' params='%s' route=%u\r\n", KCYN, pkt, prms, route1[dest-1]);
    } else {
        fprintf(COM_D,"%s[LPPM] forwarding cmd '%s' to route=%u\r\n", KCYN, pkt, route1[dest-1]);
    }
    //delay_ms(1000);

    if (dest == node) {
        fprintf(COM_D,"%s[LPPM] rcv cmd '%s' params='%s' route=%u\r\n", KCYN, pkt, prms, route1[dest-1]);
    } else {
        if (dest == route1[6]) {
            fprintf(COM_D,"%s%s\r", KCYN, pkt);
        } else if ((dest == route1[2])||(dest == route1[3])||(dest == route1[4])||(dest == route1[5])) {
            fprintf(COM_C,"%s%s\r", KCYN, pkt);
        }
    }

    // TODO: crc check

    // Giant top-level switch statement to run command handling routines
    switch (cmddef.id) {
        case 1:
            uint8_t month = pkt[6];
            uint8_t day = pkt[7];
            uint8_t yr = pkt[8];
            uint8_t weekday = pkt[9];
            uint8_t hr = pkt[10];    
            uint8_t min = pkt[11];    
            uint8_t sec = pkt[12];    
            break;
    }

    cmdmgr_clear(cmdmgr);
}
