#include "../includes/sboot_types.h"
#include "config.h"

void config_init(config_t *self) {
  self->bootstrap_uart_protocol_mode = TRUE;
  self->rclk_rate = 100;
  self->i2c_rom_slave_address = 0x54;    // MDB i2c ROM 101_0100
  self->i2c_temp_sense1_address = 0x18;  // MDB i2c 001_1000   part U41
  self->i2c_temp_sense2_address = 0x19;  // MDB i2c 001_1001   part U43
  self->i2c_switch_address = 0x70; // MDB i2c 111_0000 part U64

  self->i2c_reset_ext_address = 0x1A; // MDB U66 0x0111_010X                                                                                                                         
  // PCA9557 shows for A[2:0] = 3'b010 Address = 0x1A (26d)                                                                                      
  // GBE PHY 88e1121R (U12) reset is from U66:P2                                                                                                 
  self->board_info_in_i2c_rom = FALSE;
  self->pcie_driver_hard_reset = FALSE;
  //self->valid_boot_paths = [self->BOOT_RSHIM]
  //self->set_boot_path(self->BOOT_RSHIM)
  self->bootstrap_uart_protocol_mode = TRUE;
}
