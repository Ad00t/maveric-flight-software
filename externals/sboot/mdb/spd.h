#ifndef __MAESTRO_SPD_H
#define __MAESTRO_SPD_H

#include "../includes/sboot_types.h"

struct spd_struct {
  bool load_from_rom;
  bool use_bytes;
  bool bytes_loaded;
  uint8 num_dimms;
  // slave_addr_list
};
typedef struct spd_struct spd_t;

void spd_init_not_used(spd_t *self);
void spd_init_for_dimm(spd_t *self);
void spd_init_for_dimm_itc(spd_t *self);


#endif // __MAESTRO_SPD_H
