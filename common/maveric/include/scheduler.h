#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__


// Schedule task (as generic as possible)

typedef void (*sched_task_f)();

typedef enum {
    SCHED_TASK_ONESHOT,
    SCHED_TASK_PERIODIC,
    SCHED_TASK_FINITE
} sched_task_type_e;

typedef struct {
    sched_task_f task;

    uint32_t next_release;
    uint32_t period_ticks;

    uint16_t remaining_runs;   /* valid for FINITE */

    sched_task_type_e type;
    int1 active;
} sched_task_s;

// Scheduling manager API

typedef struct {
    rtc_time_t time; 
} scheduler_s;

void scheduler_init(scheduler_s* s, rtc_time_t time);

void scheduler_set_time(scheduler_s* s, rtc_time_t time);



#endif // !__SCHEDULER_H__
