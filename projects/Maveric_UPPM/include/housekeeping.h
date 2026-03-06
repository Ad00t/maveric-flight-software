#ifndef __HOUSEKEEPING_H__
#define __HOUSEKEEPING_H__

// Schedule housekeeping functions
void hk_init();

// SCHEDULE FUNCTIONS

void hk_get_rtc_time(void);
void hk_log(void);
void hk_heartbeats(void);
void hk_read_sensors(void);
void hk_test_ax100(void);

#endif // !__HOUSEKEEPING_H__
