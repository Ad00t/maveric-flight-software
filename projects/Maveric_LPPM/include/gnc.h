#ifndef __GNC_H__
#define __GNC_H__

#include "adcsmtq.h"
#include <stdint.h>
#include <math.h>

#define GNC_MAX_DETUMBLE_RATE       2*PI

typedef struct {
    uint8_t expected_mode;                  // Tracks expected MTQ mode to catch unexpected transitions
    int1 auto_mode_enabled;                 // Set to false to stay in Manual mode
    uint16_t unexpected_safe_count;         // Number of unexpected transitions to SAFE
    uint16_t unexpected_detumble_count;     // Number of unexpected transitions to DETUMBLING
} gnc_s;

void gnc_init(gnc_s* gnc);
void gnc_step(gnc_s* gnc, mtq_s* mtq, float* gyro_rate_deg);

#endif
