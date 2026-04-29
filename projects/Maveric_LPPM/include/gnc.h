#ifndef __GNC_H__
#define __GNC_H__

#include "adcsmtq.h"
#include <stdint.h>
#include <math.h>

#define GNC_MAX_DETUMBLE_RATE_RAD       2*PI

// Modes for GNC Planner set mode
typedef enum {
    GNC_MODE_SAFE = 0,
    GNC_MODE_AUTO = 1,
    GNC_MODE_MANUAL = 2,
    GNC_MODE_OFF = 3
} gnc_mode_e;

typedef struct {
    uint8_t gnc_mode;                       // Determines mode of the GNC Planner
    uint8_t expected_mode;                  // Tracks expected MTQ mode to catch unexpected transitions
    uint16_t unexpected_safe_count;         // Number of unexpected transitions to SAFE
    uint16_t unexpected_detumble_count;     // Number of unexpected transitions to DETUMBLING
    uint16_t sunspin_count;                 // Number of transitions to SUNSPIN
} gnc_s;

void gnc_init(gnc_s* gnc);
void gnc_step(gnc_s* gnc, mtq_s* mtq, float* gyro_rate_rad);

#endif
