//=============================================================================================================
// Lower PPM main.c
// Testing routine template for components connected into the PPMs
//=============================================================================================================

#include <24FJ256GA110.h>			// Device Header File.  Switched to brackets to use version in PICC Library

#device PASS_STRINGS=IN_RAM			// Should allow for things like sendMSGALL(port,"Message") <--- const weren't allowed there before
#device ADC=16						// ??

#build(stack=0x300)					// Use a larger stack size.
#fuses NOPROTECT					// Code not protected from reading
// #fuses NOWDT						// No automatic WDT -- it must be enabled by software.
#fuses WDT
//#fuses XT							// Primary Clock Select
//#fuses FRC_PLL				    // Internal Fast RC Oscillator with Phase Lock Loop gives 32 MHz
#fuses HS
#fuses PR_PLL
#fuses WPOSTS13						// Watchdog Postscaler.  
									// At current processor settings: WPOSTS12 = ~10 seconds
									//								  WPOSTS13 = ~17 seconds
									//								  default  = ~120 seconds
#fuses IESO							// Internal-External Switchover
//#fuses IOL1WAY					// Locks the I/O Lock after setting it once.
#fuses NOIOL1WAY					// Allows multiple changes to PIC register settings.
#fuses WRT
#fuses CKSFSM						// Clock fail-safe monitor	
#pragma case						// Makes all code case-sensitive

#use delay(clock=32MHZ,internal=8M)  // Tells compiler what the clock speed is

// PIC registers

#word RCON = 0x0740
#word OSCCON = getenv("SFR:OSCCON") 
#bit IOLOCK = OSCCON.6
#word RPINR20 = getenv("SFR:RPINR20")
#word RPOR1 = getenv("SFR:RPOR1")

#include "pinslower.h"            // Add pins Lower PPM Definition

// Ports initialization

#use rs232(baud=COM_A_BAUD, UART1, BITS=8, STREAM=COM_A, ERRORS, PARITY=N, TIMEOUT=1000)
#use rs232(baud=COM_B_BAUD, UART2, BITS=8, STREAM=COM_B, ERRORS, PARITY=N, TIMEOUT=1000)
#use rs232(baud=COM_C_BAUD, UART3, BITS=8, STREAM=COM_C, ERRORS, PARITY=N, TIMEOUT=1000)
#use rs232(baud=COM_D_BAUD, UART4, BITS=8, STREAM=COM_D, ERRORS, PARITY=N, TIMEOUT=1000)
// #use spi(MASTER, DI=SDI1, DO=SDO1, CLK=SCK1OUT, ENABLE=GYRO_ON, BITS=16, STREAM=SPI_1)
#use spi(MASTER, SPI1, BITS=16)
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

#define LOWER_PPM
#define NODE_ID         1
#define NODE_LBL        "LPPM"
#define LOG_LEVEL		LL_INFO
#define MAX_BUF_LEN     256

#include "crcnew.c"
#include "hashtable.c"
#include "circbuf.c"
#include "spi.c"
#include "uart.c"
// #include "logger.c"
#include "interrupts.c"
#include "cmdmgr.c"
#include "cmdfunc.c"
#include "gyro.c"
#include "mtq.c"

void system_init(void);
void housekeeping(void);
void system_cleanup(void);

int1 SUPERLOOP_RUNNING = TRUE;
irqmgr_s irqmgr;
cmdmgr_s cmdmgr;
gyro_s gyro;
mtq_s mtq;

void main(void) {	
    system_init();
    while (SUPERLOOP_RUNNING) {
        housekeeping(); 
    }
    system_cleanup();
}

// System initialization routine
void system_init(void) {
    // Watchdog init
    setup_wdt(WDT_ON);

    // RTC init
    setup_rtc(RTC_ENABLE | RTC_OUTPUT_SECONDS, 0); 

    // SPI init
    // spi_set_mode(FLASH_SPI_MODE);
    
    // Submodules init
    irqmgr_init(&irqmgr);
    cmdmgr_init(&cmdmgr);
    gyro_init(&gyro, GYROCS1, GYROCS2, GYROCS3, GYRO_ON);
    mtq_init(&mtq, COM_A);

    // Start interrupts
    irqmgr.started = TRUE;
    isr_enable_all();
    
    fprintf(COM_D, "%s[%s] system initialized\n", KWHT, NODE_LBL);
    delay_ms(1000);
}

// Housekeeping routine
void housekeeping(void) {
    rtc_time_t t;
    rtc_read(&t);
    fprintf(COM_D, "%s[%s] housekeeping %02u/%02u/20%u %02u:%02u:%02u\n", KWHT, NODE_LBL,
            t.tm_mon, t.tm_mday, t.tm_year, t.tm_hour, t.tm_min, t.tm_sec);

    // Kick the dog
    restart_wdt();
    
    // Handle received byte interrupts
    isr_disable_all();
    mtq_rcv_fsm(&mtq, &irqmgr.irqbufs[0]); // Do driver handling before commands so data is up to date
    cmdmgr_rcv_fsm(&cmdmgr, &irqmgr.irqbufs[1], &cmdmgr.rcvpkts[0]); // Handle COM_D FTDI commands on cmd rcvpkt 0
    isr_enable_all();
    
    // Check heartbeats
    if (!gyro_heartbeat(&gyro)) {
        fprintf(COM_D, "%s[%s] gyro flatlined\n", KRED, NODE_LBL);
    }
    if (!mtq_heartbeat(&mtq)) {
        fprintf(COM_D, "%s[%s] mtq flatlined\n", KRED, NODE_LBL);
    }

    // Read all sensors
    gyro_read_all(&gyro);
    mtq_read_ctrl(&mtq);
    
    delay_ms(1000);
}

// Cleanup routine
void system_cleanup(void) {
    mtq_destroy(&mtq);
}
