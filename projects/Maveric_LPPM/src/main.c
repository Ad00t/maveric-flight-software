//=============================================================================================================
// Lower PPM main.c
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

#define LOWER_PPM
#define NODE_ID             1
#define NODE_LBL            "LPPM"
#define LOG_LEVEL           LL_TRACE 

// Module includes (.c necessary)

#include <time.h>
#include <time.c>
#include "colors.h"
#include "crcnew.c"
#include "uart.c"
#include "hashtable.c"
#include "ringbuf.c"
#include "i2c.c"
#include "spi.c"
#include "interrupts.c"
#include "systime.c"
#include "common.c"
#include "adcsmtq.c"
#include "adis16260.c"
#include "naviguider.c"
#include "m41t81s.c"
#include "cmdmgr.c"
#include "scheduler.c"
#include "cmdimpl.c"
#include "housekeeping.c"

void system_init(void);
void system_superloop(void);
void system_cleanup(void);

int1 SUPERLOOP_RUNNING = TRUE;

irqmgr_s g_irqmgr;            // Interrupts manager
cmdmgr_s g_cmdmgr;            // Commands manager
scheduler_s g_scheduler;      // Schedules manager
ertc_s g_ertc;                // External RTC (on motherboard)
mtq_s g_mtq;                  // Magnetorquer
gyro_s g_gyro;                // Gyroscope (x3)
nvg_s g_nvg;                  // Naviguider

void main(void) {	
    system_init();
    while (SUPERLOOP_RUNNING) {
        system_superloop(); 
    }
    system_cleanup();
}

// System initialization routine
void system_init(void) {
    // Watchdog, millisecond timer init
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

    // Init ertc, irtc, system time 
    struct_tm dfl_time;
    dfl_time.tm_wday = 3;
    dfl_time.tm_mon = 1;
    dfl_time.tm_mday = 1;
    dfl_time.tm_year = 26;
    dfl_time.tm_hour = 0;
    dfl_time.tm_min = 0;
    dfl_time.tm_sec = 0;
    ertc_init(&g_ertc, &dfl_time);
    systime_init(&g_irqmgr.ms, &g_ertc.time);
    
    // Submodules & services init
    // mtq_init(&g_mtq, MTQ_PORT);
    // mtq_set_conf(&g_mtq, 0, MTQ_MODE_DETUMBLING);
    // gyro_init(&g_gyro, GYRO_CS1, GYRO_CS2, GYRO_CS3, GYRO_ON);
    nvg_init(&g_nvg, NVG_PORT);
    scheduler_init(&g_scheduler);
    cmdmgr_init(&g_cmdmgr);
    hk_init();
    cmdimpl_init();

    sprintf(LOGBUF, "system initialized"); log_flush(LL_INFO);
    delay_ms(1000);
}

// Master code of what runs every superloop iteration
void system_superloop(void) {
    // Kick the dog
    restart_wdt();

    // Handle received byte interrupts
    isr_disable_all();
    // Do driver handling before commands so data is up to date
    // mtq_parse_stream(&g_mtq, &g_irqmgr.irqbufs[0]); // Handle magnetorquer data
    nvg_parse_stream(&g_nvg, &g_irqmgr.irqbufs[1]); // Handle naviguider data
    cmdmgr_parse_stream(&g_cmdmgr, &g_irqmgr.irqbufs[2], &g_cmdmgr.rcvpkts[0]); // Handle UPPM commands 
    cmdmgr_parse_stream(&g_cmdmgr, &g_irqmgr.irqbufs[3], &g_cmdmgr.rcvpkts[1]); // Handle FTDI commands
    isr_enable_all();
   
    scheduler_run_tasks(&g_scheduler, &g_cmdmgr);
}

// Cleanup routine
void system_cleanup(void) {
    mtq_destroy(&g_mtq);
    nvg_destroy(&g_nvg);
}
