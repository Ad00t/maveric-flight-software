#include "downlinkmessage.h"

#include "config.h"

#ifdef DESKTOP
#include <stdio.h>
#include <string.h>
#else
#include "common.h"
#include <stddef.h>
#endif

#include "crc.h"
#include "inet.h"

static int8_t NotEnoughSpaceInMessage(DownlinkMessage* self, uint16_t dataSize);
static int8_t DownBufferIsInvalidSize(DownlinkMessage* self, uint16_t bufferSize);
static int8_t MessageHasNoData(DownlinkMessage* self);
static uint16_t AddCharsToBuffer(char* chars, uint16_t numChars, char* buffer);
static uint16_t AddUShortToBuffer(uint16_t uShort, char* buffer);

void DownlinkMessage__init(DownlinkMessage* self) {
	memset(self, 0, sizeof(DownlinkMessage));
}

int8_t appendDataToMessage(DownlinkMessage* self, char* data, uint16_t dataSize) {
	if (self == NULL || data == NULL) {
		return 0;
	}
	if (dataSize == 0) {
		return 0;
	}
	if (NotEnoughSpaceInMessage(self, dataSize)) {
		return 0;
	}

	memcpy(self->data + self->dataLength, data, dataSize);
	self->dataLength += dataSize;

	return 1;
}
static int8_t NotEnoughSpaceInMessage(DownlinkMessage* self, uint16_t dataSize) {
	return (dataSize > (MAX_DOWNLINK_DATA_LENGTH - self->dataLength));
}

uint16_t serializeDownlinkMessage(DownlinkMessage* self, char* buffer, uint16_t bufferSize) {
	if (self == NULL || buffer == NULL) {
		return 0;
	}
	if (DownBufferIsInvalidSize(self, bufferSize)) {
		return 0;
	}
	if (MessageHasNoData(self)) {
		return 0;
	}

	uint16_t sizeOfOutgoingMessage = 0;

	sizeOfOutgoingMessage +=
		AddCharsToBuffer(&self->messageType, sizeof(self->messageType), buffer);
	buffer += sizeof(self->messageType);

	sizeOfOutgoingMessage +=
		AddUShortToBuffer(self->messageId, buffer);
	buffer += sizeof(self->messageId);

	sizeOfOutgoingMessage += AddCharsToBuffer(&self->data[0], self->dataLength, buffer);
	buffer += self->dataLength;

	return sizeOfOutgoingMessage;
}
static int8_t DownBufferIsInvalidSize(DownlinkMessage* self, uint16_t bufferSize) {
	return (bufferSize < (sizeof(DownlinkMessage) - (MAX_DOWNLINK_DATA_LENGTH - self->dataLength)));
}
static int8_t MessageHasNoData(DownlinkMessage* self) {
	return (self->dataLength == 0);
}
/*
  Return number of bytes added to buffer
*/
static uint16_t AddCharsToBuffer(char* chars, uint16_t numChars, char* buffer) {
	memcpy(buffer, chars, numChars);
	return numChars;
}
/*
  Return number of bytes added to buffer
*/
static uint16_t AddUShortToBuffer(uint16_t uShort, char* buffer) {
	uint16_t networkShort = htons(uShort);
	memcpy(buffer, &networkShort, sizeof(networkShort));
	buffer += sizeof(networkShort);
	return sizeof(networkShort);
}