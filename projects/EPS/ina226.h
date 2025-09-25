#ifndef __INA226__
#define __INA226__

// INA226 Addresses
#define INA226_ADDRESS_1  0x40 //Address A1=GND A0=GND
#define INA226_ADDRESS_2  0x41 //Address A1=GND A0=VIN
#define INA226_ADDRESS_3  0x42 //Address A1=GND A0=SDA
#define INA226_ADDRESS_4  0x43 //Address A1=GND A0=SCL
#define INA226_ADDRESS_5  0x44 //Address A1=VIN A0=GND
#define INA226_ADDRESS_6  0x45 //Address A1=VIN A0=VIN
#define INA226_ADDRESS_7  0x46 //Address A1=VIN A0=SDA
#define INA226_ADDRESS_8  0x47 //Address A1=VIN A0=SCL
#define INA226_ADDRESS_9  0x48 //Address A1=SDA A0=GND
#define INA226_ADDRESS_10 0x49 //Address A1=SDA A0=VIN
#define INA226_ADDRESS_11 0x4A //Address A1=SDA A0=SDA
#define INA226_ADDRESS_12 0x4B //Address A1=SDA A0=SCL
#define INA226_ADDRESS_13 0x4C //Address A1=SCL A0=GND
#define INA226_ADDRESS_14 0x4D //Address A1=SCL A0=VIN
#define INA226_ADDRESS_15 0x4E //Address A1=SCL A0=SDA
#define INA226_ADDRESS_16 0x4F //Address A1=SCL A0=SCL

// INA226 Registers
#define INA226_REG_CONFIG      0x00
#define INA226_REG_SHUNTV      0x01
#define INA226_REG_VBUS        0x02
#define INA226_REG_PWR         0x03
#define INA226_REG_CURRENT     0x04
#define INA226_REG_CALIBRATION 0x05
#define INA226_REG_MASK        0x06
#define INA226_REG_ALERT       0x07
#define INA226_REG_MANID       0xFE
#define INA226_REG_ID          0xFF

//Comfiguration mode bites
#define INA226_MODE_1 0x00
#define INA226_MODE_2 0x01
#define INA226_MODE_3 0x02
#define INA226_MODE_4 0x03
#define INA226_MODE_5 0x04
#define INA226_MODE_6 0x05
#define INA226_MODE_7 0x06
#define INA226_MODE_8 0x07

extern int1 ina226_init(unsigned int8 add);
extern void ina226_configure(unsigned int8 add, unsigned int16 average=INA226_MODE_1, unsigned int16 vbusct = INA226_MODE_5, unsigned int16 vshct = INA226_MODE_5, unsigned int16 setmode = INA226_MODE_8);
extern void ina226_calibrate(unsigned int8 add, unsigned int16 calibration = 0x1400);
extern void ina226_read_data(unsigned int8 add, int16 *shunt_voltage, int16 *bus_voltage, int16 *power, int16 *current);

#endif