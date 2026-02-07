#include "cmdmgr.h"
#include "cmdfunc.h"
#include "circbuf.h"
#include "hashtable.h"
#include "crcnew.h"
#include <stdint.h>
#include <stdlib.h>

#module

// Command packet 

void cmdpkt_init(cmdpkt_s* pkt) {
    cmdpkt_clear(pkt);
}

void cmdpkt_clear(cmdpkt_s* pkt) {
    pkt->fsm = CMDPKT_FSM_HEAD;
    pkt->i_id = 0;
    pkt->i_args = 0;
    pkt->orgn = 0;
    pkt->dest = 0;
    pkt->echo = 0;
    memset(pkt->id, 0, MAX_CMD_ID_LEN);
    pkt->args_len = 0;
    memset(pkt->args_str, 0, MAX_BUF_LEN);
    pkt->crc = 0;
    pkt->running_crc = 0;
}

// Command Manager

void cmdmgr_init(cmdmgr_s* cmdmgr) {
    uint8_t b;
    for (b = 0; b < NUM_CMD_BUFS; b++) {
        cmdpkt_init(&cmdmgr->rcvpkts[b]);
    }
    cmdmgr_register_funcs(cmdmgr);
}

void cmdmgr_clear(cmdmgr_s* cmdmgr) {
    uint8_t b;
    for (b = 0; b < NUM_CMD_BUFS; b++) {
        cmdpkt_clear(&cmdmgr->rcvpkts[b]);
    }
}

void cmdmgr_rcv_parser(cmdmgr_s* cmdmgr, circbuf_s* irqbuf, cmdpkt_s* rcvpkt) {
    uint16_t iter = 0;
    while (iter < 2*CIRCBUF_MAX_SIZE) {
        uint8_t b = 0;
        if (!cb_pop(irqbuf, 1, &b)) return;

        switch (rcvpkt->fsm) {
            case CMDPKT_FSM_HEAD:
                if (b == 0xCD) {
                    rcvpkt->running_crc = compute_crc16_cont(CRC_RESTART, &b, 1);
                    rcvpkt->fsm = CMDPKT_FSM_ORGN;
                }
                break;
                
            case CMDPKT_FSM_ORGN:
                rcvpkt->orgn = b;
                rcvpkt->running_crc = compute_crc16_cont(CRC_CONTINUE_PREVIOUS, &b, 1);
                rcvpkt->fsm = CMDPKT_FSM_DEST;
                break;

            case CMDPKT_FSM_DEST:
                rcvpkt->dest = b;
                rcvpkt->running_crc = compute_crc16_cont(CRC_CONTINUE_PREVIOUS, &b, 1);
                rcvpkt->fsm = CMDPKT_FSM_ECHO;
                break;

            case CMDPKT_FSM_ECHO:
                rcvpkt->echo = b;
                rcvpkt->running_crc = compute_crc16_cont(CRC_CONTINUE_PREVIOUS, &b, 1);
                rcvpkt->fsm = CMDPKT_FSM_ARGSLEN;
                break;
                
            case CMDPKT_FSM_ARGSLEN:
                rcvpkt->args_len = b;
                rcvpkt->running_crc = compute_crc16_cont(CRC_CONTINUE_PREVIOUS, &b, 1);
                rcvpkt->fsm = CMDPKT_FSM_ID;
                break;

            case CMDPKT_FSM_ID:
                if (rcvpkt->i_id >= MAX_CMD_ID_LEN) {
                    rcvpkt->fsm = CMDPKT_FSM_ERROR;
                    break;
                }
                rcvpkt->id[rcvpkt->i_id++] = b;
                rcvpkt->running_crc = compute_crc16_cont(CRC_CONTINUE_PREVIOUS, &b, 1);
                if (b == ' ') {
                    rcvpkt->id[rcvpkt->i_id-1] = '\0';
                    rcvpkt->fsm = CMDPKT_FSM_ARGSSTR;
                }
                break;

            case CMDPKT_FSM_ARGSSTR:
                if (rcvpkt->i_args >= MAX_BUF_LEN) {
                    rcvpkt->fsm = CMDPKT_FSM_ERROR;
                    break;
                }
                rcvpkt->running_crc = compute_crc16_cont(CRC_CONTINUE_PREVIOUS, &b, 1);
                rcvpkt->args_str[rcvpkt->i_args++] = b;
                if (rcvpkt->i_args >= rcvpkt->args_len) {
                    rcvpkt->args_str[rcvpkt->i_args] = '\0';
                    rcvpkt->fsm = CMDPKT_FSM_CRC1;
                }
                break;

            case CMDPKT_FSM_CRC1:
                rcvpkt->crc = b;
                rcvpkt->fsm = CMDPKT_FSM_CRC2;
                break;
            
            case CMDPKT_FSM_CRC2:
                rcvpkt->crc = ((uint16_t)b << 8) | rcvpkt->crc; // 2 byte CRC stored as little endian in cmd
                rcvpkt->fsm = CMDPKT_FSM_DONE;
                break;
        }

        switch (rcvpkt->fsm) {
            case CMDPKT_FSM_DONE:
                cmdmgr_process_cmd(cmdmgr, rcvpkt);
                cmdpkt_clear(rcvpkt);
                break;
            case CMDPKT_FSM_ERROR:
                fprintf(FTDI_PORT, "%s[%s] cmdmgr_rcv_fsm: malformed packet\n", KRED, NODE_LBL);
                cmdpkt_clear(rcvpkt);
                break;
        }

        iter++;
    }
}

void cmdmgr_process_cmd(cmdmgr_s* cmdmgr, cmdpkt_s* pkt) {
    // Forward
    if (pkt->dest != NODE_ID) {
        fprintf(FTDI_PORT, "%s[%s] forwarding cmd '%s'\n", KCYN, NODE_LBL, pkt->id);
        return;
    } 
    
    // CRC check
    if (pkt->crc != pkt->running_crc) {
        fprintf(FTDI_PORT, "%s[%s] crc check failed on cmd '%s' crc:%u calculated:%u\n", KRED, NODE_LBL,
                pkt->id, pkt->crc, pkt->running_crc);
        return;
    } 
    
    // Parse & execute cmd here
    cmdfunc_f cmdfunc = ht_get(&cmdmgr->cmdfuncs, pkt->id);
    if (cmdfunc == NULL) {
        fprintf(FTDI_PORT, "%s[%s] cmd not recognized '%s'\n", KRED, NODE_LBL, pkt->id);
        return;
    }
    cmdfunc(pkt);
}

// COMMAND HANDLERS

