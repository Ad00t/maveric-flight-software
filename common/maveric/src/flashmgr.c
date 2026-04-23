#include "flashmgr.h"
#include "flash.h"
#include <stdint.h>

#module

status_e flashmgr_init(flashmgr_s* self) {
	memset(self, 0, sizeof(flashmgr_s));
    status_e s1 = flashmgr_increment_rbt_cnt(self);
    flashmgr_config_load_defaults(self);
    status_e s2 = flashmgr_config_load_flash(self);
    return (s1 == SUCCESS && s2 == SUCCESS) ? SUCCESS : FAILURE;
}

uint32_t flashmgr_find_last_record(flashmgr_s* self, uint32_t start_addr, uint16_t record_size, uint8_t* out) {
    uint32_t i;
    for (i = 0; i <= FLASH_BLOCK_SIZE - record_size; i += record_size) {
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

    if (new_addr > start_addr + FLASH_BLOCK_SIZE - record_size) {
        uint16_t b = FlashAddrToBlock(start_addr);
        flashEraseBlockByNumber(b);
        new_addr = start_addr;
    }

    return flashWriteSafe(new_addr, record_size, new_record, start_addr,
                          start_addr + FLASH_BLOCK_SIZE - 1);
}

status_e flashmgr_increment_rbt_cnt(flashmgr_s* self) {
    uint8_t record_size = sizeof(self->rbt_cnt);
    uint32_t curr_record_addr = flashmgr_find_last_record(self, RBT_CNT_ADDR, record_size, (uint8_t*)&self->rbt_cnt);

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
    cfg->ops_stage = OPS_INIT;
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
    uint32_t curr_record_addr = flashmgr_find_last_record(self, CONFIG_ADDR, record_size, (uint8_t*)&self->config);
    return check_crc16((uint8_t*)&self->config, crc_off, self->config.crc);
}

status_e flashmgr_config_flush(flashmgr_s* self) {
	uint16_t record_size = sizeof(config_s);
    uint8_t crc_off = offsetof(config_s, crc);
	self->config.crc = compute_crc16((uint8_t*)&self->config, crc_off);
    uint32_t curr_record_addr = flashmgr_find_last_record(self, CONFIG_ADDR, record_size, NULL);
    return flashmgr_append_record(self, CONFIG_ADDR, curr_record_addr, (uint8_t*)&self->config, record_size);
}
