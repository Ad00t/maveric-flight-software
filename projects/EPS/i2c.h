#ifndef __I2C__
#define __I2C__


extern unsigned int8 i2c1_read_8(unsigned int8 add, unsigned int8 reg);
extern void i2c1_write_8(unsigned int8 add, unsigned int8 reg, unsigned int8 write_data);
extern unsigned int16 i2c1_read_16(unsigned int8 add, unsigned int8 reg);
extern void i2c1_write_16(unsigned int8 add, unsigned int8 reg, unsigned int16 write_data);
extern void i2c1_read_word(unsigned int8 add, unsigned int8 reg, unsigned int8 *output, unsigned int8 num_bytes);

//extern unsigned int8 i2c2_read_8(unsigned int8 add, unsigned int8 reg);
//extern void i2c2_write_8(unsigned int8 add, unsigned int8 reg, unsigned int8 write_data);
//extern unsigned int16 i2c2_read_16(unsigned int8 add, unsigned int8 reg);
//extern void i2c2_write_16(unsigned int8 add, unsigned int8 reg, unsigned int16 write_data);

//extern unsigned int8 i2c3_read_8(unsigned int8 add, unsigned int8 reg);
//extern void i2c3_write_8(unsigned int8 add, unsigned int8 reg, unsigned int8 write_data);
//extern unsigned int16 i2c3_read_16(unsigned int8 add, unsigned int8 reg);
//extern void i2c3_write_16(unsigned int8 add, unsigned int8 reg, unsigned int16 write_data);

#endif