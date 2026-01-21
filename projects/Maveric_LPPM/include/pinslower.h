#ifndef __PINSLOWER_H__
#define __PINSLOWER_H__

//=====================================
// 			    SPI	
//=====================================

#define SPI_1       1   // Stream SPI 1
#pin_select SDO1 = PIN_D8 
#pin_select SDI1 = PIN_D9
#pin_select SCK1OUT = PIN_D10 

// #pin_select SDO1 = PIN_F8 
// #pin_select SDI1 = PIN_F7 
// #pin_select SCK1OUT = PIN_D9 

// #define SPI_2       2 
// #pin_select SDO2 = PIN_G8 
// #pin_select SDI2 = PIN_G7 
// #pin_select SCK2OUT = PIN_G6 

//=====================================
// 				I2C
//=====================================

// NOTE -- these pins are default and not remappable. pinouts included for completeness.

#define I2C_1       1 // Stream I2C 1
#pin_select SCL1            PIN_G2
#pin_select SDA1            PIN_G3

#define I2C_2       2 // Stream I2C 2 
#pin_select SCL2            PIN_A2
#pin_select SDA2            PIN_A3

#define I2C_3       3 // Stream I2C 3
#pin_select SCL3            PIN_E6
#pin_select SDA3            PIN_E7

//============================================
//  		        UART
//=============================================

#define COM_A		1 // Stream Port 1
#define COM_A_BAUD  115200 
#pin_select U1TX = PIN_F3 
#pin_select U1RX = PIN_F5 

#define COM_B       2 // Stream Port 2
#define COM_B_BAUD  115200  
#pin_select U2TX = PIN_F2 
#pin_select U2RX = PIN_F4 

#define COM_C       3 // Stream Port 3
#define COM_C_BAUD  115200
#pin_select U3TX = PIN_D2 
#pin_select U3RX = PIN_D3 

#define COM_D       4 // Stream Port 4
#define COM_D_BAUD  115200 
#pin_select U4TX = PIN_D4 
#pin_select U4RX = PIN_D12 

//========================================
//		Master Clear Pin
//========================================

#define RESET_PPM					PIN_G8

//========================================
//				Magnetometer Pins
//========================================

// #define MAG_I2CEN                   PIN_E5

// Bezouska: The magnetometer uses SPI-1, so we only need to define 3 additional pins
// #define MAG_SSNOT					PIN_B14 
// #define MAG_DRDY					PIN_B12 
// #define MAG_RESET					PIN_B13

// Bezouska: Note, this is for Pumpkin Development Board
//#define MAG_RESET					PIN_D14 //H1.11		
//#define MAG_DRDY					PIN_D15 //H1.12
//#define MAG_SSNOT					PIN_G9  //H1.16

// Bezouska: Note, this is for Pumpkin PPM (which is not connected to USER pins, so requires jumpering)
//#define MAG_RESET					PIN_D4
//#define MAG_DRDY					PIN_D14 //(RPI) 
//#define MAG_SSNOT					PIN_F13 
//#define MAG_MISO					PIN_G8
//#define MAG_MOSI					PIN_G6
//#define MAG_SCLK					PIN_D15

// Bezouska: Pin selects for Magnetometer on SPI 2 using defines from above.
//#pin_select SDI2 = MAG_MISO
//#pin_select SDO2 = MAG_MOSI
//#pin_select SCK2OUT = MAG_SCLK
				
//========================================
//				Gyro Pins
//========================================

// Flight Settings
#define GYRO_CS1 					PIN_B11   	// X Side, next to programming cable (short side)
#define GYRO_CS2 					PIN_B15  	// Y Side, not by programming cable (long side)
#define GYRO_CS3 					PIN_B10  	// Z Top

// Gyro Power Pin
#define GYRO_ON						PIN_A1

//========================================
//  		    Enable USB Serial
//========================================

#define USB_OE						PIN_C1

//========================================
//				Flash Pins
//========================================

#define FLASH_CHIP_SELECT			PIN_B7	
#define FLASH_WRITE_PROTECT			PIN_D6	
#define SECOND_FLASH_CS				PIN_B9

#endif
