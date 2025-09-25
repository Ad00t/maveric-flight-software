#ifndef __RM3100__
#define __RM3100__
// RM3100 Address Register
#define RM3100_ADDRESS_20  0x20 //ADD
#define RM3100_ADDRESS_21  0x21 //ADD2
#define RM3100_ADDRESS_22  0x22 //ADD3
#define RM3100_ADDRESS_23  0x23 //ADD4

// RM3100 Count Cycles Write Registers
#define RM3100_REGISTER_CMM  0x01 //Initiates Continuous Measurement Mode 
#define RM3100_REGISTER_CMX  0x04 //Cycle Count Register - X-axis MSB
#define RM3100_REGISTER_CLX  0x05 //Cycle Count Register - X-axis LSB
#define RM3100_REGISTER_CMY  0x06 //Cycle Count Register - Y-axis MSB
#define RM3100_REGISTER_CLY  0x07 //Cycle Count Register - Y-axis LSB
#define RM3100_REGISTER_CMZ  0x08 //Cycle Count Register - Z-axis MSB
#define RM3100_REGISTER_CLZ  0x09 //Cycle Count Register - Z-axis LSB
#define RM3100_REGISTER_WTMRC 0x0B //Sets Continuous Measurment Mode Data Rate

// RM3100 Count Cycles Read Registers
#define RM3100_REGISTER_RCMX 0x84 //Cycle Count Register - X-axis MSB
#define RM3100_REGISTER_RCLX 0x85 //Cycle Count Register - X-axis LSB
#define RM3100_REGISTER_RCMY 0x86 //Cycle Count Register - Y-axis MSB
#define RM3100_REGISTER_RCLY 0x87 //Cycle Count Register - Y-axis LSB
#define RM3100_REGISTER_RCMZ 0x88 //Cycle Count Register - Z-axis MSB
#define RM3100_REGISTER_RCLZ 0x89 //Cycle Count Register - Z-axis LSB
#define RM3100_REGISTER_RTMRC 0x8B //Sets Continuous Measurment Mode Data Rate

// RM3100 STATUS Register
#define RM3100_REGISTER_STATUS 0xB4 //Status of DRDY
#define RM3100_REGISTER_REVID  0xB6 //RM3100 Revision Identification

// RM3100 Magnetic Field Measurements
#define RM3100_REGISTER_MX2 0xA4
#define RM3100_REGISTER_MX1 0xA5
#define RM3100_REGISTER_MX0 0xA6
#define RM3100_REGISTER_MY2 0xA7
#define RM3100_REGISTER_MY1 0xA8
#define RM3100_REGISTER_MY0 0xA9
#define RM3100_REGISTER_MZ2 0xAA
#define RM3100_REGISTER_MZ1 0xAB
#define RM3100_REGISTER_MZ0 0xAC

//Possible max rate settings
#define RM3100_600HZ 0x92
#define RM3100_300HZ 0x93
#define RM3100_150HZ 0x94
#define RM3100_075HZ 0x95
#define RM3100_037HZ 0x96
#define RM3100_018HZ 0x97
#define RM3100_009HZ 0x98
#define RM3100_4_5HZ 0x99
#define RM3100_2_3HZ 0x9A
#define RM3100_1_2HZ 0x9B
#define RM3100_0_6HZ 0x9C
#define RM3100_0_3HZ 0x9D
#define RM3100_0_015HZ 0x92
#define RM3100_0_075HZ 0x92
#define RM3100_COUNT 0xC8
#define RM3100_MODE_CMM 0x79

extern unsigned int8 rm3100_read_8(unsigned int8 add, unsigned int8 reg);
extern unsigned int16 rm3100_read_16(unsigned int8 add, unsigned int8 reg);
extern void rm3100_read_word(unsigned int8 add, unsigned int8 reg, unsigned int8 *output, unsigned int8 num_bytes);
extern void rm3100_write_8(unsigned int8 add, unsigned int8 reg, unsigned int8 write_data);
extern void rm3100_write_16(unsigned int8 add, unsigned int8 reg, unsigned int16 write_data);
extern int8 rm3100_init(unsigned int8 add);
extern int1 rm3100_check_measurement(unsigned int8 add);
extern void rm3100_set_max_rate(unsigned int8 add, unsigned int8 rate = RM3100_037HZ);
extern void rm3100_set_cycle(unsigned int8 add, unsigned int16 count_x = RM3100_COUNT, unsigned int16 count_y = RM3100_COUNT, unsigned int16 count_z = RM3100_COUNT);
extern void rm3100_set_mode(unsigned int8 add, unsigned int8 mode = RM3100_MODE_CMM);
extern void rm3100_read_data(unsigned int8 add, int32* mag_x, int32* mag_y, int32* mag_z);

//extern void ina226_calibrate(unsigned int8 add, unsigned int16 calibration = 0x1400);
//extern void ina226_read_data(unsigned int8 add, int16 *shunt_voltage, int16 *bus_voltage, int16 *power, int16 *current);

#endif