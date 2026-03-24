#include "flashmgr.h"
#include "flash.h"
#include <stdint.h>

#module

static uint32_t GetNextWritableAddress(flashmgr_s* self, MemorySection section);
static void EraseSectionOfFlashIfNeeded(flashmgr_s* self, uint32_t address,
										uint16_t sectionLength);
static uint8_t WriteDataToFlash(flashmgr_s* self, MemorySection section, uint32_t address,
								char* data, uint16_t dataSize);
static uint32_t GetAddressOfPayloadPacket(flashmgr_s* self, uint16_t packetNumber);
static uint16_t ReadPacketAtAddressFromFlash(flashmgr_s* self, uint32_t packetAddress,
											 char* dataBuffer, uint16_t sizeOfDataBuffer);
static uint16_t ReadAmountOfDataStoredAtAddress(flashmgr_s* self, uint32_t address);

// Needs to be in the same order as the MemorySection enum and in ascending order of address
static const uint32_t BeginningAddressOfSection[NUM_SECTIONS] = {
	RESERVED_ADDR, MISC_ADDR,         MACROS_ADDR,          CONFIG_ADDR,
	HEXFILE_ADDR,  PAYLOAD_DATA_ADDR, PAYLOAD_PROGRAM_ADDR, SWAP_ADDR};
static const uint32_t AllocatedSizeOfSection[NUM_SECTIONS] = {
	RESERVED_ALLOC_SIZE, MISC_ALLOC_SIZE,         MACROS_ALLOC_SIZE,          CONFIG_ALLOC_SIZE,
	HEXFILE_ALLOC_SIZE,  PAYLOAD_DATA_ALLOC_SIZE, PAYLOAD_PROGRAM_ALLOC_SIZE, SWAP_ALLOC_SIZE};

status_e flashmgr_init(flashmgr_s* self) {
	memset(self, 0, sizeof(flashmgr_s));
#if NODE == NODE_LPPM
    self->config.gyro_rate_src = 0; 
    self->config.attitude_src = 0;
#elif NODE == NODE_UPPM
    self->config.last_holonav_seq = 0;
    self->config.last_astroboard_seq = 0;
#endif
    return flashmgr_config_load(self);
}

status_e flashmgr_increment_rbt_cnt(flashmgr_s* self) {
	uint32_t i;
	uint32_t addy;
    uint8_t record_size = sizeof(self->rbt_cnt);

	// This loops over the block, looking for a set of 2 empty bytes.
	for (i = 0; i < FLASH_BLOCK_SIZE - 1; i += record_size) {
		//		sprintf(dbgbuf,"\r\n%i",i); sendDBGALL(USER_PORT,dbgbuf);
		if (CheckFlashEmpty(RESERVED_ADDR + i, record_size))
			break;
	}
	// i now contains the location (offset from RESERVED_ADDR) of the first empty set of bytes in
	// flash, or is >= FLASH_BLOCK_SIZE if it could not find an empty spot.
	addy = RESERVED_ADDR + i - record_size;

	// Read the previous value
	if (i >= record_size)
		flashRead(addy, record_size, &self->rbt_cnt); // For values of i greater than first iteration
	else
		self->rbt_cnt = 0; // For first iteration.

	// Validate no data corruption
	// if (self->rbt_cnt < 0)
	// 	self->rbt_cnt = 0;

	// Increment the counter
	self->rbt_cnt++; // Wraps to 0 automatically

	// Now increment the value in flash

	// First disable protection
	flashSectorProtectDisable(RESERVED_ADDR);

	// Next check if we are at the end of the block, because we'll need to erase the block and start
	// over in that case
	if (i >= FLASH_BLOCK_SIZE - 1) {
		flashEraseBlockByAddr(RESERVED_ADDR);
		i = 0;
	}

	// Finally, write the new value
	return flashWriteSafe(RESERVED_ADDR + i, record_size, &self->rbt_cnt, RESERVED_ADDR,
						  RESERVED_ADDR + FLASH_BLOCK_SIZE - 1);
}

