#ifndef __ADCSMTQ_H__
#define __ADCSMTQ_H__

#include <stddef.h>
#include <stdint.h>

#define ADCSMTQ_HEAD_READ       0xC9
#define ADCSMTQ_HEAD_WRITE      0xC8
#define ADCSMTQ_REG_TABLE_LEN   116
#define ADCSMTQ_MAP_COUNT       3
#define ADCSMTQ_MAX_IDX_COUNT   256
#define ADCSMTQ_NAME_HASH_SIZE  128

typedef enum {
    T_UINT8,
    T_INT8,
    T_UINT16,
    T_INT16,
    T_FLOAT,
    T_CHAR
} ADCSMTQ_Reg_Type;

typedef struct {
    const char* name;
    uint8_t idx;
    uint8_t data_count;
    uint8_t map_idx;
    ADCSMTQ_Reg_Type type;
    void* value;
    uint8_t value_len;
} ADCSMTQ_Reg;

typedef struct {
    uint8_t port;  
    ADCSMTQ_Reg* reg_idx_map[ADCSMTQ_MAP_COUNT][ADCSMTQ_MAX_IDX_COUNT];
    ADCSMTQ_Reg* reg_name_map[ADCSMTQ_NAME_HASH_SIZE];
    ADCSMTQ_Reg reg_table[ADCSMTQ_REG_TABLE_LEN]; 
} ADCSMTQ;

/* Register format: [idx, data_count, map_idx, type]
                idx: Denotes the register that the command will read/write from/to.
                data_count: Size of register in counts. 4 bytes/count.
                map_idx: Determins which part of the register is being accesses; always a value from 0 to 2.
                type: Indicates the kind of data stored in a register.
*/

