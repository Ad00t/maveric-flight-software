#include <stdint.h>
#include "adcsmtq.h"
#include "logger.h"
#include "common.h"

// HELPERS
 

void gnc_init(gnc_s* gnc) {
    memset(gnc, 0, sizeof(gnc_s));
    gnc->gnc_mode = GNC_MODE_SAFE;
}

void gnc_step(gnc_s* state, mtq_s* mtq, float* gyro_rate_rad) {
    if (!mtq->is_init || !mtq->allow_comm) return;

    // Extract Mode and Flags from STAT
    uint32_t stat = 0;
    mtq_get_data(mtq, MTQ_STAT, &stat);
    uint8_t current_mode = mtq_stat_parse_mode(stat);
    int1 sun = mtq_stat_parse_sun(stat);
    int1 tumb = mtq_stat_parse_tumb(stat);

    sprintf(LOGBUF, "gnc_step: gnc_mode=%u curr_mode=%u exp_mode=%u sun=%u tumb=%u",
            state->gnc_mode, current_mode, state->expected_mode, sun, tumb); log_info();


    // Check for unexpected mode transitions
    if (state->expected_mode != current_mode) {
        
        // Track the unexpected transition
        if (current_mode == MTQ_MODE_SAFE) {
            state->unexpected_safe_count++;
            state->expected_mode = MTQ_MODE_SAFE;
            mtq_reset(mtq);
            return;
        } else if (current_mode == MTQ_MODE_DETUMBLING) {
            state->unexpected_detumble_count++;
        }

        // Reset planner state tracking
        state->expected_mode = current_mode;
    }
    // High-Level GNC Planner Routing
    switch (state->gnc_mode) {
        
        // Logic for GNC Safe mode transition and monitoring
        case GNC_MODE_OFF:
        case GNC_MODE_SAFE:
            // Force ADCS to SAFE and take no further actions
            if (current_mode != MTQ_MODE_SAFE) {
                if (mtq_set_mode(mtq, MTQ_MODE_SAFE) == SUCCESS) {
                    state->expected_mode = MTQ_MODE_SAFE;
                }
            }
            break;
        
        case GNC_MODE_MANUAL:
            // Force ADCS to MANUAL and take no further actions   
            if (current_mode != MTQ_MODE_MANUAL) {
                if (mtq_set_mode(mtq, MTQ_MODE_MANUAL) == SUCCESS) {
                    state->expected_mode = MTQ_MODE_MANUAL;
                }
            }
            break; // Exit the function early so no auto-transitions occur
        
        case GNC_MODE_AUTO:
            // Logic for Auto Mode Transitions based on current hardware state

            switch (current_mode) {
                case MTQ_MODE_SAFE:
                    // Call driver to transition to SUNSPIN if possible or DETUMBLE if not and rate is below 360 deg/sec
                    if (!tumb && sun) {
                        if (mtq_set_mode(mtq, MTQ_MODE_SUN_SPIN) == SUCCESS) {
                            state->expected_mode = MTQ_MODE_SUN_SPIN;
                        }
                    } else if (sqrt(pow(gyro_rate_rad[0], 2) 
                                + pow(gyro_rate_rad[1], 2)
                                + pow(gyro_rate_rad[2], 2)) < GNC_MAX_DETUMBLE_RATE_RAD) {
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
                    state->sunspin_count++;
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
}
