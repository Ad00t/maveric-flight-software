#ifndef __FLASHMGR_H__
#define __FLASHMGR_H__

#include "common.h"
#include "flash.h"

//========================================
//    		 	Address Locations
//========================================
#define RESERVED_ADDR                   0x000000
#define RBT_CNT_ADDR                    0x010000
#define CONFIG_ADDR                     0x030000
#define SWAP_ADDR                       0x7FF000
#define ADDRESS_OUT_OF_BOUNDS           0x800000

//========================================
//    		 	Size Allocations
//========================================
#define RESERVED_ALLOC_SIZE             0x010000
#define RBT_CNT_ALLOC_SIZE              0x020000
#define CONFIG_ALLOC_SIZE               0x7CF000
#define SWAP_ALLOC_SIZE                 0x001000

typedef enum {
    DATASRC_MTQ = 0,
    DATASRC_NVG = 1,
} datasrc_e;

typedef enum {
    OPS_INIT = 0,
    OPS_SAFE = 1,         
    OPS_NOMINAL = 2,
} ops_stage_e;

typedef struct {
#if NODE == NODE_LPPM
    log_level_e log_level;
    datasrc_e gyro_rate_src;
    datasrc_e mag_src;
    float paxs[3];
    char tle[140];
#elif NODE == NODE_UPPM
    log_level_e log_level;
    ops_stage_e ops_stage;
#endif
    uint16_t crc;
} config_s;

typedef struct {
    config_s config;
    uint16_t rbt_cnt;
} flashmgr_s;

// Initialize flashmgr and config
status_e flashmgr_init(flashmgr_s* self);

// Reboot counter manipulation
status_e flashmgr_increment_rbt_cnt(flashmgr_s* self);
status_e flashmgr_reset_rbt_cnt(flashmgr_s* self);

// Config manipulation
void flashmgr_config_load_defaults(flashmgr_s* self);
status_e flashmgr_config_load_flash(flashmgr_s* self);
status_e flashmgr_config_flush(flashmgr_s* self);

#endif
