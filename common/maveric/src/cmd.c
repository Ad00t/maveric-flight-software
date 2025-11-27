#include "cmd.h"
#include "circbuf.h"
#include "hashtable.h"
#include "crcnew.h"
#include <stdint.h>
#include <stdlib.h>

#module

// CMD packet 

void cmd_pkt_init(cmd_pkt_s* pkt) {
    cmd_pkt_clear(pkt);
}

void cmd_pkt_clear(cmd_pkt_s* pkt) {
    pkt->fsm = CMD_PKT_FSM_HEAD;
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

// Command func declarations

void cmd_func_set_time(cmd_pkt_s* pkt);

// Command Manager

void cmd_mgr_init(cmd_mgr_s* cmd_mgr) {
    uint8_t b;
    for (b = 0; b < NUM_CMD_BUFS; b++) {
        cmd_pkt_init(&cmd_mgr->rcvpkts[b]);
    }
    ht_init(&cmd_mgr->cmd_funcs); 
    ht_set(&cmd_mgr->cmd_funcs, "cmd_set_time", (cmd_func_f) cmd_func_set_time);
}

void cmd_mgr_clear(cmd_mgr_s* cmd_mgr) {
    uint8_t b;
    for (b = 0; b < NUM_CMD_BUFS; b++) {
        cmd_pkt_clear(&cmd_mgr->rcvpkts[b]);
    }
}

void cmd_mgr_rcv_fsm(cmd_mgr_s* cmd_mgr, circbuf_s* irqbuf, cmd_pkt_s* rcvpkt) {
    uint16_t iter = 0;
    while (iter < CIRCBUF_MAX_SIZE) {
        uint8_t b = 0;
        if (!cb_pop(irqbuf, 1, &b)) return;

        switch (rcvpkt->fsm) {
            case CMD_PKT_FSM_HEAD:
                if (b == 0xCD) {
                    rcvpkt->running_crc = get_crc16_cont(CRC_RESTART, &b, 1);
                    rcvpkt->fsm = CMD_PKT_FSM_ORGN;
                }
                break;
                
            case CMD_PKT_FSM_ORGN:
                rcvpkt->orgn = b;
                rcvpkt->running_crc = get_crc16_cont(CRC_CONTINUE_PREVIOUS, &b, 1);
                rcvpkt->fsm = CMD_PKT_FSM_DEST;
                break;

            case CMD_PKT_FSM_DEST:
                rcvpkt->dest = b;
                rcvpkt->running_crc = get_crc16_cont(CRC_CONTINUE_PREVIOUS, &b, 1);
                rcvpkt->fsm = CMD_PKT_FSM_ECHO;
                break;

            case CMD_PKT_FSM_ECHO:
                rcvpkt->echo = b;
                rcvpkt->running_crc = get_crc16_cont(CRC_CONTINUE_PREVIOUS, &b, 1);
                rcvpkt->fsm = CMD_PKT_FSM_ARGSLEN;
                break;
                
            case CMD_PKT_FSM_ARGSLEN:
                rcvpkt->args_len = b;
                rcvpkt->running_crc = get_crc16_cont(CRC_CONTINUE_PREVIOUS, &b, 1);
                rcvpkt->fsm = CMD_PKT_FSM_ID;
                break;

            case CMD_PKT_FSM_ID:
                if (rcvpkt->i_id >= MAX_CMD_ID_LEN) {
                    rcvpkt->fsm = CMD_PKT_FSM_ERROR;
                    break;
                }
                rcvpkt->id[rcvpkt->i_id++] = b;
                rcvpkt->running_crc = get_crc16_cont(CRC_CONTINUE_PREVIOUS, &b, 1);
                if (b == ' ') {
                    rcvpkt->id[rcvpkt->i_id-1] = '\0';
                    rcvpkt->fsm = CMD_PKT_FSM_ARGSSTR;
                }
                break;

            case CMD_PKT_FSM_ARGSSTR:
                if (rcvpkt->i_args >= MAX_BUF_LEN) {
                    rcvpkt->fsm = CMD_PKT_FSM_ERROR;
                    break;
                }
                rcvpkt->running_crc = get_crc16_cont(CRC_CONTINUE_PREVIOUS, &b, 1);
                if (b == ' ') b = '\0';
                rcvpkt->args_str[rcvpkt->i_args++] = b;
                if (rcvpkt->i_args >= rcvpkt->args_len) {
                    rcvpkt->id[rcvpkt->i_args] = '\0';
                    rcvpkt->fsm = CMD_PKT_FSM_CRC1;
                }
                break;

            case CMD_PKT_FSM_CRC1:
                rcvpkt->crc = b;
                rcvpkt->fsm = CMD_PKT_FSM_CRC2;
                break;
            
            case CMD_PKT_FSM_CRC2:
                rcvpkt->crc = ((uint16_t)b << 8) | rcvpkt->crc; // 2 byte CRC stored as little endian in cmd
                rcvpkt->fsm = CMD_PKT_FSM_DONE;
                break;
        }

        switch (rcvpkt->fsm) {
            case CMD_PKT_FSM_DONE:
                cmd_mgr_process_cmd(cmd_mgr, rcvpkt);
                cmd_pkt_clear(rcvpkt);
                break;
            case CMD_PKT_FSM_ERROR:
                fprintf(COM_D, "%s[LPPM] cmd_mgr_rcv_fsm: malformed packet\n", KRED);
                cmd_pkt_clear(rcvpkt);
                break;
        }

        iter++;
    }
}

void cmd_mgr_process_cmd(cmd_mgr_s* cmd_mgr, cmd_pkt_s* pkt) {
    // Forward
    if (pkt->dest != NODE_ID) {
        fprintf(COM_D, "%s[LPPM] forwarding cmd '%s'", KRED, pkt->id);
        return;
    } 
    
    // CRC check 
    if (pkt->crc != pkt->running_crc) {
        fprintf(COM_D, "%s[LPPM] crc check failed on cmd '%s' crc:%u calculated:%u\n", KRED, pkt->id, pkt->crc, pkt->running_crc);
        return;
    } 
    
    cmd_func_f cmd_func = ht_get(&cmd_mgr->cmd_funcs, pkt->id);

    if (cmd_func == NULL) {
        fprintf(COM_D, "%s[LPPM] cmd not recognized '%s'\n", KRED, pkt->id);
        return;
    }

    cmd_func(pkt);
}

// COMMAND HANDLERS

void cmd_func_set_time(cmd_pkt_s* pkt) {
    uint8_t* astr = pkt->args_str;
    uint8_t month = atoi(&astr[0]);
    uint8_t day = atoi(&astr[2]);
    uint8_t yr = atoi(&astr[4]);
    uint8_t weekday = atoi(&astr[6]);
    uint8_t hr = atoi(&astr[8]); 
    uint8_t min = atoi(&astr[10]);    
    uint8_t sec = atoi(&astr[12]);    

    fprintf(COM_D, "%s[LPPM] cmd_func_set_time [ %u %u %u %u %u %u %u ]\n", KCYN, month, day, yr, weekday, hr, min, sec);
}
