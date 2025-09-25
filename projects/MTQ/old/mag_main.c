//=============================================================================================================
// main.c
// Testing routine template for components connected into the PPMs
//=============================================================================================================
#include <24FJ256GA110.h>			// Device Header File.  Switched to brackets to use version in PICC Library
#include "pinsupper.h"              // Add pins Upper PPM Definition

#device PASS_STRINGS=IN_RAM			// Should allow for things like sendMSGALL(port,"Message") <--- const weren't allowed there before
#device ADC=16						// ??
#build(stack=0x300)					// Use a larger stack size.
#fuses NOPROTECT					// Code not protected from reading
#fuses NOWDT						// No automatic WDT -- it must be enabled by software.
//#fuses XT							// Primary Clock Select
//#fuses FRC_PLL				    // Internal Fast RC Oscillator with Phase Lock Loop gives 32 MHz
#fuses HS
#fuses PR_PLL
#fuses WPOSTS13						// Watchdog Postscaler.
									// At current processor settings: WPOSTS12 = ~12 seconds
									//								  WPOSTS13 = ~20 seconds
									//								  default  = ~120 seconds
#fuses IESO							// Internal-External Switchover
//#fuses IOL1WAY					// Locks the I/O Lock after setting it once.
#fuses NOIOL1WAY					// Allows multiple changes to PIC register settings.
#fuses WRT
#fuses CKSFSM						// Clock fail-safe monitor
#pragma case						// Makes all code case-sensitive

#use delay(clock=32MHZ,internal=8M)  // Tells compiler what the clock speed is
//#use delay(clock=8MHZ,internal=8M)  // Tells compiler what the clock speed is

//========================================
//  		PIC Registers
//========================================
#word RCON = 0x0740
#word OSCCON = getenv("SFR:OSCCON")
#bit IOLOCK = OSCCON.6
#word RPINR20 = getenv("SFR:RPINR20")
#word RPOR1 = getenv("SFR:RPOR1")
/* // DEBUG DISPLAY
#warning OSCCON is located at getenv("SFR:OSCCON")
#warning IOLOCK is located at getenv("BIT:IOLOCK")
#warning RPINR20 is located at getenv("SFR:RPINR20")
#warning RPOR1 is located at getenv("SFR:RPOR1")
*/

#include <string.h>

// Port 0
#pin_select U1TX = U1TX_PIN
#pin_select U1RX = U1RX_PIN

// Port 1
#pin_select U2TX = U2TX_PIN
#pin_select U2RX = U2RX_PIN

// Port 2
#pin_select U3TX = U3TX_PIN
#pin_select U3RX = U3RX_PIN

// Port 3
#pin_select U4TX = U4TX_PIN
#pin_select U4RX = U4RX_PIN

#define COM_A_BAUD  38400  // Beacon
#define COM_B_BAUD  19200  // Transceiver
#define COM_C_BAUD  115200 // Other PPM
#define COM_D_BAUD  115200 // Payload

#define COM_A		1 // Stream Port 1
#define COM_B       2 // Stream Port 2
#define COM_C       3 // Stream Port 3
#define COM_D       4 // Stream Port 4

#define I2C_1       1 // Stream I2C

//==================================================================
//  		Serial Port Initialization
//===================================================================
#use rs232(baud = COM_A_BAUD, UART1, bits = 8, STREAM = COM_A, ERRORS)
#use rs232(baud = COM_B_BAUD, UART2, bits = 8, STREAM = COM_B, ERRORS)
#use rs232(baud = COM_C_BAUD, UART3, bits = 8, STREAM = COM_C, ERRORS) // To/From other PIC
#use rs232(baud = COM_D_BAUD, UART4, bits = 8, STREAM = COM_D, ERRORS)
//==================================================================
//  		I2C Port Initialization
//===================================================================
#use i2c(master, sda=PIN_G3, scl=PIN_G2, STREAM=I2C_1)
//#use i2c(master, sda=PIN_A3, scl=PIN_A2, STREAM=I2C_1)
//#use i2c(master, sda=PIN_A15, scl=PIN_A14, STREAM=I2C_1)

#include "i2c.c"
#include "rm3100.c"
#include "uart_interface.c"


void main(void)
{
	unsigned int8 mag_valid;
	unsigned int8 mag_status;
	unsigned int8 mag_status_after;
	int32 mag_x, mag_y, mag_z;

	delay_ms(1000);
	mag_valid = rm3100_init(RM3100_ADDRESS_20);
	rm3100_set_max_rate(RM3100_ADDRESS_20, RM3100_037HZ);
	rm3100_set_cycle(RM3100_ADDRESS_20, RM3100_COUNT, RM3100_COUNT, RM3100_COUNT);
	rm3100_set_mode(RM3100_ADDRESS_20, RM3100_MODE_CMM);
	delay_ms(100);

	do
	{
		mag_status = rm3100_read_8(RM3100_ADDRESS_20, RM3100_REGISTER_STATUS)&0x40;
		//fprintf(COM_A, "I'm alive, I'm port 0.\r\n");
		//fprintf(COM_B, "I'm alive, I'm port 1.\r\n");
		//fprintf(COM_C, "I'm alive, I'm port 2. Mag Valid: %d; Mag Status: %X.", mag_valid, mag_status);
		//fprintf(COM_D, "I'm alive, I'm port 3.\r\n");

		if (mag_status)
		{
			//fprintf(COM_C, "I'm alive, I'm port 2. Mag Valid: %d; Mag Status: %X.", mag_valid, mag_status);
			rm3100_read_data(RM3100_ADDRESS_20, &mag_x, &mag_y, &mag_z);
			fprintf(COM_C, "%ld, %ld, %ld\n\r", mag_x, mag_y, mag_z);
			//fprintf(COM_C, "Counts X: %ld, Counts Y: %ld, Counts Z: %ld\n\r", mag_x, mag_y, mag_z);
			delay_ms(25);
		}

	} while(TRUE);

}
