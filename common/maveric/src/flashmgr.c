#include "flashmgr.h"
#include "flash.h"
#include "scheduler.h"
#include "mcpmgr.h"
#include <stdint.h>

#module

status_e flashmgr_init(flashmgr_s* self, scheduler_s* scheduler) {
	memset(self, 0, sizeof(flashmgr_s));
    status_e s1 = flashmgr_increment_rbt_cnt(self);
    flashmgr_config_load_defaults(self);
    status_e s2 = flashmgr_config_load_flash(self);
    if (s2 == FAILURE) flashmgr_config_load_defaults(self);
    return (s1 == SUCCESS && s2 == SUCCESS) ? SUCCESS : FAILURE;
}

uint32_t flashmgr_find_last_record(flashmgr_s* self, uint32_t start_addr, uint8_t* out, uint16_t record_size) {
    uint32_t i;
    for (i = 0; i <= FLASH_SECTOR_SIZE - record_size; i += record_size) {
        if (CheckFlashEmpty(start_addr + i, record_size)) {
            break;
        }
    }

    if (i == 0) {
        // Nothing written yet
        return start_addr;
    }

    uint32_t record_addr = start_addr + i - record_size;

    if (out != NULL) {
        flashRead(record_addr, record_size, out);
    }

    delay_ms(50);
    return record_addr;
}

status_e flashmgr_append_record(flashmgr_s* self, uint32_t start_addr, uint32_t record_addr, uint8_t* new_record, uint16_t record_size) {
    flashSectorProtectDisableAddr(start_addr); // Should be a sector start addr? (multiple of 65536)

    uint32_t new_addr;

    // If nothing has been written yet, write at the start.
    if (record_addr == start_addr && CheckFlashEmpty(start_addr, record_size)) {
        new_addr = start_addr;
    } else {
        new_addr = record_addr + record_size;
    }

    if (new_addr > start_addr + FLASH_SECTOR_SIZE - record_size) {
        uint16_t b = FlashAddrToBlock(start_addr);
        uint8_t i;
        for (i = 0; i < FLASH_SECTOR_SIZE / FLASH_BLOCK_SIZE; i++) {
            flashEraseBlockByNumber(b+i);
        }
        new_addr = start_addr;
    }

    status_e s = flashWriteSafe(new_addr, record_size, new_record, start_addr,
                          start_addr + FLASH_SECTOR_SIZE - 1);
    delay_ms(50);
    return s;
}

status_e flashmgr_increment_rbt_cnt(flashmgr_s* self) {
    uint8_t record_size = sizeof(self->rbt_cnt);
    uint32_t curr_record_addr = flashmgr_find_last_record(self, RBT_CNT_ADDR, (uint8_t*)&self->rbt_cnt, record_size);

	// Increment the counter
	self->rbt_cnt++; // Wraps to 0 automatically

	// Now increment the value in flash
    return flashmgr_append_record(self, RBT_CNT_ADDR, curr_record_addr, (uint8_t*)&self->rbt_cnt, record_size);
}

void flashmgr_config_load_defaults(flashmgr_s* self) {
    config_s* cfg = &self->config;
#if NODE == NODE_LPPM
    cfg->log_level = LL_INFO;
    cfg->gyro_rate_src = 0; 
    cfg->mag_src = 0;
    float dfl_paxs[3] = { 0, 0, -1 };
    memcpy(cfg->paxs, dfl_paxs, sizeof(dfl_paxs));
    char dfl_tle[140] = "1 99999U 26001A   26182.53800926  .00000000  00000-0  15000-3 0  99992 99999  97.8250 154.7171 0058009 348.1000 351.9980 14.91466332000019";
    memcpy(cfg->tle, dfl_tle, sizeof(dfl_tle));
#elif NODE == NODE_UPPM
    cfg->log_level = LL_INFO;
    cfg->ops_stage = OPS_SAFE;
    cfg->gs_delay = 1000;
    cfg->bcn_period = 3*MS_PER_MIN;
#endif
    uint8_t crc_off = offsetof(config_s, crc);
    cfg->crc = compute_crc16((uint8_t*)cfg, crc_off);
}

status_e flashmgr_config_load_flash(flashmgr_s* self) {
    // return SUCCESS;
	uint16_t record_size = sizeof(config_s);
    uint8_t crc_off = offsetof(config_s, crc);
    uint8_t* data_ptr = (uint8_t*)&self->config;
    uint32_t curr_record_addr = flashmgr_find_last_record(self, CONFIG_ADDR, data_ptr, record_size);
    return check_crc16(data_ptr, crc_off, self->config.crc);
}

