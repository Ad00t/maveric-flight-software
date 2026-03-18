#module					// !!! Important: This command makes everything below scoped only to this file.

int1 ina226_init(unsigned int8 add)
{
	//int8(i2c address) -> bool
	//Check if the ina226 device is connected in the indicated address (add)
	//example: ina226_init(INA226_ADDRESS_12) returns TRUE if there is a INA226 connected in that direction, otherwise return FALSE
	unsigned int16 rdata;
			
	rdata = i2c1_read_16(add, INA226_REG_MANID);

	if (rdata ==0x5449)
	{
		rdata = i2c1_read_16(add, INA226_REG_ID);
		if (rdata==0x2260)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	} 
	else
	{
		return 0;
	}	
}

void ina226_configure(unsigned int8 add, 
					  unsigned int16 average=INA226_MODE_1,
					  unsigned int16 vbusct = INA226_MODE_5,  
					  unsigned int16 vshct = INA226_MODE_5, 
					  unsigned int16 setmode = INA226_MODE_8)
{
	unsigned int16 ina226_config;
	// Default values for the configuration register
	ina226_config = 0x4127;

	if ((average <= INA226_MODE_8) && (average >= INA226_MODE_1))
	{
		ina226_config = average<<9 | ina226_config;
	}

	if ((vbusct <= INA226_MODE_8) && (vbusct >= INA226_MODE_1))
	{
		ina226_config = vbusct<<6 | ina226_config;
	}

	if ((vshct <= INA226_MODE_8) && (vshct >= INA226_MODE_1))
	{
		ina226_config = vshct<<3 | ina226_config;
	}

	if ((setmode <= INA226_MODE_8) && (setmode >= INA226_MODE_1))
	{
		ina226_config = setmode<<3 | ina226_config;
	}
	i2c1_write_16(add, INA226_REG_CONFIG, ina226_config);
}

void ina226_calibrate(unsigned int8 add, unsigned int16 calibration = 0x1400)
{
 // For R_shunt = 1 mohm and 1 mA/bit resolution calibration = 0x1400
	i2c1_write_16(add, INA226_REG_CALIBRATION, calibration);
}

void ina226_read_data(unsigned int8 add, int16* shunt_voltage, int16* bus_voltage, int16* power, int16* current)
{
	*shunt_voltage = 2.5 * i2c1_read_16(add, INA226_REG_SHUNTV);
	*bus_voltage = 1.25 * i2c1_read_16(add, INA226_REG_VBUS);
	*power = 25 * i2c1_read_16(add, INA226_REG_PWR);
	*current = 1* i2c1_read_16(add, INA226_REG_CURRENT);
}