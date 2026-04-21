#include "pldmgr.h"
#include "common.h"

#module

status_e pldmgr_init(pldmgr_s* self) {
    self->is_init = TRUE;

    self->state_map[NODE_HOLONAV] = &self->hn_state;
    self->lcd_display_args_map[NODE_HOLONAV] = self->hn_lcd_display_args;
    self->cam_capture_args_map[NODE_HOLONAV] = self->hn_cam_capture_args;

    self->state_map[NODE_ASTROBOARD] = &self->ab_state;
    self->lcd_display_args_map[NODE_ASTROBOARD] = self->ab_lcd_display_args;
    self->cam_capture_args_map[NODE_ASTROBOARD] = self->ab_cam_capture_args;

    return pldmgr_clear(self);
}

status_e pldmgr_clear_node(pldmgr_s* self, uint8_t node_id) {
    if (!self->is_init) return FAILURE;
    *(self->state_map[node_id]) = PLDS_OFF; 
    memset(self->lcd_display_args_map[node_id], 0, sizeof(self->lcd_display_args_map[node_id]));
    memset(self->cam_capture_args_map[node_id], 0, sizeof(self->cam_capture_args_map[node_id]));
    return SUCCESS;
}

status_e pldmgr_clear(pldmgr_s* self) {
    if (!self->is_init) return FAILURE;
    status_e s1 = pldmgr_clear_node(self, NODE_HOLONAV);
    status_e s2 = pldmgr_clear_node(self, NODE_ASTROBOARD);
    return (s1 == SUCCESS && s2 == SUCCESS) ? SUCCESS : FAILURE;
}

status_e pldmgr_fsm_init(pldmgr_s* self, uint8_t node_id, char* lcd_display_args, char* cam_capture_args) {
    if (!self->is_init) return FAILURE;
    if (node_id >= PLDMGR_MAPS_SIZE) return FAILURE;
    pldmgr_clear_node(self, node_id);
    *(self->state_map[node_id]) = PLDS_OFF;
    memcpy(self->lcd_display_args_map[node_id], lcd_display_args, strlen(lcd_display_args));
    memcpy(self->cam_capture_args_map[node_id], cam_capture_args, strlen(cam_capture_args));
    return SUCCESS;
}

