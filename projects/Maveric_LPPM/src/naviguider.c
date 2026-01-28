#include "naviguider.h"
#include "interrupts.h"
#include "circbuf.h"
#include "uart.h"
#include <string.h>
#include <stdint.h>
#include <stdlibm.h>

#module

// Naviguider packet 

void nvg_pkt_init(nvg_pkt_s* pkt) {
    nvg_pkt_clear(pkt);
}

void nvg_pkt_clear(nvg_pkt_s* pkt) {
    memset(pkt, 0, sizeof(nvg_pkt_s));
}

// Naviguider

void nvg_init(nvg_s* nvg, uint8_t port) {
    nvg->port = port;
    memcpy(nvg->sensors, NVG_INIT_SENSOR_TABLE, sizeof(NVG_INIT_SENSOR_TABLE));
    nvg_pkt_init(&nvg->rcvpkt);

    uint8_t i;
    for (i = 0; i < NVG_NUM_SENSORS; i++) {
        nvg->sensors[i].data = calloc(nvg->sensors[i].len, sizeof(float));
    }
   
    // nvg_restart(nvg); 
    // nvg_set_mounting_option(nvg);
    // nvg_start_all_sensors(nvg);
    // nvg_start_sensor(nvg, NVG_ACCELEROMETER, 1);
    nvg_send_command(nvg, "X");
    nvg_send_command(nvg, "V0");
    nvg_send_command(nvg, "M2\r");
    nvg_send_command(nvg, "s 1,1\r");
    fprintf(COM_D, "%s[%s] nvg_init: port=%u\n", KGRN, NODE_LBL, nvg->port);
}

void nvg_destroy(nvg_s* nvg) {
    nvg_stop_all_sensors(nvg);
    nvg_power_down(nvg);

    uint8_t i;
    for (i = 0; i < NVG_NUM_SENSORS; i++) {
        if (nvg->sensors[i].data != NULL)
            free(nvg->sensors[i].data);
    }
    
    fprintf(COM_D, "%s[%s] nvg_destroy\n", KGRN, NODE_LBL);
}

void nvg_clear(nvg_s* nvg) {
    uint8_t i;
    for (i = 0; i < NVG_NUM_SENSORS; i++) {
        nvg_sensor_s* sens = &nvg->sensors[i];
        if (sens->data != NULL)
            memset(sens->data, 0, sens->len * sizeof(float)); 
        sens->ts = 0;
    }
    nvg_pkt_clear(&nvg->rcvpkt);
    
    fprintf(COM_D, "%s[%s] nvg_clear\n", KGRN, NODE_LBL);
}

void nvg_send_command(nvg_s* nvg, char* cmd) {
    uint8_t len = strlen(cmd);
    uart_write_buf(nvg->port, cmd, len);
    fprintf(COM_D, "%s[%s] nvg_send_command: len=%u \"%s\"\n", KGRN, NODE_LBL, len, cmd);
    delay_ms(100);
} 

