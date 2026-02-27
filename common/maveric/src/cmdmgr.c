#include "cmdmgr.h"
#include "kiss.h"
#include "ringbuf.h"
#include "hashtable.h"
#include "crcnew.h"
#include "common.h"
#include <stdint.h>
#include <stdlib.h>

#module

// Command packet 

void cmdpkt_init(cmdpkt_s* pkt) {
    cmdpkt_clear(pkt);
}

void cmdpkt_clear(cmdpkt_s* pkt) {
    memset(pkt, 0, sizeof(cmdpkt_s));
}

// Command Manager

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

void cmdmgr_parse_stream(cmdmgr_s* cmdmgr, ringbuf_s* rcvbuf, cmdpkt_s* rcvpkt) {
    uint16_t iter = 0;
    while (iter < 2 * RINGBUF_MAX_SIZE) {
        uint8_t b = 0;
        if (!rb_pop(rcvbuf, 1, &b)) return;

        if (b == FEND) {
            if (rcvpkt->busy) { // Reached end of frame
                cmdmgr_process_cmd(cmdmgr, rcvpkt);    
                cmdpkt_clear(rcvpkt);
            } else { // Detected beginning of frame
                cmdpkt_clear(rcvpkt);
                rcvpkt->busy = TRUE;
                rcvpkt->n_skip = KISS_HEADER_SIZE; // Skip KISS header
            }
        }

        if (rcvpkt->n_skip > 0) { 
            rcvpkt->n_skip--;
            continue;
        }

        rcvpkt->buf[rcvpkt->buf_len++] = b;
        iter++;
    }
}

void cmdmgr_process_cmd(cmdmgr_s* cmdmgr, cmdpkt_s* pkt) {
    // Parse out cmdpkt fields
    uint8_t len = 0;
    pkt->orgn = pkt->buf[len++];
    pkt->dest = pkt->buf[len++];
    pkt->echo = pkt->buf[len++];
    pkt->ptype = pkt->buf[len++];
    pkt->id_len = pkt->buf[len++];
    pkt->args_len = pkt->buf[len++];
    pkt->id = &pkt->buf[len];
    len += pkt->id_len + 1;
    pkt->args = &pkt->buf[len];
    len += pkt->args_len + 1;
    uint8_t crc_low = pkt->buf[len++];
    uint8_t crc_high = pkt->buf[len++];
    pkt->crc = make16(crc_high, crc_low);

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


