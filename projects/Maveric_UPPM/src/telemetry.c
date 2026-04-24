#include "telemetry.h"
#include "systime.h"
#include "mcppkt.h"
#include "logger.h"
#include "common.h"

#module

void tlm_init(tlm_s* tlm) {
    tlm_clear(tlm);
}

void tlm_clear(tlm_s* tlm) {
    memset(tlm, 0, sizeof(tlm_s));
}

void tlm_beacon(tlm_s* tlm) {
    tlm->time = systime_epoch_ms();
    memcpy(tlm->callsign, TLM_CALLSIGN, sizeof(TLM_CALLSIGN));

    uint8_t msg[MCP_MAX_ARGS_LEN] = {0};
    uint8_t l = 0;

    memcpy(&msg[l], tlm->callsign, sizeof(tlm->callsign)); l += sizeof(tlm->callsign); 
    memcpy(&msg[l], &tlm->time, 8); l += 8; 
    memcpy(&msg[l], &tlm->ops_stage, 1); l += 1; 
    memcpy(&msg[l], &tlm->lppm_rbt_cnt, 2); l += 2; 
    memcpy(&msg[l], &tlm->lppm_rbt_cause, 1); l += 1; 
    memcpy(&msg[l], &tlm->uppm_rbt_cnt, 2); l += 2; 
    memcpy(&msg[l], &tlm->uppm_rbt_cause, 1); l += 1; 
    memcpy(&msg[l], &tlm->ertc_heartbeat, 1); l += 1; 
    memcpy(&msg[l], &tlm->mtq_heartbeat, 1); l += 1; 
    memcpy(&msg[l], &tlm->nvg_heartbeat, 1); l += 1; 
    memcpy(&msg[l], &tlm->eps_heartbeat, 1); l += 1; 
    memcpy(&msg[l], &tlm->hn_state, 1); l += 1; 
    memcpy(&msg[l], &tlm->ab_state, 1); l += 1; 

    memcpy(&msg[l], &tlm->mtq_stat, 4); l += 4; 
    memcpy(&msg[l], &tlm->gyro_rate_src, 1); l += 1; 
    memcpy(&msg[l], &tlm->mag_src, 1); l += 1; 
    memcpy(&msg[l], tlm->gyro_rate, sizeof(tlm->gyro_rate)); l += sizeof(tlm->gyro_rate); 
    memcpy(&msg[l], tlm->mag, sizeof(tlm->mag)); l += sizeof(tlm->mag); 
    memcpy(&msg[l], tlm->mtq_dipole, sizeof(tlm->mtq_dipole)); l += sizeof(tlm->mtq_dipole); 
    memcpy(&msg[l], &tlm->temp_adcs, 4); l += 4; 

    memcpy(&msg[l], &tlm->i_bus, 2); l += 2; 
    memcpy(&msg[l], &tlm->i_batt, 2); l += 2; 
    memcpy(&msg[l], &tlm->v_bus, 2); l += 2; 
    memcpy(&msg[l], &tlm->v_batt, 2); l += 2; 
    memcpy(&msg[l], &tlm->v_sys, 2); l += 2; 
    memcpy(&msg[l], &tlm->temp_adc, 2); l += 2; 
    memcpy(&msg[l], &tlm->temp_die, 2); l += 2; 
    memcpy(&msg[l], &tlm->eps_mode, 2); l += 2; 

    memcpy(&msg[l], &tlm->gnc_mode, 1); l += 1; 
    memcpy(&msg[l], &tlm->unexpected_safe_count, 2); l += 2; 
    memcpy(&msg[l], &tlm->unexpected_detumble_count, 2); l += 2; 
    memcpy(&msg[l], &tlm->sunspin_count, 2); l += 2; 

    sprintf(LOGBUF, "tlm_beacon: len=%u ta=", l); log_info();
    mcp_dispatch(NODE, NODE_GS, 0, TLM, "tlm_beacon", msg, l);
}
