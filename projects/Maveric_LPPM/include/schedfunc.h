#ifndef __SCHEDFUNC_H__
#define __SCHEDFUNC_H__

#include "scheduler.h"

typedef void (*schedfunc_f)(void);

void scheduler_init_schedules(scheduler_s* scheduler);

// SCHEDULE FUNCTIONS

void schedfunc_get_ertc_time(void);
void schedfunc_systime_sync(void);
void schedfunc_print_hk(void);
void schedfunc_heartbeats(void);

#endif // !__SCHEDFUNC_H__
