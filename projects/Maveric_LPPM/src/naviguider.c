#include "naviguider.h"
#include "interrupts.h"
#include "ringbuf.h"
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

status_e nvg_init(nvg_s* nvg, uint8_t port) {
    nvg->port = port;
    nvg->is_init = TRUE;
    memcpy(nvg->sensors, NVG_INIT_SENSOR_TABLE, sizeof(NVG_INIT_SENSOR_TABLE));
    nvg_pkt_init(&nvg->rcvpkt);

    uint8_t i;
    for (i = 0; i < NVG_NUM_SENSORS; i++) {
        nvg->sensors[NVG_SENSOR_IDS[i]].data = (float*) calloc(nvg->sensors[NVG_SENSOR_IDS[i]].len, sizeof(float));
    }
   
    status_e s1 = nvg_reset(nvg);
    status_e s2 = nvg_set_sensor(nvg, NVG_TEMPERATURE, 1);
    status_e s3 = nvg_start_all_sensors(nvg);

    sprintf(LOGBUF, "nvg_init: port=%u", nvg->port); log_info();
    return (s1 == SUCCESS && s2 == SUCCESS && s3 == SUCCESS) ? SUCCESS : FAILURE;
}

void nvg_destroy(nvg_s* nvg) {
    if (!nvg->is_init) return;
    nvg_stop_all_sensors(nvg);
    nvg_power(nvg);
    nvg_clear(nvg);
    uint8_t i;
    for (i = 0; i < NVG_NUM_SENSORS; i++) {
        if (nvg->sensors[NVG_SENSOR_IDS[i]].data != NULL)
            free(nvg->sensors[NVG_SENSOR_IDS[i]].data);
    }
    nvg->is_init = FALSE; 
    sprintf(LOGBUF, "nvg_destroy"); log_info();
}

void nvg_clear(nvg_s* nvg) {
    if (!nvg->is_init) return;
    uint8_t i;
    for (i = 0; i < NVG_SENSOR_TABLE_LEN; i++) {
        nvg_sensor_s* sens = &nvg->sensors[i];
        if (sens->data != NULL)
            memset(sens->data, 0, sens->len * sizeof(float)); 
        sens->ts = 0;
    }
    nvg_pkt_clear(&nvg->rcvpkt);
    sprintf(LOGBUF, "nvg_clear"); log_info();
}

status_e nvg_send_cmd(nvg_s* nvg, char* cmd) {
    if (!nvg->is_init) return FAILURE;
    uint8_t len = strlen(cmd);
    uart_write_buf(nvg->port, cmd, len);
    sprintf(LOGBUF, "nvg_send_cmd: len=%u \"%s\"", len, cmd); log_debug();
    delay_ms(10);
    return SUCCESS;
} 

void nvg_parse_stream(nvg_s* nvg, ringbuf_s* irqbuf) {
    if (!nvg->is_init) return;
    nvg_pkt_s* pkt = &nvg->rcvpkt;

    uint16_t iter;
    for (iter = 0; iter < 2*RINGBUF_MAX_CAPACITY; iter++) {
        uint8_t c;
        if (!rb_pop(irqbuf, 1, &c))
            return;

        // Accumulate characters until line end
        if (c != '\r' && c != '\n') {
            if (pkt->buf_len >= NVG_MAX_LINE_LEN - 1) {
                goto malformed;
            }
            pkt->buf[pkt->buf_len++] = c;
            pkt->buf[pkt->buf_len] = '\0';
            continue;
        }

        // Line end detected

        if (!is_data_line(pkt->buf, pkt->buf_len)) {
            sprintf(LOGBUF, "nvg_parse_stream: info: len=%u \"%s\"", pkt->buf_len, pkt->buf); log_trace();
            nvg_pkt_clear(pkt);
            continue;
        }

        // CSV data line detected

        if (pkt->buf_len <= 8) {
            nvg_pkt_clear(pkt);
            continue;
        }

        // sprintf(LOGBUF, "nvg_parse_stream: data: len=%u \"%s\"", pkt->buf_len, pkt->buf); log_trace();

        // Tokenize
        char* argv[16];
        uint8_t buf_cp[NVG_MAX_LINE_LEN] = {0};
        memcpy(buf_cp, pkt->buf, sizeof(buf_cp));
        uint8_t argc = split_csv(buf_cp, pkt->buf_len, argv, 16);
        if (argc < 3) goto malformed;

        // Timestamp
        char* endp;
        uint32_t ts_raw = strtoul(argv[0], &endp, 10);
        if (*endp != '\0' && *endp != ',' && *endp != ' ') goto malformed;
        pkt->ts = ts_raw / 32000.0f;

        // Sensor ID
        uint8_t id = atoi(argv[1]);
        if (id < 1 || id > 20 || nvg->sensors[id].id == 0) goto malformed;
        pkt->id = id;

        // Payload length check 
        uint8_t expected = nvg->sensors[id].len;
        if (argc != 2 + expected) goto malformed;

        // Payload parsing 
        uint8_t i;
        for (i = 0; i < expected; i++) {
            char *arg = argv[2 + i];
            if (!arg) goto malformed;
            pkt->payload[i] = strtof(arg, &endp);
            if (*endp != '\0' && *endp != ',' && *endp != ' ') goto malformed;
        }

        // Success 
        nvg_process_sensor_data(nvg);
        nvg_pkt_clear(pkt);
        continue;

    malformed:
        sprintf(LOGBUF, "nvg_parse_stream: malformed: len=%u", pkt->buf_len); log_error();
        nvg_pkt_clear(pkt);
    }
}

