#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include "schedfunc.h"
#include <stdint.h>
#include <time.h>

#define SCHEDULE_REPS_INFINITE      0xFF
#define SCHEDULER_MAX_TASKS         32 

// Schedule task (as generic as possible)

typedef struct {
    schedfunc_f func;
    uint64_t next_release;
    uint32_t period_ms;
    uint16_t remaining_reps; 
    uint8_t id;
    int1 active;
} schedtask_s;

// Scheduling manager API

typedef struct {
    schedtask_s tasks[SCHEDULER_MAX_TASKS];
    uint8_t i_free;
} scheduler_s;

// Initialize scheduler service
void scheduler_init(scheduler_s* s);

// Check scheduler for tasks that need to run and execute them
void scheduler_run_tasks(scheduler_s* s);

// Find the scheduled task with this id and clear it
void scheduler_deschedule(scheduler_s* s, uint8_t id);

/*
 * Schedule <func> to execute starting at <start_time> for <reps> repetitions every <period_ms> milliseconds 
 * To schedule a one-shot task, set reps=1 and period_ms=0
 * To schedule an infinitely recurring task, set reps=SCHEDULE_REPS_INFINITE
 * Finitely recurring tasks have a limit of 65535 repetitions
 * MAKE SURE THE SCHEDULE PERIOD IS GREATER THAN THE FUNCTION'S RUNTIME
 */
void scheduler_schedule_at(scheduler_s* s, uint8_t id, schedfunc_f* func, struct_tm start_time, uint16_t reps, uint32_t period_ms);

/*
 * Schedule <func> to execute starting in <start_delay_ms> milliseconds from now for <reps> repetitions every <period_ms> milliseconds 
 * To schedule a one-shot task, set reps=1 and period_ms=0
 * To schedule an infinitely recurring task, set reps=SCHEDULE_REPS_INFINITE
 * Finitely recurring tasks have a limit of 65535 repetitions
 * MAKE SURE THE SCHEDULE PERIOD IS GREATER THAN THE FUNCTION'S RUNTIME
 */
void scheduler_schedule_in(scheduler_s* s, uint8_t id, schedfunc_f* func, uint32_t start_delay_ms, uint16_t reps, uint32_t period_ms);

#endif // !__SCHEDULER_H__
