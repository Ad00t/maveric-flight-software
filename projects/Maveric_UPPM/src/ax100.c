#include "ax100.h"
#include "crcnew.h"
#include "uart.h"
#include "circbuf.h"
#include "common.h"

// MAIN TRANSCEIVER INTERFACE

void ax100_init(ax100_s* a, uint8_t port) {
    a->port = port;
    cb_init(&a->cmdbuf);
    ax100_set_power(a, TRUE);
    sprintf(LOGBUF, "ax100_init"); log_flush(KWHT);
}

void ax100_set_power(ax100_s* a, int1 on) {
    if (on) {
        output_high(AX100_PWR);
    } else {
        output_low(AX100_PWR);
    }
}

int1 ax100_is_on(ax100_s* a) {
	return (int1) input_state(AX100_PWR);
}

uint8_t ax100_get_avail_msg(ax100_s* a, circbuf_s* irqbuf) {
	if (cb_len(irqbuf) <= CRC32_SIZE + getKissFooterSize() + getKissHeaderSize() + getCspHeaderSize()) {
		return 0;
	}

	int frameStartIdx = 0;
	int frameEndIdx = 0;
	// Look through the incoming buffer on the desired port and see if there is an available frame
	if (!findFrame(irqbuf, &frameStartIdx, &frameEndIdx, 1)) {
        return 0;
    }

	// If there is, grab the message contained in the frame
    // We are waiting for a full frame to enter the irqbuf before processing it. Could replace with FSM.
	uint8_t msgLength = 0;
    uint8_t frameLength = frameEndIdx - frameStartIdx + 1;
    uint8_t framebuf[AX100_MAX_FRAME_SIZE] = {0};
    cb_pop(irqbuf, frameStartIdx, NULL);
    cb_pop(irqbuf, frameLength, framebuf);
    extractMessageFromFrame(framebuf, frameLength, &a->cmdbuf, &msgLength);
	return msgLength;
}

void ax100_transmit_msg(ax100_s* a, uint8_t* buf, uint8_t len) {
	uint8_t frame[AX100_MAX_FRAME_SIZE] = {0};
	uint8_t frameLength = 0;
	setupFrame(buf, len, frame, &frameLength);
    uart_write_buf(a->port, frame, frameLength);
  
    uint16_t p;
    uint8_t i;
    p += sprintf(LOGBUF, "ax100_transmit_msg: len=%u [", frameLength); 
    for (i = 0; i < frameLength - 1; i++) 
        p += sprintf(LOGBUF, "0x%02X", buf[i]); 
    p += sprintf(LOGBUF, "0x%02X ]", buf[frameLength - 1]); 
    log_flush(LL_TRACE);
}

// FRAME

void setupFrame(uint8_t* message, uint16_t messageLength, uint8_t* frame, uint16_t* frameLength) {
	addKissHeader(frame, frameLength);

	addCspHeader(frame, frameLength, getKissHeaderSize());

	applyKissByteCheck(message, messageLength, frame, frameLength,
					   getKissHeaderSize() + getCspHeaderSize());

	uint32_t crc32 = compute_crc32(message, messageLength);
	crc32 = htonl(crc32);

	applyKissByteCheck((uint8_t*)&crc32, sizeof(crc32), frame, frameLength, *frameLength);

	addKissFooter(frame, frameLength);
}

int1 findFrame(circbuf_s* irqbuf, int* frameStartIdx, int* frameEndIdx, uint16_t minFrameSize) {
	*frameStartIdx = -1;
	*frameEndIdx = -1;

    uint8_t bufferLen = cb_len(irqbuf);
	int startLimit = bufferLen - 1;
	uint8_t idx;
	for (idx = 0; idx < startLimit; idx++) {
        uint8_t b1, b2;
        cb_peek(irqbuf, idx, &b1);
        cb_peek(irqbuf, idx+1, &b2);
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
        cb_peek(irqbuf, idx, &b);
		if (b == FEND) {
			*frameEndIdx = idx;
			return TRUE;
		}
	}

    return FALSE;
}

void extractMessageFromFrame(uint8_t* framebuf, uint16_t frameLength, circbuf_s* cmdbuf, uint16_t* msgLength) {
    uint8_t msgbuf[AX100_MAX_FRAME_SIZE] = {0};
	removeKissByteCheck(framebuf, frameLength, msgbuf, msgLength, 0);
	*msgLength -= getKissHeaderSize() + getCspHeaderSize() + CRC32_SIZE + getKissFooterSize();
	uint8_t i;
	for (i = 0; i < *msgLength; ++i) {
		cb_push(cmdbuf, msgbuf[i + getKissHeaderSize() + getCspHeaderSize()]);
	}
}

////////////////////////////////////////////////////////////////////////////////

void setupWdtReset(uint8_t* msg, uint16_t* msgLength) {
	addKissHeader(msg, msgLength);

	addCspHeaderWdtReset(msg, msgLength, getKissHeaderSize());

	// empty message, so CRC32 == 0
	int crcLoc = *msgLength;

	msg[crcLoc] = 0;
	msg[crcLoc + 1] = 0;
	msg[crcLoc + 2] = 0;
	msg[crcLoc + 3] = 0;
	*msgLength += 4;

	addKissFooter(msg, msgLength);
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

uint16_t getCspHeaderSize(void) {
	return (sizeof(CSPHeader));
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

// KISS


////////////////////////////////////////////////////////////////////////////////

void addKissHeader(uint8_t* msg, uint16_t* msgLength) {
	msg[0] = FEND;
	msg[1] = DATA_FRAME;

	*msgLength += 2;
}

uint16_t getKissHeaderSize(void) {
	return (2);
}

////////////////////////////////////////////////////////////////////////////////

void addKissFooter(uint8_t* msg, uint16_t* msgLength) {
	msg[*msgLength] = FEND;
	(*msgLength)++;
}

uint16_t getKissFooterSize(void) {
	return (1);
}

////////////////////////////////////////////////////////////////////////////////

void applyKissByteCheck(uint8_t* message, uint16_t messageLength, uint8_t* frame,
						uint16_t* frameLength, uint16_t msgStartIdx) {
	uint16_t spot = msgStartIdx;

	int idx;
	for (idx = 0; idx < messageLength; idx++) {
		if (*(message + idx) == FEND) {
			frame[spot] = FESC;
			spot++;
			frame[spot] = TFEND;
			spot++;
		} else if (*(message + idx) == FESC) {
			frame[spot] = FESC;
			spot++;
			frame[spot] = TFESC;
			spot++;
		} else {
			frame[spot] = message[idx];
			spot++;
		}
	}

	*frameLength = spot;
}

////////////////////////////////////////////////////////////////////////////////

void removeKissByteCheck(uint8_t* incomingBuffer, uint16_t frameLength, uint8_t* msg, uint16_t* msgLength, uint16_t frameStartIdx) {
	int len = frameLength;
	int spot = 0;

	int idx;
	for (idx = frameStartIdx; idx < frameLength; idx++) {
		if (*(incomingBuffer + idx) == FESC) {
			len--;
			idx++;

			if (*(incomingBuffer + idx) == TFESC) {
				msg[spot] = FESC;
			} else if (*(incomingBuffer + idx) == TFEND) {
				msg[spot] = FEND;
			} else {
				msg[spot] = incomingBuffer[idx];
			}
		} else {
			msg[spot] = incomingBuffer[idx];
		}

		spot++;
	}

	*msgLength = len;
}
