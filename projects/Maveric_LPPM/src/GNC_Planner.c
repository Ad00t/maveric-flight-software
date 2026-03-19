#include <stdint.h>
#include <stdbool.h>
#include "adcsmtq.h"

// --- Mode Constants ---
#define MTQ_MODE_MANUAL             7 
#define MTQ_MODE_SUN_SPIN           6
#define MTQ_MODE_TARGET_TRACKING    5
#define MTQ_MODE_LVLH               4
#define MTQ_MODE_FINE_POINTING      3
#define MTQ_MODE_SUN_POINTING       2
#define MTQ_MODE_DETUMBLING         1
#define MTQ_MODE_SAFE               0

// --- Planner State Structure ---
typedef struct {
    uint8_t expected_mode;    // Tracks expected MTQ mode to catch unexpected transitions
    bool auto_mode_enabled;   // Set to false to stay in Manual mode
    uint16_t unexpected_safe_count;     // Number of unexpected transitions to SAFE
    uint16_t unexpected_detumble_count; // Number of unexpected transitions to DETUMBLING
} MTQ_Planner_State_t;

// --- Parsing Functions Definitions ---
uint8_t MTQ_STAT_Parse_Mode(uint32_t MTQ_STAT) {
    uint8_t mode_mask = 7;
    return (uint8_t)(MTQ_STAT & mode_mask);
}

bool MTQ_STAT_Parse_SUN(uint32_t MTQ_STAT) {
    return (MTQ_STAT & (1U << 13)) != 0;
}

bool MTQ_STAT_Parse_TUMB(uint32_t MTQ_STAT) {
    return (MTQ_STAT & (1U << 11)) != 0;
}

// --- Main GNC Planner Execution ---
// Ingests the state tracker, driver handle, STAT register, and rate telemetry
void MTQ_GNC_Planner(MTQ_Planner_State_t* state, mtq_s* mtq, uint32_t MTQ_STAT, float MTQ_RATE) {
    // Extract Mode and Flags from STAT
    uint8_t current_mode = MTQ_STAT_Parse_Mode(MTQ_STAT);
    bool SUN  = MTQ_STAT_Parse_SUN(MTQ_STAT);
    bool TUMB = MTQ_STAT_Parse_TUMB(MTQ_STAT);

    // Check for unexpected mode transitions
    if (state->expected_mode != current_mode) {
        
        // Track the unexpected transition
        if (current_mode == MTQ_MODE_SAFE) {
            state->unexpected_safe_count++;
            
            // If the MTQ unexpectedly drops into SAFE mode, immediately re-enable auto mode so the transition logic can recover the vehicle.
            state->auto_mode_enabled = true;
            
        } else if (current_mode == MTQ_MODE_DETUMBLING) {
            state->unexpected_detumble_count++;
        }

        // Reset planner state tracking
        state->expected_mode = current_mode;
    }

    // Manual Override Check
    // If auto_mode is disabled, force Manual mode and skip automatic transitions
    if (!state->auto_mode_enabled) {
        if (current_mode != MTQ_MODE_MANUAL) {
            if (mtq_set_mode(mtq, MTQ_MODE_MANUAL) == SUCCESS) {
                state->expected_mode = MTQ_MODE_MANUAL;
            }
        }
        return; // Exit the function early so no auto-transitions occur
    }

    // Logic for Auto Mode Transitions
    switch (current_mode) {
        case MTQ_MODE_SAFE:
            // Call driver to transition to SUNSPIN if possible or DETUMBLE if not and rate is below 360 deg/sec
            if (!TUMB && SUN) {
                if (mtq_set_mode(mtq, MTQ_MODE_SUN_SPIN) == SUCCESS) {
                    state->expected_mode = MTQ_MODE_SUN_SPIN;
                }
            } else if (MTQ_RATE < 360.0f) {
                if (mtq_set_mode(mtq, MTQ_MODE_DETUMBLING) == SUCCESS) {
                    state->expected_mode = MTQ_MODE_DETUMBLING;
                }
            }
            break;

        case MTQ_MODE_DETUMBLING:
            // Switch to SUN SPIN if TUMB is 0 and SUN is 1, else do nothing yet
            if (!TUMB && SUN) {
                if (mtq_set_mode(mtq, MTQ_MODE_SUN_SPIN) == SUCCESS) {
                    state->expected_mode = MTQ_MODE_SUN_SPIN;
                }
            }
            break;

        case MTQ_MODE_SUN_SPIN:
            break;

        case MTQ_MODE_MANUAL:
            // When auto_mode_enabled becomes true and we are currently in manual, 
            // attempt to go straight to SUN_SPIN if conditions allow, else drop to DETUMBLE.
            if (!TUMB && SUN) {
                if (mtq_set_mode(mtq, MTQ_MODE_SUN_SPIN) == SUCCESS) {
                    state->expected_mode = MTQ_MODE_SUN_SPIN;
                }
            } else {
                if (mtq_set_mode(mtq, MTQ_MODE_DETUMBLING) == SUCCESS) {
                    state->expected_mode = MTQ_MODE_DETUMBLING;
                }
            }
            break;

        default:
            break;
    }
}