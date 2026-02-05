#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include <stdint.h>
#include <time.h>

#define SCHEDULE_REPS_INFINITE      0xFF
#define SCHEDULER_MAX_TASKS         100

// Schedule task (as generic as possible)

typedef void (*schedfunc_f)();

typedef struct {
    schedfunc_f func;
    uint64_t next_release;
    uint32_t period_ms;
    uint16_t remaining_reps; 
} schedtask_s;

// Scheduling manager API

typedef struct {
    schedtask_s tasks[SCHEDULER_MAX_TASKS];
} scheduler_s;

// Initialize scheduler service
void scheduler_init(scheduler_s* s);

// Check scheduler for tasks that need to run and execute them
void scheduler_run_tasks(scheduler_s* s);

/*
 * Schedule <func> to execute starting at <start_time> for <reps> repetitions every <period_ms> milliseconds 
 * To schedule a one-shot task, set reps=1 and period_ms=0
 * To schedule an infinitely recurring task, set reps=SCHEDULE_REPS_INFINITE
 * Finitely recurring tasks have a limit of 65535 repetitions
 */
void schedule_absolute(scheduler_s* s, schedfunc_f* func, struct_tm start_time, uint16_t reps, uint32_t period_ms);

/*
 * Schedule <func> to execute starting at <start_delay_ms> milliseconds from now for <reps> repetitions every <period_ms> milliseconds 
 * To schedule a one-shot task, set reps=1 and period_ms=0
 * To schedule an infinitely recurring task, set reps=SCHEDULE_REPS_INFINITE
 * Finitely recurring tasks have a limit of 65535 repetitions
 */
void schedule_relative(scheduler_s* s, schedfunc_f* func, uint32_t start_delay_ms, uint16_t reps, uint32_t period_ms);

#endif // !__SCHEDULER_H__
