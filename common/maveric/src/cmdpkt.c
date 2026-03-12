#include "logger.h"
#include "framer.h"
#include "cmdpkt.h"
#include "crcnew.h"
#include "common.h"

#module

// CMDPKT PUBLIC API

void cmdpkt_init(cmdpkt_s* pkt) {
    cmdpkt_clear(pkt);
}

void cmdpkt_create(cmdpkt_s* pkt, uint8_t orgn, uint8_t dest, uint8_t echo, cmdpkt_type_e ptype, char* id, char* args) {
    cmdpkt_init(pkt);

    uint16_t len = 0;
    kiss_parser_s* p = &pkt->parser;
    uint8_t* buf = p->buf;
    uint8_t id_len = strlen(id);
    uint8_t args_len = strlen(args);
   
    // Add header
    pkt->orgn = orgn;
    buf[len++] = orgn;
    pkt->dest = dest;
    buf[len++] = dest;
    pkt->echo = echo;
    buf[len++] = echo;
    pkt->ptype = ptype;
    buf[len++] = ptype;
    pkt->id_len = id_len;
    buf[len++] = id_len;
    pkt->args_len = args_len;
    buf[len++] = args_len;

    // Add id field
    pkt->id = &buf[len];
    memcpy(&buf[len], id, id_len);
    len += id_len;
    buf[len++] = '\0';
  
    // Add args str
    pkt->args = &buf[len];
    memcpy(&buf[len], args, args_len);
    len += args_len;
    buf[len++] = '\0';
    
    uint16_t crc = compute_crc16(buf, len); 
    pkt->crc = crc;
    buf[len++] = crc & 0xFF; 
    buf[len++] = (crc >> 8) & 0xFF; 
    p->buf_len = len;
    p->i_start = 0;
}

void cmdpkt_clear(cmdpkt_s* pkt) {
    memset(pkt, 0, sizeof(cmdpkt_s));
}

uint8_t cmdpkt_parse_buf(cmdpkt_s* pkt) {
    kiss_parser_s* p = &pkt->parser;
    if (p->buf_len < 10) return STATUS_ERR;
    uint16_t len = 0;
    uint8_t* buf = &p->buf[p->i_start];

    // Parse header fields
    pkt->orgn = buf[len++];
    pkt->dest = buf[len++];
    pkt->echo = buf[len++];
    pkt->ptype = buf[len++];
    pkt->id_len = buf[len++];
    pkt->args_len = buf[len++];
    // sprintf(LOGBUF, "header parsed is=%u o=%u d=%u e=%u t=%u idl=%u al=%u", pkt->i_start, pkt->orgn, pkt->dest, pkt->echo, pkt->ptype, pkt->id_len, pkt->args_len); log_flush(LL_TRACE);

    // Parse id field
    pkt->id = &buf[len];
    if (len + pkt->id_len >= p->buf_len
        || buf[len + pkt->id_len] != '\0') return STATUS_ERR;
    len += pkt->id_len + 1;
    // sprintf(LOGBUF, "id parsed '%s'", pkt->id);log_flush(LL_TRACE);
   
    // Parse args field
    pkt->args = &buf[len];
    if (len + pkt->args_len >= p->buf_len
        || buf[len + pkt->args_len] != '\0') return STATUS_ERR;
    len += pkt->args_len + 1;
    // sprintf(LOGBUF, "args parsed '%s'", pkt->args);log_flush(LL_TRACE);
   
    // Parse CRC as uint16
    uint8_t crc_low = buf[len++];
    uint8_t crc_high = buf[len++];
    pkt->crc = make16(crc_high, crc_low);
    // sprintf(LOGBUF, "crc parsed %u %u %u", pkt->crc, len, pkt->buf_len);log_flush(LL_TRACE);
    return len == p->buf_len ? STATUS_OK : STATUS_ERR;
}

// Handles all packet routing
void cmdpkt_dispatch(cmdpkt_s* pkt) {
    uint8_t frame[FRAME_MAX_SIZE] = {0};
    int1 csp = (NODE == NODE_UPPM && pkt->dest == NODE_GS);
    kiss_parser_s* p = &pkt->parser;
    uint16_t frame_len = framer_create(&p->buf[p->i_start], p->buf_len, frame, csp);
#if NODE == NODE_LPPM
    switch (pkt->dest) {
        case NODE_EPS:
            break;
        case NODE_FTDI:
            uart_write_buf(FTDI_PORT, frame, frame_len);
            break;
        case NODE_UPPM:
        case NODE_GS:
        case NODE_ASTROBOARD:
        case NODE_HOLONAV:
            uart_write_buf(UPPM_PORT, frame, frame_len);
            break;
    }
#elif NODE == NODE_UPPM
    switch (pkt->dest) {
        case NODE_EPS:
            break;
        case NODE_FTDI:
        case NODE_LPPM:
            uart_write_buf(LPPM_PORT, frame, frame_len);
            break;
        case NODE_GS:
            uart_write_buf(AX100_PORT, frame, frame_len);
            break;
        case NODE_ASTROBOARD:
            uart_write_buf(ASTROBOARD_PORT, frame, frame_len);
            break;
        case NODE_HOLONAV:
            uart_write_buf(HOLONAV_PORT, frame, frame_len);
            break;
    }
#endif
}

// Nice little wrapper function for sending commands from anywhere
void cmd_dispatch(uint8_t orgn, uint8_t dest, uint8_t echo, cmdpkt_type_e ptype, char* id, char* args) {
    cmdpkt_s pkt;
    cmdpkt_create(&pkt, orgn, dest, echo, ptype, id, args);
    cmdpkt_dispatch(&pkt);
}
