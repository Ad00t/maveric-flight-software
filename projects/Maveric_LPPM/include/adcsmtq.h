#ifndef __ADCSMTQ_H__
#define __ADCSMTQ_H__

#include "interrupts.h"
#include "ringbuf.h"
#include <stddef.h>
#include <stdint.h>
#include <time.h>

#define MTQ_MAX_PKT_LEN         RINGBUF_MAX_CAPACITY
#define MTQ_HEADER_LEN          4
#define MTQ_CSUM_LEN            1
#define MTQ_MAX_PAYLOAD_LEN     MTQ_MAX_PKT_LEN - MTQ_HEADER_LEN - MTQ_CSUM_LEN
#define MTQ_HEAD_READ           0xC9
#define MTQ_HEAD_WRITE          0xC8
#define MTQ_REG_TABLE_LEN       23      // 116
#define MTQ_MAP_COUNT           3       // 3
#define MTQ_MAX_IDX_COUNT       256     // 256
#define MTQ_PAGE_SIZE           5

// Register type

typedef enum {
    T_UINT8,
    T_INT8,
    T_UINT16,
    T_INT16,
    T_FLOAT,
    T_CHAR
} mtq_reg_type_e;

static uint8_t MTQ_REG_TYPE_SIZES[] = { 1, 1, 2, 2, 4, 1 };

// Packet parsing FSM states

typedef enum {
    MTQ_FSM_HEAD = 0,
    MTQ_FSM_IDX,
    MTQ_FSM_CNT,
    MTQ_FSM_MIDXERR,
    MTQ_FSM_PAYLOAD,
    MTQ_FSM_CSUM,
    MTQ_FSM_DONE,
    MTQ_FSM_ERROR,
} mtq_fsm_e;

// MTQ packet parsing struct

typedef struct {
    uint8_t data[MTQ_MAX_PKT_LEN];
    // Packet parsing metadata
    mtq_fsm_e fsm;
    uint8_t i_payload;
    // Packet data
    uint8_t head;
    uint8_t idx;
    uint8_t cnt;
    uint8_t midx;
    uint8_t err;
    uint8_t csum;
} mtq_pkt_s;

// Initialize mtq pkt 
void mtq_pkt_init(mtq_pkt_s* pkt);

// Clear this adcsamtq pkt 
void mtq_pkt_clear(mtq_pkt_s* pkt);

// Verify the packet's checksum is valid
int1 mtq_pkt_verify_csum(mtq_pkt_s* pkt);

// MTQ register struct

typedef struct {
    uint8_t idx;
    uint8_t cnt;
    uint8_t midx;
    mtq_reg_type_e type;
    void* value;                // Should be free'd on reinitialization 
    uint8_t value_len;
} mtq_reg_s;

// MTQ global manager

typedef struct {
    mtq_reg_s reg_table[MTQ_REG_TABLE_LEN]; 
    mtq_reg_s* reg_idx_map[MTQ_MAP_COUNT][MTQ_MAX_IDX_COUNT];
    mtq_pkt_s rcvpkt;
    uint8_t port;  
    int1 is_init;
} mtq_s;

// Initialize mtq object
void mtq_init(mtq_s* mtq, uint8_t port);

// Free heap allocations 
void mtq_destroy(mtq_s* mtq);

// Clear mtq buffer data
void mtq_clear(mtq_s* mtq);

// Lookup register by map idx, idx, or key in reg_idx_map
mtq_reg_s* mtq_get_reg(mtq_s* mtq, uint8_t midx, uint8_t idx);
mtq_reg_s* mtq_get_reg(mtq_s* mtq, uint16_t key);

// Print out formatted space separated contents of a register to out buffer, updating p
status_e mtq_print_reg_data(mtq_s* mtq, mtq_reg_s* reg, uint8_t* out, uint8_t* p);
status_e mtq_print_reg_data(mtq_s* mtq, uint16_t key, uint8_t* out, uint8_t* p);

