#ifndef __FLASH_MANAGER_H__
#define __FLASH_MANAGER_H__

/***************************************************************************

								flashmanager.h

Company: USC/ISI
License: Proprietary
Author(s): Stephen Hunt

Purpose: Manages storage and retrieval of data to/from the ppm's flash
		 memory.

***************************************************************************/

#include <stdint.h>

typedef enum
{
	RESERVED_SECTION = 0,
	MISC_SECTION = 1,
	MACROS_SECTION = 2,
	CONFIG_SECTION = 3,
	HEXFILE_SECTION = 4,
	PAYLOAD_DATA_SECTION = 5,
	PAYLOAD_PROGRAM_SECTION = 6,
	SWAP_SECTION = 7,
	NUM_SECTIONS = 8
} MemorySection;

typedef struct FlashManager {
	uint16_t numPagesWrittenTo[NUM_SECTIONS];
} FlashManager;

void FlashManager__init(FlashManager* self);
uint8_t saveDataToFlash(FlashManager* self, char* data, uint16_t dataSize, MemorySection section);
/*
  Reads a previously stored payload packet from flash.

  Returns the size of the packet
*/
uint16_t readPayloadDataPacketFromFlash(FlashManager* self, uint16_t packetNumber, char* dataBuffer,
										uint16_t sizeOfDataBuffer);

void eraseAllOfSectionsData(FlashManager* self, MemorySection section);

void savePayloadObjectToFlash(FlashManager* self, char* payloadObjectBuffer, uint16_t bufferSize);

void readPayloadObjectFromFlash(FlashManager* self, char* payloadObjectBuffer,
								uint16_t payloadObjectSize);

#endif // __FLASH_MANAGER_H__