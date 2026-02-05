#include "payloadmanager.h"

#ifdef DESKTOP
#include "payload.h"
#include <string.h>
#else
#include "common.h"
#include "payload_smartsat.c"
#endif

#include "address.h"
#include "errors.h"
#include "flash.h"
#include "globals.h"
#include "linkmanager.h"

static uint8_t TransceiverIsSetToTransmit();
static void IncrementNumPacketsSent(PayloadManager* self);

void PayloadManager__init(PayloadManager* self) {
	memset(self, 0, sizeof(PayloadManager));
	InitializePayload();
	char readBuffer[sizeof(PayloadManager)] = {0};
	readPayloadObjectFromFlash(&gFlashManager, readBuffer, sizeof(PayloadManager));
	memcpy(&self->numPayloadPacketsSent, &readBuffer[0], sizeof(self->numPayloadPacketsSent));
	memcpy(&self->numPayloadPacketsStored, &readBuffer[sizeof(self->numPayloadPacketsSent)],
		   sizeof(self->numPayloadPacketsStored));
	gFlashManager.numPagesWrittenTo[PAYLOAD_DATA_SECTION] = self->numPayloadPacketsStored;
}

void turnPayloadPower(PayloadManager* self, PayloadPowerState option) {
	TurnPayloadPower((PayloadPower)option);
	if (option == PAYLOAD_STATE_OFF) {
		savePayloadPacketCounters(self);
	}
	return;
}

PayloadPowerState getPayloadPowerState(PayloadManager* self) {
	if (GetPayloadPowerState() == PAYLOAD_ON) {
		return PAYLOAD_STATE_ON;
	} else {
		return PAYLOAD_STATE_OFF;
	};
}

void sendDataToPayload(PayloadManager* self, char* data, int dataSize) {
	if (GetPayloadPowerState() == PAYLOAD_ON) {
		SendDataToPayload(data, dataSize);
	}
	return;
}

uint16_t getAvailablePayloadData(PayloadManager* self, char* payloadData,
								 uint16_t sizeOfPayloadDataBuffer) {
	if (GetPayloadPowerState() == PAYLOAD_ON) {
		return GetAvailableDataFromPayload(payloadData, sizeOfPayloadDataBuffer);
	}
	return 0;
}

void savePayloadData(PayloadManager* self, char* payloadData, uint16_t sizeOfPayloadData) {
	if (saveDataToFlash(&gFlashManager, payloadData, sizeOfPayloadData, PAYLOAD_DATA_SECTION)) {
		self->numPayloadPacketsStored++;
	}
	return;
}

uint8_t sendPayloadPacketNumber(PayloadManager* self, uint16_t packetNumber) {
	char packetBuffer[256] = {0};
	uint16_t packetSize = readPayloadDataPacketFromFlash(
		&gFlashManager, packetNumber, packetBuffer, sizeof(packetBuffer));
	if (packetSize > 0) {
		transmitData(&gLinkManager, packetBuffer, packetSize, PAYLOAD_DATA, packetNumber);
		return 1;
	}
	return 0;
}

void sendAvailablePayloadDataWhenReady(PayloadManager* self) {
	if (TransceiverIsSetToTransmit()) {
		sendPayloadPacketNumber(self, self->numPayloadPacketsSent + 1);
		IncrementNumPacketsSent(self);
	}
	return;
}
static uint8_t TransceiverIsSetToTransmit() {
	return gLinkManager.transceiverMode == TRANSMIT_MODE;
}
static void IncrementNumPacketsSent(PayloadManager* self) {
	self->numPayloadPacketsSent++;
	if (self->numPayloadPacketsSent >= self->numPayloadPacketsStored) {
		self->numPayloadPacketsSent = 0;
	}
}

void savePayloadPacketCounters(PayloadManager* self) {
	char saveBuffer[sizeof(PayloadManager)] = {0};
	memcpy(&saveBuffer[0], &self->numPayloadPacketsSent, sizeof(self->numPayloadPacketsSent));
	memcpy(&saveBuffer[sizeof(self->numPayloadPacketsSent)],
						 &self->numPayloadPacketsStored, sizeof(self->numPayloadPacketsStored));
	savePayloadObjectToFlash(&gFlashManager, saveBuffer, sizeof(PayloadManager));
}