// Send register read command to mtq
status_e mtq_read_start(mtq_s* mtq, mtq_reg_s* reg);
status_e mtq_read_start(mtq_s* mtq, uint16_t key);

// Handle read data received from mtq
void mtq_read_complete(mtq_s* mtq);

// Send register write command to mtq
status_e mtq_write_start(mtq_s* mtq, mtq_reg_s* reg, void* data);
status_e mtq_write_start(mtq_s* mtq, uint16_t key, void* data);

// Handle write response receieved from mtq
void mtq_write_complete(mtq_s* mtq);

// Parse mtq data packets from input stream/buffer 
void mtq_parse_stream(mtq_s* mtq, ringbuf_s* irqbuf);

// HIGH LEVEL API

// Get a copy of the data for a given register, if it exists
status_e mtq_get_data(mtq_s* mtq, uint16_t key, void* out);

// Check if we're still receiving from the mtq
status_e mtq_heartbeat(mtq_s* mtq);

// Power cycle the unit
status_e mtq_reboot(mtq_s* mtq);

// Write 1 to nvm register to power cycle, then set datetime & TLE
status_e mtq_reset(mtq_s* mtq);

// Read fast frame registers
status_e mtq_read_fast(mtq_s* mtq);

// Read control frame registers
status_e mtq_read_ctrl(mtq_s* mtq);

// Read all available registers
status_e mtq_read_all(mtq_s* mtq);

// Set date and time registers (absolute time)
status_e mtq_set_datetime(mtq_s* mtq, rtc_time_t rtc);

#define MTQ_MODE_MANUAL             7 
#define MTQ_MODE_SUN_SPIN           6
#define MTQ_MODE_TARGET_TRACKING    5
#define MTQ_MODE_LVLH               4
#define MTQ_MODE_FINE_POINTING      3
#define MTQ_MODE_SUN_POINTING       2
#define MTQ_MODE_DETUMBLING         1
#define MTQ_MODE_SAFE               0

// Set mode via conf register. Ignore target elevation.
status_e mtq_set_mode(mtq_s* mtq, uint8_t mode);

// Map Idx | Idx register keys

