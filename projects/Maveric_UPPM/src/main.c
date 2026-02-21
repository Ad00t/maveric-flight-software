//=============================================================================================================
// Upper PPM main.c
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

#include "pinsupper.h"            // Add pins Upper PPM Definition

#use rs232(baud=COM_A_BAUD, UART1, BITS=8, STREAM=COM_A, ERRORS, PARITY=N, STOP=1, TIMEOUT=1000)
#use rs232(baud=COM_B_BAUD, UART2, BITS=8, STREAM=COM_B, ERRORS, PARITY=N, STOP=1, TIMEOUT=1000)
#use rs232(baud=COM_C_BAUD, UART3, BITS=8, STREAM=COM_C, ERRORS, PARITY=N, STOP=1, TIMEOUT=1000)
#use rs232(baud=COM_D_BAUD, UART4, BITS=8, STREAM=COM_D, ERRORS, PARITY=N, STOP=1, TIMEOUT=1000)
#use spi(MASTER, FORCE_HW, SPI1, BAUD=2000000, MSB_FIRST, BITS=16, MODE=3, STREAM=SPI_1)
#use i2c(MASTER, I2C1, STREAM=I2C_1)

// Global defines

#define UPPER_PPM
#define NODE_ID             3
#define NODE_LBL            "UPPM"
#define LOG_LEVEL           LL_TRACE

// Module includes (.c necessary)

#include <time.h>
#include <time.c>
#include "colors.h"
#include "uart.c"
#include "crcnew.c"
#include "common.c"
#include "hashtable.c"
#include "ringbuf.c"
#include "i2c.c"
#include "spi.c"
#include "interrupts.c"
#include "systime.c"
#include "ax100.c"
#include "cmdmgr.c"
#include "scheduler.c"
#include "cmdimpl.c"
#include "housekeeping.c"

void system_init(void);
void system_superloop(void);
void system_cleanup(void);

int1 SUPERLOOP_RUNNING = TRUE;

irqmgr_s g_irqmgr;          // Interrupts manager
cmdmgr_s g_cmdmgr;          // Commands manager
scheduler_s g_scheduler;    // Schedules manager
struct_tm g_rtc_time;       // Global RTC time tracking instance (from lower PPM)       
ax100_s g_ax100;            // AX100 transceiver driver

void main(void) {	
    system_init();
    while (SUPERLOOP_RUNNING) {
        system_superloop(); 
    }
    system_cleanup();
}

// System initialization routine
void system_init(void) {
    // Watchdog, millisecond timer, logbuf init
    setup_wdt(WDT_ON);
	setup_timer1(TMR_INTERNAL | TMR_DIV_BY_64, 0x00FA); 
    memset(LOGBUF, 0, sizeof(LOGBUF));
    
    // SPI init
	// output_high(FLASH_CHIP_SELECT);
	// output_high(SECOND_FLASH_CS);
	// spi_set_mode(GYRO_SPI_MODE);
	// setup_spi(SPI_MASTER | SPI_XMIT_L_TO_H | SPI_CLK_DIV_16 | SPI_SCK_IDLE_HIGH);

    // Init interrupts 
    irqmgr_init(&g_irqmgr);
    g_irqmgr.started = TRUE;
    isr_enable_all();

    // Init rtc time, system time 
    g_rtc_time.tm_wday = 3;
    g_rtc_time.tm_mon = 1;
    g_rtc_time.tm_mday = 1;
    g_rtc_time.tm_year = 26;
    g_rtc_time.tm_hour = 0;
    g_rtc_time.tm_min = 0;
    g_rtc_time.tm_sec = 0;
    systime_init(&g_irqmgr.ms, &g_rtc_time);
    
    // Submodules & services init
    ax100_init(&g_ax100, AX100_PORT);
    cmdmgr_init(&g_cmdmgr);
    scheduler_init(&g_scheduler);
    cmdimpl_init();
    hk_init();

    sprintf(LOGBUF, "system initialized"); log_flush(LL_INFO);
    delay_ms(1000);
}

// Main master routine run in superloop
void system_superloop(void) {
    // Kick the dog
    restart_wdt();

    // Handle received byte interrupts
    isr_disable_all();
    // cmdmgr_parse_stream(&g_cmdmgr, &g_ax100.cmdbuf, &g_cmdmgr.rcvpkts[0]); // Handle AX100 commands
    // cmdmgr_parse_stream(&g_cmdmgr, &g_irqmgr.irqbufs[1], &g_cmdmgr.rcvpkts[1]); // Handle LPPM commands 
    cmdmgr_parse_stream(&g_cmdmgr, &g_irqmgr.irqbufs[3], &g_cmdmgr.rcvpkts[2]); // Handle FTDI commands 
    isr_enable_all();
   
    scheduler_run_tasks(&g_scheduler, &g_cmdmgr);
}

// Cleanup routine
void system_cleanup(void) {

}
