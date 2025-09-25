/***************************************************************************

									mag.c

Company: USC/ISI
License: Proprietary
Author(s): Will Bezouska

Purpose: Provide hardware interface to magnetometer.

Changelog:

Date 	 |  Au.  |	Notes
07-13-10   Will 	Initial revision.

****************************************************************************
*/

#include <mag.h> // Public header

#module // !!! Important: This command makes everything below scoped only to this file.

//======================================
//				Defines
//======================================
#define UT_PER_BIT 31.24 // Conversion to real value for mag

static void removeTorqueCoilBias(float* torqueCoilCommand, mag_s* Data);

//========================================
// 			Driver Functions
//========================================

// Note: The below functions are necessary to use hardware.

/*=================================================================
  PURPOSE: Request a single max axis
  UNMODIFIED VARIABLES: axis - 0 (X), 1 (Y), or 2 (Z)
  MODIFIED VARIABLES: None.
  GLOBALS: None
  RETURN VALUE: Error codes
  NOTES: None.
-----------------------------------------------------------
*/

ReturnErr_t requestSingleMagAxis(int axis) {
	// Select mangetomter
	output_low(MAG_SSNOT);
	delay_us(1);

	// Pulse MAG_RESET low-high-low
	output_low(MAG_RESET);
	output_high(MAG_RESET);
	delay_us(1);
	output_low(MAG_RESET);

	// Send command to get measurement for a given axis
	switch (axis) {
		// Bezouska: Commented out to lower time to avoid saturation.  See MicroMag
		// documentation discussion on gain numbers for various multipliers.
		/*
			// Sample at a "/4096" ratio which approximates to 60ms
			// This may be too long if we intend to achieve 5hz...
			// See MicroMag documentation to choose different division ratio.
			case 0: spi_write2(0x71); break; // X
			case 1: spi_write2(0x72); break; // Y
			case 2: spi_write2(0x73); break; // Z
			default: return INVALID_ARG; // The axis argument is incorrect.
		*/

	// Sample at a "/2048" ratio which approximates to 60ms
	// This may be too long if we intend to achieve 5hz...
	// See MicroMag documentation to choose different division ratio.
	case 0:
		spi_write(0x61);
		break; // X
	case 1:
		spi_write(0x62);
		break; // Y
	case 2:
		spi_write(0x63);
		break; // Z
	default:
		break; // The axis argument is incorrect.
	} // switch

	// Unselect Magnetometer
	output_high(MAG_SSNOT);

	return SUCCESS;
}

ReturnErr_t convertFrameMagToBody(signed int* readings, float* torqueCoilCommand,
								  mag_s* Data) // Mag frame and unit conversion
{
	float tempfloat;

	// First, we need to load the data into the struct
	Data->counts[0] = readings[0];
	Data->counts[1] = readings[1];
	Data->counts[2] = readings[2];

	// Next, we need to convert the data to microtesla.  The default conversion factor is 31.24
	// counts per uT, however, we are using conversion factors based on testing by Siamek.  These
	// come from the excel file HH Coil Calibration, located on Confluence at
	// https://confluence.misd.isi.edu:8443/download/attachments/10813771/HH+Coil+Calibration.xls
	Data->microTesla[0] = (float)Data->counts[0] / 30.046;
	Data->microTesla[1] = (float)Data->counts[1] / 31.683;
	Data->microTesla[2] = (float)Data->counts[2] / 31.486;

	// Last, we need to convert the frame by multiplying with a rotation matrix.
	// Here we convert:
	//   No change in X
	//   Ymag --> -Zbody
	//	 Zmag --> +Ybody
	// ---------------------
	// Before:
	//  Mag X | Body X
	//  Mag Y | Body -Z
	//  Mag Z | Body Y
	// After:
	//  Max X | Body X
	//  Max Y | Body Y
	//  Max Z | Body Z
	tempfloat = Data->microTesla[1]; // Save original Y
	Data->microTesla[1] = Data->microTesla[2]; // Turn Mag Y into Body Y.
	Data->microTesla[2] = (-1.0) * tempfloat; // Turn Mag Z into Body Z.

	// Calculate bias from torque coil and offset magnetometer readings
	// This is a terrible hack, if this flies on any satellite other than La Jument,
	// I will come back to the lab and put you on the rocket instead
	// Biases calculated experimentally
	removeTorqueCoilBias(torqueCoilCommand, Data);

	return SUCCESS;
}
static void removeTorqueCoilBias(float* torqueCoilCommand, mag_s* Data) {
	float coil_x = torqueCoilCommand[0];
	float coil_y = torqueCoilCommand[1];
	float coil_z = torqueCoilCommand[2];

	float x_bias = (BiasCoefficients[X_AXIS_X_COIL] * coil_x) +
				   (BiasCoefficients[X_AXIS_Y_COIL] * coil_y) +
				   (BiasCoefficients[X_AXIS_Z_COIL] * coil_z);
	float y_bias = (BiasCoefficients[Y_AXIS_X_COIL] * coil_x) +
				   (BiasCoefficients[Y_AXIS_Y_COIL] * coil_y) +
				   (BiasCoefficients[Y_AXIS_Z_COIL] * coil_z);
	float z_bias = (BiasCoefficients[Z_AXIS_X_COIL] * coil_x) +
				   (BiasCoefficients[Z_AXIS_Y_COIL] * coil_y) +
				   (BiasCoefficients[Z_AXIS_Z_COIL] * coil_z);

	Data->microTesla[0] = Data->microTesla[0] - x_bias;
	Data->microTesla[1] = Data->microTesla[1] - y_bias;
	Data->microTesla[2] = Data->microTesla[2] - z_bias;
	return;
}

