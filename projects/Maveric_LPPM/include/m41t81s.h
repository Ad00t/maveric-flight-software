#ifndef __M41T81S_H__
#define __M41T81S_H__

#include <stdint.h>

/* Driver for the M41T81S I2C RTC on the motherboard
 * Uses the PPM internal RTC as a backup clock
*/

#include <time.h>

typedef struct {
    rtc_time_t time; 
    rtc_time_t init_time;
    rtc_time_t halted_time;
    status_e heartbeat;
    int1 is_using_ertc; // Are we using the external I2C RTC or the internal one?
    int1 is_init;
} ertc_s;

// Initialze ertc module with initial time in buf
status_e ertc_init(ertc_s* ertc, rtc_time_t* init_time);

// Clear ertc data
void ertc_clear(ertc_s* ertc);

status_e ertc_get_time(ertc_s* ertc);

// Set ertc time to buf
status_e ertc_set_time(ertc_s* ertc, rtc_time_t* time);

// Periodicially check and store heartbeat 
void ertc_check_heartbeat(ertc_s* ertc);

// Idk what this is for
void ertc_enable_fpm(ertc_s* ertc);

#endif