static const ADCSMTQ_Reg ADCSMTQ_INIT_REG_TABLE[] = {
    // Table 6-2. User Register (0)
    { "FACT", 0, 1, 0, T_UINT16, NULL },
    { "SNID", 1, 3, 0, T_CHAR, NULL },
    { "CONF", 4, 1, 0, T_UINT8, NULL },
    { "TIME", 5, 1, 0, T_UINT8, NULL },
    { "DATE", 6, 1, 0, T_UINT8, NULL },
    { "LLA_REF", 7, 3, 0, T_FLOAT, NULL },
    { "Q_REF", 10, 4, 0, T_FLOAT, NULL },
    { "POINTING_AXIS", 14, 3, 0, T_FLOAT, NULL },
    { "TLE", 17, 35, 0, T_CHAR, NULL },
    { "GGA", 52, 32, 0, T_CHAR, NULL },
    { "ZDA", 84, 16, 0, T_CHAR, NULL },
    { "SV_USER", 100, 3, 0, T_FLOAT, NULL },
    { "MTQ_USER", 103, 3, 0, T_FLOAT, NULL },
    { "CMG0_G_RATE_USER", 106, 1, 0, T_FLOAT, NULL },
    { "CMG0_G_TOR_USER", 107, 1, 0, T_FLOAT, NULL },
    { "CMG1_G_RATE_USER", 108, 1, 0, T_FLOAT, NULL },
    { "CMG1_G_TOR_USER", 109, 1, 0, T_FLOAT, NULL },
    { "CMG2_G_RATE_USER", 110, 1, 0, T_FLOAT, NULL },
    { "CMG2_G_TOR_USER", 111, 1, 0, T_FLOAT, NULL },
    { "CMG3_G_RATE_USER", 112, 1, 0, T_FLOAT, NULL },
    { "CMG3_G_TOR_USER", 113, 1, 0, T_FLOAT, NULL },
    { "STAT", 128, 1, 0, T_UINT8, NULL },
    { "ACT_ERR", 129, 1, 0, T_UINT8, NULL },
    { "SEN_ERR", 130, 1, 0, T_UINT8, NULL },
    { "CSS_ERR", 131, 1, 0, T_UINT8, NULL },
    { "Q", 132, 4, 0, T_FLOAT, NULL },
    { "RATE", 136, 3, 0, T_FLOAT, NULL },
    { "LLA", 139, 3, 0, T_FLOAT, NULL },
    { "ATT_ERROR", 142, 3, 0, T_FLOAT, NULL },
    { "ATT_ERROR_RATE", 145, 3, 0, T_FLOAT, NULL },
    { "ADCS_TMP", 148, 1, 0, T_INT16, NULL },
    { "CMG0_TMP", 149, 1, 0, T_INT16, NULL },
    { "CMG1_TMP", 150, 1, 0, T_INT16, NULL },
    { "CMG2_TMP", 151, 1, 0, T_INT16, NULL },
    { "CMG3_TMP", 152, 1, 0, T_INT16, NULL },
    { "FSS_TMP1", 153, 1, 0, T_INT16, NULL },
    { "FSS_TMP2", 154, 1, 0, T_INT16, NULL },
    { "FSS_TMP3", 155, 1, 0, T_INT16, NULL },
    { "SV", 156, 3, 0, T_FLOAT, NULL },
    { "MAG", 159, 3, 0, T_FLOAT, NULL },
    // Table 6-3. Sensor/Actuator Register (1)
    { "MAG_MAT", 0, 9, 1, T_FLOAT, NULL },
    { "MAG_VEC", 9, 3, 1, T_FLOAT, NULL },
    { "MAG_STAT", 12, 1, 1, T_UINT8, NULL },
    { "MAG0_S", 13, 3, 1, T_FLOAT, NULL },
    { "MAG1_S", 16, 3, 1, T_FLOAT, NULL },
    { "MAG2_S", 19, 3, 1, T_FLOAT, NULL },
    { "MAG3_S", 22, 3, 1, T_FLOAT, NULL },
    { "MAG4_S", 25, 3, 1, T_FLOAT, NULL },
    { "MAG5_S", 28, 3, 1, T_FLOAT, NULL },
    { "FSS_STAT", 31, 1, 1, T_UINT8, NULL },
    { "FSS0_SV", 32, 1, 1, T_UINT16, NULL },
    { "FSS0_PDSUM", 33, 1, 1, T_UINT16, NULL },
    { "FSS1_SV", 34, 1, 1, T_UINT16, NULL },
    { "FSS1_PDSUM", 35, 1, 1, T_UINT16, NULL },
    { "FSS2_SV", 36, 1, 1, T_UINT16, NULL },
    { "FSS2_PDSUM", 37, 1, 1, T_UINT16, NULL },
    { "FSS3_SV", 38, 1, 1, T_UINT16, NULL },
    { "FSS3_PDSUM", 39, 1, 1, T_UINT16, NULL },
    { "FSS4_SV", 40, 1, 1, T_UINT16, NULL },
    { "FSS4_PDSUM", 41, 1, 1, T_UINT16, NULL },
    { "FSS5_SV", 42, 1, 1, T_UINT16, NULL },
    { "FSS5_PDSUM", 43, 1, 1, T_UINT16, NULL },
    { "IMU_STAT", 44, 1, 1, T_UINT8, NULL },
    { "IMU0_S", 45, 3, 1, T_FLOAT, NULL },
    { "IMU1_S", 48, 3, 1, T_FLOAT, NULL },
    { "IMU2_S", 51, 3, 1, T_FLOAT, NULL },
    { "IMU3_S", 54, 3, 1, T_FLOAT, NULL },
    { "STR_STAT", 57, 1, 1, T_UINT8, NULL },
    { "STR0_S", 58, 4, 1, T_FLOAT, NULL },
    { "STR1_S", 62, 4, 1, T_FLOAT, NULL },
    { "CSS", 66, 12, 1, T_FLOAT, NULL },
    { "CMG_STAT", 78, 1, 1, T_UINT8, NULL },
    { "CMG0_G_ANGLE", 79, 1, 1, T_FLOAT, NULL },
    { "CMG0_W_RATE", 80, 1, 1, T_FLOAT, NULL },
    { "CMG1_G_ANGLE", 81, 1, 1, T_FLOAT, NULL },
    { "CMG1_W_RATE", 82, 1, 1, T_FLOAT, NULL },
    { "CMG2_G_ANGLE", 83, 1, 1, T_FLOAT, NULL },
    { "CMG2_W_RATE", 84, 1, 1, T_FLOAT, NULL },
    { "CMG3_G_ANGLE", 85, 1, 1, T_FLOAT, NULL },
    { "CMG3_W_RATE", 86, 1, 1, T_FLOAT, NULL },
    { "MTQ", 87, 3, 1, T_FLOAT, NULL },
    { "CMG0_G_RATE", 90, 1, 1, T_FLOAT, NULL },
    { "CMG0_W_ACC", 91, 1, 1, T_FLOAT, NULL },
    { "CMG1_G_RATE", 92, 1, 1, T_FLOAT, NULL },
    { "CMG1_W_ACC", 93, 1, 1, T_FLOAT, NULL },
    { "CMG2_G_RATE", 94, 1, 1, T_FLOAT, NULL },
    { "CMG2_W_ACC", 95, 1, 1, T_FLOAT, NULL },
    { "CMG3_G_RATE", 96, 1, 1, T_FLOAT, NULL },
    { "CMG3_W_ACC", 97, 1, 1, T_FLOAT, NULL },
    // Table 6-4. Parameter Register (2)
    { "MASS", 0, 1, 2, T_FLOAT, NULL },
    { "INE_TEN", 1, 9, 2, T_FLOAT, NULL },
    { "POS_HB_B", 10, 3, 2, T_FLOAT, NULL },
    { "ORIEN_HB", 13, 4, 2, T_FLOAT, NULL },
    { "MAG_INFO", 17, 1, 2, T_UINT8, NULL },
    { "MAG0_ORIEN_BS", 18, 4, 2, T_FLOAT, NULL },
    { "MAG1_ORIEN_BS", 22, 4, 2, T_FLOAT, NULL },
    { "MAG2_ORIEN_BS", 26, 4, 2, T_FLOAT, NULL },
    { "MAG3_ORIEN_BS", 30, 4, 2, T_FLOAT, NULL },
    { "MAG4_ORIEN_BS", 34, 4, 2, T_FLOAT, NULL },
    { "MAG5_ORIEN_BS", 38, 4, 2, T_FLOAT, NULL },
    { "FSS_INFO", 42, 1, 2, T_UINT8, NULL },
    { "FSS0_ORIEN_BS", 43, 4, 2, T_FLOAT, NULL },
    { "FSS1_ORIEN_BS", 47, 4, 2, T_FLOAT, NULL },
    { "FSS2_ORIEN_BS", 51, 4, 2, T_FLOAT, NULL },
    { "FSS3_ORIEN_BS", 55, 4, 2, T_FLOAT, NULL },
    { "FSS4_ORIEN_BS", 59, 4, 2, T_FLOAT, NULL },
    { "FSS5_ORIEN_BS", 63, 4, 2, T_FLOAT, NULL },
    { "IMU_INFO", 67, 1, 2, T_UINT8, NULL },
    { "IMU0_ORIEN_BS", 68, 4, 2, T_FLOAT, NULL },
    { "IMU1_ORIEN_BS", 72, 4, 2, T_FLOAT, NULL },
    { "IMU2_ORIEN_BS", 76, 4, 2, T_FLOAT, NULL },
    { "IMU3_ORIEN_BS", 80, 4, 2, T_FLOAT, NULL },
    { "STR_INFO", 84, 1, 2, T_UINT8, NULL },
    { "STR0_ORIEN_BS", 85, 4, 2, T_FLOAT, NULL },
    { "STR1_ORIEN_BS", 89, 4, 2, T_FLOAT, NULL },
    { "NVM", 255, 1, 2, T_UINT8, NULL }
};

// API

// Initialize ADCSMTQ object
void ADCSMTQ_init(ADCSMTQ* a, uint8_t port);
// Lookup register by name in reg_name_map
ADCSMTQ_Reg* ADCSMTQ_get_reg_by_name(ADCSMTQ* a, char* name);
// Send register read command to ADCSMTQ
void ADCSMTQ_read_start(ADCSMTQ* a, char* name);
// Handle read data received from ADCSMTQ
void ADCSMTQ_read_complete(ADCSMTQ* a, uint8_t* status);
// Send register write command to ADCSMTQ
void ADCSMTQ_write_start(ADCSMTQ* a, char* name, void* data);
// Handle write response receieved from ADCSMTQ
void ADCSMTQ_write_complete(ADCSMTQ* a, uint8_t* status);

#endif
