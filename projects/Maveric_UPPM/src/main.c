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
#use i2c(MASTER, I2C1, STREAM=I2C_1)
#use i2c(MASTER, I2C2, STREAM=I2C_2)
#use i2c(MASTER, I2C3, STREAM=I2C_3)

#include "nodes.h"
#define UPPER_PPM
#define NODE                NODE_UPPM 
#define SCHED_ID_PPM_RST    3
#define SCHED_ID_BEACON     6

// Module includes (.c necessary)

#include <time.h>
#include <time.c>
#include "common.c"
#include "uart.c"
#include "crcnew.c"
#include "hashtable.c"
#include "ringbuf.c"
#include "i2c.c"
#include "spi.c"
#include "interrupts.c"
#include "systime.c"
#include "at25df641.c"
#include "flashmgr.c"
#include "frame.c"
#include "mcppkt.c"
#include "logger.c"
#include "ax100.c"
#include "pldmgr.c"
#include "telemetry.c"
#include "mcpmgr.c"
#include "scheduler.c"
#include "cmdimpl.c"
#include "housekeeping.c"

void system_init(void);
void system_superloop(void);
void system_ops_transition_safe(void);
void system_ops_deploy(void);
void system_ops_transmit_beacon(void);
void system_ops_check_eps(void);
void system_cleanup(void);

int1 g_superloop_running = TRUE;
uint8_t g_rbt_cause = 0;

irqmgr_s g_irqmgr = {0};            // Interrupts manager
i2cmgr_s g_i2cmgr = {0};            // I2C manager
mcpmgr_s g_mcpmgr = {0};            // MCP comms manager
scheduler_s g_scheduler = {0};      // Schedules manager
flashmgr_s g_flashmgr = {0};        // Flash manager
rtc_time_t g_rtc_time = {0};        // Global RTC time tracking instance (from lower PPM)      
ax100_s g_ax100 = {0};              // AX100 transceiver driver
pldmgr_s g_pldmgr = {0};            // Payloads manager
tlm_s g_tlm = {0};                  // Global telemetry state / data store

void main(void) {	
    system_init();
    while (g_superloop_running) {
        system_superloop(); 
    }
    system_cleanup();
}

// System initialization routine
void system_init(void) {
    // Watchdog, millisecond timer, logger, rbt_cause init
    delay_ms(4000);
    // fprintf(COM_C, "uppm init\r\n");
    setup_wdt(WDT_ON);
	setup_timer1(TMR_INTERNAL | TMR_DIV_BY_64, 249); 
    logger_init();
    g_rbt_cause = restart_cause();
    
    // SPI init
	output_high(FLASH_CHIP_SELECT);
	spi_set_mode(FLASH_SPI_MODE);
    delay_ms(50);
    
    // Init interrupts 
    irqmgr_init(&g_irqmgr);
    i2cmgr_init(&g_i2cmgr);
    g_irqmgr.started = TRUE;
    g_i2cmgr.started = TRUE;
    isr_enable_all();

    // Init rtc time, system time 
    g_rtc_time.tm_wday = 3;
    g_rtc_time.tm_mon = 1;
    g_rtc_time.tm_mday = 1;
    g_rtc_time.tm_year = 26;
    g_rtc_time.tm_hour = 0;
    g_rtc_time.tm_min = 0;
    g_rtc_time.tm_sec = 0;
    systime_init(&g_irqmgr.ms, &g_rtc_time); // It doesn't look like the UPPM built in RTC works.
    
    // Submodules & services init
    mcpmgr_init(&g_mcpmgr);
    scheduler_init(&g_scheduler);
    tlm_init(&g_tlm);
    status_e s_flashmgr = flashmgr_init(&g_flashmgr, &g_scheduler);
    status_e s_ax100 = ax100_init(&g_ax100, AX100_PORT);
    status_e s_pldmgr = pldmgr_init(&g_pldmgr);
    cmdimpl_init();
    hk_init();

    // Fetch time from LPPM
    mcp_dispatch(NODE, NODE_LPPM, 0, CMD, "ppm_get_time", "");

    // Force switch to OPS_SAFE if we're still in OPS_INIT after 100 reboots
    config_s* cfg = &g_flashmgr.config;
    if (g_flashmgr.rbt_cnt > 100 && cfg->ops_stage == OPS_INIT) {
        cfg->ops_stage = OPS_SAFE;
        flashmgr_config_flush(&g_flashmgr);
    }

    // Check operations stage and schedule tasks accordingly
    switch (cfg->ops_stage) {
        case OPS_INIT: {
            scheduler_schedule_func_in(&g_scheduler, 5, system_ops_transition_safe, 3*MS_PER_MIN, 0, 1);     // 45 min
            mcp_dispatch(NODE, NODE_EPS, 0, CMD, "eps_rst_ctn", "7199");
            mcp_dispatch(NODE, NODE_LPPM, 0, CMD, "gnc_set_mode", "0");
            break;
        }
        case OPS_SAFE: {
            scheduler_schedule_func_in(&g_scheduler, 8, system_ops_deploy, 30000, 0, 1);
        } // Fallthrough
        case OPS_NOMINAL: { 
            ax100_set_power(&g_ax100, TRUE);
            scheduler_schedule_func_in(&g_scheduler, SCHED_ID_BEACON, system_ops_transmit_beacon, 
                                       2*MS_PER_MIN, cfg->bcn_period, SCHEDULE_REPS_INFINITE); // Schedule 6
            scheduler_schedule_func_in(&g_scheduler, 7, system_ops_check_eps, 1*MS_PER_MIN, 1*MS_PER_MIN, SCHEDULE_REPS_INFINITE);
            break;
        }
    }

    sprintf(LOGBUF, "system initialized rs232_err=%u rbt_cnt=%u rbt_cause=%u s_flashmgr=%u s_ax100=%u s_pldmgr=%u ops=%u", 
            rs232_errors, g_flashmgr.rbt_cnt, g_rbt_cause, s_flashmgr, s_ax100, s_pldmgr, cfg->ops_stage); log_info();
}

