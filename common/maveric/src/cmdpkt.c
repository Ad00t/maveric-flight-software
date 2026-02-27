#include "cmdpkt.h"
#include "common.h"

#module

void cmdpkt_init(cmdpkt_s* pkt) {
    cmdpkt_clear(pkt);
}

void cmdpkt_create(cmdpkt_s* pkt, uint8_t orgn, uint8_t dest, uint8_t echo, cmdpkt_type_e ptype, char* id, char* args) {
    cmdpkt_init(pkt);
    uint8_t len = 0;
    uint8_t* buf = pkt->buf;
    uint8_t id_len = strlen(id);
    uint8_t args_len = strlen(id);
    
    buf[len++] = orgn;
    buf[len++] = dest;
    buf[len++] = echo;
    buf[len++] = ptype;
    buf[len++] = id_len;
    buf[len++] = args_len;

    memcpy(&buf[len], id, id_len);
    len += id_len;
    buf[len++] = '\0';
   
    memcpy(&buf[len], args, args_len);
    len += args_len;
    buf[len++] = '\0';
    
    // CRC should only be computed on message, not framing or anything else
    uint16_t crc = compute_crc16(&buf[KISS_HEADER_SIZE], len - KISS_HEADER_SIZE); 
    buf[len++] = crc & 0xFF; 
    buf[len++] = (crc >> 8) & 0xFF; 
    pkt->buf_len = len;
}

void cmdpkt_clear(cmdpkt_s* pkt) {
    memset(pkt, 0, sizeof(cmdpkt_s));
}

uint8_t cmdpkt_parse_buf(cmdpkt_s* pkt) {
    if (pkt->buf_len < 10) return STATUS_ERR;

    uint8_t len = 0;
    pkt->orgn = pkt->buf[len++];
    pkt->dest = pkt->buf[len++];
    pkt->echo = pkt->buf[len++];
    pkt->ptype = pkt->buf[len++];
    pkt->id_len = pkt->buf[len++];
    pkt->args_len = pkt->buf[len++];

    pkt->id = &pkt->buf[len];
    if (len + pkt->id_len >= pkt->buf_len
        || pkt->buf[len + pkt->id_len] != '\0') return STATUS_ERR;
    len += pkt->id_len + 1;
    
    pkt->args = &pkt->buf[len];
    if (len + pkt->args_len >= pkt->buf_len
        || pkt->buf[len + pkt->args_len] != '\0') return STATUS_ERR;
    len += pkt->args_len + 1;
    
    uint8_t crc_low = pkt->buf[len++];
    uint8_t crc_high = pkt->buf[len++];
    pkt->crc = make16(crc_high, crc_low);

    return len == pkt->buf_len ? STATUS_OK : STATUS_ERR;
}

