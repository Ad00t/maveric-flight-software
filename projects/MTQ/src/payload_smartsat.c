#include "payload.h"

#include <string.h>

#include "common.h"
#include "debug.h"
#include "errors.h"
#include "inet.h"
#include "ports.h"

/*
  Provides the SmartSat implementation of the payload interface
*/

// Required Pins
#define SMARTSAT_PWR PIN_G1
#define MUX_EN PIN_B0
#define PL_SEL PIN_B14
#define BUCK_EN PIN_G0

#define FRAME_SYNC_PATTERN 0xFAF320
#define PAYLOAD_MESSAGE_HEADER_SIZE 4

typedef struct PayloadMessage {
	uint8_t frameSyncPattern[3];
	uint8_t dataLength;
	char data[BUFFER_SIZE_D];
} PayloadMessage;

static void BuildPayloadMessage(PayloadMessage* self, char* data, uint8_t sizeOfData);

static void TurnPayloadPowerOn();
static void TurnPayloadPowerOff();
static uint8_t GetLengthOfIncomingPayloadMessage();
static uint8_t FoundBeginningOfPayloadMessage();
static uint8_t LengthOfReceivedMessage();

void InitializePayload() {
	output_low(BUCK_EN);
}

void TurnPayloadPower(PayloadPower state) {
	if (state == PAYLOAD_ON) {
		TurnPayloadPowerOn();
	} else if (state == PAYLOAD_OFF) {
		TurnPayloadPowerOff();
	}
	return;
}
static void TurnPayloadPowerOn() {
	output_high(SMARTSAT_PWR);
	return;
}
static void TurnPayloadPowerOff() {
	output_low(SMARTSAT_PWR);
	return;
}

PayloadPower GetPayloadPowerState() {
	return (PayloadPower)input_state(SMARTSAT_PWR);
}

uint8_t GetAvailableDataFromPayload(char* data, uint8_t dataBufferSize) {
	// Find a payload message within the receive buffer
	uint8_t incomingPayloadMessageLength = GetLengthOfIncomingPayloadMessage();
	if (incomingPayloadMessageLength == 0) {
		return 0;
	}

	// Read in the payload message from the port's buffer
	char payloadMessageBuffer[BUFFER_SIZE_D] = {0};
	getNumBytesOnPort(PAYLOAD_SMARTSAT_PORT,
					  incomingPayloadMessageLength + PAYLOAD_MESSAGE_HEADER_SIZE,
					  payloadMessageBuffer);

	PayloadMessage* payloadMessage = (PayloadMessage*)payloadMessageBuffer;
	// Copy data from receive buffer to this data buffer
	// Chose to copy data, even if this data buffer is too small, that way we get at least some
	// payload data
	uint8_t amountOfDataToCopy =
		dataBufferSize > payloadMessage->dataLength ? payloadMessage->dataLength : dataBufferSize;
	memcpy(data, payloadMessage->data, amountOfDataToCopy);
	// Tell receive buffer we've read in the whole payload message
	incrementNumReadBytesBy(PAYLOAD_SMARTSAT_PORT,
							payloadMessage->dataLength + PAYLOAD_MESSAGE_HEADER_SIZE);

	return amountOfDataToCopy;
}
static uint8_t GetLengthOfIncomingPayloadMessage() {
	if (FoundBeginningOfPayloadMessage()) {
		return LengthOfReceivedMessage();
	}
	return 0;
}
static uint8_t FoundBeginningOfPayloadMessage() {
	while (CheckByte(PAYLOAD_SMARTSAT_PORT) >= PAYLOAD_MESSAGE_HEADER_SIZE) {
		char frameSynchPatternBuffer[3] = {0};
		getNumBytesOnPort(PAYLOAD_SMARTSAT_PORT, sizeof(frameSynchPatternBuffer),
						  frameSynchPatternBuffer);
		// Are we at the beginning of a message?
		uint32_t frameSyncPattern = (((uint32_t)frameSynchPatternBuffer[0]) << 16) |
									(((uint32_t)frameSynchPatternBuffer[1]) << 8) |
									((uint32_t)frameSynchPatternBuffer[2]);
		if (frameSyncPattern == FRAME_SYNC_PATTERN) {
			return 1;
		} else {
			// Move forward in the buffer
			incrementNumReadBytesBy(PAYLOAD_SMARTSAT_PORT, 1);
		}
	}
	return 0;
}
static uint8_t LengthOfReceivedMessage() {
	uint8_t paylodMessageHeader[PAYLOAD_MESSAGE_HEADER_SIZE] = {0};
	getNumBytesOnPort(PAYLOAD_SMARTSAT_PORT, sizeof(paylodMessageHeader), (char*)paylodMessageHeader);
	if (CheckByte(PAYLOAD_SMARTSAT_PORT) >= paylodMessageHeader[3] + sizeof(paylodMessageHeader)) {
		return paylodMessageHeader[PAYLOAD_MESSAGE_HEADER_SIZE - 1];
	}
	return 0;
}

void SendDataToPayload(char* data, int dataSize) {
	PayloadMessage payloadMessage;
	BuildPayloadMessage(&payloadMessage, data, (uint8_t)dataSize);
	unsigned char* payloadBuffer = (unsigned char*)&payloadMessage;
	sendMSG(PAYLOAD_SMARTSAT_PORT, (unsigned char*)&payloadMessage,
			dataSize + PAYLOAD_MESSAGE_HEADER_SIZE);
	return;
}
static void BuildPayloadMessage(PayloadMessage* self, char* data, uint8_t sizeOfData) {
	memset(self, 0, sizeof(PayloadMessage));

	self->frameSyncPattern[0] = (uint8_t)(FRAME_SYNC_PATTERN >> 16 & 0xFF);
	self->frameSyncPattern[1] = (uint8_t)(FRAME_SYNC_PATTERN >> 8 & 0xFF);
	self->frameSyncPattern[2] = (uint8_t)(FRAME_SYNC_PATTERN & 0xFF);

	self->dataLength = sizeOfData;

	memcpy(self->data, data, sizeOfData);

	return;
}