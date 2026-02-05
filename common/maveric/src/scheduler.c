#include "scheduler.h"
#include "systime.h"
#include <time.h>

#module

void scheduler_init(scheduler_s* s) {

}

void scheduler_run_tasks(scheduler_s* s) {

}

void schedule_absolute(scheduler_s* s, schedfunc_f* func, struct_tm start_time, uint16_t reps, uint32_t period_ms) {
    schedtask_s task;
    task.func = func;
    task.next_release = rtc_to_epoch_ms(start_time);
    task.remaining_reps = reps;
    task.period_ms = period_ms;
}

void schedule_relative(scheduler_s* s, schedfunc_f* func, uint32_t start_delay_ms, uint16_t reps, uint32_t period_ms) {
    schedtask_s task;
    task.func = func;
    task.next_release = systime_epoch_ms() + start_delay_ms;
    task.remaining_reps = reps;
    task.period_ms = period_ms;
}
