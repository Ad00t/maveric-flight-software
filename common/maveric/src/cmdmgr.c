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
    cmdmgr_clear(cmdmgr);
    ht_init(&cmdmgr->cmdimpls); 
}

void cmdmgr_clear(cmdmgr_s* cmdmgr) {
    uint8_t b;
    for (b = 0; b < CMDMGR_NUM_BUFS; b++) {
        cmdpkt_clear(&cmdmgr->rcvpkts[b]);
    }
}

void cmdmgr_parse_stream(cmdmgr_s* cmdmgr, ringbuf_s* rcvbuf, cmdpkt_s* pkt, int1 csp) {
    if (rb_len(rcvbuf) < KISS_HEADER_SIZE + CMD_HEADER_SIZE + CMD_FOOTER_SIZE + KISS_FOOTER_SIZE
        + (csp ? CSP_HEADER_SIZE + CRC32_SIZE : 0)) return;

    uint16_t iter;
    for (iter = 0; iter < 2 * RINGBUF_MAX_SIZE; iter++) {
        uint8_t b;
        if (!rb_pop(rcvbuf, 1, &b))
            return;
        
        if (kiss_process_byte(pkt, b)) {
            pkt->i_start = 1;
            pkt->buf_len--;
            if (csp) { // Remove CSP header
                pkt->i_start += CSP_HEADER_SIZE;
                pkt->buf_len -= CRC32_SIZE;
            }
            cmdmgr_process_cmd(cmdmgr, pkt);    
        } 
    }

    // // Extract KISS frame
    // uint8_t frame_buf[CMD_MAX_FRAME_SIZE] = {0};
    // uint16_t frame_len = kiss_extract_frame(rcvbuf, frame_buf, sizeof(frame_buf));
    //
    // // Extract cmd and process it if we have a frame
    // if (frame_len > 0) {
    //     uint8_t cmd_buf[CMD_MAX_FRAME_SIZE] = {0};
    //     uint16_t cmd_len = 0;
    //     cmd_len = frame_len;
    //     kiss_remove_byte_check(frame_buf, frame_len, cmd_buf, &cmd_len, 0);
    //     cmd_len -= KISS_HEADER_SIZE + KISS_FOOTER_SIZE + (csp ? CSP_HEADER_SIZE + CRC32_SIZE : 0);
    //     uint8_t i;
    //     for (i = 0; i < cmd_len; i++)
    //         pkt->buf[i] = cmd_buf[i + KISS_HEADER_SIZE + (csp ? CSP_HEADER_SIZE : 0)];
    //     pkt->buf_len = cmd_len;
    //     cmdmgr_process_cmd(cmdmgr, pkt);    
    // }
}

void cmdmgr_process_cmd(cmdmgr_s* cmdmgr, cmdpkt_s* pkt) {
    uint16_t i;
    uint16_t p = 0;
    p += sprintf(&LOGBUF[p], "cmdmgr_process_cmd: pkt rcvd: len=%u [", pkt->buf_len);
    for (i = pkt->i_start; i < pkt->buf_len; i++) {
        p += sprintf(&LOGBUF[p], " %02X'%c'", pkt->buf[i], ((pkt->buf[i] >= 32 && pkt->buf[i] <= 126) ? pkt->buf[i] : '%')); 
    }
    p += sprintf(&LOGBUF[p], " ]"); log_flush(LL_TRACE);

    // Parse cmdpkt buf into fields
    if (cmdpkt_parse_buf(pkt) != STATUS_OK) {
        sprintf(LOGBUF, "cmdmgr_process_cmd: pkt buf parsing failed: len=%u", pkt->buf_len); log_flush(LL_ERROR);
        goto cleanup;
    }

    // Forward
    if (pkt->dest != NODE_ID) {
        sprintf(LOGBUF, "cmdmgr_process_cmd: forwarding cmd: '%s'", pkt->id); log_flush(LL_INFO);
        cmd_dispatch(pkt);
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

// NOT TIED TO CMDMGR INSTANCE

void cmd_dispatch(cmdpkt_s* pkt) {
    uint8_t frame[CMD_MAX_FRAME_SIZE] = {0};
    int1 csp = NODE_ID == NODE_ID_UPPM && pkt->dest == NODE_ID_GS;
    uint16_t frame_len = cmdpkt_setup_frame(pkt, frame, CMD_MAX_FRAME_SIZE, csp);
#if NODE_ID == NODE_ID_LPPM
    switch (pkt->dest) {
        case NODE_ID_EPS:
            break;
        case NODE_ID_UPPM:
        case NODE_ID_GS:
        case NODE_ID_ASTROBOARD:
        case NODE_ID_HOLONAV:
            uart_write_buf(UPPM_PORT, frame, frame_len);
            break;
    }
#elif NODE_ID == NODE_ID_UPPM
    switch (pkt->dest) {
        case NODE_ID_EPS:
            break;
        case NODE_ID_GS:
            uart_write_buf(AX100_PORT, frame, frame_len);
            break;
        case NODE_ID_ASTROBOARD:
            uart_write_buf(ASTROBOARD_PORT, frame, frame_len);
            break;
        case NODE_ID_HOLONAV:
            uart_write_buf(HOLONAV_PORT, frame, frame_len);
            break;
        case NODE_ID_LPPM:
            // uint16_t i;
            // fprintf(COM_C, "%suppm->lppm len=%u [", KMAG, frame_len);
            // for (i = 0; i < frame_len; i++) {
            //     fprintf(COM_C, " %02X", frame[i]);
            // }
            // fprintf(COM_C, " ]\n");
            uart_write_buf(LPPM_PORT, frame, frame_len);
            break;
    }
#endif
}

void cmd_dispatch(uint8_t orgn, uint8_t dest, uint8_t echo, cmdpkt_type_e ptype, char* id, char* args) {
    cmdpkt_s pkt;
    cmdpkt_create(&pkt, orgn, dest, echo, ptype, id, args);
    cmd_dispatch(&pkt);
}
