#include "scheduler.h"
#include "schedfunc.h"
#include "systime.h"
#include <time.h>

#module

// HELPERS

void schedule_task(scheduler_s* s, schedtask_s task) {
    uint8_t start = s->i_free;
    while (s->tasks[s->i_free].active) {
        s->i_free = (s->i_free + 1) % SCHEDULER_MAX_TASKS;
        if (s->i_free == start) {
            fprintf(COM_D, "%s[%s] schedule_task: scheduler tasks full\n", KRED, NODE_LBL);
            return;
        }
    }
    s->tasks[s->i_free] = task; 
}

// SCHEDULER PUBLIC API

void scheduler_init(scheduler_s* s) {
    memset(s->tasks, 0, SCHEDULER_MAX_TASKS * sizeof(schedtask_s));
    s->i_free = 0;
    scheduler_init_schedules(s);
}

void scheduler_run_tasks(scheduler_s* s) {
    uint8_t i;
    for (i = 0; i < SCHEDULER_MAX_TASKS; i++) {
        uint64_t now = systime_epoch_ms();
        int64_t diff = s->tasks[i].next_release - now; // >= is bugged for large values, so use subtraction instead
        if (s->tasks[i].active && diff <= 0) {
            if (s->tasks[i].remaining_reps != SCHEDULE_REPS_INFINITE) {
                s->tasks[i].remaining_reps--;
                s->tasks[i].active = (s->tasks[i].remaining_reps > 0);
            }
            if (s->tasks[i].active) {
                uint64_t newnext = now + s->tasks[i].period_ms;
                s->tasks[i].next_release = newnext; 
            }
            schedfunc_f schedfunc = s->tasks[i].func;
            schedfunc();
        }
    }
}

void scheduler_deschedule(scheduler_s* s, uint8_t id) {
    uint8_t i;
    for (i = 0; i < SCHEDULER_MAX_TASKS; i++) {
        if (s->tasks[i].active && s->tasks[i].id == id) {
            s->tasks[i].active = FALSE;
        }
    }
}

void scheduler_schedule_at(scheduler_s* s, uint8_t id, schedfunc_f* func, struct_tm start_time, uint16_t reps, uint32_t period_ms) {
    schedtask_s task;
    task.id = id;
    task.func = func;
    task.next_release = rtc_to_epoch_ms(start_time);
    task.remaining_reps = reps;
    task.period_ms = period_ms;
    task.active = TRUE;
    schedule_task(s, task);
}

void scheduler_schedule_in(scheduler_s* s, uint8_t id, schedfunc_f* func, uint32_t start_delay_ms, uint16_t reps, uint32_t period_ms) {
    schedtask_s task;
    task.id = id;
    task.func = func;
    task.next_release = systime_epoch_ms() + start_delay_ms;
    task.remaining_reps = reps;
    task.period_ms = period_ms;
    task.active = TRUE;
    schedule_task(s, task);
}
