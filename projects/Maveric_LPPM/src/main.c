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

#use delay(clock=32MHZ, internal=8M)  // Tells compiler what the clock speed is

// PIC registers

#word RCON = 0x0740
#word OSCCON = getenv("SFR:OSCCON") 
#bit IOLOCK = OSCCON.6
#word RPINR20 = getenv("SFR:RPINR20")
#word RPOR1 = getenv("SFR:RPOR1")

// Ports initialization

#include "pinslower.h"            // Add pins Lower PPM Definition

#use rs232(baud=COM_A_BAUD, UART1, BITS=8, STREAM=COM_A, ERRORS, PARITY=N, STOP=1, TIMEOUT=1000)
#use rs232(baud=COM_B_BAUD, UART2, BITS=8, STREAM=COM_B, ERRORS, PARITY=N, STOP=1, TIMEOUT=1000)
#use rs232(baud=COM_C_BAUD, UART3, BITS=8, STREAM=COM_C, ERRORS, PARITY=N, STOP=1, TIMEOUT=1000)
#use rs232(baud=COM_D_BAUD, UART4, BITS=8, STREAM=COM_D, ERRORS, PARITY=N, STOP=1, TIMEOUT=1000)
#use spi(MASTER, FORCE_HW, SPI1, BAUD=2000000, MSB_FIRST, BITS=16, MODE=3, STREAM=SPI_1)
#use i2c(MASTER, I2C1, STREAM=I2C_1)
// #use timer(TIMER=1, TICK=1ns, BITS=16, ISR)
// #use i2c(master, sda=PIN_A3, scl=PIN_A2, STREAM=I2C_1)
// #use i2c(master, sda=PIN_A15, scl=PIN_A14, STREAM=I2C_1)

// Global defines

#define KNRM  "\033[0m"
#define KRED  "\033[31m"
#define KGRN  "\033[32m"
#define KYEL  "\033[33m"
#define KBLU  "\033[34m"
#define KMAG  "\033[35m"
#define KCYN  "\033[36m"
#define KWHT  "\033[37m"

#define LOWER_PPM
#define NODE_ID             1
#define NODE_LBL            "LPPM"
#define LOG_LEVEL		    LL_INFO
#define MAX_BUF_LEN         256
#define FLATLINE_TIME_MS    3000

// Module includes (.c necessary)

#include <time.h>
#include <time.c>
#include "crcnew.c"
#include "hashtable.c"
#include "circbuf.c"
#include "uart.c"
#include "i2c.c"
#include "spi.c"
// #include "logger.c"
#include "interrupts.c"
#include "systime.c"
#include "adcsmtq.c"
#include "adis16260.c"
#include "naviguider.c"
#include "m41t81s.c"
#include "scheduler.h" // Needed for some reason
#include "schedfunc.c"
#include "scheduler.c"
#include "cmdmgr.c"
#include "cmdfunc.c"

void system_init(void);
void housekeeping(void);
void system_cleanup(void);

int1 SUPERLOOP_RUNNING = TRUE;

irqmgr_s irqmgr;            // Interrupts manager
cmdmgr_s cmdmgr;            // Commands manager
scheduler_s scheduler;      // Schedules manager
ertc_s ertc;                // External RTC (on motherboard)
mtq_s mtq;                  // Magnetorquer
gyro_s gyro;                // Gyroscope (x3)
nvg_s nvg;                  // Naviguider

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

    // MS timer init
	setup_timer1(TMR_INTERNAL | TMR_DIV_BY_64, 0x00FA); 

    // SPI init
	// output_high(FLASH_CHIP_SELECT);
	// output_high(SECOND_FLASH_CS);
	// spi_set_mode(GYRO_SPI_MODE);
	// setup_spi(SPI_MASTER | SPI_XMIT_L_TO_H | SPI_CLK_DIV_16 | SPI_SCK_IDLE_HIGH);

    // RTC init 
    // setup_rtc(RTC_ENABLE | RTC_OUTPUT_SECONDS, 0); 
    
    // Init interrupts 
    irqmgr_init(&irqmgr);
    irqmgr.started = TRUE;
    isr_enable_all();

    // Init rtc, system time 
    struct_tm dfl_time;
    dfl_time.tm_wday = 3;
    dfl_time.tm_mon = 1;
    dfl_time.tm_mday = 1;
    dfl_time.tm_year = 26;
    dfl_time.tm_hour = 0;
    dfl_time.tm_min = 0;
    dfl_time.tm_sec = 0;
    ertc_init(&ertc, dfl_time);
    systime_init(&irqmgr.ms, &ertc.time);
    
    // Submodules & services init
    // mtq_init(&mtq, COM_A);
    // mtq_set_conf(&mtq, 0, MTQ_MODE_DETUMBLING);
    // gyro_init(&gyro, GYRO_CS1, GYRO_CS2, GYRO_CS3, GYRO_ON);
    // nvg_init(&nvg, COM_B);
    // cmdmgr_init(&cmdmgr);
    scheduler_init(&scheduler);

    fprintf(COM_D, "%s[%s] system initialized\n", KWHT, NODE_LBL);
    delay_ms(1000);
}

// Housekeeping routine
void housekeeping(void) {
    // Kick the dog
    restart_wdt();

    // Handle received byte interrupts
    isr_disable_all();
    // mtq_rcv_parser(&mtq, &irqmgr.irqbufs[0]); // Do driver handling before commands so data is up to date
    // nvg_rcv_parser(&nvg, &irqmgr.irqbufs[1]);
    cmdmgr_rcv_parser(&cmdmgr, &irqmgr.irqbufs[3], &cmdmgr.rcvpkts[0]); // Handle COM_D FTDI commands on cmd rcvpkt 0
    isr_enable_all();
   
    scheduler_run_tasks(&scheduler);

    // Send commands to read all sensors
    // mtq_read_ctrl(&mtq);
    // mtq_read_fast(&mtq);
    // gyro_read_all(&gyro);
}

// Cleanup routine
void system_cleanup(void) {
    // mtq_destroy(&mtq);
    // nvg_destroy(&nvg);
}
