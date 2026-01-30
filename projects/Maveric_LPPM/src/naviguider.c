#include "naviguider.h"
#include "interrupts.h"
#include "circbuf.h"
#include "uart.h"
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <stdlibm.h>

#module

// HELPERS

int1 is_data_line(char* line, uint8_t len) {
    uint8_t i;
    for (i = 0; i < len; i++) {
        char c = line[i];
        if (!isdigit(c) && c != ',' && c != ' ' && c != '.' && c != '-')
            return FALSE;
    }
    return TRUE;
}

static uint8_t split_csv(char* buf, uint16_t len, char** argv, uint8_t max_args) {
    uint8_t argc = 0;
    uint16_t start = 0;
    uint16_t i;

    for (i = 0; i <= len && argc < max_args; i++) {
        char c = (i < len) ? buf[i] : '\0';
        if (c == ',' || c == '\0') {
            /* Trim leading spaces */
            while (start < i && buf[start] == ' ')
                start++;
            argv[argc++] = &buf[start];
            /* Null-terminate token */
            buf[i] = '\0';
            start = i + 1;
        }
    }

    return argc;
}

// Naviguider packet 

void nvg_pkt_init(nvg_pkt_s* pkt) {
    nvg_pkt_clear(pkt);
}

void nvg_pkt_clear(nvg_pkt_s* pkt) {
    memset(pkt, 0, sizeof(nvg_pkt_s));
}

// Naviguider

void nvg_init(nvg_s* nvg, uint8_t port) { nvg->port = port;
    memcpy(nvg->sensors, NVG_INIT_SENSOR_TABLE, sizeof(NVG_INIT_SENSOR_TABLE));
    nvg_pkt_init(&nvg->rcvpkt);

    uint8_t i;
    for (i = 0; i < NVG_NUM_SENSORS; i++) {
        nvg->sensors[NVG_SENSOR_IDS[i]].data = (float*) calloc(nvg->sensors[NVG_SENSOR_IDS[i]].len, sizeof(float));
    }
   
    nvg_reset(nvg);
    nvg_start_all_sensors(nvg);

    fprintf(COM_D, "%s[%s] nvg_init: port=%u\n", KGRN, NODE_LBL, nvg->port);
}

void nvg_destroy(nvg_s* nvg) {
    nvg_stop_all_sensors(nvg);
    nvg_power_down(nvg);

    uint8_t i;
    for (i = 0; i < NVG_NUM_SENSORS; i++) {
        if (nvg->sensors[NVG_SENSOR_IDS[i]].data != NULL)
            free(nvg->sensors[NVG_SENSOR_IDS[i]].data);
    }
    
    fprintf(COM_D, "%s[%s] nvg_destroy\n", KGRN, NODE_LBL);
}

