#ifndef __PAYLOAD_MANAGER_H__
#define __PAYLOAD_MANAGER_H__

/***************************************************************************

								payloadmanager.h

Company: USC/ISI
License: Proprietary
Author(s): Stephen Hunt

Purpose: Manages payload data and interfacing.

***************************************************************************/

#include <stdint.h>

// From BUFFER_SIZE_X (ports.h)
#define MAX_PAYLOAD_DATA_SIZE 255

typedef struct PayloadManager {
	uint16_t numPayloadPacketsStored;
	uint16_t numPayloadPacketsSent;
} PayloadManager;

typedef enum
{
	PAYLOAD_STATE_OFF = 0,
	PAYLOAD_STATE_ON = 1
} PayloadPowerState;

void PayloadManager__init(PayloadManager* self);
void turnPayloadPower(PayloadManager* self, PayloadPowerState powerState);
PayloadPowerState getPayloadPowerState(PayloadManager* self);
void sendDataToPayload(PayloadManager* self, char* data, int sizeOfData);
uint16_t getAvailablePayloadData(PayloadManager* self, char* payloadData,
								 uint16_t sizeOfPayloadDataBuffer);
void savePayloadData(PayloadManager* self, char* payloadData, uint16_t sizeOfPayloadData);
uint8_t sendPayloadPacketNumber(PayloadManager* self, uint16_t packetNumber);
void sendAvailablePayloadDataWhenReady(PayloadManager* self);
void savePayloadPacketCounters(PayloadManager* self);

#endif // __PAYLOAD_MANAGER_H__