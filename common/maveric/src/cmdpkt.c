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
    uint8_t* buf = pkt->buf;
    uint8_t id_len = strlen(id);
    uint8_t args_len = strlen(id);
   
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
    pkt->buf_len = len;
}

void cmdpkt_clear(cmdpkt_s* pkt) {
    memset(pkt, 0, sizeof(cmdpkt_s));
}

uint8_t cmdpkt_parse_buf(cmdpkt_s* pkt) {
    if (pkt->buf_len < 10) return STATUS_ERR;
    uint16_t len = 0;
    uint8_t* buf = &pkt->buf[pkt->i_start];

    // Parse header fields
    pkt->orgn = buf[len++];
    pkt->dest = buf[len++];
    pkt->echo = buf[len++];
    pkt->ptype = buf[len++];
    pkt->id_len = buf[len++];
    pkt->args_len = buf[len++];

    // Parse id field
    pkt->id = &buf[len];
    if (len + pkt->id_len >= pkt->buf_len
        || buf[len + pkt->id_len] != '\0') return STATUS_ERR;
    len += pkt->id_len + 1;
   
    // Parse args field
    pkt->args = &buf[len];
    if (len + pkt->args_len >= pkt->buf_len
        || buf[len + pkt->args_len] != '\0') return STATUS_ERR;
    len += pkt->args_len + 1;
   
    // Parse CRC as uint16
    uint8_t crc_low = buf[len++];
    uint8_t crc_high = buf[len++];
    pkt->crc = make16(crc_high, crc_low);

    return len == pkt->buf_len ? STATUS_OK : STATUS_ERR;
}

uint16_t cmdpkt_setup_frame(cmdpkt_s* pkt, uint8_t* frame, uint8_t frame_size, int1 csp) {
    if (pkt->buf_len + KISS_HEADER_SIZE + KISS_FOOTER_SIZE + (csp ? CSP_HEADER_SIZE + CRC32_SIZE : 0) > frame_size) 
        return 0;
    uint16_t len = 0;
    kiss_prepend_header(frame, &len);
    if (csp) {
        addCspHeader(frame, &len, len);
    } 
    kiss_apply_byte_check(pkt->buf, pkt->buf_len, frame, &len, len);
    if (csp) {
        uint32_t crc32 = compute_crc32(pkt->buf, pkt->buf_len);
        crc32 = htonl(crc32);
        kiss_apply_byte_check((uint8_t*)&crc32, sizeof(crc32), frame, &len, len);
    }
    kiss_append_footer(frame, &len);
    return len;
}

// INET

uint32_t htonl(uint32_t hostLong) {
	uint8_t data[4] = {0};
	memcpy(&data, &hostLong, sizeof(data));

	return ((uint32_t)data[3] << 0) | ((uint32_t)data[2] << 8) | ((uint32_t)data[1] << 16) |
		   ((uint32_t)data[0] << 24);
}

uint16_t htons(uint16_t hostShort) {
	uint8_t data[2] = {0};

	memcpy(&data, &hostShort, sizeof(data));

	return ((uint16_t)data[1] << 0) | ((uint16_t)data[0] << 8);
}

uint32_t ntohl(uint32_t netLong) {
	uint8_t data[4] = {0};
	memcpy(&data, &netLong, sizeof(data));

	return ((uint32_t)data[3] << 0) | ((uint32_t)data[2] << 8) | ((uint32_t)data[1] << 16) |
		   ((uint32_t)data[0] << 24);
}

uint16_t ntohs(uint16_t netShort) {
	uint8_t data[2] = {0};
	memcpy(&data, &netShort, sizeof(data));

	return ((uint16_t)data[1] << 0) | ((uint16_t)data[0] << 8);
}

// CSPHEADER

void addCspHeader(uint8_t* msg, uint16_t* msgLength, uint16_t startLocation) {
	CSPHeader header;
	memset(&header, 0, sizeof(CSPHeader));

	header.value |= (CSP_NORMAL_PRIORITY << 30);
	header.value |= (FSW_NODE << 25);
	header.value |= (TX_NODE << 20);
	header.value |= (MIN_PING_SRC_PORT << 14);

	header.value = htonl(header.value);

	memcpy(&(msg[startLocation]), (uint8_t*)&header.value, sizeof(header.value));
	*msgLength += sizeof(header.value);
}

void addCspHeaderWdtReset(uint8_t* msg, uint16_t* msgLength, uint16_t startLocation) {
	CSPHeader header;
	memset(&header, 0, sizeof(CSPHeader));

	header.value |= (GND_WDT_RESET_PORT << 12);
	header.value |= (GOMSPACE_NODE << 7);
	header.value |= (FSW_NODE << 2);
	header.value |= (CSP_NORMAL_PRIORITY);

	header.value = htonl(header.value);

	memcpy(&(msg[startLocation]), (uint8_t*)&header.value, sizeof(header.value));
	*msgLength += sizeof(header.value);
}

void setupWdtReset(uint8_t* msg, uint16_t* msgLength) {
	kiss_prepend_header(msg, msgLength);

	addCspHeaderWdtReset(msg, msgLength, KISS_HEADER_SIZE);

	// empty message, so CRC32 == 0
	int crcLoc = *msgLength;

	msg[crcLoc] = 0;
	msg[crcLoc + 1] = 0;
	msg[crcLoc + 2] = 0;
	msg[crcLoc + 3] = 0;
	*msgLength += 4;

	kiss_append_footer(msg, msgLength);
}

