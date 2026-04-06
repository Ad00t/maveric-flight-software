#include "telemetry.h"
#include "systime.h"
#include "mcppkt.h"
#include "logger.h"

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
    uint16_t j = sprintf(msg, "%u %Lu %u %u %u %u %u %u %u ", 
                 bcn_num, tlm->time, tlm->lppm_rbt_cnt, tlm->lppm_rbt_cause, tlm->uppm_rbt_cnt, tlm->uppm_rbt_cause,
                 tlm->ertc_heartbeat, tlm->mtq_heartbeat, tlm->nvg_heartbeat);

    switch (bcn_num) {
        case 1: {
            uint8_t i;
            j += sprintf(&msg[j], "%u %u %u %u %Lu %u %u ",
                    tlm->gnc_mode, tlm->unexpected_safe_count, tlm->unexpected_detumble_count, tlm->sunspin_count,
                    tlm->mtq_stat, tlm->gyro_rate_src, tlm->attitude_src);
            j += ftoa(tlm->adcs_temp, &msg[j], 3, 'f');
            for (i = 0; i < 3; i++) {
                j += sprintf(&msg[j], " ");
                j += ftoa(tlm->gyro_rate[i], &msg[j], 3, 'f');
            }
            for (i = 0; i < 4; i++) {
                j += sprintf(&msg[j], " ");
                j += ftoa(tlm->attitude[i], &msg[j], 3, 'f');
            }
            for (i = 0; i < 3; i++) {
                j += sprintf(&msg[j], " ");
                j += ftoa(tlm->mtq_dipole[i], &msg[j], 3, 'f');
            }
            for (i = 0; i < 3; i++) {
                j += sprintf(&msg[j], " ");
                j += ftoa(tlm->sv[i], &msg[j], 3, 'f');
            }
            break;
        } 

        case 2: {
            break;
        }
    }

    sprintf(LOGBUF, "tlm_beacon: %s", msg); log_info();
    mcp_dispatch(NODE, NODE_GS, 0, TLM, "tlm_beacon", msg);
}
