#include "ax100.h"
#include "crcnew.h"

// MAIN TRANSCEIVER INTERFACE

static void TurnTransceiverPowerOn();
static void TurnTransceiverPowerOff();
static uint16_t FindIncomingMessage(int8_t* msgBuffer, int sizeOfMsgBuffer);

void TurnTransceiverPower(TransceiverPower state) {
	if (state == TRANSCEIVER_ON) {
		TurnTransceiverPowerOn();
	} else if (state == TRANSCEIVER_OFF) {
		TurnTransceiverPowerOff();
	}
	return;
}
static void TurnTransceiverPowerOn() {
	output_high(TRANSCEIVER_PWR);
	return;
}
static void TurnTransceiverPowerOff() {
	output_low(TRANSCEIVER_PWR);
	return;
}

TransceiverPower GetTransceiverPowerState() {
	return (TransceiverPower)input_state(TRANSCEIVER_PWR);
}

uint16_t GetAvailableMessageFromTransceiver(int8_t* messageBuffer, uint16_t sizeOfMessageBuffer) {
	// Find a message within the receive buffer
	return FindIncomingMessage(messageBuffer, sizeOfMessageBuffer);
}
static uint16_t FindIncomingMessage(int8_t* msgBuffer, int sizeOfMsgBuffer) {
	if (CheckByte(TRANSCEIVER_PORT) <=
		CRC32_SIZE + getKissFooterSize() + getKissHeaderSize() + getCspHeaderSize()) {
		return 0;
	}

	int frameStartIdx = 0;
	int frameEndIdx = 0;
	// Look through the incoming buffer on the desired port and see if there is an available frame
	uint8_t incomingBuffer[BUFFER_SIZE_B] = {0};
	getNumBytesOnPort(TRANSCEIVER_PORT, CheckByte(TRANSCEIVER_PORT), incomingBuffer);
	findFrame(incomingBuffer, CheckByte(TRANSCEIVER_PORT), &frameStartIdx, &frameEndIdx);

	// If there is, grab the message contained in the frame
	uint16_t msgLength = 0;
	if (frameStartIdx >= 0 && frameEndIdx >= 0) {
		uint16_t frameLength = frameEndIdx - frameStartIdx + 1;
		extractMessageFromFrame(incomingBuffer, frameLength, frameStartIdx, msgBuffer, &msgLength);
		incrementNumReadBytesBy(TRANSCEIVER_PORT, frameLength);
	}
	return msgLength;
}

void TransmitMessage(char* message, uint16_t messageSize) {
	uint8_t frame[512] = {0};
	uint16_t frameLength = 0;
	setupFrame((uint8_t*)message, messageSize, frame, &frameLength);
	sendMSG(TRANSCEIVER_PORT, frame, (int)frameLength);
	return;
}

// FRAME

static void findFrameMinFrameSize(uint8_t* msg, uint16_t msgLen, int* msgStartIdx, int* msgEndIdx,
								  uint16_t minFrameSize);

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

void findFrame(uint8_t* incomingBuffer, uint16_t bufferLen, int* frameStartIdx, int* frameEndIdx) {
	findFrameMinFrameSize(incomingBuffer, bufferLen, frameStartIdx, frameEndIdx, 1);
}

static void findFrameMinFrameSize(uint8_t* incomingBuffer, uint16_t bufferLen, int* frameStartIdx,
								  int* frameEndIdx, uint16_t minFrameSize) {
	*frameStartIdx = -1;
	*frameEndIdx = -1;

	int startLimit = bufferLen - 1;

	int idx;
	for (idx = 0; idx < startLimit; idx++) {
		if ((*(incomingBuffer + idx) == FEND) && (*(incomingBuffer + idx + 1) != FEND)) {
			*frameStartIdx = idx;
			break;
		} else {
			incrementNumReadBytesBy(TRANSCEIVER_PORT, 1);
		}
	}

	if (*frameStartIdx < 0) {
		return;
	}

	for (idx = *frameStartIdx + 1 + minFrameSize; idx < bufferLen; idx++) {
		if (*(incomingBuffer + idx) == FEND) {
			*frameEndIdx = idx;
			return;
		}
	}
}

#define CRC32_SIZE 4
void extractMessageFromFrame(uint8_t* incomingBuffer, uint16_t frameLength, int frameStartIdx,
							 uint8_t* msg, uint16_t* msgLength) {
	removeKissByteCheck(incomingBuffer, frameLength, msg, msgLength, frameStartIdx);

	*msgLength -= getKissHeaderSize() + getCspHeaderSize() + CRC32_SIZE + getKissFooterSize();

	int i;
	for (i = 0; i < *msgLength; ++i) {
		msg[i] = msg[i + getKissHeaderSize() + getCspHeaderSize()];
	}

	return;
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

#define CSP_NORMAL_PRIORITY 2L

#define FSW_NODE 8L

#define TX_NODE 0L
#define GOMSPACE_NODE 5L

// mimic csp lib default (cnofig's port_max_bind)
#define MIN_PING_SRC_PORT 24L

#define GND_WDT_RESET_PORT 9L

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

static const int FEND = 0xc0;
static const int FESC = 0xdb;
static const int TFEND = 0xdc;
static const int TFESC = 0xdd;

#define DATA_FRAME 0x00

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

void removeKissByteCheck(uint8_t* incomingBuffer, uint16_t frameLength, uint8_t* msg,
						 uint16_t* msgLength, uint16_t frameStartIdx) {
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
