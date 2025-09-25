#ifndef __INET_H__
#define __INET_H__

/***************************************************************************

                                inet.h

Company: USC/ISI
License: Proprietary
Author(s): Stephen Hunt

Purpose: Custom inet.h. CCS compiler does not provide inet libraries.
         Provides functionality for network interfacing.

***************************************************************************/

#include <stdint.h>


// Converts unsigned integer from host byte order to network byte order
uint32_t htonl(uint32_t hostLong);
// Converts unsigned short from host byte order to network byte order
uint16_t htons(uint16_t hostShort);
// Converts unsigned integer from network byte order to host byte order
uint32_t ntohl(uint32_t netLong);
// Converts unsigned short from network byte order to host byte order
uint16_t ntohs(uint16_t netShort);

#endif // __INET_H__