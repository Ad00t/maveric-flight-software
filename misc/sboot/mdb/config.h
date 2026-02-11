#ifndef __CONFIG_H
#define __CONFIG_H

#include "../includes/sboot_types.h"

// Use these constants instead of boot_master_tile():
#define BOOT_MASTER_X 1
#define BOOT_MASTER_Y 4

struct config_struct {
  bool bootstrap_uart_protocol_mode;
  uint16 rclk_rate;
  uint16 i2c_rom_slave_address;
  uint16 i2c_temp_sense1_address;
  uint16 i2c_temp_sense2_address;
  uint16 i2c_switch_address;
  uint16 i2c_reset_ext_address;
  bool board_info_in_i2c_rom;
  bool pcie_driver_hard_reset;
};
typedef struct config_struct config_t;

void config_init(config_t *self);

#endif // __CONFIG_H
