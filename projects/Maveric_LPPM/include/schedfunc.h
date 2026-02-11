#ifndef __SCHEDFUNC_H__
#define __SCHEDFUNC_H__

#include "scheduler.h"

void scheduler_init_schedules(scheduler_s* scheduler);

// SCHEDULE FUNCTIONS

void schedfunc_get_ertc_time(void);
void schedfunc_systime_sync(void);
void schedfunc_print_hk(void);
void schedfunc_heartbeats(void);
void schedfunc_read_sensors(void);
void schedfunc_test_disable_ertc(void);

#endif // !__SCHEDFUNC_H__
