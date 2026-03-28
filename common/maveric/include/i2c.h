#ifndef __I2C__
#define __I2C__

#include <stdint.h>

// I2C Master Read a Byte
uint8_t i2c_read_8(uint8_t port, uint8_t add, uint8_t reg);
// I2C Master Write a Byte
void i2c_write_8(uint8_t port, uint8_t add, uint8_t reg, uint8_t write_data);
// I2C Master Read 2 Bytes
uint16_t i2c_read_16(uint8_t port, uint8_t add, uint8_t reg);
// I2C Master Write 2 Bytes
void i2c_write_16(uint8_t port, uint8_t add, uint8_t reg, uint16_t write_data);

void i2c_start_transaction(uint8_t port);

uint8_t i2c_byte_avail(uint8_t port);

uint8_t i2c_read_byte(uint8_t port);

void i2c_write_byte(uint8_t port, uint8_t b);

void i2c_stop_transaction(uint8_t port);

// i2c_read_byte into a given buffer
void i2c_read_buf(uint8_t port, uint8_t add, uint8_t* buf, uint16_t len);

// i2c_write_byte sequentially on a given buffer
void i2c_write_buf(uint8_t port, uint8_t add, uint8_t* buf, uint16_t len);

#endif

