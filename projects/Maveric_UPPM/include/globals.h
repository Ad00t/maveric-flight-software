// UPPER PPM GLOBALS

#include "scheduler.h"
#include "mcpmgr.h"
#include "flashmgr.h"
#include "interrupts.h"
#include "ax100.h"
#include "pldmgr.h"
#include "telemetry.h"
#include <stdint.h>

extern int1 g_superloop_running;        // Setting to FALSE will end the superloop and reset PPM
extern uint8_t g_rbt_cause;             // restart_cause() in init 

extern irqmgr_s g_irqmgr;               // Interrupts manager
extern i2cmgr_s g_i2cmgr;               // I2C manager
extern mcpmgr_s g_mcpmgr;               // MCP comms manager
extern scheduler_s g_scheduler;         // Schedules manager
extern flashmgr_s g_flashmgr;           // Flash manager
extern rtc_time_t g_rtc_time;           // Global RTC time tracking instance (from lower PPM)       
extern ax100_s g_ax100;                 // AX100 transceiver driver 
extern pldmgr_s g_pldmgr;               // Payloads manager
extern tlm_s g_tlm;                     // Global telemetry state / data store
