//=============================================================================================================
// Lower PPM main.c
// Testing routine template for components connected into the PPMs
//=============================================================================================================

#include <24FJ256GA110.h>			// Device Header File.  Switched to brackets to use version in PICC Library

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

#include "pinslower.h"            // Add pins Lower PPM Definition

//==================================================================
//  		Serial Port Initialization
//===================================================================
#use rs232(baud=COM_A_BAUD, UART1, bits=8, STREAM=COM_A, ERRORS, PARITY=N, TIMEOUT=1000)
#use rs232(baud=COM_B_BAUD, UART2, bits=8, STREAM=COM_B, ERRORS, PARITY= N, TIMEOUT=1000)
#use rs232(baud=COM_C_BAUD, UART3, bits=8, STREAM=COM_C, ERRORS, PARITY=N, TIMEOUT=1000) // To/From other PIC
#use rs232(baud=COM_D_BAUD, UART4, bits=8, STREAM=COM_D, ERRORS, PARITY= N, TIMEOUT=1000)
//==================================================================
//  		I2C Port Initialization
//===================================================================
//#use i2c(master, sda=PIN_G3, scl=PIN_G2, STREAM=I2C_1)
//#use i2c(master, sda=PIN_A3, scl=PIN_A2, STREAM=I2C_1)
//#use i2c(master, sda=PIN_A15, scl=PIN_A14, STREAM=I2C_1)

#define KNRM  "\033[0m"
#define KRED  "\033[31m"
#define KGRN  "\033[32m"
#define KYEL  "\033[33m"
#define KBLU  "\033[34m"
#define KMAG  "\033[35m"
#define KCYN  "\033[36m"
#define KWHT  "\033[37m"

#define MAX_BUF_LEN     256

#include "hashtable.c"
#include "cmd.c"
#include "uart.c"
#include "adcsmtq.c"
#include "interrupts.c"

irqmgr_s irqmgr;
cmdmgr_s cmdmgr;
adcsmtq_s tad102063;

void system_init(void);
void housekeeping(void);

void main(void) {	
    system_init();

    while (TRUE) {
//        irqmgr_log_bufs(&irqmgr); // Print all interrupt rcv buffers 
        irqmgr_handle_rcv(&irqmgr, &tad102063); // Handle all rcv'd interrupts
        housekeeping(); // Housekeeping routine
    }
}

void system_init(void) {
	//Interrupt enabling
	//CRCCON = 0x07;
	//CRCXOR = 0x0106;
	setup_crc(8,2,1);
	delay_ms(1000);
	crc_init(0);

    irqmgr_init(&irqmgr);
    irqmgr.start_flag = TRUE;
    isr_enable_all();

    adcsmtq_init(&tad102063, COM_A);

	delay_ms(1000);
}

void housekeeping(void) {
    fprintf(COM_D, "%s[LPPM] HK COM_D ACTIVE \r\n", KWHT);
    
    adcsmtq_readback(&tad102063);
    
//    adcsmtq_read_start(&tad102063, "SNID");
    
    adcsmtq_read_start(&tad102063, "TIME");
    
    float mass = 15.0;
    adcsmtq_write_start(&tad102063, "MASS", &mass);
    
//    float axis[3];
//    axis[0] = 1f;
//    axis[1] = 0f;
//    axis[2] = 0f;
//    adcsmtq_write_start(&tad102063, "POINTING_AXIS", axis);

    delay_ms(1000);
}
