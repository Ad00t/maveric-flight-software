#ifndef __PLDMGR_H__
#define __PLDMGR_H__

#define PLDMGR_MAPS_SIZE        NODE_ASTROBOARD+1
#define PLDMGR_FN_SIZE          64

typedef enum {
    PLDS_OFF = 0,
    PLDS_STARTED = 1,
    PLDS_DISPLAYED = 2,
    PLDS_CAPTURED = 3
} pld_state_e;

typedef struct {
    uint8_t hn_state;
    char hn_lcd_fn[PLDMGR_FN_SIZE];
    char hn_cam_fn[PLDMGR_FN_SIZE];
    uint8_t hn_quantity;
    float hn_focus;
    uint32_t hn_exposure_us;

    uint8_t ab_state;
    char ab_lcd_fn[PLDMGR_FN_SIZE];
    char ab_cam_fn[PLDMGR_FN_SIZE];
    uint8_t ab_quantity;
    float ab_focus;
    uint32_t ab_exposure_us;

    pld_state_e* state_map[PLDMGR_MAPS_SIZE]; 
    char* lcd_fn_map[PLDMGR_MAPS_SIZE]; 
    char* cam_fn_map[PLDMGR_MAPS_SIZE]; 
    uint8_t* quantity_map[PLDMGR_MAPS_SIZE];
    float* focus_map[PLDMGR_MAPS_SIZE];
    uint32_t* exposure_us_map[PLDMGR_MAPS_SIZE];
    int1 is_init;
} pldmgr_s;

status_e pldmgr_init(pldmgr_s* self);
status_e pldmgr_clear(pldmgr_s* self);

status_e pldmgr_fsm_init(uint8_t node_id, char* lcd_fn, char* cam_fn, uint8_t quantity, float focus, uint32_t exposure_us);

#endif