status_e flashmgr_reset_rbt_cnt(flashmgr_s* self) {
	self->rbt_cnt = 0;
	flashSectorProtectDisable(RESERVED_ADDR);
	flashEraseBlockByAddr(RESERVED_ADDR);
	flashWriteSafe(RESERVED_ADDR, sizeof(self->rbt_cnt), &self->rbt_cnt, RESERVED_ADDR,
				   RESERVED_ADDR + FLASH_BLOCK_SIZE - 1);
    return SUCCESS;
}

status_e flashmgr_config_load(flashmgr_s* self) {
	uint16_t n = sizeof(config_s);
	static const uint8_t TIMES_TO_CHECK = 10;
	// Setup
	waitForFlash();
	// Read and check CRC, exiting successfully if the CRC matches
    uint16_t i;
	for (i = 0; i < TIMES_TO_CHECK; i++) {
		flashRead(CONFIG_ADDR, n, (uint8_t*)self->config);
		if (check_crc16((uint8_t*)self->config, n-2, self->config.crc) == SUCCESS)
			return SUCCESS;
	}
	return FAILURE;
}

status_e flashmgr_config_flush(flashmgr_s* self) {
	uint16_t n = sizeof(config_s);
	self->config.crc = compute_crc16((uint8_t*)self->config, n-2);

    uint16_t m = 0;
	for (m = 0; m < n; m += FLASH_BLOCK_SIZE){
		flashEraseBlockByAddr((uint32_t)(CONFIG_ADDR + m)); // Erase the block at Config + m
		// -------------NOTE THAT A REBOOT HERE WOULD BE REALLY BAD ----------//
	}

	// Now program the new stuff, handling any errors if they happen
	return flashWriteSafe(CONFIG_ADDR, n, self->config, CONFIG_ADDR, CONFIG_ADDR + n + 1);
}

//////////////////////////////////////////////////////////////////////////////////

uint8_t saveDataToFlash(flashmgr_s* self, char* data, uint16_t dataSize, MemorySection section) {
	uint32_t nextWritableAddress = GetNextWritableAddress(self, section);
	if (nextWritableAddress == ADDRESS_OUT_OF_BOUNDS) {
		return 0;
	}
	// Shrink data size if passed size is too large. Still want to save at least some data.
	dataSize = dataSize + 2 <= getPageSize() ? dataSize : getPageSize() - 2;
	EraseSectionOfFlashIfNeeded(self, nextWritableAddress, dataSize + 2);
	return WriteDataToFlash(self, section, nextWritableAddress, data, dataSize);
}
// Return next page to write to in flash
static uint32_t GetNextWritableAddress(flashmgr_s* self, MemorySection section) {
	uint32_t nextPage =
		FlashAddrToPage(BeginningAddressOfSection[section]) + self->numPagesWrittenTo[section];
	if (section == PAYLOAD_DATA_SECTION) {
		nextPage++;
	}
	// Check if we've filled this section of flash up with data
	if (nextPage >= FlashAddrToPage(BeginningAddressOfSection[section + 1])) {
		return ADDRESS_OUT_OF_BOUNDS;
	}
	uint32_t beginningOfPageAddress = 0;
	uint32_t endOfPageAddress = 0;
	PageToFlashAddr(nextPage, &beginningOfPageAddress, &endOfPageAddress);
	return beginningOfPageAddress;
}
static void EraseSectionOfFlashIfNeeded(flashmgr_s* self, uint32_t address,
										uint16_t sectionLength) {
	if (!CheckFlashEmpty(address, sectionLength)) {
		flashErasePage(FlashAddrToPage(address));
	}
}
static uint8_t WriteDataToFlash(flashmgr_s* self, MemorySection section, uint32_t address,
								char* data, uint16_t dataSize) {
	// Add data to a buffer, with length of data as first two bytes
	uint8_t dataBuffer[256] = {0};
	dataBuffer[0] = (uint8_t)dataSize >> 8 & 0xFF;
	dataBuffer[1] = (uint8_t)dataSize & 0xFF;
	memcpy(dataBuffer + 2, data, dataSize);

	// Write the length and data to flash
	if (flashWriteSafe(address, dataSize + 2, dataBuffer, address, address + dataSize + 2) ==
		SUCCESS) {
		self->numPagesWrittenTo[section]++;
		return 1;
	}
	return 0;
}

