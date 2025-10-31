#ifndef __ADCSMTQ_H__
#define __ADCSMTQ_H__

#include "interrupts.h"
#include <stddef.h>
#include <stdint.h>

#define ADCSMTQ_HEAD_READ       0xC9
#define ADCSMTQ_HEAD_WRITE      0xC8
#define ADCSMTQ_REG_TABLE_LEN   116
#define ADCSMTQ_MAP_COUNT       3
#define ADCSMTQ_MAX_IDX_COUNT   256
#define ADCSMTQ_NAME_HASH_SIZE  2048

typedef enum {
    T_UINT8,
    T_INT8,
    T_UINT16,
    T_INT16,
    T_FLOAT,
    T_CHAR
} adcsmtq_reg_type_e;

typedef struct {
    char name[17];
    uint8_t idx;
    uint8_t data_count;
    uint8_t map_idx;
    adcsmtq_reg_type_e type;
    void* value;
    uint8_t value_len;
    int1 dirty;
} adcsmtq_reg_s;

typedef struct {
    uint8_t port;  
    adcsmtq_reg_s reg_table[ADCSMTQ_REG_TABLE_LEN]; 
    adcsmtq_reg_s* reg_idx_map[ADCSMTQ_MAP_COUNT][ADCSMTQ_MAX_IDX_COUNT];
    adcsmtq_reg_s* reg_name_map[ADCSMTQ_NAME_HASH_SIZE];
} adcsmtq_s;

// Initialize adcsmtq object
void adcsmtq_init(adcsmtq_s* a, uint8_t port);

// Lookup register by name in reg_name_map
adcsmtq_reg_s* adcsmtq_get_reg_by_name(adcsmtq_s* a, char* name);

// Lookup register by map idx, idx in reg_idx_map
adcsmtq_reg_s* adcsmtq_get_reg_by_idx(adcsmtq_s* a, uint8_t map_idx, uint8_t idx);

// Send register read command to adcsmtq
void adcsmtq_read_start(adcsmtq_s* a, adcsmtq_reg_s* reg);
void adcsmtq_read_start(adcsmtq_s* a, char* name);

// Handle read data received from adcsmtq
void adcsmtq_read_complete(adcsmtq_s* a, irqbuf_s* rcv_buf);

// Send register write command to adcsmtq
void adcsmtq_write_start(adcsmtq_s* a, adcsmtq_reg_s* reg, void* data);
void adcsmtq_write_start(adcsmtq_s* a, char* name, void* data);

// Handle write response receieved from adcsmtq
void adcsmtq_write_complete(adcsmtq_s* a, irqbuf_s* rcv_buf);

// Read back values from registers that were recently written
void adcsmtq_readback(adcsmtq_s* a);

/* Register format: [idx, data_count, map_idx, type]
                idx: Denotes the register that the command will read/write from/to.
                data_count: Size of register in counts. 4 bytes/count.
                map_idx: Determins which part of the register is being accesses; always a value from 0 to 2.
                type: Indicates the kind of data stored in a register.
*/

