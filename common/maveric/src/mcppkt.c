#include "mcppkt.h"
#include "logger.h"
#include "frame.h"
#include "crcnew.h"
#include "common.h"
#include "i2c.h"

#module

// MCPPKT PUBLIC API

void mcppkt_init(mcppkt_s* pkt) {
    mcppkt_clear(pkt);
}

void mcppkt_create(mcppkt_s* pkt, uint8_t orgn, uint8_t dest, uint8_t echo, mcppkt_type_e ptype, char* id, uint8_t* args, uint8_t args_len) {
    mcppkt_init(pkt);

    uint16_t len = 0;
    kiss_parser_s* p = &pkt->parser;
    uint8_t* buf = p->buf;
    uint8_t id_len = strlen(id);
   
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

void mcppkt_create(mcppkt_s* pkt, uint8_t orgn, uint8_t dest, uint8_t echo, mcppkt_type_e ptype, char* id, char* args) {
    mcppkt_create(pkt, orgn, dest, echo, ptype, id, args, strlen(args));
}


void mcppkt_clear(mcppkt_s* pkt) {
    memset(&pkt->parser, 0, sizeof(kiss_parser_s));
    memset(pkt, 0, sizeof(mcppkt_s));
}

status_e mcppkt_parse_buf(mcppkt_s* pkt) {
    kiss_parser_s* p = &pkt->parser;
    if (p->buf_len < 10) return FAILURE;
    uint16_t len = 0;
    uint8_t* buf = &p->buf[p->i_start];

    // Parse header fields
    pkt->orgn = buf[len++];
    pkt->dest = buf[len++];
    pkt->echo = buf[len++];
    pkt->ptype = buf[len++];
    pkt->id_len = buf[len++];
    pkt->args_len = buf[len++];
    // sprintf(LOGBUF, "header parsed is=%u o=%u d=%u e=%u t=%u idl=%u al=%u", 
    //         p->i_start, pkt->orgn, pkt->dest, pkt->echo, pkt->ptype, pkt->id_len, pkt->args_len); log_debug();

    // Parse id field
    pkt->id = &buf[len];
    if (len + pkt->id_len > p->buf_len || buf[len + pkt->id_len] != '\0')
        return FAILURE;
    len += pkt->id_len + 1;
    // sprintf(LOGBUF, "id parsed '%s'", pkt->id); log_debug();
   
    // Parse args field
    pkt->args = &buf[len];
    if (pkt->args_len > 0) {
        if (len + pkt->args_len > p->buf_len || buf[len + pkt->args_len] != '\0') 
            return FAILURE;
        // sprintf(LOGBUF, "args parsed id='%s' end=%u", pkt->id, pkt->args[pkt->args_len-1]); log_debug();
    } else {
        // sprintf(LOGBUF, "args empty"); log_debug();
    }
    len += pkt->args_len + 1;
   
    // Parse CRC as uint16
    uint8_t crc_low = buf[len++];
    uint8_t crc_high = buf[len++];
    pkt->crc = make16(crc_high, crc_low);
    // sprintf(LOGBUF, "crc parsed id='%s' %u %u %u", pkt->id, pkt->crc, len, p->buf_len); log_debug();
    return len == p->buf_len ? SUCCESS : FAILURE;
}

#if NODE == NODE_UPPM
extern i2cmgr_s g_i2cmgr;
#endif

// Handles all packet routing
void mcppkt_dispatch(mcppkt_s* pkt) {
    uint8_t frame[FRAME_MAX_SIZE] = {0};
    int1 csp = (NODE == NODE_UPPM && pkt->dest == NODE_GS);
    kiss_parser_s* p = &pkt->parser;
    uint16_t frame_len = frame_create(&p->buf[p->i_start], p->buf_len, frame, csp);
#if NODE == NODE_LPPM
    switch (pkt->dest) {
        case NODE_FTDI:
            uart_write_buf(FTDI_PORT, frame, frame_len);
            break;
        case NODE_EPS:
        case NODE_UPPM:
        case NODE_GS:
        case NODE_ASTROBOARD:
        case NODE_HOLONAV:
            uart_write_buf(UPPM_PORT, frame, frame_len);
            break;
    }
#elif NODE == NODE_UPPM
    switch (pkt->dest) {
        case NODE_EPS: {
            i2c_write_buf(I2C_1, 0x18, frame, frame_len);
            delay_ms(50);
            uint8_t res[FRAME_MAX_SIZE] = {0}; 
            i2c_read_buf(I2C_1, 0x18, res, I2C_MAX_SIZE-1);
            rb_push_n(&g_i2cmgr.rxbufs[0], res, I2C_MAX_SIZE-1);
            break;
        }
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
void mcp_dispatch(uint8_t orgn, uint8_t dest, uint8_t echo, mcppkt_type_e ptype, char* id, uint8_t* args, uint8_t args_len) {
    mcppkt_s pkt = {0};
    mcppkt_create(&pkt, orgn, dest, echo, ptype, id, args, args_len);
    mcppkt_dispatch(&pkt);
}

void mcp_dispatch(uint8_t orgn, uint8_t dest, uint8_t echo, mcppkt_type_e ptype, char* id, char* args) {
    mcp_dispatch(orgn, dest, echo, ptype, id, args, strlen(args));
}

// Public helpers 

void mcp_respond(mcppkt_s* pkt, mcppkt_type_e type, uint8_t* res, uint8_t res_len) {
    mcp_dispatch(NODE, pkt->orgn, pkt->echo, type, pkt->id, res, res_len); 
}

void mcp_respond(mcppkt_s* pkt, mcppkt_type_e type, char* res) {
    mcp_respond(pkt, type, res, strlen(res)); 
}


