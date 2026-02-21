#include "scheduler.h"
#include "cmdmgr.h"
#include "systime.h"
#include <time.h>

#module

// HELPERS

void schedule_task(scheduler_s* s, schedtask_s task) {
    if (task.id < 0) {
        sprintf(LOGBUF, "schedule_task: task id invalid"); log_flush(LL_ERROR);
        return;
    }
    uint8_t start = s->i_free;
    while (s->tasks[s->i_free].id >= 0) {
        s->i_free = (s->i_free + 1) % SCHEDULER_MAX_TASKS;
        if (s->i_free == start) {
            sprintf(LOGBUF, "schedule_task: tasks buffer full"); log_flush(LL_ERROR);
            return;
        }
    }
    s->tasks[s->i_free] = task; 
    s->id_to_idx[task.id] = s->i_free;
}

// SCHEDULER PUBLIC API

void scheduler_init(scheduler_s* s) {
    memset(s->tasks, 0, sizeof(s->tasks));
    memset(s->id_to_idx, 0, sizeof(s->id_to_idx));
    uint8_t i;
    for (i = 0; i < SCHEDULER_MAX_TASKS; i++) {
        s->tasks[i].id = -1;
    }
    s->i_free = 0;
}

void scheduler_run_tasks(scheduler_s* s, cmdmgr_s* cmdmgr) {
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
            if (s->tasks[i].is_cmd) {
                ringbuf_s rcvbuf;
                rb_init(&rcvbuf);
                uint8_t j;
                for (j = 0; j < CMD_MAX_LEN; j++) {
                    rb_push(&rcvbuf, (uint8_t) s->tasks[i].cmd[j]); 
                }
                cmdpkt_s pkt;
                cmdpkt_init(&pkt);
                cmdmgr_parse_stream(cmdmgr, &rcvbuf, &pkt);
            } else {
                schedfunc_f schedfunc = s->tasks[i].func;
                schedfunc();
            }
        }
    }
}

void scheduler_deschedule(scheduler_s* s, uint8_t id) {
    memset(&s->tasks[s->id_to_idx[id]], 0, sizeof(schedtask_s));
    s->id_to_idx[id] = 0;
}

void scheduler_schedule_at(scheduler_s* s, int8_t id, schedfunc_f func, struct_tm start_time, uint16_t reps, uint32_t period_ms) {
    schedtask_s task = {0};
    task.id = id;
    task.func = func;
    memset(task.cmd, 0, sizeof(task.cmd));
    task.is_cmd = FALSE;
    task.next_release = rtc_to_epoch_ms(start_time);
    task.remaining_reps = reps;
    task.period_ms = period_ms;
    task.active = TRUE;
    schedule_task(s, task);
}

void scheduler_schedule_at(scheduler_s* s, int8_t id, char* cmd, struct_tm start_time, uint16_t reps, uint32_t period_ms) {
    schedtask_s task = {0};
    task.id = id;
    task.func = NULL;
    memcpy(task.cmd, cmd, sizeof(task.cmd));
    task.is_cmd = TRUE;
    task.next_release = rtc_to_epoch_ms(start_time);
    task.remaining_reps = reps;
    task.period_ms = period_ms;
    task.active = TRUE;
    schedule_task(s, task);
}

void scheduler_schedule_in(scheduler_s* s, int8_t id, schedfunc_f func, uint32_t start_delay_ms, uint16_t reps, uint32_t period_ms) {
    schedtask_s task = {0};
    task.id = id;
    task.func = func;
    memset(task.cmd, 0, sizeof(task.cmd));
    task.is_cmd = FALSE;
    task.next_release = systime_epoch_ms() + start_delay_ms;
    task.remaining_reps = reps;
    task.period_ms = period_ms;
    task.active = TRUE;
    schedule_task(s, task);
}

void scheduler_schedule_in(scheduler_s* s, int8_t id, char* cmd, uint32_t start_delay_ms, uint16_t reps, uint32_t period_ms) {
    schedtask_s task = {0};
    task.id = id;
    task.func = NULL;
    memcpy(task.cmd, cmd, sizeof(task.cmd));
    task.is_cmd = TRUE;
    task.next_release = systime_epoch_ms() + start_delay_ms;
    task.remaining_reps = reps;
    task.period_ms = period_ms;
    task.active = TRUE;
    schedule_task(s, task);
}
