#include "../includes/sboot_types.h"
#include "spd.h"

void spd_init_not_used(spd_t *self) {
  spd_init_for_dimm(self);
}

void spd_init_for_dimm(spd_t *self) {
  self->load_from_rom = TRUE;
  self->use_bytes = TRUE;
  self->bytes_loaded = FALSE;
  self->num_dimms = 0;
  // self->slave_addr_list = slave_addr_list
}

void spd_init_for_dimm_itc(spd_t *self) {
  // we will have to configure the memory ourself                                                                                                                             
  self->load_from_rom = TRUE;
  self->use_bytes = FALSE;
  self->bytes_loaded = FALSE;
  self->num_dimms = 0;
  // self->slave_addr_list = slave_addr_list

}
