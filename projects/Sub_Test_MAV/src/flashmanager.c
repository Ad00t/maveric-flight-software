#include "flashmanager.h"

#include "address.h"
#include "common.h"
#include "flash.h"

static unsigned long GetNextWritableAddress(FlashManager* self, MemorySection section);
static void EraseSectionOfFlashIfNeeded(FlashManager* self, unsigned long address,
										uint16_t sectionLength);
static uint8_t WriteDataToFlash(FlashManager* self, MemorySection section, unsigned long address,
								char* data, uint16_t dataSize);
static unsigned long GetAddressOfPayloadPacket(FlashManager* self, uint16_t packetNumber);
static uint16_t ReadPacketAtAddressFromFlash(FlashManager* self, unsigned long packetAddress,
											 char* dataBuffer, uint16_t sizeOfDataBuffer);
static uint16_t ReadAmountOfDataStoredAtAddress(FlashManager* self, unsigned long address);

// Needs to be in the same order as the MemorySection enum and in ascending order of address
static const uint32_t BeginningAddressOfSection[NUM_SECTIONS] = {
	RESERVED_ADDR, MISC_ADDR,         Macros_ADDR,          Config_ADDR,
	HEXFILE_ADDR,  PAYLOAD_DATA_ADDR, PAYLOAD_PROGRAM_ADDR, SWAP_ADDR};
static const uint32_t AllocatedSizeOfSection[NUM_SECTIONS] = {
	RESERVED_ALLOC_SIZE, MISC_ALLOC_SIZE,         Macros_ALLOC_SIZE,          Config_ALLOC_SIZE,
	HEXFILE_ALLOC_SIZE,  PAYLOAD_DATA_ALLOC_SIZE, PAYLOAD_PROGRAM_ALLOC_SIZE, SWAP_ALLOC_SIZE};

void FlashManager__init(FlashManager* self) {
	memset(self, 0, sizeof(FlashManager));
}

uint8_t saveDataToFlash(FlashManager* self, char* data, uint16_t dataSize, MemorySection section) {
	unsigned long nextWritableAddress = GetNextWritableAddress(self, section);
	if (nextWritableAddress == ADDRESS_OUT_OF_BOUNDS) {
		return 0;
	}
	// Shrink data size if passed size is too large. Still want to save at least some data.
	dataSize = dataSize + 2 <= getPageSize() ? dataSize : getPageSize() - 2;
	EraseSectionOfFlashIfNeeded(self, nextWritableAddress, dataSize + 2);
	return WriteDataToFlash(self, section, nextWritableAddress, data, dataSize);
}
// Return next page to write to in flash
static unsigned long GetNextWritableAddress(FlashManager* self, MemorySection section) {
	uint32_t nextPage =
		FlashAddrToPage(BeginningAddressOfSection[section]) + self->numPagesWrittenTo[section];
	if (section == PAYLOAD_DATA_SECTION) {
		nextPage++;
	}
	// Check if we've filled this section of flash up with data
	if (nextPage >= FlashAddrToPage(BeginningAddressOfSection[section + 1])) {
		return ADDRESS_OUT_OF_BOUNDS;
	}
	unsigned long beginningOfPageAddress = 0;
	unsigned long endOfPageAddress = 0;
	PageToFlashAddr(nextPage, &beginningOfPageAddress, &endOfPageAddress);
	return beginningOfPageAddress;
}
static void EraseSectionOfFlashIfNeeded(FlashManager* self, unsigned long address,
										uint16_t sectionLength) {
	if (!CheckFlashEmpty(address, sectionLength)) {
		flashErasePage(FlashAddrToPage(address));
	}
}
static uint8_t WriteDataToFlash(FlashManager* self, MemorySection section, unsigned long address,
								char* data, uint16_t dataSize) {
	// Add data to a buffer, with length of data as first two bytes
	unsigned char dataBuffer[256] = {0};
	dataBuffer[0] = (unsigned char)dataSize >> 8 & 0xFF;
	dataBuffer[1] = (unsigned char)dataSize & 0xFF;
	memcpy(dataBuffer + 2, data, dataSize);

	// Write the length and data to flash
	if (flashWriteSafe(address, dataSize + 2, dataBuffer, address, address + dataSize + 2) ==
		SUCCESS) {
		self->numPagesWrittenTo[section]++;
		return 1;
	}
	return 0;
}

