/***************************************************************************

								gyro.h

Company: USC/ISI
License: Proprietary
Author(s): Will Bezouska, Adhit Siripurapu

Purpose: Header file for the ADIS16260 MEMS Gyroscope

Usage: See the descriptions of each function below.

Changelog:

Date 	 |  Au.  |	Notes
12-05-25  Adhit Modularity improvements
07-15-10  Will	Initial revision.		

****************************************************************************
*/


/*  Notes on mounting:

	Identification of gryoscopes is very arbitrary.  Their numbering comes
	from the original numbering for the chip select lines.  These lables
 	(CS1, CS2, etc) have tended to shift around.  Therefore it has been
	chosen to identify the gyroscopes as mounted according to the obvious:

	Gyroscope 1 - X
	Gyroscope 2 - Y
	Gyroscope 3 - Z

	This is basically determined in one of the pin header files.  By choosing
	which chip select line GYROCS1 refers to, for example, one is essentially
	choosing which gyroscope to communicat with.  

	The X, Y, Z, axis refer to those found on confluence.  Z axis points out 
	through the antenna feedhorn.  The +X axis points out the RBF/USB panel 
	and the +Y axis is the one formed such that sweeping from +X to +Y produces
	+Z according to the right hand rule.

	Positive spin rate reading represents a clockwise rotation of
	the gyroscope when the gyroscope is viewed from the top. See datasheet.

	Gyroscope 1 - X
	Gyroscope 2 - 
	Gyroscope 3 - 

*/

#ifndef __ADIS16260_H__
#define __ADIS16260_H__

#include <stdint.h>

#define NUM_GYROS			3

// The following defines are commands straight from the datasheet
#define GYRO_FLASH_CNT				0x00		// Flash memory write count
#define GYRO_SUPPLY_OUT				0x02		// Output, power supply measurement
#define GYRO_GYRO_OUT				0x04		// Output, rate of rotation measurement
#define GYRO_AUX_ADC				0x0A		// Output, analog input channel measurement
#define GYRO_TEMP_OUT				0x0C		// Output, internal temperature measurement
#define	GYRO_ANGL_OUT				0x0E		// Output, angle displacement
#define GYRO_GYRO_CAL_OFFSET		0x14		// Calibration, offset/bias adjustment (Default: 0x0000)
#define GYRO_GYRO_CAL_SCALE			0x16		// Calibration, scale adjustment (Default: 0x0800)
#define GYRO_ALM_MAG1				0x20		// Alarm 1 magnitude/polarity setting (Default: 0x0000)
#define	GYRO_ALM_MAG2				0x22		// Alarm 2 magnitude/polarity setting (Default: 0x0000)
#define GYRO_ALM_SMPL1				0x24		// Alarm 1 dynamic rate of change setting (Default: 0x0000)
#define GYRO_ALM_SMPL2				0x26		// Alarm 2 dynamic rate of change setting (Default: 0x0000)
#define GYRO_ALM_CTRL				0x28		// Alarm control register (Default: 0x0000)
#define GYRO_AUX_DAC				0x30		//
#define GYRO_GPIO_CTRL				0x32
#define GYRO_MSC_CTRL				0x34
#define GYRO_SMPL_PRD				0x36
#define GYRO_SENS_AVG				0x38		// Control, dynamic range, filtering.  (Default: 0x0402)
#define GYRO_DIAG_STAT              0x3C
// more skipped here...
#define GYRO_LOT_ID_1				0x52
#define GYRO_LOT_ID_2				0x54
#define GYRO_PRODUCT_ID				0x56		// Product ID (should be 0x3F84)
#define GYRO_SERIAL_NUM				0x58		// Serial Number (

// Gyro data structure, holding all the most recent readings from the gyro.

typedef struct {
    uint8_t cs_x, cs_y, cs_z, on;

    uint16_t rate_raw[NUM_GYROS];   // Raw rate data
    uint16_t temp_raw[NUM_GYROS];   // Raw temp data
	uint16_t rate_cnt[NUM_GYROS];	// Rates in counts
	uint16_t temp_cnt[NUM_GYROS];	// Temperature in counts
	uint16_t error[NUM_GYROS];		// Status Bitfield
	
    int1 rate_nd[NUM_GYROS];		// Rate New Data Indicator
	int1 temp_nd[NUM_GYROS];		// Temp New Data Indicator
	int1 rate_ea[NUM_GYROS];		// Rate Alarm Indicator
	int1 temp_ea[NUM_GYROS];		// Temp Alarm Indicator 
	
    float rate_dps[NUM_GYROS];		// Rates in deg/sec
	float rate_rps[NUM_GYROS];		// Rates in rad/sec
	float temp[NUM_GYROS];		    // Temperature in degrees C
} gyro_s;

// API

// Initialize gyro with given pins & turn on
void gyro_init(gyro_s* gyro, uint8_t cs_x, uint8_t cs_y, uint8_t cs_z, uint8_t on);

// Clear gyro data
void gyro_clear(gyro_s* gyro);

// Set gyro power pin
void gyro_set_power(gyro_s* gyro, int1 on);

// Make a read request at DMA address reg and output result in res
void gyro_read_reg(gyro_s* gyro, uint8_t reg, uint16_t* res);
// Make a write request at DMA address reg with data data
void gyro_write_reg(gyro_* gyro, uint8_t reg, uint16_t data);

// Verifies gyro operation by querying the product ID
int1 gyro_heartbeat(gyro_s* gyro);					

// Read & process read, temp, and error status for all gyros 
void gyro_read_all(gyro_s* gyro);			

#endif // __GYRO_H__
