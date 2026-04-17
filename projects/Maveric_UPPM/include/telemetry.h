#ifndef __TELEMETRY_H__
#define __TELEMETRY_H__

#include <stdint.h>

typedef struct {
    // Generic
    uint64_t time;                      // UPPM
    uint8_t ops_stage;                  // UPPM
    uint16_t lppm_rbt_cnt;              // LPPM
    uint8_t lppm_rbt_cause;             // LPPM
    uint16_t uppm_rbt_cnt;              // UPPM
    uint8_t uppm_rbt_cause;             // UPPM
    uint8_t ertc_heartbeat;             // LPPM
    uint8_t mtq_heartbeat;              // LPPM
    uint8_t nvg_heartbeat;              // LPPM
    uint8_t hn_state;                   // UPPM
    uint8_t ab_state;                   // UPPM
    // Beacon 1
    uint32_t mtq_stat;                  // LPPM
    uint8_t gnc_mode;                   // LPPM
    uint16_t unexpected_safe_count;     // LPPM 
    uint16_t unexpected_detumble_count; // LPPM 
    uint16_t sunspin_count;             // LPPM 
    uint8_t gyro_rate_src;              // LPPM
    uint8_t mag_src;                    // LPPM
    float gyro_rate[3];                 // LPPM
    float mag[3];                       // LPPM
    float mtq_dipole[3];                // LPPM
    float adcs_temp;                    // LPPM
    // Beacon 2                          
    uint8_t eps_state;                  // EPS
    uint8_t eps_pwr_out[2];             // EPS
    float die_temp;                     // EPS
    float batt_temp;                    // EPS
    float batt_voltage[4];              // EPS
    float batt_current[4];              // EPS
    float solarcell_voltage[5];         // EPS
    float solarcell_current[10];        // EPS
} tlm_s;

void tlm_init(tlm_s* tlm);
void tlm_clear(tlm_s* tlm);

// Format a telemetry message packet for a specified beacon for transmission. Only creates the message buffer, not command or frame.
void tlm_beacon(tlm_s* tlm, uint8_t bcn_num);

#endif
