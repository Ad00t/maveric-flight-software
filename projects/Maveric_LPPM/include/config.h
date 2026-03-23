#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdint.h>

//=====================================================
//    		 LOWER Globals definition and defaults
//=====================================================
#define NUM_STORED_CMDS 			12
#define MAX_STORED_CMD_SIZE 		32

#define GYRO_SCALE					0b00001000
#define MAG_SCALE					0b00010000
#define SS_SCALE					0b00100000
#define RWA_SCALE					0b01000000
#define MTR_SCALE					0b10000000

#define REWRITE_X					0b00000001
#define REWRITE_Y					0b00000010
#define REWRITE_Z					0b00000100

typedef struct {
    uint8_t RTU_ID[4]; // RTU id
    uint8_t Dest_ID[8]; // Default destination ID
//    uint8_t Broadcast[NUM_BCAST_ADDR][4]; // Broadcast Address
    uint8_t Bspacing; // Byte spacing timer between Packet Radio
    uint8_t StoredCmds[NUM_STORED_CMDS][MAX_STORED_CMD_SIZE]; // The Schedule
    // Settings
    uint8_t BatteryThreshold[2];			// Two threshold to seperate Low, Medium, and High
    // Scales and rotations for all sensors
    float GyroScale[3];
    float GyroRot[9];
    float MagScale[3];
    float MagRot[9];
    float SSScale[3];
    float SSRot[9];
    float RWAScale[3];
    float RWARot[9];
    float MTRScale[3];
    float MTRRot[9];
    // Models
    float orbitInit[6];						// Stores default Orbit initialization
    uint8_t orbitInitTime[7];			// Stores an associated time for the orbit init
    // Flags
    uint8_t Geo_Enable;						// Flag for geomag model.
    uint8_t ReadGeomagFromFlash;				// Signal if we read geomag from flash or not.
    uint8_t orbitEnable;						// Flag for orbit model
    uint8_t GNC_Enable;						// Flag for GNC model running
    uint8_t load_Enable;						// Flag for loading values into GNC
    uint8_t attitudeSensorsEnable;			// Flag for Attitude Sensors
    uint8_t IMIEnable;
    uint8_t Padding; 				// Sometimes used because the PIC has a problem with certain struct sizes.
    uint8_t CRC[2]; 				// Checksum.  THIS MEMBER MUST BE LAST!
} config_s;

void config_load_defaults(config_s* cfg);

#endif
