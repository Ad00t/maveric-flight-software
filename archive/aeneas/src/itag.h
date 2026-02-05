/***************************************************************************

								itag.h

Company: USC/ISI
License: Proprietary
Author(s): Will Bezouska

Purpose: Provide hardware interface to magnetometer.

Usage: See the descriptions of each function below.

Changelog:

Date 	 |  Au.  |	Notes
10-25-10  Will	Initial revision.		

****************************************************************************
*/

#ifndef __ITAG_H__
#define __ITAG_H__


//========================================
// 			Driver Functions
//========================================

void itag_init(unsigned char port);
ReturnErr_t itag_functional_check(unsigned char port, unsigned int timeout_seconds);
void itag_on(unsigned char port);
void itag_off(unsigned char port);
void itag_once(unsigned char port);
void itag_ping(unsigned char port);
void sendVIAiTag(unsigned char port, unsigned char * data, unsigned char length, unsigned char * timestamp);

//========================================
// 		Development Functions
//========================================

#ifdef DEBUG


#endif // DEBUG

#endif // __MAG_H__