/*=================================================================
  PURPOSE: Measure the magnetic field in a single axis
  UNMODIFIED VARIABLES: axis - 0 (X), 1 (Y), or 2 (Z)
  MODIFIED VARIABLES: reading - 2 byte int representing magnetic field
  GLOBALS: None
  RETURN VALUE: Error codes
  NOTES: This can be called continuously since it keeps a state.
-----------------------------------------------------------
*/
ReturnErr_t readSingleMagAxis(signed int* reading, int axis, short reset) {
	static short state = 0; // State of the function.

	if (reset)
		state = 0;

	switch (state) {
	case 0: {
		// sprintf(dbgbuf,"\r\nStarting axis %u",axis);
		// sendDBGALL(USER_PORT,dbgbuf);

		// Send request to magnetometer.  Return on FAILURE.
		if (requestSingleMagAxis(axis) != SUCCESS)
			return SUBFUNCTION_ERR;
	}
	case 1: {
		// Second, check if DRDY is still LOW
		if (!input_state(MAG_DRDY)) {
			// Make us keep looking
			state = 1;

			// Notify the calling function that we are still searching
			return BUSY;
		}

		// Variable for the reading
		unsigned char measurement[2];

		// Set flash to mag mode (gyro mode works too)
		SetSPIMode(MAG_SPI_MODE);

		// Select Magnetometer
		output_low(MAG_SSNOT);

		delay_us(1);

		// Aquire Measurements (Note that the MSB arrives first and
		// that this compiler places MSB in the higher byte of an int,
		// hence the reverse indexing).
		measurement[1] = spi_read(0xff);
		measurement[0] = spi_read(0xff);

		// Transfer the readings to the modified function argument
		memcpy(reading, measurement, 2);

		// Unselect Magnetometer
		output_high(MAG_SSNOT);

		// To Do: Change to a debug message
		// sendDBGALL(USER_PORT,"\r\nAxis complete.");

		state = 0;

		return SUCCESS;
	} // case
	} // switch

	return UNREACHABLE;
}

/*=================================================================
  PURPOSE: Initialize the magnetometer interface and device
  UNMODIFIED VARIABLES: None
  MODIFIED VARIABLES: None
  GLOBALS: None
  RETURN VALUE: None
  NOTES: None
-----------------------------------------------------------
*/

ReturnErr_t mag_init() {
	//	ReturnErr_t err;

	// Unselect Magnetometer
	output_high(MAG_SSNOT);

	return SUCCESS;
}

ReturnErr_t mag_functional_check(unsigned int max_time_seconds) {
	signed int data[3];
	ReturnErr_t response;
	unsigned long timeout;
	timeout = SEC_TIMER + max_time_seconds;

	// Initialize the Mag
	readAllMagAxis(data, 1);

	// Loop until response or failure
	do {
		response = readAllMagAxis(data, 0);
		if (SEC_TIMER > timeout) {
			response = FAILURE;
			break;
		}
	} while (response == BUSY);
	return response;
}

/*=================================================================
  PURPOSE: Convience function to read all three axis.
  UNMODIFIED VARIABLES: None
  MODIFIED VARIABLES: 3 element singed integer array
  GLOBALS: None
  RETURN VALUE: Error code.
  NOTES: Should be called repeatedly.
-----------------------------------------------------------
*/

ReturnErr_t readAllMagAxis(signed int* reading, short reset) {
	static int axis = 0;
	ReturnErr_t response;

	if (reset)
		axis = 0;

	// For each axis
	while (axis < 3) {
		// sendDBGALL(USER_PORT,"R");

		// Look for response.
		response = readSingleMagAxis(&reading[axis], axis, reset);

		// If BUSY was returned, leave
		if (response == BUSY) {
			// sendDBGALL(USER_PORT,"B");
			return BUSY;
		}

		// If the response is a failure, give up
		if (response != SUCCESS) {
			// Force the user to reset the function
			axis = 4;

			return SUBFUNCTION_ERR;
		}

		// If we found the result of the third axis, yay!
		if (response == SUCCESS && axis == 2) {
			// Force the user to reset the function
			axis = 4;
			// sendDBGALL(USER_PORT,"S");
			return SUCCESS;
		}

		// Move to next axis
		axis++;
	} // while

	// sendDBGALL(USER_PORT,"F");

	// Arrive here if the mag has not been reset
	return FAILURE;
}