void nvg_process_sensor_data(nvg_s* nvg) { 
    if (!nvg->is_init) return;

    nvg_sensor_s* sens = &nvg->sensors[nvg->rcvpkt.id];
    if (sens->len == 0) return;
    sens->ts = nvg->rcvpkt.ts;
    memcpy(sens->data, nvg->rcvpkt.payload, sens->len * sizeof(float));
   
    uint8_t p = sprintf(LOGBUF, "nvg_rcv_complete: ts=");
    p += ftoa(sens->ts, &LOGBUF[p], 3, 'f');
    p += sprintf(&LOGBUF[p], " sens=%s [", NVG_ID_TO_TEXT[sens->id]); 
    uint8_t i;
    for (i = 0; i < sens->len; i++) {
        p += sprintf(&LOGBUF[p], " ");
        p += ftoa(sens->data[i], &LOGBUF[p], 3, 'f');
    }
    p += sprintf(&LOGBUF[p], " ]"); log_trace();
}

status_e nvg_get_sensor_data(nvg_s* nvg, uint8_t id, float* out) {
    if (!nvg->is_init) return FAILURE;
    nvg_sensor_s* sens = &nvg->sensors[id];
    if (sens->len == 0 || sens->data == NULL)
        return FAILURE;
    memcpy(out, sens->data, sens->len * sizeof(float));
    return SUCCESS;
}

// HIGH LEVEL API

status_e nvg_set_sensor(nvg_s* nvg, uint8_t id, uint16_t rate) {
    if (!nvg->is_init) return FAILURE;
    uint8_t buf[10];
    sprintf(buf, "s %u,%u\r", id, rate);
    return nvg_send_cmd(nvg, buf); 
}

status_e nvg_start_all_sensors(nvg_s* nvg) {
    if (!nvg->is_init) return FAILURE;
    status_e s = SUCCESS;
    uint8_t i;
    for (i = 0; i < NVG_NUM_SENSORS; i++) {
        if (nvg_set_sensor(nvg, NVG_SENSOR_IDS[i], 1) == FAILURE)
            s = FAILURE;
    } 
    return s;
}
        
status_e nvg_stop_all_sensors(nvg_s* nvg) {
    if (!nvg->is_init) return FAILURE;
    status_e s = SUCCESS;
    uint8_t i;
    for (i = 0; i < NVG_NUM_SENSORS; i++) {
        if (nvg_set_sensor(nvg, NVG_SENSOR_IDS[i], 0) == FAILURE)
            s = FAILURE;
    } 
    return s;
}
        
void nvg_check_heartbeat(nvg_s* nvg) {
    if (!nvg->is_init) {
        nvg->heartbeat = FAILURE;
        return;
    }
    status_e hb = (nvg->sensors[NVG_TEMPERATURE].ts > 0) ? SUCCESS : FAILURE;

    // if (hb == FAILURE) {
    //     sprintf(LOGBUF, "nvg_heartbeat: flatlined. resetting..."); log_error();
    //     uint8_t port = nvg->port;
    //     nvg_destroy(nvg);
    //     delay_ms(500);
    //     nvg_init(nvg, port);
    // }

    nvg->sensors[NVG_TEMPERATURE].ts = 0;
    nvg->heartbeat = hb;
}

status_e nvg_power(nvg_s* nvg) {
    if (!nvg->is_init) return FAILURE;
    return nvg_send_cmd(nvg, "P");
}

status_e nvg_reset(nvg_s* nvg) {
    if (!nvg->is_init) return FAILURE;
    status_e s1 = nvg_send_cmd(nvg, "X");
    status_e s2 = nvg_send_cmd(nvg, "V0");
    status_e s3 = nvg_send_cmd(nvg, "M1\r");
    status_e s4 = nvg_send_cmd(nvg, "m0");
    status_e s5 = nvg_send_cmd(nvg, "D1");
    return (s1 == SUCCESS && s2 == SUCCESS && s3 == SUCCESS && s4 == SUCCESS && s5 == SUCCESS) ? SUCCESS : FAILURE;
}

status_e nvg_magnetometer_mode(nvg_s* nvg) {
    if (!nvg->is_init) return FAILURE;
    status_e s1 = nvg_stop_all_sensors(nvg);
    status_e s2 = nvg_set_sensor(nvg, NVG_MAGNETOMETER_UNCAL, 1);
    status_e s3 = nvg_set_sensor(nvg, NVG_MAGNETOMETER_CAL, 1);
    return (s1 == SUCCESS && s2 == SUCCESS && s3 == SUCCESS) ? SUCCESS : FAILURE;
}
