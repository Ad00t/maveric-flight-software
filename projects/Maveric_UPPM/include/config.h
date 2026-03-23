#ifndef __CONFIG_H__
#define __CONFIG_H__

#define NUM_STORED_CMDS 12
#define MAX_STORED_CMD_SIZE 32

typedef struct {
	uint8_t RTU_ID[4]; // RTU id __ 4 bytes
	uint8_t
		Dest_ID[8]; // Default destination ID __ 8 bytes
	uint16_t
		Bspacing; // Byte spacing timer between Packet Radio __ 2 bytes
	uint8_t BeaconCallsign[6]; // Callsign used in beaconing __ 6 bytes
	uint8_t StoredCmds[NUM_STORED_CMDS][MAX_STORED_CMD_SIZE]; // The Schedule __ 384
																	// bytes (12x32)
	uint8_t
		BeaconOnFlag; // Flag controlling whether the beacon will broadcast	 __ 1 byte
	uint8_t
		Padding; // Padding because the compiler can't odd-sized stucts  __ 1 byte
	uint8_t
		CRC[2]; // Checksum.  THIS MEMBER MUST BE LAST!	__ 2 byte
} config_s;

void config_load_defaults(config_s* cfg);

#endif