static const adcsmtq_reg_s ADCSMTQ_INIT_REG_TABLE[] = {
    // Table 6-2. User Register (0)
    { "FACT", 0, 1, 0, T_UINT16, NULL, 0, FALSE },
    { "SNID", 1, 3, 0, T_CHAR, NULL, 0, FALSE },
    { "CONF", 4, 1, 0, T_UINT8, NULL, 0, FALSE },
    { "TIME", 5, 1, 0, T_UINT8, NULL, 0, FALSE },
    { "DATE", 6, 1, 0, T_UINT8, NULL, 0, FALSE },
    { "LLA_REF", 7, 3, 0, T_FLOAT, NULL, 0, FALSE },
    { "Q_REF", 10, 4, 0, T_FLOAT, NULL, 0, FALSE },
    { "POINTING_AXIS", 14, 3, 0, T_FLOAT, NULL, 0, FALSE },
    { "TLE", 17, 35, 0, T_CHAR, NULL, 0, FALSE },
    { "GGA", 52, 32, 0, T_CHAR, NULL, 0, FALSE },
    { "ZDA", 84, 16, 0, T_CHAR, NULL, 0, FALSE },
    { "SV_USER", 100, 3, 0, T_FLOAT, NULL, 0, FALSE },
    { "MTQ_USER", 103, 3, 0, T_FLOAT, NULL, 0, FALSE },
    { "CMG0_G_RATE_USER", 106, 1, 0, T_FLOAT, NULL, 0, FALSE },
    { "CMG0_G_TOR_USER", 107, 1, 0, T_FLOAT, NULL, 0, FALSE },
    { "CMG1_G_RATE_USER", 108, 1, 0, T_FLOAT, NULL, 0, FALSE },
    { "CMG1_G_TOR_USER", 109, 1, 0, T_FLOAT, NULL, 0, FALSE },
    { "CMG2_G_RATE_USER", 110, 1, 0, T_FLOAT, NULL, 0, FALSE },
    { "CMG2_G_TOR_USER", 111, 1, 0, T_FLOAT, NULL, 0, FALSE },
    { "CMG3_G_RATE_USER", 112, 1, 0, T_FLOAT, NULL, 0, FALSE },
    { "CMG3_G_TOR_USER", 113, 1, 0, T_FLOAT, NULL, 0, FALSE },
    { "STAT", 128, 1, 0, T_UINT8, NULL, 0, FALSE },
    { "ACT_ERR", 129, 1, 0, T_UINT8, NULL, 0, FALSE },
    { "SEN_ERR", 130, 1, 0, T_UINT8, NULL, 0, FALSE },
    { "CSS_ERR", 131, 1, 0, T_UINT8, NULL, 0, FALSE },
    { "Q", 132, 4, 0, T_FLOAT, NULL, 0, FALSE },
    { "RATE", 136, 3, 0, T_FLOAT, NULL, 0, FALSE },
    { "LLA", 139, 3, 0, T_FLOAT, NULL, 0, FALSE },
    { "ATT_ERROR", 142, 3, 0, T_FLOAT, NULL, 0, FALSE },
    { "ATT_ERROR_RATE", 145, 3, 0, T_FLOAT, NULL, 0, FALSE },
    { "ADCS_TMP", 148, 1, 0, T_INT16, NULL, 0, FALSE },
    { "CMG0_TMP", 149, 1, 0, T_INT16, NULL, 0, FALSE },
    { "CMG1_TMP", 150, 1, 0, T_INT16, NULL, 0, FALSE },
    { "CMG2_TMP", 151, 1, 0, T_INT16, NULL, 0, FALSE },
    { "CMG3_TMP", 152, 1, 0, T_INT16, NULL, 0, FALSE },
    { "FSS_TMP1", 153, 1, 0, T_INT16, NULL, 0, FALSE },
    { "FSS_TMP2", 154, 1, 0, T_INT16, NULL, 0, FALSE },
    { "FSS_TMP3", 155, 1, 0, T_INT16, NULL, 0, FALSE },
    { "SV", 156, 3, 0, T_FLOAT, NULL, 0, FALSE },
    { "MAG", 159, 3, 0, T_FLOAT, NULL, 0, FALSE },
    // Table 6-3. Sensor/Actuator Register (1)
    { "MAG_MAT", 0, 9, 1, T_FLOAT, NULL, 0, FALSE },
    { "MAG_VEC", 9, 3, 1, T_FLOAT, NULL, 0, FALSE },
    { "MAG_STAT", 12, 1, 1, T_UINT8, NULL, 0, FALSE },
    { "MAG0_S", 13, 3, 1, T_FLOAT, NULL, 0, FALSE },
    { "MAG1_S", 16, 3, 1, T_FLOAT, NULL, 0, FALSE },
    { "MAG2_S", 19, 3, 1, T_FLOAT, NULL, 0, FALSE },
    { "MAG3_S", 22, 3, 1, T_FLOAT, NULL, 0, FALSE },
    { "MAG4_S", 25, 3, 1, T_FLOAT, NULL, 0, FALSE },
    { "MAG5_S", 28, 3, 1, T_FLOAT, NULL, 0, FALSE },
    { "FSS_STAT", 31, 1, 1, T_UINT8, NULL, 0, FALSE },
    { "FSS0_SV", 32, 1, 1, T_UINT16, NULL, 0, FALSE },
    { "FSS0_PDSUM", 33, 1, 1, T_UINT16, NULL, 0, FALSE },
    { "FSS1_SV", 34, 1, 1, T_UINT16, NULL, 0, FALSE },
    { "FSS1_PDSUM", 35, 1, 1, T_UINT16, NULL, 0, FALSE },
    { "FSS2_SV", 36, 1, 1, T_UINT16, NULL, 0, FALSE },
    { "FSS2_PDSUM", 37, 1, 1, T_UINT16, NULL, 0, FALSE },
    { "FSS3_SV", 38, 1, 1, T_UINT16, NULL, 0, FALSE },
    { "FSS3_PDSUM", 39, 1, 1, T_UINT16, NULL, 0, FALSE },
    { "FSS4_SV", 40, 1, 1, T_UINT16, NULL, 0, FALSE },
    { "FSS4_PDSUM", 41, 1, 1, T_UINT16, NULL, 0, FALSE },
    { "FSS5_SV", 42, 1, 1, T_UINT16, NULL, 0, FALSE },
    { "FSS5_PDSUM", 43, 1, 1, T_UINT16, NULL, 0, FALSE },
    { "IMU_STAT", 44, 1, 1, T_UINT8, NULL, 0, FALSE },
    { "IMU0_S", 45, 3, 1, T_FLOAT, NULL, 0, FALSE },
    { "IMU1_S", 48, 3, 1, T_FLOAT, NULL, 0, FALSE },
    { "IMU2_S", 51, 3, 1, T_FLOAT, NULL, 0, FALSE },
    { "IMU3_S", 54, 3, 1, T_FLOAT, NULL, 0, FALSE },
    { "STR_STAT", 57, 1, 1, T_UINT8, NULL, 0, FALSE },
    { "STR0_S", 58, 4, 1, T_FLOAT, NULL, 0, FALSE },
    { "STR1_S", 62, 4, 1, T_FLOAT, NULL, 0, FALSE },
    { "CSS", 66, 12, 1, T_FLOAT, NULL, 0, FALSE },
    { "CMG_STAT", 78, 1, 1, T_UINT8, NULL, 0, FALSE },
    { "CMG0_G_ANGLE", 79, 1, 1, T_FLOAT, NULL, 0, FALSE },
    { "CMG0_W_RATE", 80, 1, 1, T_FLOAT, NULL, 0, FALSE },
    { "CMG1_G_ANGLE", 81, 1, 1, T_FLOAT, NULL, 0, FALSE },
    { "CMG1_W_RATE", 82, 1, 1, T_FLOAT, NULL, 0, FALSE },
    { "CMG2_G_ANGLE", 83, 1, 1, T_FLOAT, NULL, 0, FALSE },
    { "CMG2_W_RATE", 84, 1, 1, T_FLOAT, NULL, 0, FALSE },
    { "CMG3_G_ANGLE", 85, 1, 1, T_FLOAT, NULL, 0, FALSE },
    { "CMG3_W_RATE", 86, 1, 1, T_FLOAT, NULL, 0, FALSE },
    { "MTQ", 87, 3, 1, T_FLOAT, NULL, 0, FALSE },
    { "CMG0_G_RATE", 90, 1, 1, T_FLOAT, NULL, 0, FALSE },
    { "CMG0_W_ACC", 91, 1, 1, T_FLOAT, NULL, 0, FALSE },
    { "CMG1_G_RATE", 92, 1, 1, T_FLOAT, NULL, 0, FALSE },
    { "CMG1_W_ACC", 93, 1, 1, T_FLOAT, NULL, 0, FALSE },
    { "CMG2_G_RATE", 94, 1, 1, T_FLOAT, NULL, 0, FALSE },
    { "CMG2_W_ACC", 95, 1, 1, T_FLOAT, NULL, 0, FALSE },
    { "CMG3_G_RATE", 96, 1, 1, T_FLOAT, NULL, 0, FALSE },
    { "CMG3_W_ACC", 97, 1, 1, T_FLOAT, NULL, 0, FALSE },
    // Table 6-4. Parameter Register (2)
    { "MASS", 0, 1, 2, T_FLOAT, NULL, 0, FALSE },
    { "INE_TEN", 1, 9, 2, T_FLOAT, NULL, 0, FALSE },
    { "POS_HB_B", 10, 3, 2, T_FLOAT, NULL, 0, FALSE },
    { "ORIEN_HB", 13, 4, 2, T_FLOAT, NULL, 0, FALSE },
    { "MAG_INFO", 17, 1, 2, T_UINT8, NULL, 0, FALSE },
    { "MAG0_ORIEN_BS", 18, 4, 2, T_FLOAT, NULL, 0, FALSE },
    { "MAG1_ORIEN_BS", 22, 4, 2, T_FLOAT, NULL, 0, FALSE },
    { "MAG2_ORIEN_BS", 26, 4, 2, T_FLOAT, NULL, 0, FALSE },
    { "MAG3_ORIEN_BS", 30, 4, 2, T_FLOAT, NULL, 0, FALSE },
    { "MAG4_ORIEN_BS", 34, 4, 2, T_FLOAT, NULL, 0, FALSE },
    { "MAG5_ORIEN_BS", 38, 4, 2, T_FLOAT, NULL, 0, FALSE },
    { "FSS_INFO", 42, 1, 2, T_UINT8, NULL, 0, FALSE },
    { "FSS0_ORIEN_BS", 43, 4, 2, T_FLOAT, NULL, 0, FALSE },
    { "FSS1_ORIEN_BS", 47, 4, 2, T_FLOAT, NULL, 0, FALSE },
    { "FSS2_ORIEN_BS", 51, 4, 2, T_FLOAT, NULL, 0, FALSE },
    { "FSS3_ORIEN_BS", 55, 4, 2, T_FLOAT, NULL, 0, FALSE },
    { "FSS4_ORIEN_BS", 59, 4, 2, T_FLOAT, NULL, 0, FALSE },
    { "FSS5_ORIEN_BS", 63, 4, 2, T_FLOAT, NULL, 0, FALSE },
    { "IMU_INFO", 67, 1, 2, T_UINT8, NULL, 0, FALSE },
    { "IMU0_ORIEN_BS", 68, 4, 2, T_FLOAT, NULL, 0, FALSE },
    { "IMU1_ORIEN_BS", 72, 4, 2, T_FLOAT, NULL, 0, FALSE },
    { "IMU2_ORIEN_BS", 76, 4, 2, T_FLOAT, NULL, 0, FALSE },
    { "IMU3_ORIEN_BS", 80, 4, 2, T_FLOAT, NULL, 0, FALSE },
    { "STR_INFO", 84, 1, 2, T_UINT8, NULL, 0, FALSE },
    { "STR0_ORIEN_BS", 85, 4, 2, T_FLOAT, NULL, 0, FALSE },
    { "STR1_ORIEN_BS", 89, 4, 2, T_FLOAT, NULL, 0, FALSE },
    { "NVM", 255, 1, 2, T_UINT8, NULL, 0 }
};

#endif
