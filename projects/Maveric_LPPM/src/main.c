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

#include "cmd.c"
#include "uart.c"
#include "interrupts.c"
#include "adcsmtq.c"

ADCSMTQ tad102063;

void system_init(void);
void handle_cmd(void);
void handle_rcv(void);
void handle_hk(void);

void main(void) {	
    system_init();

    while (TRUE) {
		if (INTERRUPT_CMD_FLAG) { // Received command routine
            handle_cmd();
        } else if (INTERRUPT_RCV_FLAG) { // Other received data routine
            handle_rcv();
        } else { // House keeping routine
            handle_hk();
        }
	}
}

void system_init(void) {
    ADCSMTQ_init(&tad102063, COM_A);

	//Interrupt enabling
	//CRCCON = 0x07;
	//CRCXOR = 0x0106;
	INTERRUPT_START_FLAG = FALSE;
	INTERRUPT_CMD_FLAG = FALSE;
    INTERRUPT_RCV_FLAG = FALSE;
	setup_crc(8,2,1);
	delay_ms(1000);
	crc_init(0);
	enable_all_interrupts();
	INTERRUPT_START_FLAG = TRUE;
	delay_ms(1000);
}

void handle_cmd(void) {
    disable_all_interrupts();

    //Lower PPM routes
	int8 node = 1;
	int8 route1[7]={0,2,3,3,3,3,7};
	int8 route2[3]={0,3,2};

	//Upper PPM routes
	//int node = 3;
	//int route1[7]={1,2,0,4,5,6,1};
	//int route2[3]={2,1,0};

	//EPS BBQ routes
	//int node = 2;
	//int route1[7]={1,0,3,3,3,3,1};
	//int route1[3]={3,0,1};

	unsigned int16 crc_value;
	
    //unsigned char fix_cmd[27];
    delay_ms(10);
    crc_init(255);
    delay_ms(100);			
    crc_value = crc_calc8(&INTERRUPT_RCV_BUF,INTERRUPT_RCV_MSG_LEN-1);
    //crc_value = crc_calc8(&fix_cmd,2,8);
    delay_ms(100);
    fprintf(COM_D,"\033[31m[SYS] Solving cmd: %s; len: %u; start: %u; check: %s; crc: %2u \n\r",
            INTERRUPT_RCV_BUF, INTERRUPT_RCV_MSG_LEN, INTERRUPT_RCV_BUF[0], &INTERRUPT_RCV_BUF[INTERRUPT_RCV_MSG_LEN-1], crc_value);
    //Get the cmd components
    cmd_get_command(INTERRUPT_RCV_BUF,&orgn,&dest,&ech, cmd, prms);			
    fprintf(COM_D,"[SYS] origin: %u; destination: %u; echo: %u\n\r",orgn,dest,ech);
    //fprintf(COM_D,"[SYS] command: %s; params: %s\n\r",cmd,prms);
    if (dest == node) {
        fprintf(COM_D,"[SYS] Decoding command: %s; params: %s; route: %u\n\r",cmd,prms,route1[dest-1]);
    } else {
        fprintf(COM_D,"[SYS] forwarding command: %s to route: %u\n\r",INTERRUPT_RCV_BUF,route1[dest-1]);
    }
    //delay_ms(1000);

    if (dest == node) {
        fprintf(COM_D,"[SYS] Recieved command: %s; params: %s; route: %u\n\r",cmd,prms,route1[dest-1]);
    } else {
        if (dest == route1[6]) {
            fprintf(COM_D,"%s\r",INTERRUPT_RCV_BUF);
        } else if ((dest == route1[2])||(dest == route1[3])||(dest == route1[4])||(dest == route1[5])) {
            fprintf(COM_C,"%s\r",INTERRUPT_RCV_BUF);
        }
    }

    INTERRUPT_CMD_FLAG = FALSE;
    INTERRUPT_RCV_FLAG = FALSE;
    enable_all_interrupts();
}

void handle_rcv(void) {
    disable_all_interrupts();
    
    uint8_t rcv_status;
    uint8_t b0 = INTERRUPT_RCV_BUF[0];
    if (b0 == ADCSMTQ_HEAD_READ) {
        ADCSMTQ_read_complete(&tad102063, &rcv_status);
    } else if (b0 == ADCSMTQ_HEAD_WRITE) {
        ADCSMTQ_write_complete(&tad102063, &rcv_status);
    }

    INTERRUPT_RCV_FLAG = FALSE;
    enable_all_interrupts();
}

void handle_hk(void) {
    fprintf(COM_A, "\033[32m[HK] I'm alive, Starting port 0.\r\n");
    delay_ms(250);	
    fprintf(COM_B, "\033[32m[HK] I'm alive, Starting port 1.\r\n");
    delay_ms(250);
    //fprintf(COM_C, "\033[32m[HK] I'm alive, Starting port 2.\r\n");
    //fprintf(COM_C, "131COM_PING 3 C\r");
    delay_ms(250);
    fprintf(COM_D, "\033[32m[HK] I'm alive, Starting port 3.\r\n");
    delay_ms(250);

    ADCSMTQ_read_start(&tad102063, "SNID");
}
