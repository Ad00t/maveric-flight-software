#ifndef __AX100_H__
#define __AX100_H__

/***************************************************************************

								ax100.h

Company: USC/ISI
License: Proprietary
Author(s): Stephen Hunt

Purpose: Provides a base definition for interfacing with the AX100 transceiver 

***************************************************************************/

#include <stdint.h>
#include "ringbuf.h"
#include "cmdpkt.h"
#include "cmdmgr.h"

// kiss header size (2), csp header size (4), crc32 size (4), kiss footer size (1)
#define AX100_MAX_FRAME_SIZE          256
#define AX100_MIN_MESSAGE_SIZE        11
#define AX100_MAX_MESSAGE_SIZE        AX100_MAX_FRAME_SIZE - AX100_MIN_MESSAGE_SIZE

// MAIN TRANSCEIVER INTERFACE

typedef struct {
    uint8_t port; 
    int1 is_init;
} ax100_s;

void ax100_init(ax100_s* a, uint8_t port);
void ax100_set_power(ax100_s* a, int1 on);
int1 ax100_is_on(ax100_s* a);
// Transmits an arbitrary frame. You should not ever need to use this function. Just use cmd_dispatch() with dest as GS
void ax100_transmit_frame(ax100_s* a, uint8_t* frame, uint16_t len);

#endif