void nvg_rcv_fsm(nvg_s* nvg, circbuf_s* irqbuf) {
    nvg_pkt_s* rcvpkt = &nvg->rcvpkt;
    uint16_t iter = 0;
    fprintf(COM_D, "%sNVG_FSM: r=%u w=%u\n", KGRN, irqbuf->r, irqbuf->w);
    while (iter < 5*CIRCBUF_MAX_SIZE) {
        uint8_t b;
        if (!cb_pop(irqbuf, 1, &b)) return;
        fprintf(COM_D, "%s%c", KGRN, b);

        switch (rcvpkt->fsm) {
            case NVG_FSM_TS:
                if (b == '\n' || b == '\r' || b == ' ') break;
                if (isdigit(b)) {
                    // Check if we are receiving too many characters in one arg
                    if (rcvpkt->i_arg >= MAX_NVG_ARG_SIZE) {
                        rcvpkt->fsm = NVG_FSM_ERROR;
                        break;
                    }
                    rcvpkt->curr_arg[rcvpkt->i_arg++] = b;
                } else if (b == ',') {
                    rcvpkt->fsm = NVG_FSM_ID;
                    rcvpkt->ts = atof(rcvpkt->curr_arg) / 32000.0;
                    memset(rcvpkt->curr_arg, 0, sizeof(rcvpkt->curr_arg));
                    rcvpkt->i_arg = 0;
                } else {
                    if (rcvpkt->i_arg > 0)
                        rcvpkt->fsm = NVG_FSM_ERROR;
                }
                break;

            case NVG_FSM_ID:
                if (b == ' ') break;
                if (isdigit(b)) {
                    if (rcvpkt->i_arg >= MAX_NVG_ARG_SIZE) {
                        rcvpkt->fsm = NVG_FSM_ERROR;
                        break;
                    }
                    rcvpkt->curr_arg[rcvpkt->i_arg++] = b;
                } else if (b == ',') {
                    rcvpkt->fsm = NVG_FSM_PAYLOAD;
                    rcvpkt->id = atoi(rcvpkt->curr_arg);
                    // Check if id is valid
                    if (rcvpkt->id < 1 || rcvpkt->id > 20 || nvg->sensors[rcvpkt->id].id == 0) {
                        rcvpkt->fsm = NVG_FSM_ERROR;
                        break;
                    }
                    memset(rcvpkt->curr_arg, 0, sizeof(rcvpkt->curr_arg));
                    rcvpkt->i_arg = 0;
                } else {
                    rcvpkt->fsm = NVG_FSM_ERROR;
                }                
                break;

            case NVG_FSM_PAYLOAD:
                if (b == ' ') break;
                if (isdigit(b)) {
                    if (rcvpkt->i_arg >= MAX_NVG_ARG_SIZE) {
                        rcvpkt->fsm = NVG_FSM_ERROR;
                        break;
                    }
                    rcvpkt->curr_arg[rcvpkt->i_arg++] = b;
                } else if (b == ',') {
                    // Check if we are receiving more payloads than expected
                    if (rcvpkt->i_payload >= nvg->sensors[rcvpkt->id].len) {
                        rcvpkt->fsm = NVG_FSM_ERROR;
                        break;
                    }
                    rcvpkt->payload[rcvpkt->i_payload++] = atof(rcvpkt->curr_arg);
                    memset(rcvpkt->curr_arg, 0, sizeof(rcvpkt->curr_arg));
                    rcvpkt->i_arg = 0;
                } else if (b == '\n') {
                    // Check if we've received too few payload args
                    if (rcvpkt->i_payload != nvg->sensors[rcvpkt->id].len) {
                        rcvpkt->fsm = NVG_FSM_ERROR;
                        break;
                    }
                    rcvpkt->fsm = NVG_FSM_DONE;
                } else {
                    rcvpkt->fsm = NVG_FSM_ERROR;
                }
                break;
        } 

        // Could wait till next superloop iteration or just handle end states immediately (currently doing the latter)
        switch (nvg->fsm) {
            case NVG_FSM_DONE:
                nvg_rcv_complete(nvg);
                nvg_pkt_clear(rcvpkt);
                break;
            case NVG_FSM_ERROR:
                fprintf(COM_D, "%s[%s] nvg_rcv_fsm: malformed packet\n", KRED, NODE_LBL);
                nvg_pkt_clear(rcvpkt);
                break;
        }

        iter++;
    }
}

void nvg_rcv_complete(nvg_s* nvg) {
    nvg_sensor_s* sens = &nvg->sensors[nvg->rcvpkt.id];
    sens->ts = nvg->rcvpkt->ts;
    memcpy(sens->data, nvg->rcvpkt->payload, sens->len * sizeof(float));
    
    fprintf(COM_D, "%s[%s] nvg_rcv_complete: %.3f, %u, ", KGRN, NODE_LBL, sens->ts, sens->id);
    uint8_t i;
    for (i = 0; i < sens->len-1; i++) {
        fprintf(COM_D, "%.3f, ", sens->data[i]);
    }
    fprintf(COM_D, "%.3f\n", sens->data[sens->len-1]);
}

// HIGH LEVEL API

void nvg_start_sensor(nvg_s* nvg, uint8_t id, uint16_t rate) {
    uint8_t buf[10];
    sprintf(buf, "s %u,%u\r", id, rate);
    nvg_send_command(nvg, buf); 
    delay_ms(10);
}

void nvg_start_all_sensors(nvg_s* nvg) {
    uint8_t i;
    for (i = 0; i < NVG_NUM_SENSORS; i++) {
        nvg_start_sensor(nvg, NVG_SENSOR_IDS[i], 1);
    } 
}
        
void nvg_stop_all_sensors(nvg_s* nvg) {
    uint8_t i;
    for (i = 0; i < NVG_NUM_SENSORS; i++) {
        nvg_start_sensor(nvg, NVG_SENSOR_IDS[i], 0);
    } 
}
        
// COMMAND FUNCTIONS

int1 nvg_heartbeat(nvg_s* nvg) {
    uint8_t i;
    int1 heartbeat = TRUE;
    for (i = 0; i < NVG_NUM_SENSORS; i++) {
        if (nvg->sensors[i].ts - nvg->last_heartbeat > FLATLINE_TIME_MS/1000) {
            heartbeat = FALSE;
            break;
        }
    } 
    nvg->last_heartbeat += 1; // TODO: rework heartbeat timing to use rtcc
    return heartbeat;
}

void nvg_power_down(nvg_s* nvg) {
    nvg_send_command(nvg, "P");
}

void nvg_restart(nvg_s* nvg) {
    nvg_send_command(nvg, "X");
}

void nvg_set_mounting_option(nvg_s* nvg) {
    nvg_send_command(nvg, "M2\r");
}

void nvg_magnetometer_mode(nvg_s* nvg) {
    nvg_stop_all_sensors(nvg);
    nvg_start_sensor(nvg, 2, 1);
    nvg_start_sensor(nvg, 14, 1);
}
