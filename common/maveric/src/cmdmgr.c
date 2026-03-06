#include "cmdmgr.h"
#include "ringbuf.h"
#include "hashtable.h"
#include "crcnew.h"
#include "uart.h"
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
    uint16_t n_bytes = rb_len(rcvbuf);

    uint16_t iter;
    for (iter = 0; iter < n_bytes; iter++) {
        uint8_t b = 0;
        if (!rb_pop(rcvbuf, 1, &b)) return;

        if (kiss_process_byte(pkt, b)) {
            // Full frame received
            pkt->i_start = 1;
            pkt->buf_len--;
            if (csp) { // Remove CSP header
                pkt->i_start += CSP_HEADER_SIZE;
                pkt->buf_len -= CSP_HEADER_SIZE + CRC32_SIZE;
            }
            cmdmgr_process_cmd(cmdmgr, pkt);    
            pkt->fsm = KISS_IN_FRAME;
        } 
    }
}

void cmdmgr_process_cmd(cmdmgr_s* cmdmgr, cmdpkt_s* pkt) {
    if (!cmdmgr->is_init) return;

    // Parse cmdpkt buf into fields
    if (cmdpkt_parse_buf(pkt) != STATUS_OK) {
        sprintf(LOGBUF, "cmdmgr_process_cmd: pkt buf parsing failed: len=%u", pkt->buf_len); log_flush(LL_ERROR);
        goto cleanup;
    }

    // Forward
    if (pkt->dest != NODE_ID) {
        sprintf(LOGBUF, "cmdmgr_process_cmd: forwarding cmd: '%s'", pkt->id); log_flush(LL_INFO);
        cmdpkt_dispatch(pkt);
        goto cleanup;
    } 
    
    // CRC check
    uint16_t calc_crc = compute_crc16(&pkt->buf[pkt->i_start], pkt->buf_len - 2);
    if (pkt->crc != calc_crc) {
        sprintf(LOGBUF, "cmdmgr_process_cmd: crc check failed on cmd: '%s' crc=%u calculated=%u",
                pkt->id, pkt->crc, calc_crc); log_flush(LL_ERROR);
        goto cleanup;
    } 
    
    // Find and execute cmd implementation
    cmdimpl_f cmdimpl = ht_get(&cmdmgr->cmdimpls, pkt->id);
    if (cmdimpl == NULL) {
        sprintf(LOGBUF, "cmdmgr_process_cmd: cmd not recognized: '%s'", pkt->id); log_flush(LL_ERROR);
        goto cleanup;
    }
    cmdimpl(pkt);

cleanup:
    cmdpkt_clear(pkt);
}
