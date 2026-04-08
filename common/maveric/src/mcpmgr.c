#include "mcpmgr.h"
#include "ringbuf.h"
#include "hashtable.h"
#include "crcnew.h"
#include "uart.h"
#include "frame.h"
#include "mcppkt.h"
#include "common.h"
#include "logger.h"
#include <stdint.h>
#include <stdlib.h>

#module

void mcpmgr_init(mcpmgr_s* mcpmgr) {
    mcpmgr->is_init = TRUE;
    mcpmgr_clear(mcpmgr);
    ht_init(&mcpmgr->cmdimpls); 
}

void mcpmgr_clear(mcpmgr_s* mcpmgr) {
    if (!mcpmgr->is_init) return;
    uint8_t b;
    for (b = 0; b < MCPMGR_NUM_BUFS; b++) {
        mcppkt_clear(&mcpmgr->rcvpkts[b]);
    }
}

void mcpmgr_parse_stream(mcpmgr_s* mcpmgr, ringbuf_s* rcvbuf, mcppkt_s* pkt, int1 csp) {
    if (!mcpmgr->is_init) return;
    kiss_parser_s* p = &pkt->parser;
    // uint16_t n_bytes = rb_len(rcvbuf);

    uint16_t iter;
    for (iter = 0; iter < 2*RINGBUF_MAX_CAPACITY; iter++) {
        uint8_t b = 0;
        if (!rb_pop(rcvbuf, 1, &b)) return;

        // char c = (b >= 32 && b <= 126) ? b : '.';
        // if (b == FEND) {
        //     fprintf(COM_A, "%s%u:%02X'%c' ", KMAG, p->buf_len, b, c);
        // } else {
        //     fprintf(COM_A, "%s%u:%02X'%c' ", KYEL, p->buf_len, b, c);
        // }

        if (kiss_process_byte(p, b)) {
            // fprintf(COM_A, "%sFRAME\r\n\n", KGRN);
            // mcppkt_clear(pkt);
            // p->fsm = KISS_WAIT_FEND;
            // continue;
            
            // Full frame received
            p->i_start = 1;
            p->buf_len--;
            if (csp) { // Remove CSP header
                p->i_start += CSP_HEADER_SIZE;
                p->buf_len -= (CSP_HEADER_SIZE + CRC32_SIZE);
            }
            mcpmgr_process_pkt(mcpmgr, pkt);    
        } 
    }
}

void mcpmgr_process_pkt(mcpmgr_s* mcpmgr, mcppkt_s* pkt) {
    if (!mcpmgr->is_init) return;
    kiss_parser_s* p = &pkt->parser;

    // Parse mcppkt buf into fields
    if (mcppkt_parse_buf(pkt) != SUCCESS) {
        sprintf(LOGBUF, "proc_pkt: pkt buf parsing failed: len=%u", p->buf_len); log_error();
        // fprintf(COM_A, "%sproc_pkt: pkt buf parsing failed: len=%u\r\n\n", KRED, p->buf_len);
        goto cleanup;
    }

    sprintf(LOGBUF, "proc_pkt: parsed o=%u d=%u e=%u p=%u id='%s' arglen=%u",
            pkt->orgn, pkt->dest, pkt->echo, pkt->ptype, pkt->id, pkt->args_len); log_debug();
    // fprintf(COM_A, "%sproc_pkt: parsed o=%u d=%u e=%u p=%u id='%s' arglen=%u\r\n\n", KGRN,
    //         pkt->orgn, pkt->dest, pkt->echo, pkt->ptype, pkt->id, pkt->args_len);

    // ACK every CMD packet a node receives from GS or FTDI
    if (pkt->ptype == CMD && (pkt->orgn == NODE_GS || pkt->orgn == NODE_FTDI)) {
        sprintf(LOGBUF, "proc_pkt: acking o=%u d=%u e=%u p=%u id='%s' arglen=%u",
                pkt->orgn, pkt->dest, pkt->echo, pkt->ptype, pkt->id, pkt->args_len); log_debug();
        mcp_respond(pkt, ACK, (uint8_t*) pkt->args, pkt->args_len);
    }

    // Forward
    if (pkt->dest != NODE) {
        sprintf(LOGBUF, "proc_pkt: forwarding: o=%u d=%u e=%u p=%u id='%s' arglen=%u", 
                pkt->orgn, pkt->dest, pkt->echo, pkt->ptype, pkt->id, pkt->args_len); log_debug();
        mcppkt_dispatch(pkt);
        goto cleanup;
    } 
    
    // CRC check
    uint16_t calc_crc = compute_crc16(&p->buf[p->i_start], p->buf_len - 2);
    if (pkt->crc != calc_crc) {
        sprintf(LOGBUF, "proc_pkt: crc check failed on mcp: '%s' crc=%u calculated=%u",
                pkt->id, pkt->crc, calc_crc); log_error();
        goto cleanup;
    } 
   
    // Find and execute cmd implementation
    cmdimpl_f cmdimpl = ht_get(&mcpmgr->cmdimpls, pkt->id);
    if (cmdimpl == NULL) {
        sprintf(LOGBUF, "proc_pkt: mcp not recognized: '%s'", pkt->id); log_error();
        goto cleanup;
    }
    sprintf(LOGBUF, "proc_pkt: executing: o=%u d=%u e=%u p=%u id='%s' arglen=%u", 
            pkt->orgn, pkt->dest, pkt->echo, pkt->ptype, pkt->id, pkt->args_len); log_info();
    cmdimpl(pkt);

cleanup:
    mcppkt_clear(pkt);
    p->fsm = KISS_WAIT_FEND;
}

void mcp_process(mcpmgr_s* mcpmgr, uint8_t orgn, uint8_t dest, uint8_t echo, mcppkt_type_e ptype, char* id, uint8_t* args, uint8_t args_len) {
    mcppkt_s pkt;
    mcppkt_create(&pkt, orgn, dest, echo, ptype, id, args, args_len);
    mcpmgr_process_pkt(mcpmgr, &pkt);
}

void mcp_process(mcpmgr_s* mcpmgr, uint8_t orgn, uint8_t dest, uint8_t echo, mcppkt_type_e ptype, char* id, char* args) {
    mcppkt_s pkt;
    mcppkt_create(&pkt, orgn, dest, echo, ptype, id, args);
    mcpmgr_process_pkt(mcpmgr, &pkt);
}
