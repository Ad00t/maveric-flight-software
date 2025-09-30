#ifndef __GLOBALS_H__
#define __GLOBALS_H__

/***************************************************************************

								globals.h

Company: USC/ISI
License: Proprietary
Author(s): Stephen Hunt

Purpose: Declares global objects to be used throughout program

***************************************************************************/

#include "address.h"
#include "config.h"

extern TFlashProtected globals;

#ifdef UPPER_PPM
#include "flashmanager.h"
#include "linkmanager.h"
#include "payloadmanager.h"

extern LinkManager gLinkManager;
extern PayloadManager gPayloadManager;
extern FlashManager gFlashManager;
#endif

#ifdef LOWER_PPM
#endif

#endif // __GLOBALS_H__