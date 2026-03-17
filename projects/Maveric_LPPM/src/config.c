#include "config.h"
#include <stdint.h>

#module

void config_load_defaults(config_s* cfg) {
    int i;
    float matrix_setting;

    // set all values to zero to start
    // memset(G, 0, sizeof(TFlashProtected));

    // Default Source
    cfg->RTU_ID[3] = 5;				// Our Address
    cfg->RTU_ID[2] = 0;
    cfg->RTU_ID[1] = 0;
    cfg->RTU_ID[0] = 0;

    // Default Destination
    cfg->Dest_ID[3] = 5;             // Destination RTU_ID (most significant Byte)
    cfg->Dest_ID[2] = 0;             // Destination RTU_ID
    cfg->Dest_ID[1] = 0;             // Destination RTU_ID ...this is a PAN address
    cfg->Dest_ID[0] = 0;             // Destination RTU_ID (least significant Byte)..this sets a handheld as the destination

    // Default Broadcast address
    //    cfg->Broadcast[0][3] = 255;      // Broadcast "Short" address (most significant Byte)
    //    cfg->Broadcast[0][2] = 255;      // Broadcast "Short" address
    //    cfg->Broadcast[0][1] = 255;      // PAN_ID Broadcast Address
    //    cfg->Broadcast[0][0] = 255;      // PAN_ID Broadcast address (least significant Byte)
    //    memcpy(&cfg->Broadcast[1], &cfg->Broadcast[0], 4); // Same for 2nd
    //    memcpy(&cfg->Broadcast[2], &cfg->Broadcast[0], 4); // Same for 3rd
    //    cfg->Broadcast[1][3] = 254;      // Broadcast address (most significant Byte)
    //    cfg->Broadcast[2][3] = 253;      // Broadcast address (most significant Byte)

    // Default B spacing for defining delays between macros
    cfg->Bspacing = 3;               // Bspacing				(not used often)

    // No Default schedule
      // StoredCmds[NUM_STORED_CMDS][MAX_STORED_CMD_SIZE];

    // Default Battery Thresholds
    cfg->BatteryThreshold[0] = 265;		// Low Threshold  (a higher count!)
    cfg->BatteryThreshold[1] = 200;		// High Threshold (a lower count!)

    // Default Scales and rotations...
    for (i = 0; i < 3; i++) {
        // Set all scales to 1.
        cfg->GyroScale[i] = 1.0;
        cfg->MagScale[i] = 1.0;
        cfg->SSScale[i] = 1.0;
        cfg->RWAScale[i] = 1.0;
        cfg->MTRScale[i] = 1.0;
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
        cfg->GyroRot[i] = matrix_setting;
        cfg->MagRot[i] = matrix_setting;
        cfg->SSRot[i] = matrix_setting;
        cfg->RWARot[i] = matrix_setting;
        cfg->MTRRot[i] = matrix_setting;
    }

    // Default orbit and init time
    cfg->orbitInit[0] = 421.53;					// Pos X
    cfg->orbitInit[1] = 6820999.95;				// Pos Y
    cfg->orbitInit[2] = 649.0;					// Pos Z
    cfg->orbitInit[3] = 4215.3;					// Vel X
    cfg->orbitInit[4] = (-1.0) * 0.856;			// Vel Y
    cfg->orbitInit[5] = 6490.0;					// Vel Z
    uint8_t time[7] = { 3, 1, 1, 26, 0, 0, 0 };
    memcpy(cfg->orbitInitTime, time, 7);
    // setTimeVectorOnly(&cfg->orbitInitTime[0], 6, 19, 10, 2, 19, 0, 0);

    // Default Flags
    cfg->Geo_Enable = 1;						// Flag for running the geomagnetic model.
    cfg->ReadGeomagFromFlash = 0;				// Flag for reading geomag coefficients from flash or not.
    cfg->orbitEnable = 1;						// Flag for propogating orbit model
    cfg->GNC_Enable = 1;						// Flag for running Guidance, Navigation and Control (GNC)
    cfg->load_Enable = 1;						// Flag for loading values into GNC input vector (or using whatever's already there)
    cfg->attitudeSensorsEnable = 1;				// Flag for polling the Attitude Sensors
    cfg->IMIEnable = 0;							// Flag for real writes to the wheels and torque coils
}
