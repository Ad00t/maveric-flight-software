// LOWER PPM GLOBALS

#include "scheduler.h"
#include "cmdmgr.h"
#include "flashmgr.h"
#include "interrupts.h"
#include "adcsmtq.h"
#include "m41t81s.h"
#include "adis16260.h"
#include "naviguider.h"
#include <stdint.h>

extern int1 g_superloop_running;        // Setting to FALSE will end the superloop and reset PPM
extern uint8_t g_rbt_cause;             // restart_cause() in init 

extern irqmgr_s g_irqmgr;               // Interrupts manager
extern cmdmgr_s g_cmdmgr;               // Commands manager
extern flashmgr_s g_flashmgr;           // Flash manager
extern scheduler_s g_scheduler;         // Schedules manager
extern ertc_s g_ertc;                   // External RTC (on motherboard)
extern mtq_s g_mtq;                     // Magnetorquer
extern gyro_s g_gyro;                   // Gyroscope (x3)
extern nvg_s g_nvg;                     // Naviguider
