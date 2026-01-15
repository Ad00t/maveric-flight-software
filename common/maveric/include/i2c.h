#ifndef __I2C__
#define __I2C__

#include <stdint.h>

uint8_t i2c_read_8(uint8_t add, uint8_t reg);
void i2c_write_8(uint8_t add, uint8_t reg, uint8_t write_data);
uint16_t i2c_read_16(uint8_t add, uint8_t reg);
void i2c_write_16(uint8_t add, uint8_t reg, uint16_t write_data);
void i2c_read_word(uint8_t add, uint8_t reg, uint8_t* out, uint8_t len);

#endif
