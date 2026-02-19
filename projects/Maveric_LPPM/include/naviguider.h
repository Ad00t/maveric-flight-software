#ifndef __NAVIGUIDER_H__
#define __NAVIGUIDER_H__

#include "circbuf.h"
#include <stddef.h>
#include <stdint.h>

#define NVG_MAX_LINE_LEN        256 
#define NVG_MAX_ARG_SIZE        20
#define NVG_MAX_PAYLOAD_LEN     7
#define NVG_SENSOR_TABLE_LEN    21

// Naviguider UART FSM

typedef enum {
   NVG_FSM_TS = 0,
   NVG_FSM_ID,
   NVG_FSM_PAYLOAD,
   NVG_FSM_DONE,
   NVG_FSM_ERROR
} nvg_fsm_e;

// Naviguider sensor data reception packet

typedef struct {
    uint8_t rcvline[NVG_MAX_LINE_LEN];
    float payload[NVG_MAX_PAYLOAD_LEN];
    char curr_arg[NVG_MAX_ARG_SIZE];
    float ts;
    uint8_t id;
    uint16_t rl_len;
    nvg_fsm_e fsm;
} nvg_pkt_s;

// Naviguider pkt initialization
void nvg_pkt_init(nvg_pkt_s* pkt);

// Naviguider pkt clear all data
void nvg_pkt_clear(nvg_pkt_s* pkt);

// Naviguider sensor metadata + latest report data

typedef struct {
    uint8_t id;
    uint8_t len;
    float ts; // TODO: use absolute time
    float* data;    // Should be free'd on reinitialization
} nvg_sensor_s;

// Naviguider object

typedef struct {
    nvg_sensor_s sensors[NVG_SENSOR_TABLE_LEN];
    nvg_pkt_s rcvpkt;
    uint8_t port;
} nvg_s;

// Initialize naviguider on provided COM port
void nvg_init(nvg_s* nvg, uint8_t port);

// Free sensor heap allocations
void nvg_destroy(nvg_s* nvg);

// Clear naviguider data, do not deallocate
void nvg_clear(nvg_s* nvg);

// Send command to naviguider
void nvg_send_command(nvg_s* nvg, char* cmd); 

// Advance naviguider fsm & packet parsinvg
void nvg_rcv_parser(nvg_s* nvg, circbuf_s* irqbuf);

// Read back sensor data into naviguider object on successful full data packet reception
void nvg_rcv_complete(nvg_s* nvg);

// Return a copy of the data for a sensor
void nvg_get_sensor_data(nvg_s* nvg, uint8_t id, float* out);

// HIGH LEVEL API

/*
Starts a sensor at a given rate.

Args:
    id (uint8_t): The ID of the sensor to start (see Table 4-2).
    rate(uint16_t): The data rate for the sensor (aggregate rate <= 1200 Hz).
*/
void nvg_start_sensor(nvg_s* nvg, uint8_t id, uint16_t rate);
        
// Starts all continuous sensors a 1 Hz.
void nvg_start_all_sensors(nvg_s* nvg);
        
// Stops all continuous sensors.
void nvg_stop_all_sensors(nvg_s* nvg);
        
// COMMAND FUNCTIONS

/*
Checks last timestamp for each sensor to determine if sensor has reported in last FLATLINE_TIME_MS 

Returns
    status (bool): Whether heartbeat check passed for all sensors or not 
*/
int1 nvg_heartbeat(nvg_s* nvg);

// Sends the 'P' command to toggle sensor power
void nvg_power_down(nvg_s* nvg);

// Standard reset initialization sequence 
void nvg_reset(nvg_s* nvg);

// Starts magnetometer sensors at continuous 1 Hz.
void nvg_magnetometer_mode(nvg_s* nvg);

// Naviguider sensor IDs

#define NVG_ACCELEROMETER        1
#define NVG_MAGNETOMETER_CAL     2
#define NVG_ORIENTATION          3
#define NVG_GYROSCOPE_CAL        4
#define NVG_PRESSURE             6
#define NVG_TEMPERATURE          7
#define NVG_ACCEL_GRAVITY        9
#define NVG_ACCEL_LINEAR         10
#define NVG_QUAT_RV              11
#define NVG_QUAT_GAME            15
#define NVG_QUAT_GEOMAG          20
#define NVG_MAGNETOMETER_UNCAL   14
#define NVG_GYROSCOPE_UNCAL      16

#define NVG_NUM_SENSORS          13
static const uint8_t NVG_SENSOR_IDS[] = {
    NVG_ACCELEROMETER,        
    NVG_MAGNETOMETER_CAL,     
    NVG_ORIENTATION,     
    NVG_GYROSCOPE_CAL,       
    NVG_PRESSURE,      
    NVG_TEMPERATURE,       
    NVG_ACCEL_GRAVITY,    
    NVG_ACCEL_LINEAR,   
    NVG_QUAT_RV,
    NVG_QUAT_GAME,
    NVG_QUAT_GEOMAG,
    NVG_MAGNETOMETER_UNCAL,
    NVG_GYROSCOPE_UNCAL
};

static const nvg_sensor_s NVG_INIT_SENSOR_TABLE[] = {
    { 0, 0, 0, NULL },
    { NVG_ACCELEROMETER, 4, 0, NULL },        
    { NVG_MAGNETOMETER_CAL, 4, 0, NULL },     
    { NVG_ORIENTATION, 4, 0, NULL },     
    { NVG_GYROSCOPE_CAL, 4, 0, NULL },       
    { 0, 0, 0, NULL },
    { NVG_PRESSURE, 1, 0, NULL },      
    { NVG_TEMPERATURE, 1, 0, NULL },       
    { 0, 0, 0, NULL },
    { NVG_ACCEL_GRAVITY, 4, 0, NULL },    
    { NVG_ACCEL_LINEAR, 4, 0, NULL },   
    { NVG_QUAT_RV, 5, 0, NULL },
    { 0, 0, 0, NULL },
    { 0, 0, 0, NULL },
    { NVG_MAGNETOMETER_UNCAL, 7, 0, NULL },
    { NVG_QUAT_GAME, 5, 0, NULL },
    { NVG_GYROSCOPE_UNCAL, 7, 0, NULL },
    { 0, 0, 0, NULL },
    { 0, 0, 0, NULL },
    { 0, 0, 0, NULL },
    { NVG_QUAT_GEOMAG, 5, 0, NULL },
};

#endif