// Main master routine run in superloop
void system_superloop(void) {
    // Kick the dog
    restart_wdt();

    // Handle received byte interrupts
    mcpmgr_parse_stream(&g_mcpmgr, &g_i2cmgr.rxbufs[0], &g_mcpmgr.rcvpkts[0], FALSE); // Handle EPS packets 
    mcpmgr_parse_stream(&g_mcpmgr, &g_irqmgr.irqbufs[HOLONAV_PORT-1], &g_mcpmgr.rcvpkts[1], FALSE); // Handle Holonav packets 
    mcpmgr_parse_stream(&g_mcpmgr, &g_irqmgr.irqbufs[AX100_PORT-1], &g_mcpmgr.rcvpkts[2], TRUE); // Handle AX100 packets 
    mcpmgr_parse_stream(&g_mcpmgr, &g_irqmgr.irqbufs[LPPM_PORT-1], &g_mcpmgr.rcvpkts[3], FALSE); // Handle LPPM packets 
    mcpmgr_parse_stream(&g_mcpmgr, &g_irqmgr.irqbufs[ASTROBOARD_PORT-1], &g_mcpmgr.rcvpkts[4], FALSE); // Handle Astroboard packets 
   
    scheduler_run_tasks(&g_scheduler, &g_mcpmgr);
}

void system_ops_transition_safe(void) {
    sprintf(LOGBUF, "system_ops_transition_safe"); log_info();
    mcp_dispatch(NODE, NODE_LPPM, 0, CMD, "ppm_reset", "");
    config_s* cfg = &g_flashmgr.config;
    uint8_t i;
    for (i = 0; i < 5; i++) {
        cfg->ops_stage = OPS_SAFE;
        flashmgr_config_flush(&g_flashmgr);
        flashmgr_config_load_flash(&g_flashmgr);
        if (cfg->ops_stage == OPS_SAFE) break;
        else delay_ms(1000);
    }
    g_superloop_running = FALSE;    // Reset so init sees ops_stage=1 and runs deploy
}

void system_ops_deploy(void) {
    sprintf(LOGBUF, "DEPLOYING"); log_info();
    mcp_dispatch(NODE, NODE_EPS, 0, CMD, "eps_burn", "3");
}

void system_ops_transmit_beacon(void) {
    tlm_beacon(&g_tlm);
}

void system_ops_check_eps(void) {
    sprintf(LOGBUF, "system_ops_check_eps eps=%u gnc=%u", g_tlm.eps_mode, g_tlm.gnc_mode); log_info();
    if (g_tlm.gnc_mode == 3) return; // In manual off state
    if (g_tlm.eps_mode >= 1 && g_tlm.gnc_mode == 0) {               
        // Safe power levels -> enable GNC if not already
        mcp_dispatch(NODE, NODE_LPPM, 0, CMD, "gnc_set_mode", "1");
    } else if (g_tlm.eps_mode == 0 && g_tlm.gnc_mode != 0) {    
        // Critical power -> disable GNC if not already
        mcp_dispatch(NODE, NODE_LPPM, 0, CMD, "gnc_set_mode", "0");
    }
}

// Cleanup routine
void system_cleanup(void) {
    ax100_set_power(&g_ax100, FALSE);
    reset_cpu();
}
