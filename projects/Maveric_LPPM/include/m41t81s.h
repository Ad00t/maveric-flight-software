#ifndef __M41T81S_H__
#define __M41T81S_H__

#include <time.h>

typedef struct {
    struct_tm time; 
    struct_tm init_time;
    struct_tm halted_time;
} ertc_s;

// Initialze ertc module with initial time in buf
void ertc_init(ertc_s* ertc, struct_tm time);

void ertc_get_time(ertc_s* ertc);

// Set ertc time to buf
void ertc_set_time(ertc_s* ertc, struct_tm time);

// Check if values are valid and reset if not
int1 ertc_heartbeat(ertc_s* ertc);

// Idk what this is for
void ertc_enable_fpm(ertc_s* ertc);

#endif
