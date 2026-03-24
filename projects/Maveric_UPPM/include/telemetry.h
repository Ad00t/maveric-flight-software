#ifndef __TELEMETRY_H__
#define __TELEMETRY_H__

#include <stdint.h>

typedef struct {
    // Generic
    uint64_t time;                      // UPPM
    uint16_t lppm_rbt_cnt;              // LPPM
    uint8_t lppm_rbt_cause;             // LPPM
    uint16_t uppm_rbt_cnt;              // UPPM
    uint8_t uppm_rbt_cause;             // UPPM
    uint8_t ertc_heartbeat;             // LPPM
    uint8_t mtq_heartbeat;              // LPPM
    uint8_t nvg_heartbeat;              // LPPM
    uint8_t mtq_stat;                   // LPPM
    // Beacon 1
    float gyro_rate[3];                 // LPPM
    float attitude[4];                  // LPPM
    float sv[3];                        // LPPM
    float ss[3];                        // LPPM
    float mag[3];                       // LPPM
    float mtq_dipole[3];                // LPPM
    float quat_error[4];                // LPPM
    float rate_error[4];                // LPPM
    float mtq_act_error[3];             // LPPM
    // Beacon 2
    float batt_voltage[4];              // EPS
    float batt_current[4];              // EPS
    float solarcell_voltage[5];         // EPS
    float solarcell_current[10];        // EPS
    float eps_temp[8];                  // EPS
    float adcs_temp;                    // LPPM
    uint8_t eps_state;                  // EPS
    // Beacon 3
    uint8_t ax100_flash_status[3];      // UPPM
    float uppm_temp[4];                 // UPPM
    uint8_t holonav_status;             // HOLONAV 
    uint8_t astroboard_status;          // ASTROBOARD 
    float ax100_temp[2];                // UPPM
    uint64_t next_downlink;             // UPPM             
    uint8_t dep_mag_status[2];          // UPPM
    uint16_t ab_seq;                    // ASTROBOARD
    uint16_t hn_seq;                    // HOLONAV
} tlm_s;

void tlm_init(tlm_s* tlm);
void tlm_clear(tlm_s* tlm);

// Format a telemetry message packet for a specified beacon for transmission. Only creates the message buffer, not command or frame.
void tlm_beacon(tlm_s* tlm, uint8_t bcn_num);

#endif
