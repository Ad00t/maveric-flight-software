
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
//  		    TX/RX and Handshaking Pins
//=============================================
// Port 0
#define	U1TX_PIN	PIN_F3
#define	U1RX_PIN	PIN_F2
#define U1CTS_PIN	PIN_A6	   // was D15	
#define U1RTS_PIN   PIN_A6     // was D14

// Port 1
#define	U2TX_PIN	PIN_F5
#define	U2RX_PIN	PIN_F4
#define U2CTS_PIN	TRANSCEIVER_CLEAR_TO_SEND
#define U2RTS_PIN	TRANSCEIVER_REQUEST_TO_SEND

// Port 2
#define	U3TX_PIN	PIN_D8
#define	U3RX_PIN	PIN_D3
#define U3CTS_PIN	PIN_A6
#define U3RTS_PIN	PIN_A6

// Port 3
// ***CMR* These are the definitions for the iTAG.
// ***CMR* The MAESTRO uses different TX and RX pins for UART4, see above.
#define	U4TX_PIN	PIN_D4
#define	U4RX_PIN	PIN_D12		// PIN_D12 IS INPUT ONLY AND CAN'T BE TX!!! <---- AHHH!!!
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
