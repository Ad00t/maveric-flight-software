#include "rm3100.h"

#module					// !!! Important: This command makes everything below scoped only to this file.

unsigned int8 rm3100_read_8(unsigned int8 add, unsigned int8 reg)
{
	return i2c_read_8(add, reg & ~0x80);
}

unsigned int16 rm3100_read_16(unsigned int8 add, unsigned int8 reg)
{
	return i2c_read_16(add, reg & ~0x80);	
}

void rm3100_read_word(unsigned int8 add, unsigned int8 reg, unsigned int8 *output, unsigned int8 num_bytes)
{
	i2c_read_word(add, reg & ~0x80, *output, num_bytes);	
}

void rm3100_write_8(unsigned int8 add, unsigned int8 reg, unsigned int8 write_data)
{
	i2c_write_8(add, reg, write_data);	
}

void rm3100_write_16(unsigned int8 add, unsigned int8 reg, unsigned int16 write_data)
{
	i2c_write_16(add, reg, write_data);	
}

int8 rm3100_init(unsigned int8 add)
{
	//int8(i2c address) -> bool
	//Check if the rm3100 device is connected in the indicated address (add)
	//example: rm3100_init(RM3100_ADDRESS_20) returns a no-zerobyte if there is a RM3100 connected in that direction, otherwise return zero byte
			
	return rm3100_read_8(add, RM3100_REGISTER_REVID);

}

void rm3100_set_max_rate(unsigned int8 add, 
						 unsigned int8 rate = RM3100_037HZ)
{
	rm3100_write_8(add, RM3100_REGISTER_WTMRC, rate);
}

void rm3100_set_cycle(unsigned int8 add, 
                      unsigned int16 count_x = RM3100_COUNT,
                      unsigned int16 count_y = RM3100_COUNT, 
                      unsigned int16 count_z = RM3100_COUNT)
{
	rm3100_write_16(add, RM3100_REGISTER_CMX, count_x);
	rm3100_write_16(add, RM3100_REGISTER_CMY, count_y);
	rm3100_write_16(add, RM3100_REGISTER_CMZ, count_z);
}

void rm3100_set_mode(unsigned int8 add, 
                     unsigned int8 mode = RM3100_MODE_CMM)
{
	rm3100_write_8(add, RM3100_REGISTER_CMM, mode);
}

void rm3100_read_data(unsigned int8 add, int32* mag_x, int32* mag_y, int32* mag_z)
{	
	byte read_data[9];
	unsigned int8 num_bytes = 9;
	
 	i2c_read_word(add, RM3100_REGISTER_MX2, &read_data[0], num_bytes);
	if (read_data[0] > 0x80)
	{
		*mag_x = 0xFF << 24;
	}
	else
	{
		*mag_x = 0x00 << 24;
	}
	*mag_x += (int32)read_data[0] << 16;
	*mag_x += (int32)read_data[1] << 8;
	*mag_x += (int32)read_data[2];

	if (read_data[3] > 0x80)
	{
		*mag_y = 0xFF << 24;
	}
	else
	{
		*mag_y = 0x00 << 24;
	}
	*mag_y += (int32)read_data[4] << 16;
	*mag_y += (int32)read_data[5] << 8;
	*mag_y += (int32)read_data[6];

	if (read_data[7] > 0x80)
	{
		*mag_z = 0xFF << 24;
	}
	else
	{
		*mag_z = 0x00 << 24;
	}
	*mag_z += (int32)read_data[7] << 16;
	*mag_z += (int32)read_data[8] << 8;
	*mag_z += (int32)read_data[9];
}