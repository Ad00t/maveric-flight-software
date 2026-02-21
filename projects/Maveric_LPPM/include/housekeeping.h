#ifndef __HOUSEKEEPING_H__
#define __HOUSEKEEPING_H__

void hk_init(void);

// HOUSEKEEPING FUNCTIONS

void hk_get_ertc_time(void);
void hk_systime_sync(void);
void hk_log(void);
void hk_heartbeats(void);
void hk_read_sensors(void);
void hk_test_disable_ertc(void);

#endif // !__HOUSEKEEPING_H__