// KISS

int1 kiss_process_byte(cmdpkt_s* p, uint8_t byte) {
    // fprintf(COM_D, "b: 0x%02X'%c'\n", byte, byte);
    switch (p->fsm) {
        case KISS_WAIT_FEND:
            if (byte == FEND) {
                p->buf_len = 0;
                p->fsm = KISS_IN_FRAME;
            }
            break;
        
        case KISS_IN_FRAME:
            if (byte == FEND) {
                // End of frame
                if (p->buf_len > 0) {
                    return TRUE;
                }
            } else if (byte == FESC) {
                p->fsm = KISS_IN_ESCAPE;
            } else {
                if (p->buf_len < CMD_MAX_FRAME_SIZE) {
                    p->buf[p->buf_len++] = byte;
                } else {
                    // overflow -> drop frame
                    p->fsm = KISS_WAIT_FEND;
                    p->buf_len = 0;
                }
            }
            break;

        case KISS_IN_ESCAPE:
            if (byte == TFEND) {
                byte = FEND;
            } else if (byte == TFESC) {
                byte = FESC;
            } else {
                // invalid escape → drop frame
                p->fsm = KISS_WAIT_FEND;
                p->buf_len = 0;
                break;
            }

            if (p->buf_len < CMD_MAX_FRAME_SIZE) {
                p->buf[p->buf_len++] = byte;
                p->fsm = KISS_IN_FRAME;
            } else {
                p->fsm = KISS_WAIT_FEND;
                p->buf_len = 0;
            }
            break;
    }
    return FALSE;
}

void kiss_prepend_header(uint8_t* msg, uint16_t* msgLength) {
	msg[0] = FEND;
	msg[1] = DATA_FRAME;
	*msgLength += 2;
}

void kiss_append_footer(uint8_t* msg, uint16_t* msgLength) {
	msg[*msgLength] = FEND;
	(*msgLength)++;
}

void kiss_apply_byte_check(uint8_t* message, uint16_t messageLength, uint8_t* frame, uint16_t* frameLength, uint16_t msgStartIdx) {
	uint16_t spot = msgStartIdx;

	uint16_t i;
	for (i = 0; i < messageLength; i++) {
		if (message[i] == FEND) {
			frame[spot] = FESC;
			spot++;
			frame[spot] = TFEND;
			spot++;
		} else if (message[i] == FESC) {
			frame[spot] = FESC;
			spot++;
			frame[spot] = TFESC;
			spot++;
		} else {
			frame[spot] = message[i];
			spot++;
		}
	}

	*frameLength = spot;
}

void kiss_remove_byte_check(uint8_t* buf, uint16_t frameLength, uint8_t* msg, uint16_t* msgLength, uint16_t frameStartIdx) {
	uint16_t len = frameLength;
	uint16_t spot = 0;

	uint16_t i;
	for (i = frameStartIdx; i < frameLength; i++) {
		if (buf[i] == FESC) {
			len--;
			i++;

			if (buf[i] == TFESC) {
				msg[spot] = FESC;
			} else if (buf[i] == TFEND) {
				msg[spot] = FEND;
			} else {
				msg[spot] = buf[i];
			}
		} else {
			msg[spot] = buf[i];
		}

		spot++;
	}

	*msgLength = len;
}

uint16_t kiss_extract_frame(ringbuf_s* rcvbuf, uint8_t* frame_buf, uint16_t frame_buf_size) {
	int16_t frameStartIdx = 0;
	int16_t frameEndIdx = 0;
	// Look through the incoming buffer on the desired port and see if there is an available frame
	if (!kiss_find_frame(rcvbuf, &frameStartIdx, &frameEndIdx, 1)) 
        return 0;

	// If there is, grab the message contained in the frame
    // We are waiting for a full frame to enter the rcvbuf before processing it. Could replace with FSM.
    uint16_t frameLength = frameEndIdx - frameStartIdx + 1;
    if (frameLength <= frame_buf_size) {
        rb_pop(rcvbuf, frameStartIdx, NULL);
        rb_pop(rcvbuf, frameLength, frame_buf);
        return frameLength;
    } else {
        rb_pop(rcvbuf, frameStartIdx + frameLength, NULL);
        return 0; 
    }
}

int1 kiss_find_frame(ringbuf_s* rcvbuf, int16_t* frameStartIdx, int16_t* frameEndIdx, uint16_t minFrameSize) {
	*frameStartIdx = -1;
	*frameEndIdx = -1;

    uint8_t bufferLen = rb_len(rcvbuf);
	uint16_t startLimit = bufferLen - 1;
	uint16_t idx;
	for (idx = 0; idx < startLimit; idx++) {
        uint8_t b1, b2;
        rb_peek(rcvbuf, idx, &b1);
        rb_peek(rcvbuf, idx+1, &b2);
		if (b1 == FEND && b2 != FEND) {
            *frameStartIdx = idx;
			break;
        }
	}

	if (*frameStartIdx < 0) {
		return FALSE;
	}

	for (idx = *frameStartIdx + 1 + minFrameSize; idx < bufferLen; idx++) {
        uint8_t b;
        rb_peek(rcvbuf, idx, &b);
		if (b == FEND) {
			*frameEndIdx = idx;
			return TRUE;
		}
	}

    return FALSE;
}
