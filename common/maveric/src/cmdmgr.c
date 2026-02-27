#include "cmdmgr.h"
#include "kiss.h"
#include "ringbuf.h"
#include "hashtable.h"
#include "crcnew.h"
#include "cmdpkt.h"
#include "common.h"
#include <stdint.h>
#include <stdlib.h>

#module

void cmdmgr_init(cmdmgr_s* cmdmgr) {
    cmdmgr_clear(cmdmgr);
    ht_init(&cmdmgr->cmdimpls); 
}

void cmdmgr_clear(cmdmgr_s* cmdmgr) {
    uint8_t b;
    for (b = 0; b < CMD_NUM_BUFS; b++) {
        cmdpkt_clear(&cmdmgr->rcvpkts[b]);
    }
}

void cmdmgr_parse_stream(cmdmgr_s* cmdmgr, ringbuf_s* rcvbuf, cmdpkt_s* pkt) {
    uint16_t iter;
    for (iter = 0; iter < 2 * RINGBUF_MAX_SIZE; iter++) {
        uint8_t b = 0;
        if (!rb_pop(rcvbuf, 1, &b)) return;

        // Accumulate bytes within a frame
        if (b != FEND) {
            if (pkt->buf_len >= CMD_MAX_LEN) {
                sprintf(LOGBUF, "cmdmgr_parse_stream: buffer length exceeded"); log_flush(LL_ERROR);
                cmdpkt_clear(pkt);
            }
            if (pkt->n_skip > 0) { 
                pkt->n_skip--;
                continue;
            }
            pkt->buf[pkt->buf_len++] = b;
            continue;
        }

        // Packet start or end detected
        
        // Start of frame detected
        if (!pkt->busy) { 
            cmdpkt_clear(pkt);
            pkt->busy = TRUE;
            pkt->n_skip = 1; // Skip KISS header
            continue;
        }

        // End of frame detected
        cmdmgr_process_cmd(cmdmgr, pkt);
    }
}

void cmdmgr_process_cmd(cmdmgr_s* cmdmgr, cmdpkt_s* pkt) {
    // Parse cmdpkt buf into fields
    if (cmdpkt_parse_buf(pkt) != STATUS_OK) {
        sprintf(LOGBUF, "cmdmgr_process_cmd: packet buffer parsing failed len=%u", pkt->buf_len); log_flush(LL_ERROR);
        cmdpkt_clear(pkt);
        return;
    }

    // Forward
    if (pkt->dest != NODE_ID) {
        sprintf(LOGBUF, "cmdmgr_process_cmd: forwarding cmd '%s'", pkt->id); log_flush(LL_INFO);
        return;
    } 
    
    // CRC check
    uint16_t calc_crc = compute_crc16(pkt->buf, pkt->buf_len - 2);
    if (pkt->crc != calc_crc) {
        sprintf(LOGBUF, "cmdmgr_process_cmd: crc check failed on cmd '%s' crc:%u calculated:%u",
                pkt->id, pkt->crc, calc_crc); log_flush(LL_ERROR);
        return;
    } 
    
    // Find and execute cmd implementation
    if (pkt->ptype == REQUEST) {
        cmdimpl_f cmdimpl = ht_get(&cmdmgr->cmdimpls, pkt->id);
        if (cmdimpl == NULL) {
            sprintf(LOGBUF, "cmdmgr_process_cmd: cmd not recognized '%s'", pkt->id); log_flush(LL_ERROR);
            return;
        }
        cmdimpl(pkt);
    }
}


