#include "linkmanager.h"

#ifdef DESKTOP
#include "transceiver.h"
#include <string.h>
#else
#include "common.h"
#include "transceiver_gomspace.c"
#endif
#include "downlinkmessage.c"
#include "globals.h"
#include "uplinkmessage.c"

static int MessageIdHasBeenReceived(LinkManager* self, UplinkMessage* message);
static void TrackThisMessageId(LinkManager* self, uint16_t messageId);

void LinkManager__init(LinkManager* self) {
	memset(self, 0, sizeof(LinkManager));
	memset(self->previouslyReceivedMessageIds, 0xff, sizeof(self->previouslyReceivedMessageIds));
	self->nextHealthAndStatusMessageId = 1;
}

void switchTransceiverPowerTo(LinkManager* self, RadioPowerState state) {
	TurnTransceiverPower((TransceiverPower)state);
	return;
}

void switchBeaconTransmitEnabledTo(LinkManager* self, int8_t enabled) {
	globals.BeaconOnFlag = enabled;
}

void switchTransceiverModeTo(LinkManager* self, TransceiverMode mode) {
	self->transceiverMode = mode;
	if (mode == RECEIVE_MODE) {
		self->numCommandsReceived = 0;
		savePayloadPacketCounters(&gPayloadManager);
	}
}

RadioPowerState getTransceiverPowerState(LinkManager* self) {
	if (GetTransceiverPowerState() == TRANSCEIVER_OFF) {
		return RADIO_STATE_OFF;
	} else {
		return RADIO_STATE_ON;
	}
}

uint16_t getAvailableCommand(LinkManager* self, char* commandBuffer, uint16_t sizeOfCommandBuffer) {
	char messageBuffer[MAX_MESSAGE_SIZE] = {0};
	uint16_t messageSize = GetAvailableMessageFromTransceiver(messageBuffer, sizeof(messageBuffer));

	if (messageSize > 0) {
		UplinkMessage uplinkMessage;
		UplinkMessage__init(&uplinkMessage);

		if (!deserializeIncomingMessage(&uplinkMessage, messageBuffer, messageSize)) {
			return 0;
		}
		if (MessageIdHasBeenReceived(self, &uplinkMessage)) {
			return 0;
		}
		memcpy(commandBuffer, uplinkMessage.data, uplinkMessage.dataLength);
		self->numCommandsReceived++;
		return uplinkMessage.dataLength;
	}
	return 0;
}
static int MessageIdHasBeenReceived(LinkManager* self, UplinkMessage* message) {
	int i;
	for (i = 0; i < NUM_TRACKED_IDS; ++i) {
		if (message->messageID == self->previouslyReceivedMessageIds[i]) {
			return 1;
		}
	}
	TrackThisMessageId(self, message->messageID);
	return 0;
}
static void TrackThisMessageId(LinkManager* self, uint16_t messageId) {
	if (self->indexOfLastReceivedMessageId >= (NUM_TRACKED_IDS - 1)) {
		self->indexOfLastReceivedMessageId = 0;
		self->previouslyReceivedMessageIds[self->indexOfLastReceivedMessageId] = messageId;
	} else {
		self->previouslyReceivedMessageIds[++self->indexOfLastReceivedMessageId] = messageId;
	}
	return;
}

void transmitData(LinkManager* self, char* data, int dataSize, DataType dataType, uint16_t messageId) {
	DownlinkMessage downlinkMessage;
	DownlinkMessage__init(&downlinkMessage);

	if (dataType == HEALTH_AND_STATUS) {
		downlinkMessage.messageId = self->nextHealthAndStatusMessageId++;
	} else if (dataType == PAYLOAD_DATA) {
		downlinkMessage.messageId = messageId;
	}
	downlinkMessage.messageType = dataType;
	appendDataToMessage(&downlinkMessage, data, (uint16_t)dataSize);

	char messageBuffer[MAX_DOWNLINK_DATA_LENGTH] = {0};
	uint16_t messageSize =
		serializeDownlinkMessage(&downlinkMessage, messageBuffer, sizeof(messageBuffer));
	if (messageSize > 0) {
		TransmitMessage(messageBuffer, messageSize);
	}

	return;
}