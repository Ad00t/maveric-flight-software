#ifndef __SCHEDFUNC_H__
#define __SCHEDFUNC_H__

#include "scheduler.h"

void scheduler_init_schedules(scheduler_s* scheduler);

// SCHEDULE FUNCTIONS

void schedfunc_print_hk(void);
void schedfunc_heartbeats(void);
void schedfunc_read_sensors(void);
void schedfunc_test_ax100(void);

#endif // !__SCHEDFUNC_H__
