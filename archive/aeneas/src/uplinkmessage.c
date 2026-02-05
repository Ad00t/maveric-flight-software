#include "uplinkmessage.h"

#include "config.h"

#ifdef DESKTOP
#include <stdio.h>
#include <string.h>
#else
#include "common.h"
#include <stddef.h>
#include <string.h>
#endif

#include "crc.h"
#include "inet.h"

static int8_t UpBufferIsInvalidSize(int bufferSize);
static int8_t ProjectIdCopyAndCheckFailed(UplinkMessage* self, char* pointerToBufferCallsign);
static uint16_t GetMessageHeaderLength(UplinkMessage* self);

void UplinkMessage__init(UplinkMessage* self) {
	memset(self, 0, sizeof(UplinkMessage));
}

int8_t deserializeIncomingMessage(UplinkMessage* self, char* message, uint16_t messageSize) {
	if (self == NULL || message == NULL) {
		return 0;
	}
	if (UpBufferIsInvalidSize(messageSize)) {
		return 0;
	}
	if (ProjectIdCopyAndCheckFailed(self, message)) {
		return 0;
	}
	message += sizeof(self->projectId);

	self->messageID = ntohs(*(uint16_t*)message);
	message += sizeof(self->messageID);

	self->dataLength = messageSize - GetMessageHeaderLength(self);

	memcpy(&self->data[0], message, messageSize - GetMessageHeaderLength(self));
	message += messageSize - GetMessageHeaderLength(self);

	return 1;
}
static int8_t UpBufferIsInvalidSize(int bufferSize) {
	return (bufferSize <= (sizeof(UplinkMessage) - MAX_UPLINK_DATA_LENGTH - 2));
}
static int8_t ProjectIdCopyAndCheckFailed(UplinkMessage* self, char* pointerToBufferCallsign) {
	// Store the buffer's callsign data
	memcpy(&self->projectId[0], pointerToBufferCallsign, sizeof(self->projectId));
	// Check if we received the expected callsign
	return memcmp((char*)&self->projectId[0], PROJECT_ID, sizeof(PROJECT_ID) - 1);
}

static uint16_t GetMessageHeaderLength(UplinkMessage* self) {
	return sizeof(UplinkMessage) - MAX_UPLINK_DATA_LENGTH - 2;
}