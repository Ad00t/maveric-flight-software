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


// Format a telemetry message packet for a specified beacon for transmission. Only creates the message buffer, not command or frame.
void tlm_beacon(tlm_s* tlm, uint8_t bcn_num) {
    tlm->time = systime_epoch_ms();

    char msg[MCP_MAX_ARGS_LEN] = {0};
    char timebuf[32] = {0};
    sprintf(timebuf, "%Lu", tlm->time);
    uint16_t j = sprintf(msg, "%u %s %u %u %u %u %u %u %u %u %u %u ", 
                 bcn_num, timebuf, tlm->ops_stage, 
                 tlm->lppm_rbt_cnt, tlm->lppm_rbt_cause, tlm->uppm_rbt_cnt, tlm->uppm_rbt_cause,
                 tlm->ertc_heartbeat, tlm->mtq_heartbeat, tlm->nvg_heartbeat, tlm->hn_state, tlm->ab_state);

    switch (bcn_num) {
        case 1: {
            uint8_t i;
            j += sprintf(&msg[j], "%Lu %u %u %u %u %u %u ",
                    tlm->mtq_stat, tlm->gnc_mode, tlm->unexpected_safe_count, tlm->unexpected_detumble_count, 
                    tlm->sunspin_count, tlm->gyro_rate_src, tlm->mag_src);
            for (i = 0; i < 3; i++) {
                j += ftoa(tlm->gyro_rate[i], &msg[j], 3, 'f');
                j += sprintf(&msg[j], " ");
            }
            for (i = 0; i < 3; i++) {
                j += ftoa(tlm->mag[i], &msg[j], 3, 'f');
                j += sprintf(&msg[j], " ");
            }
            for (i = 0; i < 3; i++) {
                j += ftoa(tlm->mtq_dipole[i], &msg[j], 3, 'f');
                j += sprintf(&msg[j], " ");
            }
            j += ftoa(tlm->adcs_temp, &msg[j], 3, 'f');
            break;
        } 

        case 2: {
            break;
        }
    }

    sprintf(LOGBUF, "tlm_beacon: %s", msg); log_info();
    mcp_dispatch(NODE, NODE_GS, 0, TLM, "tlm_beacon", msg);
}
