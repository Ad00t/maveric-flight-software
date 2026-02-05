/***************************************************************************

									geomag.h

Company: USC/ISI/iControl
License: Proprietary
Author(s): Michael Aherne (mra), Fred Tubb

Purpose: Geomagnetic Model

Changelog:

Date 	 |  Au.  |	Notes
09-20-10	mra		Created using Fred's geomag code.

TODO: 
  -


****************************************************************************
*/

#ifndef __GEOMAG__
#define __GEOMAG__


//========================================
//  			Dependencies
//========================================
//#include "common.h"
//extern unsigned char Lbuf[256];		// Work on removing this dependency

//================================
//  	Public Globals
//================================
float B_field_ECI[3];	//based on model
int globalCRCErrorCounter=0;

//================================
//  	Public Functions
//================================
void geomag(float *B, float *posECI ,float GHA, float days, short ReadGeomagFromFlash);


#endif //__GEOMAG__
