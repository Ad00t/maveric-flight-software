#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include "mcppkt.h"
#include <stdint.h>

#define SCHEDULER_MAX_FUNC_TASKS        12 
#define SCHEDULER_MAX_CMD_TASKS         4
#define SCHEDULER_MAX_TASKS             SCHEDULER_MAX_CMD_TASKS + SCHEDULER_MAX_FUNC_TASKS
#define SCHEDULER_MAX_IDS               32
#define SCHEDULE_REPS_INFINITE          0xFFFF

// Schedtask: provides base management and timing functionality to specialized schedule tasks

typedef enum {
    ST_TYPE_NONE = 0,
    ST_TYPE_FUNC = 1,
    ST_TYPE_CMD = 2
} schedtask_type_e;

typedef void (*schedfunc_f)(void);

typedef struct {
    uint64_t next_release;
    uint32_t period_ms;
    uint16_t remaining_reps; 
    schedfunc_f func;
    mcppkt_s* cmd_ptr; // Only store a pointer to the cmd in the scheduler cmds buffer so we don't have 20 cmd buffers
    schedtask_type_e type;
    uint8_t id;
    int1 active;
} schedtask_s;

void schedtask_create(schedtask_s* task, uint8_t id, schedtask_type_e type, uint64_t next_release, uint32_t period_ms, uint16_t reps);

// Scheduling manager API

typedef struct {
    schedtask_s tasks[SCHEDULER_MAX_TASKS]; // Internally segmented between schedtask types
    schedtask_s* id_map[SCHEDULER_MAX_IDS]; // Contains mapping of task ids to pointer to schedtask in tasks buffer
    mcppkt_s cmds[SCHEDULER_MAX_CMD_TASKS]; // Buffer of cmds
} scheduler_s;

// Initialize scheduler service
void scheduler_init(scheduler_s* s);

// Check scheduler for tasks that need to run and execute them
void scheduler_run_tasks(scheduler_s* s, cmdmgr_s* cmdmgr);

// Find the scheduled task with this id and deschedule it
status_e scheduler_deschedule(scheduler_s* s, uint8_t id);

// Reschedule a previously descheduled task
status_e scheduler_reschedule_at(scheduler_s*, uint8_t id, rtc_time_t rtc);
status_e scheduler_reschedule_in(scheduler_s*, uint8_t id, uint32_t start_delay_ms);

// Find the scheduled task with this id and clear it
status_e scheduler_clear_task(scheduler_s* s, uint8_t id);

// Update the next releases of all queued schedules if we've updated systime
void scheduler_refresh_all(scheduler_s* s, uint64_t oldtime);

/*
 * Schedule <func> to execute starting at <start_time> for <reps> repetitions every <period_ms> milliseconds 
 * To schedule a one-shot task, set reps=1 and period_ms=0
 * To schedule an infinitely recurring task, set reps=SCHEDULE_REPS_INFINITE
 * Finitely recurring tasks have a limit of 65535 repetitions
 * MAKE SURE THE SCHEDULE PERIOD IS GREATER THAN THE FUNCTION'S RUNTIME
 */
status_e scheduler_schedule_func_at(scheduler_s* s, uint8_t id, schedfunc_f func, rtc_time_t start_time, uint32_t period_ms, uint16_t reps);
// The command version
status_e scheduler_schedule_cmd_at(scheduler_s* s, uint8_t id, mcppkt_s* p, rtc_time_t start_time, uint32_t period_ms, uint16_t reps);

/*
 * Schedule <func> to execute starting in <start_delay_ms> milliseconds from now for <reps> repetitions every <period_ms> milliseconds 
 * To schedule a one-shot task, set reps=1 and period_ms=0
 * To schedule an infinitely recurring task, set reps=SCHEDULE_REPS_INFINITE
 * Finitely recurring tasks have a limit of 65535 repetitions
 * MAKE SURE THE SCHEDULE PERIOD IS GREATER THAN THE FUNCTION'S RUNTIME
 */
status_e scheduler_schedule_func_in(scheduler_s* s, uint8_t id, schedfunc_f func, uint32_t start_delay_ms, uint32_t period_ms, uint16_t reps);
// The command version
status_e scheduler_schedule_cmd_in(scheduler_s* s, uint8_t id, mcppkt_s* p, uint32_t start_delay_ms, uint32_t period_ms, uint16_t reps);

#endif // !__SCHEDULER_H__
