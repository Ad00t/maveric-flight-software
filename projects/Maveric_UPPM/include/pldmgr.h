#ifndef __PLDMGR_H__
#define __PLDMGR_H__

#define PLDMGR_MAPS_SIZE        NODE_ASTROBOARD+1

typedef enum {
    PLDS_OFF = 0,
    PLDS_STARTED = 1,
    PLDS_DISPLAYED = 2,
    PLDS_CAPTURED = 3
} pld_state_e;

typedef struct {
    uint8_t hn_state;
    char hn_lcd_display_args[MCP_MAX_ARGS_LEN];
    char hn_cam_capture_args[MCP_MAX_ARGS_LEN];

    uint8_t ab_state;
    char ab_lcd_display_args[MCP_MAX_ARGS_LEN];
    char ab_cam_capture_args[MCP_MAX_ARGS_LEN];

    pld_state_e* state_map[PLDMGR_MAPS_SIZE]; 
    char* lcd_display_args_map[PLDMGR_MAPS_SIZE]; 
    char* cam_capture_args_map[PLDMGR_MAPS_SIZE]; 
    int1 is_init;
} pldmgr_s;

status_e pldmgr_init(pldmgr_s* self);
status_e pldmgr_clear_node(pldmgr_s* self, uint8_t node_id);
status_e pldmgr_clear(pldmgr_s* self);
status_e pldmgr_fsm_init(pldmgr_s* self, uint8_t node_id, char* lcd_display_args, char* cam_capture_args);

#endif