status_e flashmgr_config_flush(flashmgr_s* self) {
	uint16_t record_size = sizeof(config_s);
    uint8_t crc_off = offsetof(config_s, crc);
    uint8_t* data_ptr = (uint8_t*)&self->config;
	self->config.crc = compute_crc16((uint8_t*)&self->config, crc_off);
    uint32_t curr_record_addr = flashmgr_find_last_record(self, CONFIG_ADDR, NULL, record_size);
    status_e s = flashmgr_append_record(self, CONFIG_ADDR, curr_record_addr, data_ptr, record_size);
    return s;
}

status_e flashmgr_schedules_load_flash(flashmgr_s* self, scheduler_s* scheduler) {
#if NODE == NODE_UPPM
    if (self->flash_scheds_loaded) return FAILURE;
    uint16_t record_size = SCHEDULER_MAX_CMD_TASKS * (sizeof(schedtask_s) + sizeof(mcppkt_s)) + 2;
    uint8_t data_ptr[SCHEDULER_MAX_CMD_TASKS*256] = {0};
    uint32_t curr_record_addr = flashmgr_find_last_record(self, SCHEDULES_ADDR, data_ptr, record_size);
    if (CheckFlashEmpty(curr_record_addr, record_size)) {
        return SUCCESS;
    }
    uint16_t crc = make16(data_ptr[record_size-1], data_ptr[record_size-2]);
    status_e s = check_crc16(data_ptr, record_size-2, crc);
    if (s == SUCCESS) {
        uint8_t i;
        for (i = 0; i < SCHEDULER_MAX_CMD_TASKS; i++) {
            uint16_t i_sched_start = i*(sizeof(schedtask_s)+sizeof(mcppkt_s)); 
            schedtask_s* st_ptr = &scheduler->tasks[SCHEDULER_MAX_FUNC_TASKS + i]; 
            mcppkt_s* cmd_ptr = &scheduler->cmds[i];
            if (st_ptr->type != ST_TYPE_NONE) continue;
            memcpy(st_ptr, &data_ptr[i_sched_start], sizeof(schedtask_s));
            memcpy(cmd_ptr, &data_ptr[i_sched_start+sizeof(schedtask_s)], sizeof(mcppkt_s));
            st_ptr->cmd_ptr = cmd_ptr; // Update pointer to cmd in schedtask
            if (st_ptr->type == ST_TYPE_NONE) continue;
            scheduler->id_map[st_ptr->id] = st_ptr; // Update pointer to schedtask in id map
        }
        sprintf(LOGBUF, "flashmgr_schedules_load_flash: success len=%u crc=%u", record_size, crc); log_info();
    } else {
        sprintf(LOGBUF, "flashmgr_schedules_load_flash: crc check failed: len=%u crc=%u compcrc=%u",
                record_size, crc, compute_crc16(data_ptr, record_size-2)); log_error();
    }
    self->flash_scheds_loaded = TRUE;
    return s;
#endif
    return SUCCESS;
}

status_e flashmgr_schedules_flush(flashmgr_s* self, scheduler_s* scheduler) {
#if NODE == NODE_UPPM
    uint16_t record_size = SCHEDULER_MAX_CMD_TASKS * (sizeof(schedtask_s) + sizeof(mcppkt_s)) + 2;
    uint32_t curr_record_addr = flashmgr_find_last_record(self, SCHEDULES_ADDR, NULL, record_size);
    uint8_t data_ptr[SCHEDULER_MAX_CMD_TASKS*256] = {0};
    uint8_t i;
    for (i = 0; i < SCHEDULER_MAX_CMD_TASKS; i++) {
        uint16_t i_sched_start = i*(sizeof(schedtask_s)+sizeof(mcppkt_s)); 
        memcpy(&data_ptr[i_sched_start], &scheduler->tasks[SCHEDULER_MAX_FUNC_TASKS + i], sizeof(schedtask_s));
        memcpy(&data_ptr[i_sched_start+sizeof(schedtask_s)], &scheduler->cmds[i], sizeof(mcppkt_s));
    }
    uint16_t crc = compute_crc16(data_ptr, record_size-2);
    data_ptr[record_size-2] = make8(crc, 0);
    data_ptr[record_size-1] = make8(crc, 1);
    status_e s = flashmgr_append_record(self, SCHEDULES_ADDR, curr_record_addr, data_ptr, record_size);
    if (s == SUCCESS) {
        sprintf(LOGBUF, "flashmgr_schedules_flush: append success len=%u crc=%u", record_size, crc); log_info();
    } else {
        sprintf(LOGBUF, "flashmgr_schedules_flush: append failure len=%u crc=%u", record_size, crc); log_error();
    }
    return s;
#endif
    return SUCCESS;
}
