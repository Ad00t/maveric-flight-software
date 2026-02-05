#ifndef __PAYLOAD_H__
#define __PAYLOAD_H__

/***************************************************************************

								payload.h

Company: USC/ISI
License: Proprietary
Author(s): Stephen Hunt

Purpose: Provides a base definition for interfacing with the payload

***************************************************************************/

#include <stdint.h>

typedef enum
{
	PAYLOAD_OFF = 0,
	PAYLOAD_ON = 1
} PayloadPower;

void InitializePayload();
/*
  Toggles payload power to provided state
*/
void TurnPayloadPower(PayloadPower state);
/*
  Returns state of payload power
*/
PayloadPower GetPayloadPowerState();
/*
  Reads data from payload.

  Returns size of read data. Returns 0 if data is available.
*/
uint8_t GetAvailableDataFromPayload(char* data, uint8_t dataBufferSize);
void SendDataToPayload(char* data, int dataSize);

#endif // __PAYLOAD_H__