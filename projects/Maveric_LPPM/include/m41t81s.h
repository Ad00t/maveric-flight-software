#ifndef __M41T81S_H__
#define __M41T81S_H__

typedef struct {
    uint8_t wday;
    uint8_t mday;
    uint8_t month;
    uint8_t year;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
} ertc_s;

// Initialze ertc module with initial time in buf
void ertc_init(ertc_s* ertc, uint8_t wday, uint8_t month, uint8_t mday, uint8_t year, uint8_t hour, uint8_t min, uint8_t sec);

void ertc_get_time(ertc_s* ertc);

// Set ertc time to buf
void ertc_set_time(ertc_s* ertc, uint8_t wday, uint8_t month, uint8_t mday, uint8_t year, uint8_t hour, uint8_t min, uint8_t sec);

// Idk what this is for
void ertc_enable_fpm(ertc_s* ertc);

#endif
