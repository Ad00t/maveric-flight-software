#ifndef __M41T81S_H__
#define __M41T81S_H__

#include <stdint.h>

/* Driver for the M41T81S I2C RTC on the motherboard
 * Uses the PPM internal RTC as a backup clock
*/

#include <time.h>

typedef struct {
    struct_tm time; 
    struct_tm init_time;
    struct_tm halted_time;
    int1 is_using_ertc; // Are we using the external I2C RTC or the internal one?
} ertc_s;

// Initialze ertc module with initial time in buf
void ertc_init(ertc_s* ertc, struct_tm* init_time);

// Clear ertc data
void ertc_clear(ertc_s* ertc);

void ertc_get_time(ertc_s* ertc);

// Set ertc time to buf
void ertc_set_time(ertc_s* ertc, struct_tm* time);

// Periodicially reset if not using ertc
int1 ertc_heartbeat(ertc_s* ertc);

// Idk what this is for
void ertc_enable_fpm(ertc_s* ertc);

#endif