#define MTQ_FACT                0 << 8 | 0
#define MTQ_SNID                0 << 8 | 1
#define MTQ_CONF                0 << 8 | 4
#define MTQ_TIME                0 << 8 | 5
#define MTQ_DATE                0 << 8 | 6
#define MTQ_LLA_REF             0 << 8 | 7
#define MTQ_Q_REF               0 << 8 | 10
#define MTQ_POINTING_AXIS       0 << 8 | 14
#define MTQ_TLE                 0 << 8 | 17
#define MTQ_GGA                 0 << 8 | 52
#define MTQ_ZDA                 0 << 8 | 84
#define MTQ_SV_USER             0 << 8 | 100
#define MTQ_MTQ_USER            0 << 8 | 103
#define MTQ_CMG0_G_RATE_USER    0 << 8 | 106
#define MTQ_CMG0_G_TOR_USER     0 << 8 | 107
#define MTQ_CMG1_G_RATE_USER    0 << 8 | 108
#define MTQ_CMG1_G_TOR_USER     0 << 8 | 109
#define MTQ_CMG2_G_RATE_USER    0 << 8 | 110
#define MTQ_CMG2_G_TOR_USER     0 << 8 | 111
#define MTQ_CMG3_G_RATE_USER    0 << 8 | 112
#define MTQ_CMG3_G_TOR_USER     0 << 8 | 113
#define MTQ_STAT                0 << 8 | 128
#define MTQ_ACT_ERR             0 << 8 | 129
#define MTQ_SEN_ERR             0 << 8 | 130
#define MTQ_CSS_ERR             0 << 8 | 131
#define MTQ_Q                   0 << 8 | 132
#define MTQ_RATE                0 << 8 | 136
#define MTQ_LLA                 0 << 8 | 139
#define MTQ_ATT_ERROR           0 << 8 | 142
#define MTQ_ATT_ERROR_RATE      0 << 8 | 145
#define MTQ_ADCS_TMP            0 << 8 | 148
#define MTQ_CMG0_TMP            0 << 8 | 149
#define MTQ_CMG1_TMP            0 << 8 | 150
#define MTQ_CMG2_TMP            0 << 8 | 151
#define MTQ_CMG3_TMP            0 << 8 | 152
#define MTQ_FSS_TMP1            0 << 8 | 153
#define MTQ_FSS_TMP2            0 << 8 | 154
#define MTQ_FSS_TMP3            0 << 8 | 155
#define MTQ_SV                  0 << 8 | 156
#define MTQ_MAG                 0 << 8 | 159
#define MTQ_MAG_MAT             1 << 8 | 0
#define MTQ_MAG_VEC             1 << 8 | 9
#define MTQ_MAG_STAT            1 << 8 | 12
#define MTQ_MAG0_S              1 << 8 | 13
#define MTQ_MAG1_S              1 << 8 | 16
#define MTQ_MAG2_S              1 << 8 | 19
#define MTQ_MAG3_S              1 << 8 | 22
#define MTQ_MAG4_S              1 << 8 | 25
#define MTQ_MAG5_S              1 << 8 | 28
#define MTQ_FSS_STAT            1 << 8 | 31
#define MTQ_FSS0_SV             1 << 8 | 32
#define MTQ_FSS0_PDSUM          1 << 8 | 33
#define MTQ_FSS1_SV             1 << 8 | 34
#define MTQ_FSS1_PDSUM          1 << 8 | 35
#define MTQ_FSS2_SV             1 << 8 | 36
#define MTQ_FSS2_PDSUM          1 << 8 | 37
#define MTQ_FSS3_SV             1 << 8 | 38
#define MTQ_FSS3_PDSUM          1 << 8 | 39
#define MTQ_FSS4_SV             1 << 8 | 40
#define MTQ_FSS4_PDSUM          1 << 8 | 41
#define MTQ_FSS5_SV             1 << 8 | 42
#define MTQ_FSS5_PDSUM          1 << 8 | 43
#define MTQ_IMU_STAT            1 << 8 | 44
#define MTQ_IMU0_S              1 << 8 | 45
#define MTQ_IMU1_S              1 << 8 | 48
#define MTQ_IMU2_S              1 << 8 | 51
#define MTQ_IMU3_S              1 << 8 | 54
#define MTQ_STR_STAT            1 << 8 | 57
#define MTQ_STR0_S              1 << 8 | 58
#define MTQ_STR1_S              1 << 8 | 62
#define MTQ_CSS                 1 << 8 | 66
#define MTQ_CMG_STAT            1 << 8 | 78
#define MTQ_CMG0_G_ANGLE        1 << 8 | 79
#define MTQ_CMG0_W_RATE         1 << 8 | 80
#define MTQ_CMG1_G_ANGLE        1 << 8 | 81
#define MTQ_CMG1_W_RATE         1 << 8 | 82
#define MTQ_CMG2_G_ANGLE        1 << 8 | 83
#define MTQ_CMG2_W_RATE         1 << 8 | 84
#define MTQ_CMG3_G_ANGLE        1 << 8 | 85
#define MTQ_CMG3_W_RATE         1 << 8 | 86
#define MTQ_MTQ                 1 << 8 | 87
#define MTQ_CMG0_G_RATE         1 << 8 | 90
#define MTQ_CMG0_W_ACC          1 << 8 | 91
#define MTQ_CMG1_G_RATE         1 << 8 | 92
#define MTQ_CMG1_W_ACC          1 << 8 | 93
#define MTQ_CMG2_G_RATE         1 << 8 | 94
#define MTQ_CMG2_W_ACC          1 << 8 | 95
#define MTQ_CMG3_G_RATE         1 << 8 | 96
#define MTQ_CMG3_W_ACC          1 << 8 | 97
#define MTQ_MASS                2 << 8 | 0
#define MTQ_INE_TEN             2 << 8 | 1
#define MTQ_POS_HB_B            2 << 8 | 10
#define MTQ_ORIEN_HB            2 << 8 | 13
#define MTQ_MAG_INFO            2 << 8 | 17
#define MTQ_MAG0_ORIEN_BS       2 << 8 | 18
#define MTQ_MAG1_ORIEN_BS       2 << 8 | 22
#define MTQ_MAG2_ORIEN_BS       2 << 8 | 26
#define MTQ_MAG3_ORIEN_BS       2 << 8 | 30
#define MTQ_MAG4_ORIEN_BS       2 << 8 | 34
#define MTQ_MAG5_ORIEN_BS       2 << 8 | 38
#define MTQ_FSS_INFO            2 << 8 | 42
#define MTQ_FSS0_ORIEN_BS       2 << 8 | 43
#define MTQ_FSS1_ORIEN_BS       2 << 8 | 47
#define MTQ_FSS2_ORIEN_BS       2 << 8 | 51
#define MTQ_FSS3_ORIEN_BS       2 << 8 | 55
#define MTQ_FSS4_ORIEN_BS       2 << 8 | 59
#define MTQ_FSS5_ORIEN_BS       2 << 8 | 63
#define MTQ_IMU_INFO            2 << 8 | 67
#define MTQ_IMU0_ORIEN_BS       2 << 8 | 68
#define MTQ_IMU1_ORIEN_BS       2 << 8 | 72
#define MTQ_IMU2_ORIEN_BS       2 << 8 | 76
#define MTQ_IMU3_ORIEN_BS       2 << 8 | 80
#define MTQ_STR_INFO            2 << 8 | 84
#define MTQ_STR0_ORIEN_BS       2 << 8 | 85
#define MTQ_STR1_ORIEN_BS       2 << 8 | 89
#define MTQ_NVM                 2 << 8 | 255

