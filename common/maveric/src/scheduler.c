#include "scheduler.h"
#include "mcpmgr.h"
#include "flashmgr.h"
#include "systime.h"
#include "common.h"
#include <time.h>

#module

// HELPERS

status_e schedule_task(scheduler_s* s, schedtask_s task, uint8_t i_start, uint8_t max_tasks, uint8_t* i_task) {
    if (s->id_map[task.id] != NULL && s->id_map[task.id]->type != ST_TYPE_NONE) {  
        sprintf(LOGBUF, "schedule_task: task id in use: %u", task.id); log_error();
        return FAILURE;
    }
    uint8_t i_free = i_start;
    while (s->tasks[i_free].type != ST_TYPE_NONE) {
        if (++i_free == i_start + max_tasks) {
            sprintf(LOGBUF, "schedule_task: tasks buffer full while scheduling id: %u", task.id); log_error();
            return FAILURE;
        }
    }
    s->tasks[i_free] = task; 
    s->id_map[task.id] = &s->tasks[i_free];
    *i_task= i_free;
    return SUCCESS;
}

// SCHEDTASK

void schedtask_create(schedtask_s* task, uint8_t id, schedtask_type_e type, uint64_t next_release, uint32_t period_ms, uint16_t reps) {
    task->id = id;
    task->type = type;
    task->active = TRUE;
    task->next_release = next_release;
    task->period_ms = period_ms;
    task->remaining_reps = reps;
}

// SCHEDULER PUBLIC API

extern flashmgr_s g_flashmgr;

void scheduler_init(scheduler_s* s) {
    memset(s, 0, sizeof(scheduler_s));
    uint8_t i;
    for (i = 0; i < SCHEDULER_MAX_CMD_TASKS; i++) {
        mcppkt_init(&s->cmds[i]);
    }
}

void scheduler_run_tasks(scheduler_s* s, mcpmgr_s* mcpmgr) {
    uint8_t i;
    for (i = 0; i < SCHEDULER_MAX_TASKS; i++) {
        uint64_t now = systime_epoch_ms();
        int64_t diff = (int64_t)s->tasks[i].next_release - (int64_t)now; // >= is bugged for large values, so use subtraction instead
        if (!s->tasks[i].active || diff > 0) continue;
        switch (s->tasks[i].type) {
            case ST_TYPE_FUNC: {
                schedfunc_f schedfunc = s->tasks[i].func;
                if (schedfunc != NULL) schedfunc();
                break;
            }
            case ST_TYPE_CMD: {
                mcppkt_s cmdcpy; // Make a local copy so each repetition gets a fresh cmd
                memcpy(&cmdcpy, s->tasks[i].cmd_ptr, sizeof(mcppkt_s));
                mcpmgr_process_pkt(mcpmgr, &cmdcpy);
                break;
            }
        }
        if (s->tasks[i].remaining_reps > 0 && s->tasks[i].remaining_reps != SCHEDULE_REPS_INFINITE) {
            s->tasks[i].remaining_reps--;
        }
        if (s->tasks[i].remaining_reps > 0) {
            uint64_t newnext = now + s->tasks[i].period_ms;
            s->tasks[i].next_release = newnext; 
            if (s->tasks[i].type == ST_TYPE_CMD) flashmgr_schedules_flush(&g_flashmgr, s);
        } else if (s->tasks[i].type != ST_TYPE_NONE) { // Check if schedule has not already been cleared
            scheduler_clear_task(s, s->tasks[i].id);
        }
    }
}

status_e scheduler_deschedule(scheduler_s* s, uint8_t id) {
    schedtask_s* task_ptr = s->id_map[id];
    if (task_ptr == NULL || task_ptr->id != id) return FAILURE;
    task_ptr->active = FALSE;
    if (task_ptr->type == ST_TYPE_CMD) flashmgr_schedules_flush(&g_flashmgr, s);
    return SUCCESS;
}

status_e scheduler_reschedule_at(scheduler_s* s, uint8_t id, rtc_time_t rtc) {
    schedtask_s* task_ptr = s->id_map[id];
    if (task_ptr == NULL || task_ptr->id != id) return FAILURE;
    task_ptr->active = TRUE;
    task_ptr->next_release = rtc_to_epoch_ms(rtc);
    if (task_ptr->type == ST_TYPE_CMD) flashmgr_schedules_flush(&g_flashmgr, s);
    return SUCCESS;
}

status_e scheduler_reschedule_in(scheduler_s* s, uint8_t id, uint32_t start_delay_ms) {
    schedtask_s* task_ptr = s->id_map[id];
    if (task_ptr == NULL || task_ptr->id != id) return FAILURE;
    task_ptr->active = TRUE;
    task_ptr->next_release = systime_epoch_ms() + start_delay_ms;
    if (task_ptr->type == ST_TYPE_CMD) flashmgr_schedules_flush(&g_flashmgr, s);
    return SUCCESS;
}

