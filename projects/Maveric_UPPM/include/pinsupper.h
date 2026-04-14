
//========================================
//  	Burn Driver Pins (PEC BOARD)
//========================================
#define BurnA 						PIN_F1
#define BurnB						PIN_F0

//========================================
//  	Temperature Pins (PEC BOARD)
//========================================
#define TEMP_ENABLE					PIN_B3

//========================================
//  		    MHX Pins
//========================================
#define AX100_PWR                     PIN_B9
#define AX100_OUTPUT_ENABLE           PIN_E2
#define AX100_RESET                   PIN_D1
#define AX100_DATA_TERMINAL_READY     PIN_D2
#define AX100_DATA_SEND_READY         PIN_C3
#define AX100_REQUEST_TO_SEND         PIN_D5
#define AX100_CLEAR_TO_SEND           PIN_C4
#define AX100_CARRIER_DETECT          PIN_C2

//========================================
//  		    Enable USB Serial
//========================================
#define USB_OE						PIN_C1

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


//========================================
//				Flash Pins
//========================================
#define FLASH_CHIP_SELECT			PIN_D13
#define FLASH_WRITE_PROTECT			PIN_D6

// Pin Mappings for SPI interface
#pin_select SDI1 = PIN_D10
#pin_select SDO1 = PIN_D0
#pin_select SCK1OUT = PIN_D11

//========================================
//		Master Clear Pin
//========================================
#define RESET_PPM				PIN_B4

//============================================
//  		        UART
//=============================================

#define COM_A		1 // Holonav 
#define COM_A_BAUD  115200 
#pin_select U1TX = PIN_F3 
#pin_select U1RX = PIN_F2 

#define COM_B       2 // AX100 
#define COM_B_BAUD  500000 
#pin_select U2TX = PIN_F5 
#pin_select U2RX = PIN_F4 
#define U2CTS_PIN	AX100_CLEAR_TO_SEND
#define U2RTS_PIN	AX100_REQUEST_TO_SEND

#define COM_C       3 // LPPM 
#define COM_C_BAUD  115200
#pin_select U3TX = PIN_D8 
#pin_select U3RX = PIN_D3 

#define COM_D       4 // Astroboard 
#define COM_D_BAUD  115200 
#pin_select U4TX = PIN_D4 
#pin_select U4RX = PIN_D12 

#define HOLONAV_PORT        COM_A
#define AX100_PORT          COM_B
#define LPPM_PORT           COM_C
#define ASTROBOARD_PORT     COM_D
