#ifndef __ADDRESSES_H__
#define __ADDRESSES_H__

/*
address.h

Header file to store useful addresses.
It's a good idea to check for consistency between the
addresses and the size allocations.

See the associated Address_Helper.xls file.
*/

#include "config.h"

#ifdef UPPER_PPM
#include "address_upper.h"
#endif
#ifdef LOWER_PPM
#include "address_lower.h"
#endif

#endif // __ADDRESSES_H__