void nvg_clear(nvg_s* nvg) {
    uint8_t i;
    for (i = 0; i < NVG_SENSOR_TABLE_LEN; i++) {
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

void nvg_rcv_parser(nvg_s* nvg, circbuf_s* irqbuf) {
    nvg_pkt_s* rcvpkt = &nvg->rcvpkt;

    uint16_t iter;
    for (iter = 0; iter < 2 * CIRCBUF_MAX_SIZE; iter++) {
        uint8_t c;
        if (!cb_pop(irqbuf, 1, &c))
            return;

        /* Accumulate characters until line end */
        if (c != '\r' && c != '\n') {
            if (rcvpkt->rl_len < MAX_BUF_LEN - 1) {
                rcvpkt->rcvline[rcvpkt->rl_len++] = c;
                rcvpkt->rcvline[rcvpkt->rl_len] = '\0';
            } else {
                nvg_pkt_clear(rcvpkt);
            }
            continue;
        }

        /* Line end detected */

        if (rcvpkt->rl_len <= 8) {
            nvg_pkt_clear(rcvpkt);
            continue;
        }

        if (!is_data_line(rcvpkt->rcvline, rcvpkt->rl_len)) {
            fprintf(COM_D, "%s[%s] nvg_rcv_parser: info: len=%u \"%s\"\n",
                    KGRN, NODE_LBL, rcvpkt->rl_len, rcvpkt->rcvline);
            nvg_pkt_clear(rcvpkt);
            continue;
        }

        /* ---- CSV parsing starts here ---- */

        // fprintf(COM_D, "%s[%s] nvg_rcv_parser: data: len=%u \"%s\"\n",
        //         KGRN, NODE_LBL, rcvpkt->rl_len, rcvpkt->rcvline);

        /* Tokenize */
        char* argv[16];
        uint8_t argc = split_csv(rcvpkt->rcvline, rcvpkt->rl_len, argv, 16);
        if (argc < 3) goto malformed;

        /* ---- Timestamp ---- */
        char* endp;
        uint32_t ts_raw = strtoul(argv[0], &endp, 10);
        if (*endp != '\0' && *endp != ',' && *endp != ' ')
            goto malformed;
        rcvpkt->ts = ts_raw / 32000.0f;

        /* ---- Sensor ID ---- */
        uint8_t id = atoi(argv[1]);
        if (id < 1 || id > 20 || nvg->sensors[id].id == 0) goto malformed;
        rcvpkt->id = id;

        /* ---- Payload length check ---- */
        uint8_t expected = nvg->sensors[id].len;
        if (argc != 2 + expected) goto malformed;

        /* ---- Payload parsing ---- */
        uint8_t i;
        for (i = 0; i < expected; i++) {
            char *arg = argv[2 + i];
            if (!arg) goto malformed;

            rcvpkt->payload[i] = strtof(arg, &endp);
            if (*endp != '\0' && *endp != ',' && *endp != ' ')
                goto malformed;
        }

        /* ---- Success ---- */
        nvg_rcv_complete(nvg);
        nvg_pkt_clear(rcvpkt);
        continue;

    malformed:
        fprintf(COM_D, "%s[%s] nvg_rcv_parser: malformed: len=%u \"%s\"\n",
                KRED, NODE_LBL, rcvpkt->rl_len, rcvpkt->rcvline);
        nvg_pkt_clear(rcvpkt);
    }
}

void nvg_rcv_complete(nvg_s* nvg) {
    nvg_sensor_s* sens = &nvg->sensors[nvg->rcvpkt.id];
    if (sens->len == 0) return;
    sens->ts = nvg->rcvpkt.ts;
    memcpy(sens->data, nvg->rcvpkt.payload, sens->len * sizeof(float));
    
    fprintf(COM_D, "%s[%s] nvg_rcv_complete: ts=%.3f id=%u [ ", KGRN, NODE_LBL, sens->ts, sens->id);
    uint8_t i;
    for (i = 0; i < sens->len-1; i++) {
        fprintf(COM_D, "%.3f ", sens->data[i]);
    }
    fprintf(COM_D, "%.3f ]\n", sens->data[sens->len-1]);
}

void nvg_get_sensor_data(nvg_s* nvg, uint8_t id, float* out) {
    nvg_sensor_s* sens = &nvg->sensors[id];
    if (sens->len == 0 || sens->data == NULL) {
        out = NULL;
        return;
    };
    memcpy(out, sens->data, sens->len * sizeof(float));
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
    for (i = 0; i < NVG_SENSOR_TABLE_LEN; i++) {
        if (nvg->sensors[i].id != 0 && nvg->sensors[i].ts - nvg->last_heartbeat > FLATLINE_TIME_MS/1000) {
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

void nvg_reset(nvg_s* nvg) {
    nvg_send_command(nvg, "X");
    nvg_send_command(nvg, "V0");
    nvg_send_command(nvg, "M1\r");
    nvg_send_command(nvg, "D1");
}

void nvg_magnetometer_mode(nvg_s* nvg) {
    nvg_stop_all_sensors(nvg);
    nvg_start_sensor(nvg, 2, 1);
    nvg_start_sensor(nvg, 14, 1);
}
