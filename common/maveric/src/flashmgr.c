#include "flashmgr.h"
#include "flash.h"
#include <stdint.h>

#module

status_e flashmgr_init(flashmgr_s* self) {
	memset(self, 0, sizeof(flashmgr_s));
    flashmgr_config_load_defaults(self);
    return flashmgr_config_load_flash(self);
}

status_e flashmgr_increment_rbt_cnt(flashmgr_s* self) {
	uint32_t i;
	uint32_t addy;
    uint8_t record_size = sizeof(self->rbt_cnt);

	// This loops over the block, looking for a set of 2 empty bytes.
	for (i = 0; i < FLASH_BLOCK_SIZE - 1; i += record_size) {
		//		sprintf(dbgbuf,"\r\n%i",i); sendDBGALL(USER_PORT,dbgbuf);
		if (CheckFlashEmpty(RBT_CNT_ADDR + i, record_size))
			break;
	}
	// i now contains the location (offset from RBT_CNT_ADDR) of the first empty set of bytes in
	// flash, or is >= FLASH_BLOCK_SIZE if it could not find an empty spot.
	addy = RBT_CNT_ADDR + i - record_size;

	// Read the previous value
	if (i >= record_size)
		flashRead(addy, record_size, &self->rbt_cnt); // For values of i greater than first iteration
	else
		self->rbt_cnt = 0; // For first iteration.

	// Validate no data corruption
	// if (self->rbt_cnt < 0)
	// 	self->rbt_cnt = 0;

	// Increment the counter
	self->rbt_cnt++; // Wraps to 0 automatically

	// Now increment the value in flash

	// First disable protection
	flashSectorProtectDisable(RBT_CNT_ADDR);

	// Next check if we are at the end of the block, because we'll need to erase the block and start
	// over in that case
	if (i >= FLASH_BLOCK_SIZE - 1) {
		flashEraseBlockByAddr(RBT_CNT_ADDR);
		i = 0;
	}

	// Finally, write the new value
	return flashWriteSafe(RBT_CNT_ADDR + i, record_size, &self->rbt_cnt, RBT_CNT_ADDR,
						  RBT_CNT_ADDR + FLASH_BLOCK_SIZE - 1);
}

status_e flashmgr_reset_rbt_cnt(flashmgr_s* self) {
	self->rbt_cnt = 0;
	flashSectorProtectDisable(RBT_CNT_ADDR);
	flashEraseBlockByAddr(RBT_CNT_ADDR);
	return flashWriteSafe(RBT_CNT_ADDR, sizeof(self->rbt_cnt), &self->rbt_cnt, RBT_CNT_ADDR,
				          RBT_CNT_ADDR + FLASH_BLOCK_SIZE - 1);
}

void flashmgr_config_load_defaults(flashmgr_s* self) {
#if NODE == NODE_LPPM
    self->config.log_level = LL_INFO;
    self->config.gyro_rate_src = 0; 
    self->config.mag_src = 0;
    float dfl_paxs[3] = { 0, 0, -1 };
    memcpy(self->config.paxs, dfl_paxs, sizeof(dfl_paxs));
    char dfl_tle[140] = "1 99999U 26001A   26182.53800926  .00000000  00000-0  15000-3 0  99992 99999  97.8250 154.7171 0058009 348.1000 351.9980 14.91466332000019";
    memcpy(self->config.tle, dfl_tle, sizeof(dfl_tle));
#elif NODE == NODE_UPPM
    self->config.log_level = LL_INFO;
#endif
}

status_e flashmgr_config_load_flash(flashmgr_s* self) {
    return SUCCESS;
	uint16_t n = sizeof(config_s);
	// Setup
	waitForFlash();
	// Read and check CRC, exiting successfully if the CRC matches
    uint16_t i;
	for (i = 0; i < 10; i++) {
		flashRead(CONFIG_ADDR, n, (uint8_t*)self->config);
		if (check_crc16((uint8_t*)self->config, n-2, self->config.crc) == SUCCESS)
			return SUCCESS;
	}
	return FAILURE;
}

status_e flashmgr_config_flush(flashmgr_s* self) {
	uint16_t n = sizeof(config_s);
	self->config.crc = compute_crc16((uint8_t*)self->config, n-2);

    uint16_t m = 0;
	for (m = 0; m < n; m += FLASH_BLOCK_SIZE){
		flashEraseBlockByAddr((uint32_t)(CONFIG_ADDR + m)); // Erase the block at Config + m
		// -------------NOTE THAT A REBOOT HERE WOULD BE REALLY BAD ----------//
	}

	// Now program the new stuff, handling any errors if they happen
	return flashWriteSafe(CONFIG_ADDR, n, self->config, CONFIG_ADDR, CONFIG_ADDR + n + 1);
}
