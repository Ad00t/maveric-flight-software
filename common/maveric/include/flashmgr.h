#ifndef __FLASHMGR_H__
#define __FLASHMGR_H__

#include "common.h"
#include "flash.h"
#include "logger.h"
#include "cmdpkt.h"

//========================================
//    		 	Address Locations
//========================================
#define RESERVED_ADDR                   0x000000
#define MISC_ADDR                       0x010000
#define MACROS_ADDR                     0x034000
#define GNC_INPUTS_ADDR                 0x037000
#define CONFIG_ADDR                     0x03A000
#define TEXTMSG_ADDR                    0x03C000
#define MAG_COEFF_ADDR                  0x03C000
#define HEXFILE_ADDR                    0x03D000
#define PAYLOAD_DATA_ADDR               0x137000
#define PAYLOAD_PROGRAM_ADDR            0x75F000
#define SWAP_ADDR                       0x7FF000
#define ADDRESS_OUT_OF_BOUNDS           0x800000

//========================================
//    		 	Size Allocations
//========================================
#define RESERVED_ALLOC_SIZE             0x010000
#define MISC_ALLOC_SIZE                 0x024000
#define MACROS_ALLOC_SIZE               0x003000
#define GNC_INPUTS_ALLOC_SIZE           0x003000
#define CONFIG_ALLOC_SIZE               0x002000
#define TEXTMSG_ALLOC_SIZE              0x000000
#define MAG_COEFF_ALLOC_SIZE            0x001000
#define HEXFILE_ALLOC_SIZE              0x7c2000
#define PAYLOAD_DATA_ALLOC_SIZE         0x628000
#define PAYLOAD_PROGRAM_ALLOC_SIZE      0x0A0000
#define SWAP_ALLOC_SIZE                 0x001000

typedef enum {
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

typedef enum {
    DATASRC_MTQ = 0,
    DATASRC_NVG = 1,
    DATASRC_GYRO = 2
} datasrc_e;

typedef struct {
#if NODE == NODE_LPPM
    log_level_e log_level;
    datasrc_e gyro_rate_src;
    datasrc_e attitude_src;
#elif NODE == NODE_UPPM
    log_level_e log_level;
    uint16_t last_holonav_seq;
    uint16_t last_astroboard_seq;
#endif
    uint16_t crc;
} config_s;

typedef struct {
    config_s config;
	uint16_t numPagesWrittenTo[NUM_SECTIONS];
    uint16_t rbt_cnt;
} flashmgr_s;

// Initialize flashmgr and config
status_e flashmgr_init(flashmgr_s* self);

// Reboot counter manipulation
status_e flashmgr_increment_rbt_cnt(flashmgr_s* self);
status_e flashmgr_reset_rbt_cnt(flashmgr_s* self);

// Config manipulation
void flashmgr_config_load_defaults(flashmgr_s* self);
status_e flashmgr_config_load_flash(flashmgr_s* self);
status_e flashmgr_config_flush(flashmgr_s* self);

uint8_t saveDataToFlash(flashmgr_s* self, char* data, uint16_t dataSize, MemorySection section);
/*
  Reads a previously stored payload packet from flash.

  Returns the size of the packet
*/
uint16_t readPayloadDataPacketFromFlash(flashmgr_s* self, uint16_t packetNumber, char* dataBuffer,
										uint16_t sizeOfDataBuffer);

void eraseAllOfSectionsData(flashmgr_s* self, MemorySection section);

void savePayloadObjectToFlash(flashmgr_s* self, char* payloadObjectBuffer, uint16_t bufferSize);

void readPayloadObjectFromFlash(flashmgr_s* self, char* payloadObjectBuffer,
								uint16_t payloadObjectSize);

#endif
