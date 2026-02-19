#include "naviguider.h"
#include "interrupts.h"
#include "circbuf.h"
#include "uart.h"
#include "common.h"
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

void nvg_init(nvg_s* nvg, uint8_t port) {
    nvg->port = port;
    memcpy(nvg->sensors, NVG_INIT_SENSOR_TABLE, sizeof(NVG_INIT_SENSOR_TABLE));
    nvg_pkt_init(&nvg->rcvpkt);

    uint8_t i;
    for (i = 0; i < NVG_NUM_SENSORS; i++) {
        nvg->sensors[NVG_SENSOR_IDS[i]].data = (float*) calloc(nvg->sensors[NVG_SENSOR_IDS[i]].len, sizeof(float));
    }
   
    nvg_reset(nvg);
    nvg_start_sensor(nvg, NVG_TEMPERATURE, 1);
    nvg_start_all_sensors(nvg);

    sprintf(LOGBUF, "nvg_init: port=%u", nvg->port); log_flush(LL_INFO);
}

void nvg_destroy(nvg_s* nvg) {
    nvg_stop_all_sensors(nvg);
    nvg_power_down(nvg);
    nvg_clear(nvg);

    uint8_t i;
    for (i = 0; i < NVG_NUM_SENSORS; i++) {
        if (nvg->sensors[NVG_SENSOR_IDS[i]].data != NULL)
            free(nvg->sensors[NVG_SENSOR_IDS[i]].data);
    }
    
    sprintf(LOGBUF, "nvg_destroy"); log_flush(LL_INFO);
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
    
    sprintf(LOGBUF, "nvg_clear"); log_flush(LL_INFO);
}

void nvg_send_command(nvg_s* nvg, char* cmd) {
    uint8_t len = strlen(cmd);
    uart_write_buf(nvg->port, cmd, len);
    sprintf(LOGBUF, "nvg_send_command: len=%u \"%s\"", len, cmd); log_flush(LL_INFO);
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
            if (rcvpkt->rl_len < NVG_MAX_LINE_LEN - 1) {
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
            sprintf(LOGBUF, "nvg_rcv_parser: info: len=%u \"%s\"", rcvpkt->rl_len, rcvpkt->rcvline); log_flush(LL_TRACE);
            nvg_pkt_clear(rcvpkt);
            continue;
        }

        /* ---- CSV parsing starts here ---- */

        // sprintf(LOGBUF, "nvg_rcv_parser: data: len=%u \"%s\"", rcvpkt->rl_len, rcvpkt->rcvline); log_flush(LL_TRACE);

        /* Tokenize */
        char* argv[16];
        uint8_t rcvline_cp[NVG_MAX_LINE_LEN] = {0};
        memcpy(rcvline_cp, rcvpkt->rcvline, sizeof(rcvline_cp));
        uint8_t argc = split_csv(rcvline_cp, rcvpkt->rl_len, argv, 16);
        if (argc < 3) goto malformed;

        // fprintf(FTDI_PORT, "%sline='%s' tokens: cnt=%u [ ", KYEL, rcvpkt->rcvline, argc);
        // uint8_t j;
        // for (j = 0; j < argc; j++)
        //     fprintf(FTDI_PORT, "%s ", argv[j]);
        // fprintf(FTDI_PORT, "]\n");

        /* ---- Timestamp ---- */
        char* endp;
        uint32_t ts_raw = strtoul(argv[0], &endp, 10);
        if (*endp != '\0' && *endp != ',' && *endp != ' ') goto malformed;
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
            if (*endp != '\0' && *endp != ',' && *endp != ' ') goto malformed;
        }

        /* ---- Success ---- */
        nvg_rcv_complete(nvg);
        nvg_pkt_clear(rcvpkt);
        continue;

    malformed:
        sprintf(LOGBUF, "nvg_rcv_parser: malformed: len=%u \"%s\"", rcvpkt->rl_len, rcvpkt->rcvline); log_flush(LL_ERROR);
        nvg_pkt_clear(rcvpkt);
    }
}

void nvg_rcv_complete(nvg_s* nvg) { 
    static char* id_to_text[] = { 
        "NULL", "ACCELEROMETER", "MAGNETOMETER_CAL", "ORIENTATION", "GYROSCOPE_CAL", 
        "NULL", "PRESSURE", "TEMPERATURE", "NULL", "ACCEL_GRAVITY", "ACCEL_LINEAR", 
        "QUAT_RV", "NULL", "NULL", "MAGNETOMETER_UNCAL", "QUAT_GAME", 
        "GYROSCOPE_UNCAL", "NULL", "NULL", "NULL", "QUAT_GEOMAG" 
    };

    nvg_sensor_s* sens = &nvg->sensors[nvg->rcvpkt.id];
    if (sens->len == 0) return;
    sens->ts = nvg->rcvpkt.ts;
    memcpy(sens->data, nvg->rcvpkt.payload, sens->len * sizeof(float));
   
    uint16_t p = 0;
    uint8_t i;
    p += sprintf(LOGBUF, "nvg_rcv_complete: ts=%.3f sens=%s [", sens->ts, id_to_text[sens->id]); 
    for (i = 0; i < sens->len-1; i++)
        p += sprintf(&LOGBUF[p], " %.3f", sens->data[i]);
    p += sprintf(&LOGBUF[p], " %.3f ]", sens->data[sens->len-1]); log_flush(LL_TRACE);
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
    int1 hb = (nvg->sensors[NVG_TEMPERATURE].ts > 0);
    if (!hb) {
        sprintf(LOGBUF, "nvg_heartbeat: flatlined. resetting..."); log_flush(LL_ERROR);
        uint8_t port = nvg->port;
        nvg_destroy(nvg);
        delay_ms(500);
        nvg_init(nvg, port);
    }
    nvg->sensors[NVG_TEMPERATURE].ts = 0;
    return hb;
}

void nvg_power_down(nvg_s* nvg) {
    nvg_send_command(nvg, "P");
}

void nvg_reset(nvg_s* nvg) {
    nvg_send_command(nvg, "X");
    nvg_send_command(nvg, "V0");
    nvg_send_command(nvg, "M1\r");
    nvg_send_command(nvg, "m0");
    nvg_send_command(nvg, "D1");
}

void nvg_magnetometer_mode(nvg_s* nvg) {
    nvg_stop_all_sensors(nvg);
    nvg_start_sensor(nvg, NVG_MAGNETOMETER_UNCAL, 1);
    nvg_start_sensor(nvg, NVG_MAGNETOMETER_CAL, 1);
}
