#include "telemetry.h"
#include "systime.h"

#module

void tlm_init(tlm_s* tlm) {
    tlm_clear(tlm);
}

void tlm_clear(tlm_s* tlm) {
    memset(tlm, 0, sizeof(tlm_s));
}

// Format a telemetry message packet for a specified beacon for transmission. Only creates the message buffer, not command or frame.
void tlm_beacon_1(tlm_s* tlm, uint8_t* out) {
    tlm->time = systime_epoch_ms();

}

void tlm_beacon_2(tlm_s* tlm, uint8_t* out) {
    tlm->time = systime_epoch_ms();

}

void tlm_beacon_3(tlm_s* tlm, uint8_t* out) {
    tlm->time = systime_epoch_ms();

}

void tlm_beacon_4(tlm_s* tlm, uint8_t* out) {
    tlm->time = systime_epoch_ms();

}

void tlm_beacon_5(tlm_s* tlm, uint8_t* out) {
    tlm->time = systime_epoch_ms();

}

void tlm_beacon_6(tlm_s* tlm, uint8_t* out) {
    tlm->time = systime_epoch_ms();

}

void tlm_beacon_7(tlm_s* tlm, uint8_t* out) {
    tlm->time = systime_epoch_ms();

}
