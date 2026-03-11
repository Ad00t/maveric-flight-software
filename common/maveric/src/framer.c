#include "framer.h"
#include <stdint.h>

// FRAMER

uint16_t framer_create(uint8_t* msg, uint16_t msg_len, uint8_t* frame, int1 csp) {
    if (msg_len + KISS_HEADER_SIZE + KISS_FOOTER_SIZE + (csp ? CSP_HEADER_SIZE + CRC32_SIZE : 0) > FRAME_MAX_SIZE) 
        return 0;
    uint16_t len = 0;
    kiss_prepend_header(frame, &len);
    if (csp) {
        addCspHeader(frame, &len, len);
    } 
    kiss_apply_byte_check(msg, msg_len, frame, &len, len);
    if (csp) {
        uint32_t crc32 = compute_crc32(msg, msg_len);
        crc32 = htonl(crc32);
        kiss_apply_byte_check((uint8_t*)&crc32, sizeof(crc32), frame, &len, len);
    }
    kiss_append_footer(frame, &len);
    return len;
}

// KISS

int1 kiss_process_byte(kiss_parser_s* p, uint8_t byte) {
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
                if (p->buf_len < FRAME_MAX_SIZE) {
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
                // invalid escape -> drop frame
                p->fsm = KISS_WAIT_FEND;
                p->buf_len = 0;
                break;
            }

            if (p->buf_len < FRAME_MAX_SIZE) {
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
