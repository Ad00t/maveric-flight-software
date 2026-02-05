/***************************************************************************

									orbit.h

Company: USC/ISI/iControl
License: Proprietary
Author(s): Michael Aherne (mra), Fred Tubb

Purpose: To propogate an orbit.

Changelog:

Date 	 |  Au.  |	Notes
07-26-10	mra		Created using Fred's prop code.
07-27-10	mra		Updated to seperate init functions from propogator functions
					Also fixed the following:
					 - in tsec(), corrected pointer derefencing of *LOCtime
					 - in julian(), replaced the cast to int and back with a 
						CEIL_FLOOR() truncation.  Avoids integer rollover issues.
					 - in rk4(), corrected pointer dereferencing of *time.
					 - in inrange(), corrected call to fmodfred (should have had pointer passed)
					   and corrected return of the function (should have dereferenced *angle).

TODO: 


****************************************************************************
*/

#ifndef __ORBIT__
#define __ORBIT__


//========================================
//  			Dependencies
//========================================
#include "common.h"
#include "math.h"	
#include "debug.h"


//================================
//  	Public Defines
//================================
#define J2 1.08263e-3
#define J3 -2.532153e-6
#define J4 -1.610988e-6
#define J5 -2.357857e-7
#define TWOPI 6.2831853071796e0
#define DAYS_PER_SEC 0.11574074074e-4
#define ERATE  7.292115856e-5
#define DPR 57.29577951e0
#define RPOL 3432.370692e0
#define CNMF 6076.115486e0
#define GM  1.407644381e16
#define G_M 1.407644381e16
#define REM 3443.917387e0
//#define SUNATEPOCH 278.833540
#define SUNATEPOCH 280.46454		// EPOCH adjusted to 2010
#define TROPICALYR 365.2422
#define LONGATPERIGEE 282.596403
#define SUNECC 0.016718
#define RAD2DEG 57.2957787
#define DEG2RAD 0.01745329
#define L0 64.975464
#define P0 349.383063
#define N0 151.950429
//#define MOONINC 5.145396

#define ORBIT_DATA_SIZE		16
#define ORBIT_NO_INIT 		0
#define ORBIT_INIT			1

#define ORBIT_POS_X_INDEX			6
#define ORBIT_POS_Y_INDEX			7
#define ORBIT_POS_Z_INDEX			8
#define ORBIT_VEL_X_INDEX			9
#define ORBIT_VEL_Y_INDEX			10
#define ORBIT_VEL_Z_INDEX			11
#define ORBIT_GHA_INDEX				15


//================================
//  	Public Globals
//================================
float orbitData[ORBIT_DATA_SIZE];			// Changed size in newest model. Lost time vector.
float Sun_ECI[3];							// Position of sun in ECI

//================================
//  	Public Functions
//================================
void orbit(unsigned char initFlag, unsigned char *DateTime, float dt, float* orbitData);
void initOrbit(unsigned char * OrbitItime, float * pos_eci, float * vel_eci, unsigned char * currentTime);
void orbitPrintData(unsigned char format=0);			// Prints orbit data according to format
void sunmodel(unsigned char *gmt, float *sunvec);			
float fmodfred(float a,float b);
long orbitPeriod(float* oData);

#endif //__ORBIT__
