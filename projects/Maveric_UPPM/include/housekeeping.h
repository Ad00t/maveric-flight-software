#ifndef __HOUSEKEEPING_H__
#define __HOUSEKEEPING_H__

// Schedule housekeeping functions
void hk_init();

// SCHEDULE FUNCTIONS

void hk_get_rtc_time(void);
void hk_log(void);
void hk_update_tlm(void);
void hk_tlm_beacon(void);
void hk_ppm_reset(void);

#endif // !__HOUSEKEEPING_H__
