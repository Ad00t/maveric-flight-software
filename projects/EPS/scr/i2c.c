

unsigned int8 i2c1_read_8(unsigned int8 add, unsigned int8 reg)
{
	//I2C 1  byte reading routine
	//int8(i2c address), int8(i2c register)  -> int8
	//reads the bytes present in the i2c register of the addressed device and saves it on read_data 
	//example: i2c_write_8(ADDRESS, REG)
	
	unsigned int8 addw, addr;
	unsigned int8 read_data;

	addw = add<<1;
	addr = addw | 0x01;
	
	i2c_start(I2C_1);
	i2c_write(I2C_1, addw);
	i2c_write(I2C_1, reg);
	//i2c_stop(I2C_1);
	i2c_start(I2C_1);
	i2c_write(I2C_1, addr);

	read_data = i2c_read(I2C_1, FALSE);
	//read_data = (read_data<<8);
	//read_data = read_data | i2c_read(I2C_1);
	i2c_stop(I2C_1);

	return read_data;		
}

void i2c1_write_8(unsigned int8 add, unsigned int8 reg, unsigned int8 write_data)
{
	//I2C writing byte routine
	//int8(i2c address), int8(i2c register), char(1 byte write register pointer)  -> none
	//Writes the bytes present in write_data into the i2c register of the addressed device 
	//example: i2c_write_16(ADDRESS, REG, setting)

	unsigned int8 addw, addr, lsb;

	addw = add<<1;
	addr = addw | 0x01;

	lsb = write_data;
	//msb = write_data>>8;
	
	i2c_start(I2C_1);
	i2c_write(I2C_1, addw);
	i2c_write(I2C_1, reg);
	//i2c_write(I2C_1, msb);
	i2c_write(I2C_1, lsb);
	i2c_stop(I2C_1);	
}


unsigned int16 i2c1_read_16(unsigned int8 add, unsigned int8 reg)
{
	//I2C INA226 reading routine
	//int8(i2c address), int8(i2c register), -> int16
	//reads the bytes present in the i2c register of the addressed device and saves it on read_data 
	//example: i2c_write_16(ADDRESS, REG, setting)
	
	unsigned int8 addw, addr;
	unsigned int16 read_data;

	addw = add<<1;
	addr = addw | 0x01;
		
	i2c_start(I2C_1);
	i2c_write(I2C_1, addw);
	i2c_write(I2C_1, reg);
	i2c_start(I2C_1);
	i2c_write(I2C_1, addr);

	read_data = i2c_read(I2C_1);
	read_data = (read_data<<8);
	read_data = read_data | i2c_read(I2C_1, FALSE);
	i2c_stop(I2C_1);

	return read_data;		
}

void i2c1_write_16(unsigned int8 add, unsigned int8 reg, unsigned int16 write_data)
{
	//I2C INA226 writing routine
	//int8(i2c address), int8(i2c register), char(2 bytes write register pointer)  -> bool
	//Writes the bytes present in write_data into the i2c register of the addressed device 
	//example: i2c_write_16(ADDRESS, REG, setting)
	// unsigned char ina226_wdata[2]; // 2-byte ina226 writing

	unsigned int8 addw, addr, msb, lsb;

	addw = add<<1;
	addr = addw | 0x01;

	lsb = write_data;
	msb = write_data>>8;
	
	i2c_start(I2C_1);
	i2c_write(I2C_1, addw);
	i2c_write(I2C_1, reg);
	i2c_write(I2C_1, msb);
	i2c_write(I2C_1, lsb);
	i2c_stop(I2C_1);	
}

void i2c1_read_word(unsigned int8 add, unsigned int8 reg, unsigned int8 *output, unsigned int8 num_bytes)
{
	//I2C rm3100 reading routine
	//int8(i2c address), int8(i2c register), -> int16
	//reads the bytes present in the i2c register of the addressed device and saves it on read_data 
	//example: i2c_write_16(ADDRESS, REG, setting)
	
	unsigned int8 addw, addr, i;
	unsigned int16 read_data;

	addw = add<<1;
	addr = addw | 0x01;
		
	i2c_start(I2C_1);
	i2c_write(I2C_1, addw);
	i2c_write(I2C_1, reg);
	i2c_start(I2C_1);
	i2c_write(I2C_1, addr);
	
	for (i=0; i<num_bytes; ++i)
	{
	*(read_data+i) = i2c_read(I2C_1);
	}
	*(read_data+i) = read_data | i2c_read(I2C_1, FALSE);
	i2c_stop(I2C_1);		
}