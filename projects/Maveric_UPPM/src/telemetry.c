#include "telemetry.h"
#include "systime.h"
#include "cmdpkt.h"
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

    char msg[CMD_MAX_ARGS_LEN] = {0};
    uint8_t p = sprintf(msg, "%u %Lu %u %u %u %u", 
                 bcn_num, tlm->time, tlm->lppm_rbt_cnt, tlm->lppm_rbt_cause, tlm->uppm_rbt_cnt, tlm->uppm_rbt_cause);

    switch (bcn_num) {
        case 1: {
            uint8_t i;
            for (i = 0; i < 3; i++) {
                p += sprintf(&msg[p], " ");
                p += ftoa(tlm->gyro_rate[i], &msg[p], 3, 'f');
            }
            for (i = 0; i < 4; i++) {
                p += sprintf(&msg[p], " ");
                p += ftoa(tlm->attitude[i], &msg[p], 3, 'f');
            }
            break;
        } 

        case 2: {
            break;
        }

        case 3: {
            break;
        } 
    }

    sprintf(LOGBUF, "tlm_beacon: %s", msg); log_info();
    cmd_dispatch(NODE, NODE_GS, 0, REQ, "tlm_beacon", msg);
}
