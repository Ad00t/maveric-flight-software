#include "scheduler.h"
#include "cmdmgr.h"
#include "systime.h"
#include "common.h"
#include <time.h>

#module

// HELPERS

status_e schedule_task(scheduler_s* s, schedtask_s task, uint8_t i_start, uint8_t max_tasks, uint8_t* i_task) {
    if (s->id_map[task.id] != NULL) {  
        sprintf(LOGBUF, "schedule_task: task id in use"); log_error();
        return FAILURE;
    }
    uint8_t i_free = i_start;
    while (s->tasks[i_free].active) {
        if (++i_free == i_start + max_tasks) {
            sprintf(LOGBUF, "schedule_task: tasks buffer full"); log_error();
            return FAILURE;
        }
    }
    s->tasks[i_free] = task; 
    s->id_map[task.id] = &s->tasks[i_free];
    *i_task= i_free;
    return SUCCESS;
}

void create_schedtask(schedtask_s* task, uint8_t id, schedtask_type_e type, uint64_t next_release, uint32_t period_ms, uint16_t reps) {
    task->id = id;
    task->type = type;
    task->active = TRUE;
    task->next_release = next_release;
    task->period_ms = period_ms;
    task->remaining_reps = reps;
}

// SCHEDULER PUBLIC API

void scheduler_init(scheduler_s* s) {
    memset(s, 0, sizeof(scheduler_s));
    uint8_t i;
    for (i = 0; i < SCHEDULER_MAX_TASKS; i++) {
        s->tasks[i].id = 0;
    }
    for (i = 0; i < SCHEDULER_MAX_CMD_TASKS; i++) {
        cmdpkt_init(&s->cmds[i]);
    }
}

void scheduler_run_tasks(scheduler_s* s, cmdmgr_s* cmdmgr) {
    uint8_t i;
    for (i = 0; i < SCHEDULER_MAX_TASKS; i++) {
        uint64_t now = systime_epoch_ms();
        int64_t diff = s->tasks[i].next_release - now; // >= is bugged for large values, so use subtraction instead
        if (!(s->tasks[i].active && diff <= 0)) continue;
        if (s->tasks[i].remaining_reps != SCHEDULE_REPS_INFINITE) {
            s->tasks[i].remaining_reps--;
            s->tasks[i].active = (s->tasks[i].remaining_reps > 0);
        }
        if (s->tasks[i].active) {
            uint64_t newnext = now + s->tasks[i].period_ms;
            s->tasks[i].next_release = newnext; 
        }
        switch (s->tasks[i].type) {
            case FUNC:
                schedfunc_f schedfunc = s->tasks[i].func;
                if (schedfunc != NULL) schedfunc();
                break;
            case CMD:
                cmdmgr_process_cmd(cmdmgr, s->tasks[i].cmd_ptr);
                break;
        }
    }
}

status_e scheduler_deschedule(scheduler_s* s, uint8_t id) {
    schedtask_s* task_ptr = s->id_map[id];
    if (task_ptr == NULL) return FAILURE;
    if (task_ptr->type == CMD) memset(task_ptr->cmd_ptr, 0, CMD_MAX_LEN);
    memset(task_ptr, 0, sizeof(schedtask_s));
    s->id_map[id] = NULL;
    return SUCCESS;
}

void scheduler_refresh_all(scheduler_s* s, uint64_t oldtime) {
    uint8_t i;
    uint64_t now = systime_epoch_ms();
    for (i = 0; i < SCHEDULER_MAX_TASKS; i++) {
        if (!s->tasks[i].active) continue;
        int64_t diff = s->tasks[i].next_release - oldtime; // >= is bugged for large values, so use subtraction instead
        uint64_t newnext = now + diff;
        s->tasks[i].next_release = newnext; 
    }
}

status_e scheduler_schedule_func_at(scheduler_s* s, uint8_t id, schedfunc_f func, rtc_time_t start_time, uint32_t period_ms, uint16_t reps) {
    schedtask_s task = {0};
    create_schedtask(&task, id, FUNC, rtc_to_epoch_ms(start_time), period_ms, reps);
    task.func = func;
    uint8_t i_task;
    return schedule_task(s, task, 0, SCHEDULER_MAX_FUNC_TASKS, &i_task); // Func tasks segment of tasks buffer
}

status_e scheduler_schedule_cmd_at(scheduler_s* s, uint8_t id, cmdpkt_s* p, rtc_time_t start_time, uint32_t period_ms, uint16_t reps) {
    schedtask_s task = {0};
    create_schedtask(&task, id, CMD, rtc_to_epoch_ms(start_time), period_ms, reps);
    uint8_t i_task;
    if (schedule_task(s, task, SCHEDULER_MAX_FUNC_TASKS, SCHEDULER_MAX_CMD_TASKS, &i_task) == FAILURE) // Cmd tasks segment of tasks buffer
        return FAILURE; 
    cmdpkt_s* cmd_ptr = &s->cmds[i_task - SCHEDULER_MAX_FUNC_TASKS]; 
    memcpy(cmd_ptr, p, sizeof(cmdpkt_s));
    s->tasks[i_task].cmd_ptr = cmd_ptr;
    return SUCCESS;
}

status_e scheduler_schedule_func_in(scheduler_s* s, uint8_t id, schedfunc_f func, uint32_t start_delay_ms, uint32_t period_ms, uint16_t reps) {
    schedtask_s task = {0};
    create_schedtask(&task, id, FUNC, systime_epoch_ms() + start_delay_ms, period_ms, reps);
    task.func = func;
    uint8_t i_task;
    return schedule_task(s, task, 0, SCHEDULER_MAX_FUNC_TASKS, &i_task);
}

status_e scheduler_schedule_cmd_in(scheduler_s* s, uint8_t id, cmdpkt_s* p, uint32_t start_delay_ms, uint32_t period_ms, uint16_t reps) {
    schedtask_s task = {0};
    create_schedtask(&task, id, CMD, systime_epoch_ms() + start_delay_ms, period_ms, reps);
    uint8_t i_task; 
    if (schedule_task(s, task, SCHEDULER_MAX_FUNC_TASKS, SCHEDULER_MAX_CMD_TASKS, &i_task) == FAILURE)
        return FAILURE;
    cmdpkt_s* cmd_ptr = &s->cmds[i_task - SCHEDULER_MAX_FUNC_TASKS]; 
    memcpy(cmd_ptr, p, sizeof(cmdpkt_s));
    s->tasks[i_task].cmd_ptr = cmd_ptr;
    return SUCCESS;
}
