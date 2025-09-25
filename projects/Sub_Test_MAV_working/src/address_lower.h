//========================================
// 			Dependencies
//========================================
#include "fredtime.h"

//========================================
//    		 	Address Locations
//========================================
#define RESERVED_ADDR                   0x000000
#define MISC_ADDR                       0x010000
#define Macros_ADDR                     0x034000
#define GNC_INPUTS_ADDR                 0x037000
#define Config_ADDR                     0x03A000
#define TextMsg_ADDR                    0x03C000
#define MAG_COEFF_ADDR                  0x03C000
#define HEXFILE_ADDR                    0x03D000
#define SWAP_ADDR                       0x7FF000
#define ADDRESS_OUT_OF_BOUNDS           0x800000

//========================================
//    		 	Size Allocations
//========================================
#define RESERVED_ALLOC_SIZE             0x010000
#define MISC_ALLOC_SIZE                 0x024000
#define Macros_ALLOC_SIZE               0x003000
#define GNC_INPUTS_ALLOC_SIZE           0x003000
#define Config_ALLOC_SIZE               0x002000
#define TextMsg_ALLOC_SIZE              0x000000
#define MAG_COEFF_ALLOC_SIZE            0x001000
#define HEXFILE_ALLOC_SIZE              0x7c2000
#define SWAP_ALLOC_SIZE                 0x001000

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
    unsigned char RTU_ID[4]; // RTU id
    unsigned char Dest_ID[8]; // Default destination ID
//    unsigned char Broadcast[NUM_BCAST_ADDR][4]; // Broadcast Address
    unsigned int Bspacing; // Byte spacing timer between Packet Radio
    unsigned char StoredCmds[NUM_STORED_CMDS][MAX_STORED_CMD_SIZE]; // The Schedule
    // Settings
    unsigned int BatteryThreshold[2];			// Two threshold to seperate Low, Medium, and High
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
    unsigned char orbitInitTime[7];			// Stores an associated time for the orbit init
    // Flags
    unsigned char Geo_Enable;						// Flag for geomag model.
    unsigned char ReadGeomagFromFlash;				// Signal if we read geomag from flash or not.
    unsigned char orbitEnable;						// Flag for orbit model
    unsigned char GNC_Enable;						// Flag for GNC model running
    unsigned char load_Enable;						// Flag for loading values into GNC
    unsigned char attitudeSensorsEnable;			// Flag for Attitude Sensors
    unsigned char IMIEnable;
    unsigned char Padding; 				// Sometimes used because the PIC has a problem with certain struct sizes.
    unsigned char CRC[2]; 				// Checksum.  THIS MEMBER MUST BE LAST!
} TFlashProtected;

//== Load globals with defaults
void loadDefaults(TFlashProtected* G) {
    int i;
    float matrix_setting;

    // set all values to zero to start
    // memset(G, 0, sizeof(TFlashProtected));

    // Default Source
    G->RTU_ID[3] = 5;				// Our Address
    G->RTU_ID[2] = 0;
    G->RTU_ID[1] = 0;
    G->RTU_ID[0] = 0;

    // Default Destination
    G->Dest_ID[3] = 5;             // Destination RTU_ID (most significant Byte)
    G->Dest_ID[2] = 0;             // Destination RTU_ID
    G->Dest_ID[1] = 0;             // Destination RTU_ID ...this is a PAN address
    G->Dest_ID[0] = 0;             // Destination RTU_ID (least significant Byte)..this sets a handheld as the destination

    // Default Broadcast address
//    G->Broadcast[0][3] = 255;      // Broadcast "Short" address (most significant Byte)
//    G->Broadcast[0][2] = 255;      // Broadcast "Short" address
//    G->Broadcast[0][1] = 255;      // PAN_ID Broadcast Address
//    G->Broadcast[0][0] = 255;      // PAN_ID Broadcast address (least significant Byte)
//    memcpy(&G->Broadcast[1], &G->Broadcast[0], 4); // Same for 2nd
//    memcpy(&G->Broadcast[2], &G->Broadcast[0], 4); // Same for 3rd
//    G->Broadcast[1][3] = 254;      // Broadcast address (most significant Byte)
//    G->Broadcast[2][3] = 253;      // Broadcast address (most significant Byte)

    // Default B spacing for defining delays between macros
    G->Bspacing = 3;               // Bspacing				(not used often)

    // No Default schedule
      // StoredCmds[NUM_STORED_CMDS][MAX_STORED_CMD_SIZE];

    // Default Battery Thresholds
    G->BatteryThreshold[0] = 265;		// Low Threshold  (a higher count!)
    G->BatteryThreshold[1] = 200;		// High Threshold (a lower count!)

    // Default Scales and rotations...
    for (i = 0; i < 3; i++) {
        // Set all scales to 1.
        G->GyroScale[i] = 1.0;
        G->MagScale[i] = 1.0;
        G->SSScale[i] = 1.0;
        G->RWAScale[i] = 1.0;
        G->MTRScale[i] = 1.0;
    }

    // This should initialize all rotation matrices to the identity matrix
    // [ 1 0 0 ]
    // [ 0 1 0 ]
    // [ 0 0 1 ]
    //
    for (i = 0; i < 9; i++) {
        if (i == 0 || i == 4 || i == 8)	// if we are on the diagonal, set to 1
            matrix_setting = 1;
        else						// otherwise set to zero.
            matrix_setting = 0;

        // Set each element
        G->GyroRot[i] = matrix_setting;
        G->MagRot[i] = matrix_setting;
        G->SSRot[i] = matrix_setting;
        G->RWARot[i] = matrix_setting;
        G->MTRRot[i] = matrix_setting;
    }

    // Default orbit and init time
    G->orbitInit[0] = 421.53;					// Pos X
    G->orbitInit[1] = 6820999.95;				// Pos Y
    G->orbitInit[2] = 649.0;					// Pos Z
    G->orbitInit[3] = 4215.3;					// Vel X
    G->orbitInit[4] = (-1.0) * 0.856;			// Vel Y
    G->orbitInit[5] = 6490.0;					// Vel Z
    setTimeVectorOnly(&G->orbitInitTime[0], 6, 19, 10, 2, 19, 0, 0);

    // Default Flags
    G->Geo_Enable = 1;						// Flag for running the geomagnetic model.
    G->ReadGeomagFromFlash = 0;				// Flag for reading geomag coefficients from flash or not.
    G->orbitEnable = 1;						// Flag for propogating orbit model
    G->GNC_Enable = 1;						// Flag for running Guidance, Navigation and Control (GNC)
    G->load_Enable = 1;						// Flag for loading values into GNC input vector (or using whatever's already there)
    G->attitudeSensorsEnable = 1;				// Flag for polling the Attitude Sensors
    G->IMIEnable = 0;							// Flag for real writes to the wheels and torque coils
}
