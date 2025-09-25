
// Pin Mappings for SPI interface
//#pin_select SDI1 = PIN_D10
//#pin_select SDO1 = PIN_D0
//#pin_select SCK1OUT = PIN_D11

// Pin Mappings for SPI interface for iControl PPM
// These apply for flash, gyros, and magnetomter
#pin_select SDI1 = PIN_D9
#pin_select SDO1 = PIN_D8
#pin_select SCK1OUT = PIN_D10

// Make sure these match the pin_selects above
#define SDI1_PIN 		PIN_D9
#define SDO1_PIN 		PIN_D8
#define SCK1OUT_PIN 	PIN_D10


//============================================
//  		    TX/RX and Handshaking Pins
//=============================================
// Port 0
#define	U1TX_PIN    PIN_F3
#define	U1RX_PIN    PIN_F5     // was PIN_F2.  Changed to compensate for crossed wire in PPM design. 10-25-10.
#define U1CTS_PIN   PIN_A6	   // was D15	
#define U1RTS_PIN   PIN_A6     // was D14

// Port 1
#define	U2TX_PIN    PIN_F2     // was PIN_F5.  Changed to compensate for crossed wire in PPM design. 10-25-10.
#define	U2RX_PIN    PIN_F4
#define U2CTS_PIN   PIN_A6
#define U2RTS_PIN   PIN_A6

// Port 2
#define	U3TX_PIN    PIN_D2     // Bezouska: Changed per Tim's Rev 5 Sheet
#define	U3RX_PIN    PIN_D3
#define U3CTS_PIN   PIN_A6
#define U3RTS_PIN   PIN_A6

// Port 3
#define	U4TX_PIN    PIN_G7
#define	U4RX_PIN    PIN_G9     // Bezouska: Changed per Tim's Rev 5 Pin Sheet
#define U4CTS_PIN   PIN_A6
#define U4RTS_PIN   PIN_A6

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


//========================================
//		Master Clear Pin
//========================================
#define RESET_PPM					PIN_G8


//========================================
//				Magnetometer Pins
//========================================
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

// Bezouska: The magnetometer uses SPI-1, so we only need to define 3 additional pins
#define MAG_SSNOT					PIN_B14 
#define MAG_DRDY					PIN_B12 
#define MAG_RESET					PIN_B13

// Bezouska: Pin selects for Magnetometer on SPI 2 using defines from above.
//#pin_select SDI2 = MAG_MISO
//#pin_select SDO2 = MAG_MOSI
//#pin_select SCK2OUT = MAG_SCLK
				
//========================================
//				Gyro Pins Pins
//========================================
// Names used during Fred's Bitbanging
// Should match pin_selects above.
#define SDI1 						SDI1_PIN	
#define SDO1 						SDO1_PIN
#define SCK1OUT 					SCK1OUT_PIN

// Flight Settings
#define GYROCS1 					PIN_B11   	// X Side, next to programming cable (short side)
#define GYROCS2 					PIN_B15  	// Y Side, not by programming cable (long side)
#define GYROCS3 					PIN_B10  	// Z Top

// Gyro Power Pin
#define GYRO_ON						PIN_A1

//========================================
//				IMI-100 Pins
//========================================

// Bezouska: This came from the Pumpkin code, but not needed.
//#define IMI_PWR 					PIN_E8 //H2.18
#define IMI_HEARTBEAT				PIN_E9

// Used for external interrupt (this sets up the register RPINR0 to have the correct bits for externally triggered interrupts)
#pin_select INT1=IMI_HEARTBEAT

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
#define FLASH_CHIP_SELECT			PIN_B7	
#define FLASH_WRITE_PROTECT			PIN_D6	
#define SECOND_FLASH_CS				PIN_B9


// This function is meant to clean up the rest of the PIC
// by driving all remaining pins to LOW.  This is done
// in accordance with Section 2.8 of the PIC Datasheet:
//
// "Unused I/O pins should be configured as outputs and
// driven to a logic low state. Alternatively, connect a 1 kO
// to 10 kO resistor to VSS on unused pins and drive the
// output to logic low."

/*
void DriveUnusedPins()
{

// This comes from the "Secondary Pin Mapping.xls" excel file on SVN under the /Analysis/ folder
return;

}
*/