#define MTQ_NUM_FAST_REGS       17
static const uint16_t MTQ_FAST_FRAME_REGS[] = {
    MTQ_CONF, MTQ_TIME, MTQ_DATE, MTQ_MTQ_USER, MTQ_STAT, MTQ_ACT_ERR, MTQ_SEN_ERR,
    MTQ_CSS_ERR, MTQ_Q, MTQ_RATE, MTQ_LLA, MTQ_ATT_ERROR, MTQ_ATT_ERROR_RATE, MTQ_SV, MTQ_MAG, MTQ_MTQ, MTQ_MTQ_USER
};

#define MTQ_NUM_CTRL_REGS       10
static const uint16_t MTQ_CTRL_FRAME_REGS[] = {
    MTQ_Q, MTQ_RATE, MTQ_LLA, MTQ_MAG, MTQ_IMU0_S, MTQ_IMU1_S, MTQ_IMU2_S, MTQ_IMU3_S, MTQ_MTQ, MTQ_MTQ_USER
};

/* Register format: [idx, cnt, midx, type, data, len]
                idx: Denotes the register that the command will read/write from/to.
                cnt: Size of register in counts. 4 bytes/count.
                midx: Determins which part of the register is being accesses; always a value from 0 to 2.
                type: Indicates the kind of data stored in a register.
                data: Pointer to a buffer to store read register data.
                len: Length of data in bytes.
*/

