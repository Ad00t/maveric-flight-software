#include <stdint.h>
#include "adcsmtq.h"

// HELPERS

void gnc_step(gnc_s* state, mtq_s* mtq, float* gyro_rate_rad) {
    // Extract Mode and Flags from STAT
    uint32_t stat = 0;
    mtq_get_data(mtq, MTQ_STAT, &stat);
    uint8_t current_mode = mtq_stat_parse_mode(stat);
    int1 sun = mtq_stat_parse_sun(stat);
    int1 tumb = mtq_stat_parse_tumb(stat);

    // Check for unexpected mode transitions
    if (state->expected_mode != current_mode) {
        
        // Track the unexpected transition
        if (current_mode == MTQ_MODE_SAFE) {
            state->unexpected_safe_count++;
            
            // If the MTQ unexpectedly drops into SAFE mode, immediately re-enable auto mode so the transition logic can recover the vehicle.
            state->auto_mode_enabled = TRUE;
            
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
            if (!tumb && sun) {
                if (mtq_set_mode(mtq, MTQ_MODE_SUN_SPIN) == SUCCESS) {
                    state->expected_mode = MTQ_MODE_SUN_SPIN;
                }
            } else if (gyro_rate_rad[0] < GNC_MAX_DETUMBLE_RATE && gyro_rate_rad[1] < GNC_MAX_DETUMBLE_RATE && gyro_rate_rad[2] < GNC_MAX_DETUMBLE_RATE) {
                if (mtq_set_mode(mtq, MTQ_MODE_DETUMBLING) == SUCCESS) {
                    state->expected_mode = MTQ_MODE_DETUMBLING;
                }
            }
            break;

        case MTQ_MODE_DETUMBLING:
            // Switch to SUN SPIN if TUMB is 0 and SUN is 1, else do nothing yet
            if (!tumb && sun) {
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
            if (!tumb && sun) {
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
