#ifndef __TELEMETRY_H__
#define __TELEMETRY_H__

#include <stdint.h>

typedef struct {
    // Generic
    uint64_t time;
    uint16_t lppm_rbt_cnt;
    uint8_t lppm_rbt_cause;
    uint16_t uppm_rbt_cnt;
    uint8_t uppm_rbt_cause;
    uint32_t mode;
    float quat_attitude[4];
    // Beacon 1
    float gyro_rate[3];
    float mtq_sv[3];
    float ss[3];
    float mag[3];
    uint64_t gnc_state;
    uint8_t mtq_mode;
    float mtq_dipole[3];
    // Beacon 2
    float model_orbit[7];
    float model_sun[3];
    float model_geomag[3];
    float gyro_temp[3];
    int1 sp_check_failed_out;
    // Beacon 3
    float quat_error[4];
    float rate_error[4];
    int1 global_en;
    float mtq_act_error[3];
    float sl_error;
    // Beacon 4
    float batt_voltage[4];
    float batt_current[4];
    float solarcell_voltage[5];
    float solarcell_current[10];
    float batt_sens[6];
    float eps_temp[8];
    float adcs_temp;
    uint8_t eps_state;
    // Beacon 5
    uint8_t ax100_flash_status[3];
    float uppm_temp[4];
    uint8_t payload_status[2];
    float ax100_temp[2];
    uint64_t next_downlink;
    uint8_t dep_mag_status;
    // Beacon 6
    uint16_t ab_seq; 
    // Beacon 7
    uint16_t hn_seq;
} tlm_s;

void tlm_init(tlm_s* tlm);
void tlm_clear(tlm_s* tlm);

// Format a telemetry message packet for a specified beacon for transmission. Only creates the message buffer, not command or frame.
void tlm_beacon(tlm_s* tlm, uint8_t bcn_num);

#endif
