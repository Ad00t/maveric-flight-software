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

    uint16_t p = 0;
    char bcn_buf[CMD_MAX_ARGS_LEN] = {0};
    p += sprintf(bcn_buf, "%u %Lu %u %u ", bcn_num, tlm->time, tlm->lppm_rbt_cnt, tlm->lppm_rbt_cause);

    switch (bcn_num) {
        case 1: break;

        case 2: break;

        case 3: break;

        case 4: break;

        case 5: break;

        case 6: break;

        case 7: break;

    }

    sprintf(LOGBUF, "tlm_beacon: %s", bcn_buf); log_info();
    // cmd_dispatch(NODE, NODE_GS, 0, "tlm_beacon", bcn_buf);
}