uint16_t readPayloadDataPacketFromFlash(flashmgr_s* self, uint16_t packetNumber, char* dataBuffer,
										uint16_t sizeOfDataBuffer) {
	// Make sure this packet has been saved
	if (packetNumber > self->numPagesWrittenTo[PAYLOAD_DATA_SECTION]) {
		return 0;
	}
	uint32_t packetAddress = GetAddressOfPayloadPacket(self, packetNumber);
	if (packetAddress == ADDRESS_OUT_OF_BOUNDS) {
		return 0;
	}
	return ReadPacketAtAddressFromFlash(self, packetAddress, dataBuffer, sizeOfDataBuffer);
}
static uint32_t GetAddressOfPayloadPacket(flashmgr_s* self, uint16_t packetNumber) {
	unsigned int packetPage = FlashAddrToPage(BeginningAddressOfSection[PAYLOAD_DATA_SECTION]) + packetNumber;
	// Check if the requested packet is out of range
	if (packetPage == FlashAddrToPage(BeginningAddressOfSection[PAYLOAD_DATA_SECTION + 1])) {
		return ADDRESS_OUT_OF_BOUNDS;
	}
	uint32_t beginningOfPageAddress = 0;
	uint32_t endOfPageAddress = 0;
	PageToFlashAddr(packetPage, &beginningOfPageAddress, &endOfPageAddress);
	return beginningOfPageAddress;
}
static uint16_t ReadPacketAtAddressFromFlash(flashmgr_s* self, uint32_t packetAddress,
											 char* dataBuffer, uint16_t sizeOfDataBuffer) {
	uint16_t sizeOfDataAtAddress = ReadAmountOfDataStoredAtAddress(self, packetAddress);
	sizeOfDataAtAddress =
		sizeOfDataBuffer < sizeOfDataAtAddress ? sizeOfDataBuffer : sizeOfDataAtAddress;
	flashRead(packetAddress + 2, sizeOfDataAtAddress, dataBuffer);
	return sizeOfDataAtAddress;
}
static uint16_t ReadAmountOfDataStoredAtAddress(flashmgr_s* self, uint32_t address) {
	// Put data size into big endian buffer
	uint8_t dataSizeBuffer[2] = {0};
	flashRead(address, 2, dataSizeBuffer);
	return ((uint16_t)dataSizeBuffer[1] << 0) | ((uint16_t)dataSizeBuffer[0] << 8);
}

void eraseAllOfSectionsData(flashmgr_s* self, MemorySection section) {
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

void savePayloadObjectToFlash(flashmgr_s* self, char* payloadObjectBuffer, uint16_t bufferSize) {
	uint32_t saveAddress = BeginningAddressOfSection[PAYLOAD_DATA_SECTION];
	flashErasePage(FlashAddrToPage(saveAddress));
	flashWriteSafe(saveAddress, bufferSize, payloadObjectBuffer, saveAddress,
				   saveAddress + bufferSize);
	return;
}

void readPayloadObjectFromFlash(flashmgr_s* self, char* payloadObjectBuffer,
								uint16_t payloadObjectSize) {
	uint32_t readAddress = BeginningAddressOfSection[PAYLOAD_DATA_SECTION];
	if (CheckFlashEmpty(readAddress, payloadObjectSize)) {
		memset(payloadObjectBuffer, 0, payloadObjectSize);
		return;
	}
	flashRead(readAddress, payloadObjectSize, payloadObjectBuffer);
	return;
}
