/***************************************************************************

									pins.h

Company: USC/ISI
License: Proprietary
Author(s): Michael Aherne (mra)

Purpose: To define the pins used in cubesat projects

Changelog:

Date 	 |  Au.  |	Notes
03-28-10	mra		Created.
07-28-10	mra		Upgraded to include pins for the port TX/RX.  Previously
					had the #pin selects here, but they belong in the private
					c file.  Now, through a layer of indirection, we can change
					the pins without going to the pin_select code.
07-19-11	mra		Included warnings if neither UPPER_PPM nor LOWER_PPM is defined.

****************************************************************************
*/

#ifndef __PINS_H__
#define __PINS_H__


	#ifdef CUSTOM_PINS
		#include "pinscustom.h"
	#else

		#if defined(UPPER_PPM)
			#include "pinsupper.h"				// Final upper pins
		#elif  defined(LOWER_PPM)
			#include "pinslower.h"				// Final lower pins
		#else
			#error "No processor defined in pins.h"
		#endif // UPPER_PPM

	#endif // CUSTOM_PINS	

#endif

