#include "pldmgr.h"
#include "common.h"

#module

status_e pldmgr_init(pldmgr_s* self) {
    self->is_init = TRUE;

    self->state_map[NODE_HOLONAV] = &self->hn_state;
    self->lcd_fn_map[NODE_HOLONAV] = self->hn_lcd_fn;
    self->cam_fn_map[NODE_HOLONAV] = self->hn_cam_fn;
    self->quantity_map[NODE_HOLONAV] = &self->hn_quantity;
    self->focus_map[NODE_HOLONAV] = &self->hn_focus;
    self->exposure_us_map[NODE_HOLONAV] = &self->hn_exposure_us;

    self->state_map[NODE_ASTROBOARD] = &self->ab_state;
    self->lcd_fn_map[NODE_ASTROBOARD] = self->ab_lcd_fn;
    self->cam_fn_map[NODE_ASTROBOARD] = self->ab_cam_fn;
    self->quantity_map[NODE_ASTROBOARD] = &self->ab_quantity;
    self->focus_map[NODE_ASTROBOARD] = &self->ab_focus;
    self->exposure_us_map[NODE_ASTROBOARD] = &self->ab_exposure_us;

    return pldmgr_clear(self);
}

status_e pldmgr_clear_node(pldmgr_s* self, uint8_t node_id) {
    if (!self->is_init) return FAILURE;
    *(self->state_map[node_id]) = PLDS_OFF; 
    memset(self->lcd_fn_map[node_id], 0, sizeof(self->lcd_fn_map[node_id]));
    memset(self->cam_fn_map[node_id], 0, sizeof(self->cam_fn_map[node_id]));
    *(self->quantity_map[node_id]) = 1;
    *(self->focus_map[node_id]) = 6;
    *(self->exposure_us_map[node_id]) = 60000;
    return SUCCESS;
}

status_e pldmgr_clear(pldmgr_s* self) {
    if (!self->is_init) return FAILURE;
    status_e s1 = pldmgr_clear_node(self, NODE_HOLONAV);
    status_e s2 = pldmgr_clear_node(self, NODE_ASTROBOARD);
    return (s1 == SUCCESS && s2 == SUCCESS) ? SUCCESS : FAILURE;
}

status_e pldmgr_fsm_init(pldmgr_s* self, uint8_t node_id, char* lcd_fn, char* cam_fn, uint8_t quantity, float focus, uint32_t exposure_us) {
    if (!self->is_init) return FAILURE;
    if (node_id >= PLDMGR_MAPS_SIZE) return FAILURE;
    uint8_t lcd_fn_len = strlen(lcd_fn);
    uint8_t cam_fn_len = strlen(cam_fn);
    if (lcd_fn_len > PLDMGR_FN_SIZE || cam_fn_len > PLDMGR_FN_SIZE) return FAILURE;

    *(self->state_map[node_id]) = PLDS_OFF;
    memcpy(self->lcd_fn_map[node_id], lcd_fn, lcd_fn_len);
    memcpy(self->cam_fn_map[node_id], cam_fn, cam_fn_len);
    *(self->quantity_map[node_id]) = quantity;
    *(self->focus_map[node_id]) = focus;
    *(self->exposure_us_map[node_id]) = exposure_us;
    
    return SUCCESS;
}

