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
    if (rb_len(rcvbuf) < KISS_HEADER_SIZE + CMD_HEADER_SIZE + CMD_FOOTER_SIZE + KISS_FOOTER_SIZE) return;
    
    // uint8_t i;
    // for (i = 0; i < rb_len(rcvbuf); i++) {
    //     uint8_t b = 0;
    //     rb_peek(rcvbuf, i, &b);
    //     fprintf(COM_D, "%u:%02X ", i, b);
    // }
    // fprintf(COM_D, "\n");

    // Extract KISS frame
    uint8_t frame_buf[CMD_MAX_FRAME_SIZE] = {0};
    uint16_t frame_len = kiss_extract_frame(rcvbuf, frame_buf, sizeof(frame_buf));

    // Extract cmd and process it if we have a frame
    if (frame_len > 0) {
        uint8_t cmd_buf[CMD_MAX_FRAME_SIZE] = {0};
        uint16_t cmd_len = 0;
        cmd_len = frame_len;
        // kiss_remove_byte_check(frame_buf, frame_len, cmd_buf, &cmd_len, 0);
        cmd_len -= KISS_HEADER_SIZE + KISS_FOOTER_SIZE;
        uint8_t i;
        for (i = 0; i < cmd_len; i++)
            pkt->buf[i] = frame_buf[i + KISS_HEADER_SIZE];
        pkt->buf_len = cmd_len;
        cmdmgr_process_cmd(cmdmgr, pkt);    
    }
}

void cmdmgr_process_cmd(cmdmgr_s* cmdmgr, cmdpkt_s* pkt) {
    uint8_t i;
    fprintf(COM_D, "buf: len=%u [", pkt->buf_len);
    for (i = 0; i < pkt->buf_len; i++) {
        fprintf(COM_D, " %u:0x%02X'%c'", i, pkt->buf[i], 
                (pkt->buf[i] >= 32 && pkt->buf[i] <= 126) ? pkt->buf[i] : '%');
    }
    fprintf(COM_D, " ]\n");

    // Parse cmdpkt buf into fields
    if (cmdpkt_parse_buf(pkt) != STATUS_OK) {
        sprintf(LOGBUF, "cmdmgr_process_cmd: packet buffer parsing failed len=%u", pkt->buf_len); log_flush(LL_ERROR);
        cmdpkt_clear(pkt);
        return;
    }

    fprintf(COM_D, "cmd parse %s\n", pkt->id);
    
    // Forward
    if (pkt->dest != NODE_ID) {
        sprintf(LOGBUF, "cmdmgr_process_cmd: forwarding cmd '%s'", pkt->id); log_flush(LL_INFO);
        cmdpkt_clear(pkt);
        return;
    } 
    
    // CRC check
    uint16_t calc_crc = compute_crc16(pkt->buf, pkt->buf_len - 2);
    if (pkt->crc != calc_crc) {
        sprintf(LOGBUF, "cmdmgr_process_cmd: crc check failed on cmd '%s' crc:%u calculated:%u",
                pkt->id, pkt->crc, calc_crc); log_flush(LL_ERROR);
        cmdpkt_clear(pkt);
        return;
    } 
    
    fprintf(COM_D, "cmd exec %s\n", pkt->id);
    // Find and execute cmd implementation
    if (pkt->ptype == REQUEST) {
        cmdimpl_f cmdimpl = ht_get(&cmdmgr->cmdimpls, pkt->id);
        if (cmdimpl == NULL) {
            sprintf(LOGBUF, "cmdmgr_process_cmd: cmd not recognized '%s'", pkt->id); log_flush(LL_ERROR);
            return;
        }
        cmdimpl(pkt);
    }

    cmdpkt_clear(pkt);
}