uint16_t readPayloadDataPacketFromFlash(FlashManager* self, uint16_t packetNumber, char* dataBuffer,
										uint16_t sizeOfDataBuffer) {
	// Make sure this packet has been saved
	if (packetNumber > self->numPagesWrittenTo[PAYLOAD_DATA_SECTION]) {
		return 0;
	}
	unsigned long packetAddress = GetAddressOfPayloadPacket(self, packetNumber);
	if (packetAddress == ADDRESS_OUT_OF_BOUNDS) {
		return 0;
	}
	return ReadPacketAtAddressFromFlash(self, packetAddress, dataBuffer, sizeOfDataBuffer);
}
static unsigned long GetAddressOfPayloadPacket(FlashManager* self, uint16_t packetNumber) {
	unsigned int packetPage =
		FlashAddrToPage(BeginningAddressOfSection[PAYLOAD_DATA_SECTION]) + packetNumber;
	// Check if the requested packet is out of range
	if (packetPage == FlashAddrToPage(BeginningAddressOfSection[PAYLOAD_DATA_SECTION + 1])) {
		return ADDRESS_OUT_OF_BOUNDS;
	}
	unsigned long beginningOfPageAddress = 0;
	unsigned long endOfPageAddress = 0;
	PageToFlashAddr(packetPage, &beginningOfPageAddress, &endOfPageAddress);
	return beginningOfPageAddress;
}
static uint16_t ReadPacketAtAddressFromFlash(FlashManager* self, unsigned long packetAddress,
											 char* dataBuffer, uint16_t sizeOfDataBuffer) {
	uint16_t sizeOfDataAtAddress = ReadAmountOfDataStoredAtAddress(self, packetAddress);
	sizeOfDataAtAddress =
		sizeOfDataBuffer < sizeOfDataAtAddress ? sizeOfDataBuffer : sizeOfDataAtAddress;
	flashRead(packetAddress + 2, sizeOfDataAtAddress, dataBuffer);
	return sizeOfDataAtAddress;
}
static uint16_t ReadAmountOfDataStoredAtAddress(FlashManager* self, unsigned long address) {
	// Put data size into big endian buffer
	unsigned char dataSizeBuffer[2] = {0};
	flashRead(address, 2, dataSizeBuffer);
	return ((uint16_t)dataSizeBuffer[1] << 0) | ((uint16_t)dataSizeBuffer[0] << 8);
}

void eraseAllOfSectionsData(FlashManager* self, MemorySection section) {
#ifdef DEBUG
	sendDBGALL(USER_PORT, "\r\nErasing a whole section of data. This will take a moment");
#endif
	unsigned int blockNumber = 0;
	unsigned int sectionsFirstBlock = FlashAddrToBlock(BeginningAddressOfSection[section]);
	unsigned int sectionsLastBlock = FlashAddrToBlock(BeginningAddressOfSection[section + 1]) - 1;

	for (blockNumber = sectionsFirstBlock; blockNumber <= sectionsLastBlock; ++blockNumber) {
		flashEraseBlockByNumber(blockNumber);
	}
#ifdef DEBUG
	sendDBGALL(USER_PORT, "\r\nDone erasing the section");
#endif
}

void savePayloadObjectToFlash(FlashManager* self, char* payloadObjectBuffer, uint16_t bufferSize) {
	unsigned long saveAddress = BeginningAddressOfSection[PAYLOAD_DATA_SECTION];
	flashErasePage(FlashAddrToPage(saveAddress));
	flashWriteSafe(saveAddress, bufferSize, payloadObjectBuffer, saveAddress,
				   saveAddress + bufferSize);
	return;
}

void readPayloadObjectFromFlash(FlashManager* self, char* payloadObjectBuffer,
								uint16_t payloadObjectSize) {
	unsigned long readAddress = BeginningAddressOfSection[PAYLOAD_DATA_SECTION];
	if (CheckFlashEmpty(readAddress, payloadObjectSize)) {
		memset(payloadObjectBuffer, 0, payloadObjectSize);
		return;
	}
	flashRead(readAddress, payloadObjectSize, payloadObjectBuffer);
	return;
}