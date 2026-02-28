#include "ax100.h"
#include "crcnew.h"
#include "uart.h"
#include "ringbuf.h"
#include "cmdpkt.h"
#include "common.h"
#include "cmdmgr.h"
#include "kiss.h"

// MAIN TRANSCEIVER INTERFACE

void ax100_init(ax100_s* a, uint8_t port) {
    a->port = port;
    ax100_set_power(a, TRUE);
    sprintf(LOGBUF, "ax100_init"); log_flush(LL_INFO);
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

void ax100_parse_stream(ax100_s* a, cmdmgr_s* cmdmgr, ringbuf_s* irqbuf, cmdpkt_s* pkt) {
	if (rb_len(irqbuf) < CRC32_SIZE + KISS_HEADER_SIZE + KISS_FOOTER_SIZE + CSP_HEADER_SIZE) return;

    // Extract KISS frame
    uint8_t frame_buf[AX100_MAX_FRAME_SIZE] = {0};
    uint16_t frame_len = kiss_extract_frame(irqbuf, frame_buf, sizeof(frame_buf));
   
    // Extract cmd and process it if we have a frame
    if (frameLength > 0) {
        uint8_t msg_buf[AX100_MAX_FRAME_SIZE] = {0};
        uint16_t msg_len = 0;
        kiss_remove_byte_check(frame_buf, frame_len, msg_buf, &msg_len, 0);
        msg_len -= KISS_HEADER_SIZE + CSP_HEADER_SIZE + CRC32_SIZE + KISS_FOOTER_SIZE;
        uint8_t i;
        for (i = 0; i < msg_len; i++) {
            pkt->buf[i] = msg_buf[i + KISS_HEADER_SIZE + CSP_HEADER_SIZE];
        }
        pkt->buf_len = msg_len;
        cmdmgr_process_cmd(cmdmgr, pkt);    
    }
}

void ax100_transmit_msg(ax100_s* a, uint8_t* buf, uint8_t len) {
	uint8_t frame[AX100_MAX_FRAME_SIZE] = {0};
	uint8_t frameLength = 0;
	setupFrame(buf, len, frame, &frameLength);
    uart_write_buf(a->port, frame, frameLength);
  
    uint16_t p = 0;
    uint8_t i;
    p += sprintf(LOGBUF, "ax100_transmit_msg: len=%u [", frameLength); 
    for (i = 0; i < frameLength - 1; i++) 
        p += sprintf(&LOGBUF[p], " %02X", buf[i]); 
    p += sprintf(&LOGBUF[p], " %02X ]", buf[frameLength - 1]); 
    log_flush(LL_TRACE);
}

// FRAME

void setupFrame(uint8_t* message, uint16_t messageLength, uint8_t* frame, uint16_t* frameLength) {
	kiss_prepend_header(frame, frameLength);

	addCspHeader(frame, frameLength, KISS_HEADER_SIZE);

	kiss_apply_byte_check(message, messageLength, frame, frameLength, KISS_HEADER_SIZE + CSP_HEADER_SIZE);

	uint32_t crc32 = compute_crc32(message, messageLength);
	crc32 = htonl(crc32);

	kiss_apply_byte_check((uint8_t*)&crc32, sizeof(crc32), frame, frameLength, *frameLength);

	kiss_append_footer(frame, frameLength);
}


void extractMessageFromFrame(uint8_t* framebuf, uint16_t frameLength, cmdpkt_s* pkt) {
    uint8_t msgbuf[AX100_MAX_FRAME_SIZE] = {0};
    uint16_t msgLength = 0;
	kiss_remove_byte_check(framebuf, frameLength, msgbuf, &msgLength, 0);
	msgLength -= KISS_HEADER_SIZE + CSP_HEADER_SIZE + CRC32_SIZE + KISS_FOOTER_SIZE;
	uint8_t i;
	for (i = 0; i < msgLength; i++) {
		pkt->buf[i] = msgbuf[i + KISS_HEADER_SIZE + CSP_HEADER_SIZE];
	}
    pkt->buf_len = msgLength;
}

////////////////////////////////////////////////////////////////////////////////

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