static const mtq_reg_s MTQ_INIT_REG_TABLE[] = {
    // Table 6-2. User Register (0)
    // /* FACT */             { 0, 1, 0, T_UINT16, NULL, 0 },
    /* SNID */             { 1, 3, 0, T_CHAR,   NULL, 0 },
    /* CONF */             { 4, 1, 0, T_UINT8,  NULL, 0 },
    /* TIME */             { 5, 1, 0, T_UINT8,  NULL, 0 },
    /* DATE */             { 6, 1, 0, T_UINT8,  NULL, 0 },
    // /* LLA_REF */          { 7, 3, 0, T_FLOAT,  NULL, 0 },
    // /* Q_REF */            { 10, 4, 0, T_FLOAT, NULL, 0 },
    /* POINTING_AXIS */    { 14, 3, 0, T_FLOAT, NULL, 0 },
    /* TLE */              { 17, 35, 0, T_CHAR, NULL, 0 },
    // /* GGA */              { 52, 32, 0, T_CHAR, NULL, 0 },
    // /* ZDA */              { 84, 16, 0, T_CHAR, NULL, 0 },
    // /* SV_USER */          { 100, 3, 0, T_FLOAT, NULL, 0 },
    /* MTQ_USER */         { 103, 3, 0, T_FLOAT, NULL, 0 },
    // /* CMG0_G_RATE_USER */ { 106, 1, 0, T_FLOAT, NULL, 0 },
    // /* CMG0_G_TOR_USER */  { 107, 1, 0, T_FLOAT, NULL, 0 },
    // /* CMG1_G_RATE_USER */ { 108, 1, 0, T_FLOAT, NULL, 0 },
    // /* CMG1_G_TOR_USER */  { 109, 1, 0, T_FLOAT, NULL, 0 },
    // /* CMG2_G_RATE_USER */ { 110, 1, 0, T_FLOAT, NULL, 0 },
    // /* CMG2_G_TOR_USER */  { 111, 1, 0, T_FLOAT, NULL, 0 },
    // /* CMG3_G_RATE_USER */ { 112, 1, 0, T_FLOAT, NULL, 0 },
    // /* CMG3_G_TOR_USER */  { 113, 1, 0, T_FLOAT, NULL, 0 },
    /* STAT */             { 128, 1, 0, T_UINT8, NULL, 0 },
    /* ACT_ERR */          { 129, 1, 0, T_UINT8, NULL, 0 },
    /* SEN_ERR */          { 130, 1, 0, T_UINT8, NULL, 0 },
    /* CSS_ERR */          { 131, 1, 0, T_UINT8, NULL, 0 },
    /* Q */                { 132, 4, 0, T_FLOAT, NULL, 0 },
    /* RATE */             { 136, 3, 0, T_FLOAT, NULL, 0 },
    /* LLA */              { 139, 3, 0, T_FLOAT, NULL, 0 },
    /* ATT_ERROR */        { 142, 3, 0, T_FLOAT, NULL, 0 },
    /* ATT_ERROR_RATE */   { 145, 3, 0, T_FLOAT, NULL, 0 },
    // /* ADCS_TMP */         { 148, 1, 0, T_INT16, NULL, 0 },
    // /* CMG0_TMP */         { 149, 1, 0, T_INT16, NULL, 0 },
    // /* CMG1_TMP */         { 150, 1, 0, T_INT16, NULL, 0 },
    // /* CMG2_TMP */         { 151, 1, 0, T_INT16, NULL, 0 },
    // /* CMG3_TMP */         { 152, 1, 0, T_INT16, NULL, 0 },
    // /* FSS_TMP1 */         { 153, 1, 0, T_INT16, NULL, 0 },
    // /* FSS_TMP2 */         { 154, 1, 0, T_INT16, NULL, 0 },
    // /* FSS_TMP3 */         { 155, 1, 0, T_INT16, NULL, 0 },
    /* SV */               { 156, 3, 0, T_FLOAT, NULL, 0 },
    /* MAG */              { 159, 3, 0, T_FLOAT, NULL, 0 },
    // Table 6-3. Sensor/Actuator Register (1)
    // /* MAG_MAT */          { 0, 9, 1, T_FLOAT, NULL, 0 },
    // /* MAG_VEC */          { 9, 3, 1, T_FLOAT, NULL, 0 },
    // /* MAG_STAT */         { 12, 1, 1, T_UINT8, NULL, 0 },
    // /* MAG0_S */           { 13, 3, 1, T_FLOAT, NULL, 0 },
    // /* MAG1_S */           { 16, 3, 1, T_FLOAT, NULL, 0 },
    // /* MAG2_S */           { 19, 3, 1, T_FLOAT, NULL, 0 },
    // /* MAG3_S */           { 22, 3, 1, T_FLOAT, NULL, 0 },
    // /* MAG4_S */           { 25, 3, 1, T_FLOAT, NULL, 0 },
    // /* MAG5_S */           { 28, 3, 1, T_FLOAT, NULL, 0 },
    // /* FSS_STAT */         { 31, 1, 1, T_UINT8, NULL, 0 },
    // /* FSS0_SV */          { 32, 1, 1, T_UINT16, NULL, 0 },
    // /* FSS0_PDSUM */       { 33, 1, 1, T_UINT16, NULL, 0 },
    // /* FSS1_SV */          { 34, 1, 1, T_UINT16, NULL, 0 },
    // /* FSS1_PDSUM */       { 35, 1, 1, T_UINT16, NULL, 0 },
    // /* FSS2_SV */          { 36, 1, 1, T_UINT16, NULL, 0 },
    // /* FSS2_PDSUM */       { 37, 1, 1, T_UINT16, NULL, 0 },
    // /* FSS3_SV */          { 38, 1, 1, T_UINT16, NULL, 0 },
    // /* FSS3_PDSUM */       { 39, 1, 1, T_UINT16, NULL, 0 },
    // /* FSS4_SV */          { 40, 1, 1, T_UINT16, NULL, 0 },
    // /* FSS4_PDSUM */       { 41, 1, 1, T_UINT16, NULL, 0 },
    // /* FSS5_SV */          { 42, 1, 1, T_UINT16, NULL, 0 },
    // /* FSS5_PDSUM */       { 43, 1, 1, T_UINT16, NULL, 0 },
    // /* IMU_STAT */         { 44, 1, 1, T_UINT8, NULL, 0 },
    /* IMU0_S */           { 45, 3, 1, T_FLOAT, NULL, 0 },
    /* IMU1_S */           { 48, 3, 1, T_FLOAT, NULL, 0 },
    /* IMU2_S */           { 51, 3, 1, T_FLOAT, NULL, 0 },
    /* IMU3_S */           { 54, 3, 1, T_FLOAT, NULL, 0 },
    // /* STR_STAT */         { 57, 1, 1, T_UINT8, NULL, 0 },
    // /* STR0_S */           { 58, 4, 1, T_FLOAT, NULL, 0 },
    // /* STR1_S */           { 62, 4, 1, T_FLOAT, NULL, 0 },
    // /* CSS */              { 66, 12,1, T_FLOAT, NULL, 0 },
    // /* CMG_STAT */         { 78, 1, 1, T_UINT8, NULL, 0 },
    // /* CMG0_G_ANGLE */     { 79, 1, 1, T_FLOAT, NULL, 0 },
    // /* CMG0_W_RATE */      { 80, 1, 1, T_FLOAT, NULL, 0 },
    // /* CMG1_G_ANGLE */     { 81, 1, 1, T_FLOAT, NULL, 0 },
    // /* CMG1_W_RATE */      { 82, 1, 1, T_FLOAT, NULL, 0 },
    // /* CMG2_G_ANGLE */     { 83, 1, 1, T_FLOAT, NULL, 0 },
    // /* CMG2_W_RATE */      { 84, 1, 1, T_FLOAT, NULL, 0 },
    // /* CMG3_G_ANGLE */     { 85, 1, 1, T_FLOAT, NULL, 0 },
    // /* CMG3_W_RATE */      { 86, 1, 1, T_FLOAT, NULL, 0 },
    /* MTQ */              { 87, 3, 1, T_FLOAT, NULL, 0 },
    // /* CMG0_G_RATE */      { 90, 1, 1, T_FLOAT, NULL, 0 },
    // /* CMG0_W_ACC */       { 91, 1, 1, T_FLOAT, NULL, 0 },
    // /* CMG1_G_RATE */      { 92, 1, 1, T_FLOAT, NULL, 0 },
    // /* CMG1_W_ACC */       { 93, 1, 1, T_FLOAT, NULL, 0 },
    // /* CMG2_G_RATE */      { 94, 1, 1, T_FLOAT, NULL, 0 },
    // /* CMG2_W_ACC */       { 95, 1, 1, T_FLOAT, NULL, 0 },
    // /* CMG3_G_RATE */      { 96, 1, 1, T_FLOAT, NULL, 0 },
    // /* CMG3_W_ACC */       { 97, 1, 1, T_FLOAT, NULL, 0 },
    // // Table 6-4. Parameter Register (2)
    // /* MASS */             { 0, 1, 2, T_FLOAT, NULL, 0 },
    // /* INE_TEN */          { 1, 9, 2, T_FLOAT, NULL, 0 },
    // /* POS_HB_B */         { 10, 3, 2, T_FLOAT, NULL, 0 },
    // /* ORIEN_HB */         { 13, 4, 2, T_FLOAT, NULL, 0 },
    // /* MAG_INFO */         { 17, 1, 2, T_UINT8, NULL, 0 },
    // /* MAG0_ORIEN_BS */    { 18, 4, 2, T_FLOAT, NULL, 0 },
    // /* MAG1_ORIEN_BS */    { 22, 4, 2, T_FLOAT, NULL, 0 },
    // /* MAG2_ORIEN_BS */    { 26, 4, 2, T_FLOAT, NULL, 0 },
    // /* MAG3_ORIEN_BS */    { 30, 4, 2, T_FLOAT, NULL, 0 },
    // /* MAG4_ORIEN_BS */    { 34, 4, 2, T_FLOAT, NULL, 0 },
    // /* MAG5_ORIEN_BS */    { 38, 4, 2, T_FLOAT, NULL, 0 },
    // /* FSS_INFO */         { 42, 1, 2, T_UINT8, NULL, 0 },
    // /* FSS0_ORIEN_BS */    { 43, 4, 2, T_FLOAT, NULL, 0 },
    // /* FSS1_ORIEN_BS */    { 47, 4, 2, T_FLOAT, NULL, 0 },
    // /* FSS2_ORIEN_BS */    { 51, 4, 2, T_FLOAT, NULL, 0 },
    // /* FSS3_ORIEN_BS */    { 55, 4, 2, T_FLOAT, NULL, 0 },
    // /* FSS4_ORIEN_BS */    { 59, 4, 2, T_FLOAT, NULL, 0 },
    // /* FSS5_ORIEN_BS */    { 63, 4, 2, T_FLOAT, NULL, 0 },
    // /* IMU_INFO */         { 67, 1, 2, T_UINT8, NULL, 0 },
    // /* IMU0_ORIEN_BS */    { 68, 4, 2, T_FLOAT, NULL, 0 },
    // /* IMU1_ORIEN_BS */    { 72, 4, 2, T_FLOAT, NULL, 0 },
    // /* IMU2_ORIEN_BS */    { 76, 4, 2, T_FLOAT, NULL, 0 },
    // /* IMU3_ORIEN_BS */    { 80, 4, 2, T_FLOAT, NULL, 0 },
    // /* STR_INFO */         { 84, 1, 2, T_UINT8, NULL, 0 },
    // /* STR0_ORIEN_BS */    { 85, 4, 2, T_FLOAT, NULL, 0 },
    // /* STR1_ORIEN_BS */    { 89, 4, 2, T_FLOAT, NULL, 0 },
    /* NVM */              { 255, 1, 2, T_UINT8, NULL, 0 }
};

#endif
