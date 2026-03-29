#include "cmdmgr.h"
#include "ringbuf.h"
#include "hashtable.h"
#include "crcnew.h"
#include "uart.h"
#include "framer.h"
#include "cmdpkt.h"
#include "common.h"
#include "logger.h"
#include <stdint.h>
#include <stdlib.h>

#module

void cmdmgr_init(cmdmgr_s* cmdmgr) {
    cmdmgr->is_init = TRUE;
    cmdmgr_clear(cmdmgr);
    ht_init(&cmdmgr->cmdimpls); 
}

void cmdmgr_clear(cmdmgr_s* cmdmgr) {
    if (!cmdmgr->is_init) return;
    uint8_t b;
    for (b = 0; b < CMDMGR_NUM_BUFS; b++) {
        cmdpkt_clear(&cmdmgr->rcvpkts[b]);
    }
}

void cmdmgr_parse_stream(cmdmgr_s* cmdmgr, ringbuf_s* rcvbuf, cmdpkt_s* pkt, int1 csp) {
    if (!cmdmgr->is_init) return;
    kiss_parser_s* p = &pkt->parser;
    uint16_t n_bytes = rb_len(rcvbuf);

    uint16_t iter;
    for (iter = 0; iter < n_bytes; iter++) {
        uint8_t b = 0;
        if (!rb_pop(rcvbuf, 1, &b)) return;

        // if (b == FEND) {
        //     fprintf(COM_D, "%s%02X ", KRED, b);
        // } else {
        //     fprintf(COM_D, "%s%02X ", KYEL, b);
        // }

        if (kiss_process_byte(p, b)) {
            // fprintf(COM_D, "%sFRAME\n", KGRN);
            // cmdpkt_clear(pkt);
            // p->fsm = KISS_IN_FRAME;
            // continue;
            
            // Full frame received
            p->i_start = 1;
            p->buf_len--;
            if (csp) { // Remove CSP header
                p->i_start += CSP_HEADER_SIZE;
                p->buf_len -= CSP_HEADER_SIZE + CRC32_SIZE;
            }
            cmdmgr_process_cmd(cmdmgr, pkt);    
            p->fsm = KISS_IN_FRAME;
        } 
    }
}

void cmdmgr_process_cmd(cmdmgr_s* cmdmgr, cmdpkt_s* pkt) {
    if (!cmdmgr->is_init) return;
    kiss_parser_s* p = &pkt->parser;

    // Parse cmdpkt buf into fields
    if (cmdpkt_parse_buf(pkt) != SUCCESS) {
        sprintf(LOGBUF, "proc_cmd: pkt buf parsing failed: len=%u", p->buf_len); log_error();
        goto cleanup;
    }

    sprintf(LOGBUF, "proc_cmd: parsed o=%u d=%u e=%u p=%u id='%s' arglen=%u",
            pkt->orgn, pkt->dest, pkt->echo, pkt->ptype, pkt->id, pkt->args_len); log_debug();

    // ACK every request packet a node receives from GS or FTDI
    if (pkt->ptype == REQ && (pkt->orgn == NODE_GS || pkt->orgn == NODE_FTDI)) {
        sprintf(LOGBUF, "proc_cmd: ack o=%u d=%u e=%u p=%u id='%s' arglen=%u",
                pkt->orgn, pkt->dest, pkt->echo, pkt->ptype, pkt->id, pkt->args_len); log_debug();
        cmd_respond(pkt, ACK, "");
        delay_ms(100);
    }

    // Forward
    if (pkt->dest != NODE) {
        sprintf(LOGBUF, "proc_cmd: forwarding: o=%u d=%u e=%u p=%u id='%s' arglen=%u", 
                pkt->orgn, pkt->dest, pkt->echo, pkt->ptype, pkt->id, pkt->args_len); log_debug();
        cmdpkt_dispatch(pkt);
        goto cleanup;
    } 
    
    // CRC check
    uint16_t calc_crc = compute_crc16(&p->buf[p->i_start], p->buf_len - 2);
    if (pkt->crc != calc_crc) {
        sprintf(LOGBUF, "proc_cmd: crc check failed on cmd: '%s' crc=%u calculated=%u",
                pkt->id, pkt->crc, calc_crc); log_error();
        goto cleanup;
    } 
    
    // // Don't continue handling rcvd ACKs
    // if (pkt->ptype != REQ && pkt->ptype != RES) {
    //     goto cleanup;
    // }

    // Find and execute cmd implementation
    cmdimpl_f cmdimpl = ht_get(&cmdmgr->cmdimpls, pkt->id);
    if (cmdimpl == NULL) {
        sprintf(LOGBUF, "proc_cmd: cmd not recognized: '%s'", pkt->id); log_error();
        goto cleanup;
    }
    sprintf(LOGBUF, "proc_cmd: executing: o=%u d=%u e=%u p=%u id='%s' arglen=%u", 
            pkt->orgn, pkt->dest, pkt->echo, pkt->ptype, pkt->id, pkt->args_len); log_debug();
    cmdimpl(pkt);

cleanup:
    cmdpkt_clear(pkt);
}

void cmd_process(cmdmgr_s* cmdmgr, uint8_t orgn, uint8_t dest, uint8_t echo, cmdpkt_type_e ptype, char* id, uint8_t* args, uint8_t args_len) {
    cmdpkt_s pkt;
    cmdpkt_create(&pkt, orgn, dest, echo, ptype, id, args, args_len);
    cmdmgr_process_cmd(cmdmgr, &pkt);
}

void cmd_process(cmdmgr_s* cmdmgr, uint8_t orgn, uint8_t dest, uint8_t echo, cmdpkt_type_e ptype, char* id, char* args) {
    cmdpkt_s pkt;
    cmdpkt_create(&pkt, orgn, dest, echo, ptype, id, args);
    cmdmgr_process_cmd(cmdmgr, &pkt);
}
