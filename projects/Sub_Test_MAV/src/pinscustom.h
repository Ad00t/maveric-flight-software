
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
#define MHX_PWR		   				PIN_B0	// was PIN_E3
#define MHX_OUTPUT_ENABLE			PIN_E2
#define MHX_RESET					PIN_D1			
#define MHX_DATA_TERMINAL_READY		PIN_D2			
#define MHX_DATA_SEND_READY			PIN_C3			
#define MHX_REQUEST_TO_SEND			PIN_D5			
#define MHX_CLEAR_TO_SEND			PIN_C4			
#define MHX_CARRIER_DETECT			PIN_C2
#define MHX_SEND_DATA				PIN_F5
#define MHX_RCV_DATA				PIN_F4

//========================================
//  		    MAESTRO Pins
//========================================
// ***CMR* The MAESTRO does not use these TX/RX pins on the development board.
// ***CMR* It uses the TX/RX pins defined below.
#define MAESTRO_PWR				PIN_A14
#define MAESTRO_RESET                           PIN_B5
#define MAESTRO_SEND_DATA                       PIN_F13
#define MAESTRO_RCV_DATA                        PIN_D15

//=====================================
// 				I2C
//=====================================
#define I2C_ON						PIN_E5
#define Device_SDA 					PIN_G3
#define Device_SCL 					PIN_G2

//========================================
//  		    Enable USB Serial
//========================================
#define USB_OE						PIN_C1

//========================================
//				Flash Pins
//========================================
#define FLASH_CHIP_SELECT			PIN_D13			
#define FLASH_WRITE_PROTECT			PIN_D6	


//========================================
//				Beacon Pins
//========================================
// Stensat Pins (TBD)
#define BEACON_TX		PIN_F3
#define BEACON_RX		PIN_F2
//#define BEACON_RESET	PIN_B6		// Stensat has no reset pin
//#define BEACON_SHUTDOWN	PIN_B7	// Stensat has no shutdown pin

// Neon Pins
/*
#define BEACON_TX		PIN_F3
#define BEACON_RX		PIN_F2
#define BEACON_RESET	PIN_B6	// Active High.  Needs to be low during use. Pulse high to reset.
#define BEACON_SHUTDOWN	PIN_B7	// Active Low. Needs to be high for beacon to work. Low to turn off.
*/

// Pin Mappings for SPI interface
#pin_select SDI1 = PIN_D10
#pin_select SDO1 = PIN_D0
#pin_select SCK1OUT = PIN_D11

//========================================
//		Master Clear Pin
//========================================
#define RESET_PPM				PIN_B4



//============================================
//  		 LEDS!
//=============================================
#define LED1_GROUND		PIN_G0		// H1.41, H2.21
#define LED1_POWER		PIN_A14		// H1.39, H2.20
#define LED3_PIN		PIN_A15		// H1.37, H2.19


//============================================
//  		    TX/RX and Handshaking Pins
//=============================================
// Port 0
#define	U1TX_PIN	PIN_G7
#define	U1RX_PIN	PIN_G9
#define U1CTS_PIN	PIN_D15
#define U1RTS_PIN	PIN_D14

// Port 1
#define	U2TX_PIN	PIN_F5
#define	U2RX_PIN	PIN_F4
#define U2CTS_PIN	PIN_A6
#define U2RTS_PIN	PIN_A6

// Port 2
#define	U3TX_PIN	PIN_D8
#define	U3RX_PIN	PIN_D3
#define U3CTS_PIN	PIN_A6
#define U3RTS_PIN	PIN_A6

// Port 3
// ***CMR* The MAESTRO usees these TX/RX pins on the development board.
#define	U4TX_PIN	PIN_F3
#define	U4RX_PIN	PIN_F2
#define U4CTS_PIN	PIN_A6
#define U4RTS_PIN	PIN_A6


// Additional Layer of indirection for ports.c
// TODO: Remove layer and refer directly to U1CTS_PIN
#define COM_A_CTS		U1CTS_PIN
#define COM_B_CTS		U2CTS_PIN
#define COM_C_CTS		U3CTS_PIN
#define COM_D_CTS		U4CTS_PIN

#define COM_A_RTS		U1RTS_PIN
#define COM_B_RTS		U2RTS_PIN
#define COM_C_RTS		U3RTS_PIN
#define COM_D_RTS		U4RTS_PIN