//========================================
// 		Development Functions
//========================================

// Note: The below functions are only necessary for testing and development.  They
// can be removed during actual production

#ifdef DEBUG

/*=================================================================
  PURPOSE: Print out a human readable version of the data
  MODIFIED VARIABLES: None.
  UNMODIFIED: reading - a 3 element array of ints, one for each axis.
  GLOBALS: None.
  RETURN VALUE: None.
  NOTES: Untested.
-----------------------------------------------------------
*/

void printMagData(signed int* reading) {
	float a[3];

	// Raw Bytes and Count Values
	sprintf(dbgbuf, "\r\nX = %lX (%d), Y = %lX (%d), Z = %lX (%d)", reading[0], reading[0],
			reading[1], reading[1], reading[2], reading[2]);
	sendDBGALL(USER_PORT, dbgbuf);

	// Converted Measurements
	a[0] = (float)reading[0] / (float)UT_PER_BIT;
	a[1] = (float)reading[1] / (float)UT_PER_BIT;
	a[2] = (float)reading[2] / (float)UT_PER_BIT;

	sendDBGALL(USER_PORT, "\r\nX = ");
	printFloat(dbgbuf, a[0]);
	sendDBGALL(USER_PORT, dbgbuf);

	sendDBGALL(USER_PORT, "uT, Y = ");
	printFloat(dbgbuf, a[1]);
	sendDBGALL(USER_PORT, dbgbuf);

	sendDBGALL(USER_PORT, "uT, Z = ");
	printFloat(dbgbuf, a[2]);
	sendDBGALL(USER_PORT, dbgbuf);

	sendDBGALL(USER_PORT, "uT");

	return;
}

/*=================================================================
  PURPOSE: Print out a machine readable version of the data
  MODIFIED VARIABLES: None.
  UNMODIFIED: reading - a 3 element array of ints, one for each axis.
		line_number - an externally supplied line number to print.
  GLOBALS: None.
  RETURN VALUE: None.
  NOTES: Untested.
-----------------------------------------------------------
*/

void printMagDataCSV(signed int* reading, long line_number) {
	// Counter Variable
	int i;

	// Line Number
	sprintf(dbgbuf, "\r\n%Ld", line_number);
	sendDBGALL(USER_PORT, dbgbuf);

	// Rates
	for (i = 0; i < 3; i++) {
		sprintf(dbgbuf, ",%d", reading[i]);
		sendDBGALL(USER_PORT, dbgbuf);
	}

	return;
}

/*=================================================================
  PURPOSE: Automatically collect lots of gyroscope data (user specified
	or infinite).  Then, printit out in either human readable form or
	CSV.
  MODIFIED VARIABLES: None.
  UNMODIFIED: iterations - number of times to collect data.
		continuous - if true, collect data until user presses "q".
		human - if true, print in human readable form.  Otherwise, CSV.
  GLOBALS: None.
  RETURN VALUE: None.
  NOTES: All data is sampled at 5Hz.
-----------------------------------------------------------
*/

void magAutomaticDataCollection(long iterations, short continuous, short human) {
	signed int reading[3]; // The data structure to use for each iteration
	long i = 0; // Iteration counter
	long timeout; // Timeout used to pause before data collection
	short userFlag = 1; // Indicator that user wants to quit.  (False = quit)
	unsigned char temp; // Temporary character holder to check user keystroke
	ReturnErr_t response; // Error code returned by readAllMagAxis();

	// Let user know how to escape!
	sendDBGALL(USER_PORT,
			   "\r\nCollecting Mag data.  Press 'q' to quit.  Starting in 1 seconds ...");

	// Set the timeout timer
	timeout = SEC_TIMER + 1;

	// Loop until current time passes timeout.
	while (SEC_TIMER < timeout) {
		// Do nothing.
	}

	// userFlag defaults to true and will remain true until user
	// hits a 'q' keystroke.  If the function call has continuous as
	// true, then it will ignore the i vs. iterations condition.
	// if not, it will watch the counter and then quit when it supersedes
	// iterations.
	while (userFlag && (continuous || (i < iterations))) {
		// Iterate
		i++;

		short reset = 1;
		do {
			// Look for response.
			response = readAllMagAxis(&reading, reset);
			reset = 0;
			// sendDBGALL(USER_PORT,".");
		} while (response == BUSY);

		// If we never found the packet
		if (response != SUCCESS) {
			// Let user know
			sendDBGALL(USER_PORT, "\r\n ... failed!");
			return;
		} else {
			// Print out the information in some form
			if (human)
				printMagData(&reading);
			else
				printMagDataCSV(&reading, i);
		}

		// Check if a character is available
		if (getByte(USER_PORT, &temp)) {
			// If it is any form of 'q', signal that we want to quit.
			if (temp == 'q' || temp == 'Q')
				userFlag = 0;
		}

		// Force into 5Hz Mode (Determined empirically)
		delay_ms(125);
	}

	return;
}

#endif // DEBUG