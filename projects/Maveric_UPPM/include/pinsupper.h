
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
#define TRANSCEIVER_PWR                     PIN_B1
#define TRANSCEIVER_OUTPUT_ENABLE           PIN_E2
#define TRANSCEIVER_RESET                   PIN_D1
#define TRANSCEIVER_DATA_TERMINAL_READY     PIN_D2
#define TRANSCEIVER_DATA_SEND_READY         PIN_C3
#define TRANSCEIVER_REQUEST_TO_SEND         PIN_D5
#define TRANSCEIVER_CLEAR_TO_SEND           PIN_C4
#define TRANSCEIVER_CARRIER_DETECT          PIN_C2

//========================================
//  		    Enable USB Serial
//========================================
#define USB_OE						PIN_C1

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

#define COM_A		1 // Stream Port 1
#define COM_A_BAUD  115200 
#pin_select U1TX = PIN_F3 
#pin_select U1RX = PIN_F2 

#define COM_B       2 // Transceiver 
#define COM_B_BAUD  115200  
#pin_select U2TX = PIN_F5 
#pin_select U2RX = PIN_F4 
#define U2CTS_PIN	TRANSCEIVER_CLEAR_TO_SEND
#define U2RTS_PIN	TRANSCEIVER_REQUEST_TO_SEND

#define COM_C       3 // Stream Port 3
#define COM_C_BAUD  115200
#pin_select U3TX = PIN_D8 
#pin_select U3RX = PIN_D3 

#define COM_D       4 // FTDI 
#define FTDI_PORT   COM_D
#define COM_D_BAUD  115200 
#pin_select U4TX = PIN_D4 
#pin_select U4RX = PIN_D12 
