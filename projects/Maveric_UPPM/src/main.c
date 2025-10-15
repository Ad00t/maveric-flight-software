//=============================================================================================================
// Upper PPM main.c
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

//#word CRCCON = 0x0640
//#word CRCXOR = 0x0642

/* // DEBUG DISPLAY
#warning OSCCON is located at getenv("SFR:OSCCON")
#warning IOLOCK is located at getenv("BIT:IOLOCK")
#warning RPINR20 is located at getenv("SFR:RPINR20")
#warning RPOR1 is located at getenv("SFR:RPOR1")
*/

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

//#define COM_A_BAUD  38400  // Beacon
//#define COM_B_BAUD  19200  // Transceiver
#define COM_A_BAUD  115200  // Beacon (Test purposes)
#define COM_B_BAUD  115200  // Transceiver (Test purposes)
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
//#use rs232(baud = COM_A_BAUD, UART1, bits = 8, STREAM = COM_A, ERRORS, DISABLE_INTS, TIMEOUT=1000)
//#use rs232(baud = COM_B_BAUD, UART2, bits = 8, STREAM = COM_B, ERRORS, DISABLE_INTS, TIMEOUT=1000)
//#use rs232(baud = COM_C_BAUD, UART3, bits = 8, STREAM = COM_C, ERRORS, DISABLE_INTS, TIMEOUT=1000) // To/From other PIC
//#use rs232(baud = COM_D_BAUD, UART4, bits = 8, STREAM = COM_D, ERRORS, DISABLE_INTS, TIMEOUT=1000)
#use rs232(baud = COM_A_BAUD, UART1, bits = 8, STREAM = COM_A, ERRORS, PARITY = N, TIMEOUT=1000)
#use rs232(baud = COM_B_BAUD, UART2, bits = 8, STREAM = COM_B, ERRORS, PARITY = N, TIMEOUT=1000)
#use rs232(baud = COM_C_BAUD, UART3, bits = 8, STREAM = COM_C, ERRORS, PARITY = N, TIMEOUT=1000) // To/From other PIC
#use rs232(baud = COM_D_BAUD, UART4, bits = 8, STREAM = COM_D, ERRORS, PARITY = N, TIMEOUT=1000)
//==================================================================
//  		I2C Port Initialization
//===================================================================
//#use i2c(master, sda=PIN_G3, scl=PIN_G2, STREAM=I2C_1)
//#use i2c(master, sda=PIN_A3, scl=PIN_A2, STREAM=I2C_1)
//#use i2c(master, sda=PIN_A15, scl=PIN_A14, STREAM=I2C_1)

#include "cmd.c"
#include "interrupts.h"

void main(void) 
{	
	//Lower PPM routes
	//int node = 1;
	//int route1[7]={0,2,3,3,3,3,7};
	//int route2[3]={0,3,2};

	//Upper PPM routes
	int node = 3;
	int route1[7]={1,2,0,4,5,6,1};
	int route2[3]={2,1,0};

	//EPS BBQ routes
	//int node = 2;
	//int route1[7]={1,0,3,3,3,3,1};
	//int route1[3]={3,0,1};

	unsigned int16 crc_value;
	//Interrupt enabling
	//CRCCON = 0x07;
	//CRCXOR = 0x0106;
	start_flag = FALSE;
	cmd_flag = FALSE;
	setup_crc(8,2,1);
	delay_ms(1000);
	crc_init(0);
	enable_all_interrupts();
	start_flag = TRUE;
	delay_ms(1000);
	
	
	do
	{	
		
		//Received comand routine
		if (cmd_flag)
		{
			
			//unsigned char fix_cmd[27];
			
			
			disable_all_interrupts();
			//fix_cmd = "POGJJJJ";
			
			
			delay_ms(10);
			crc_init(255);
			delay_ms(100);			
			crc_value = crc_calc8(&rcv_cmd,len-1);
			//crc_value = crc_calc8(&fix_cmd,2,8);
			delay_ms(100);
			fprintf(COM_D,"\033[31m[SYS] Solving cmd: %s; len: %u; start: %u; check: %s; crc: %2u \n\r",rcv_cmd, len, rcv_cmd[0], &rcv_cmd[len-1], crc_value);
			//Get the cmd components
			cmd_get_command(rcv_cmd,&orgn,&dest,&ech, cmd, prms);			
			fprintf(COM_D,"[SYS] origin: %u; destination: %u; echo: %u\n\r",orgn,dest,ech);
			//fprintf(COM_D,"[SYS] command: %s; params: %s\n\r",cmd,prms);
			if (dest == node)
			{
				fprintf(COM_D,"[SYS] Decoding command: %s; params: %s; route: %u\n\r",cmd,prms,route1[dest-1]);
			}
			else
			{
				fprintf(COM_D,"[SYS] forwarding command: %s to route: %u\n\r",rcv_cmd,route1[dest-1]);
			}

			if (dest == node)
			{
				fprintf(COM_D,"[SYS] Received command: %s; params: %s; route: %u\n\r",cmd,prms,route1[dest-1]);
			}
			else
			{
				if ((dest == route1[0])||(dest == route1[6]))
				{
					fprintf(COM_C,"%s\r",rcv_cmd);
				}	
				else if ((dest == route1[3])||(dest == route1[4]))
				{
					fprintf(COM_C,"%s\r",rcv_cmd);
				}
			}
			cmd_flag = FALSE;
			enable_all_interrupts();
		}
		//House keeping routine
		else
		{
			fprintf(COM_A, "\033[32m[HK] I'm alive, Starting port 0.\r\n");
			delay_ms(250);	
			fprintf(COM_B, "\033[32m[HK] I'm alive, Starting port 1.\r\n");
			delay_ms(250);
			//fprintf(COM_C, "\033[32m[HK] I'm alive, Starting port 2.\r\n");
			//fprintf(COM_C, "313COM_PING 1 C\r");
			delay_ms(250);
			fprintf(COM_D, "\033[32m[HK] I'm alive, Starting port 3.\r\n");
			delay_ms(250);
		}
	} while(TRUE);
	
}

