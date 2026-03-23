#include "config.h"
#include <stdint.h>

#module

void config_load_defaults(config_s* cfg) {
	//	int n;

	// set all values to zero to start
	// memset(G, 0, sizeof(TFlashProtected));

	// Default Source
	cfg->RTU_ID[3] = 5; // Our Address
	cfg->RTU_ID[2] = 0;
	cfg->RTU_ID[1] = 0;
	cfg->RTU_ID[0] = 0;

	// Default Destination
	cfg->Dest_ID[3] = 5; // Destination RTU_ID (most significant Byte)
	cfg->Dest_ID[2] = 0; // Destination RTU_ID
	cfg->Dest_ID[1] = 0; // Destination RTU_ID ...this is a PAN address
	cfg->Dest_ID[0] = 0; // Destination RTU_ID (least significant Byte)..this sets a handheld as the destination

	// Default B spacing for defining delays between macros
	cfg->Bspacing = 3; // Bspacing				(not used often)

	// Default Beacon Callsign
	cfg->BeaconCallsign[0] = 'W';
	cfg->BeaconCallsign[1] = 'S';
	cfg->BeaconCallsign[2] = '9';
	cfg->BeaconCallsign[3] = 'X';
	cfg->BeaconCallsign[4] = 'S';
	cfg->BeaconCallsign[5] = 'W';

	// No Default schedule
	// StoredCmds[NUM_STORED_CMDS][MAX_STORED_CMD_SIZE];

	// Beacon
	cfg->BeaconOnFlag = 0; // Off by default.
}
