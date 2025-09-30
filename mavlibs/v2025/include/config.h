#ifndef __CONFIG_H__
#define __CONFIG_H__

/***************************************************************************

								config.h

Company: USC/ISI
License: Proprietary
Author(s): Stephen Hunt

Purpose: Application level configuration settings.

***************************************************************************/

/*
 * Only preprocessor directives are allowed in this file
 */

#define PROJECT_ID "USCLAJ"

// Which PPM is this being compiled for? Choose one
//#define UPPER_PPM
#define LOWER_PPM

// Is this being compiled for flight?
//#define FLIGHT

// Print debug info?
#define DEBUG

#endif // __CONFIG_H__