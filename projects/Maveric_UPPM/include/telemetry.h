#ifndef __TELEMETRY_H__
#define __TELEMETRY_H__

#include <stdint.h>

static char TLM_CALLSIGN[7] = "WQ2XIC";

typedef struct {
    char callsign[7];                   // UPPM
    uint64_t time;                      // UPPM
    uint8_t ops_stage;                  // UPPM
    uint16_t lppm_rbt_cnt;              // LPPM
    uint8_t lppm_rbt_cause;             // LPPM
    uint16_t uppm_rbt_cnt;              // UPPM
    uint8_t uppm_rbt_cause;             // UPPM
    uint8_t ertc_heartbeat;             // LPPM
    uint8_t mtq_heartbeat;              // LPPM
    uint8_t nvg_heartbeat;              // LPPM
    uint8_t eps_heartbeat;              // UPPM
    uint64_t eps_heartbeat_time;        // UPPM -- NOT TRANSMITTED
    uint8_t hn_state;                   // UPPM
    uint8_t ab_state;                   // UPPM

    uint32_t mtq_stat;                  // LPPM
    uint8_t gyro_rate_src;              // LPPM
    uint8_t mag_src;                    // LPPM
    float gyro_rate[3];                 // LPPM
    float mag[3];                       // LPPM
    float mtq_dipole[3];                // LPPM
    float temp_adcs;                    // LPPM

    uint16_t i_bus;                     // EPS
    uint16_t i_batt;                    // EPS
    uint16_t v_bus;                     // EPS
    uint16_t v_batt;                    // EPS
    uint16_t v_sys;                     // EPS
    uint16_t temp_adc;                  // EPS
    uint16_t temp_die;                  // EPS
    uint16_t eps_mode;                  // EPS

    uint8_t gnc_mode;                   // LPPM
    uint16_t unexpected_safe_count;     // LPPM 
    uint16_t unexpected_detumble_count; // LPPM 
    uint16_t sunspin_count;             // LPPM 
} tlm_s;

void tlm_init(tlm_s* tlm);
void tlm_clear(tlm_s* tlm);

// Format & transmit telemetry message packet 
void tlm_beacon(tlm_s* tlm);

#endif
