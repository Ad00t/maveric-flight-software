/*
address_upper.h

Address locations for the upper ppm -- larger flash memory.

This file is maintained as an Excel file in the Subversion repository.
Location: \repos\aeneas\Analysis\Flash_Address_Helper.xls
*/

//========================================
//              Address Locations
//========================================
#define RESERVED_ADDR 0x000000
#define MISC_ADDR 0x010000
#define Macros_ADDR 0x032000
#define Config_ADDR 0x035000
#define HEXFILE_ADDR 0x037000
#define PAYLOAD_DATA_ADDR 0x137000
#define PAYLOAD_PROGRAM_ADDR 0x75F000
#define SWAP_ADDR 0x7FF000
#define ADDRESS_OUT_OF_BOUNDS 0x800000

//========================================
//              Size Allocations
//========================================
#define RESERVED_ALLOC_SIZE 0x010000
#define MISC_ALLOC_SIZE 0x022000
#define Macros_ALLOC_SIZE 0x003000
#define Config_ALLOC_SIZE 0x002000
#define HEXFILE_ALLOC_SIZE 0x100000
#define PAYLOAD_DATA_ALLOC_SIZE 0x628000
#define PAYLOAD_PROGRAM_ALLOC_SIZE 0x0A0000
#define SWAP_ALLOC_SIZE 0x001000

//========================================
//    		UPPER Globals definition and defaults
//========================================
#define NUM_STORED_CMDS 12
#define MAX_STORED_CMD_SIZE 32

typedef struct {
	unsigned char RTU_ID[4]; // RTU id __ 4 bytes
	unsigned char
		Dest_ID[8]; // Default destination ID __ 8 bytes
	unsigned int
		Bspacing; // Byte spacing timer between Packet Radio __ 2 bytes
	unsigned char BeaconCallsign[6]; // Callsign used in beaconing __ 6 bytes
	unsigned char StoredCmds[NUM_STORED_CMDS][MAX_STORED_CMD_SIZE]; // The Schedule __ 384
																	// bytes (12x32)
	unsigned char
		BeaconOnFlag; // Flag controlling whether the beacon will broadcast	 __ 1 byte
	unsigned char
		Padding; // Padding because the compiler can't odd-sized stucts  __ 1 byte
	unsigned char
		CRC[2]; // Checksum.  THIS MEMBER MUST BE LAST!	__ 2 byte
} TFlashProtected;

//== Load globals with defaults
void loadDefaults(TFlashProtected* G) {
	//	int n;

	// set all values to zero to start
	// memset(G, 0, sizeof(TFlashProtected));

	// Default Source
	G->RTU_ID[3] = 5; // Our Address
	G->RTU_ID[2] = 0;
	G->RTU_ID[1] = 0;
	G->RTU_ID[0] = 0;

	// Default Destination
	G->Dest_ID[3] = 5; // Destination RTU_ID (most significant Byte)
	G->Dest_ID[2] = 0; // Destination RTU_ID
	G->Dest_ID[1] = 0; // Destination RTU_ID ...this is a PAN address
	G->Dest_ID[0] = 0; // Destination RTU_ID (least significant Byte)..this sets a handheld as the destination

	// Default B spacing for defining delays between macros
	G->Bspacing = 3; // Bspacing				(not used often)

	// Default Beacon Callsign
	G->BeaconCallsign[0] = 'W';
	G->BeaconCallsign[1] = 'S';
	G->BeaconCallsign[2] = '9';
	G->BeaconCallsign[3] = 'X';
	G->BeaconCallsign[4] = 'S';
	G->BeaconCallsign[5] = 'W';

	// No Default schedule
	// StoredCmds[NUM_STORED_CMDS][MAX_STORED_CMD_SIZE];

	// Beacon
	G->BeaconOnFlag = 0; // Off by default.
}