status_e scheduler_update_task(scheduler_s* s, uint8_t id, uint32_t period_ms, uint16_t remaining_reps) {
    schedtask_s* task_ptr = s->id_map[id];
    if (task_ptr == NULL || task_ptr->id != id) return FAILURE;
    task_ptr->period_ms = period_ms;
    task_ptr->remaining_reps = remaining_reps;
    if (task_ptr->type == ST_TYPE_CMD) flashmgr_schedules_flush(&g_flashmgr, s);
}

status_e scheduler_clear_task(scheduler_s* s, uint8_t id) {
    schedtask_s* task_ptr = s->id_map[id];
    if (task_ptr == NULL || task_ptr->id != id) return FAILURE;
    int1 is_cmd = task_ptr->type == ST_TYPE_CMD;
    if (is_cmd) memset(task_ptr->cmd_ptr, 0, MCP_MAX_LEN);
    memset(task_ptr, 0, sizeof(schedtask_s));
    s->id_map[id] = NULL;
    if (is_cmd) flashmgr_schedules_flush(&g_flashmgr, s);
    return SUCCESS;
}

void scheduler_refresh_all(scheduler_s* s, uint64_t oldtime) {
    uint8_t i;
    uint64_t now = systime_epoch_ms();
    for (i = 0; i < SCHEDULER_MAX_TASKS; i++) {
        if (!s->tasks[i].active) continue;
        int64_t diff = (int64_t)s->tasks[i].next_release - (int64_t)oldtime; // >= is bugged for large values, so use subtraction instead
        uint64_t newnext = now + diff;
        s->tasks[i].next_release = newnext; 
    }
}

status_e scheduler_schedule_func_at(scheduler_s* s, uint8_t id, schedfunc_f func, rtc_time_t start_time, uint32_t period_ms, uint16_t reps) {
    schedtask_s task = {0};
    schedtask_create(&task, id, ST_TYPE_FUNC, rtc_to_epoch_ms(start_time), period_ms, reps);
    task.func = func;
    uint8_t i_task;
    return schedule_task(s, task, 0, SCHEDULER_MAX_FUNC_TASKS, &i_task); // Func tasks segment of tasks buffer
}

status_e scheduler_schedule_cmd_at(scheduler_s* s, uint8_t id, mcppkt_s* p, rtc_time_t start_time, uint32_t period_ms, uint16_t reps) {
    schedtask_s task = {0};
    schedtask_create(&task, id, ST_TYPE_CMD, rtc_to_epoch_ms(start_time), period_ms, reps);
    uint8_t i_task;
    if (schedule_task(s, task, SCHEDULER_MAX_FUNC_TASKS, SCHEDULER_MAX_CMD_TASKS, &i_task) == FAILURE) // Cmd tasks segment of tasks buffer
        return FAILURE; 
    mcppkt_s* cmd_ptr = &s->cmds[i_task - SCHEDULER_MAX_FUNC_TASKS]; 
    memcpy(cmd_ptr, p, sizeof(mcppkt_s));
    s->tasks[i_task].cmd_ptr = cmd_ptr;
    flashmgr_schedules_flush(&g_flashmgr, s);
    return SUCCESS;
}

status_e scheduler_schedule_func_in(scheduler_s* s, uint8_t id, schedfunc_f func, uint32_t start_delay_ms, uint32_t period_ms, uint16_t reps) {
    schedtask_s task = {0};
    schedtask_create(&task, id, ST_TYPE_FUNC, systime_epoch_ms() + start_delay_ms, period_ms, reps);
    task.func = func;
    uint8_t i_task;
    return schedule_task(s, task, 0, SCHEDULER_MAX_FUNC_TASKS, &i_task);
}

status_e scheduler_schedule_cmd_in(scheduler_s* s, uint8_t id, mcppkt_s* p, uint32_t start_delay_ms, uint32_t period_ms, uint16_t reps) {
    schedtask_s task = {0};
    schedtask_create(&task, id, ST_TYPE_CMD, systime_epoch_ms() + start_delay_ms, period_ms, reps);
    uint8_t i_task; 
    if (schedule_task(s, task, SCHEDULER_MAX_FUNC_TASKS, SCHEDULER_MAX_CMD_TASKS, &i_task) == FAILURE)
        return FAILURE;
    mcppkt_s* cmd_ptr = &s->cmds[i_task - SCHEDULER_MAX_FUNC_TASKS]; 
    memcpy(cmd_ptr, p, sizeof(mcppkt_s));
    s->tasks[i_task].cmd_ptr = cmd_ptr;
    flashmgr_schedules_flush(&g_flashmgr, s);
    return SUCCESS;
}
