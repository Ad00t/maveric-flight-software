// LOWER PPM COMMAND IMPLEMENTATIONS

#include "cmdimpl.h"
#include "cmdmgr.h"
#include "systime.h"
#include "hashtable.h"
//#include "interrupts.h"
#include "interrupt.h"
//#include "scheduler.h"
//#include "flashmgr.h"
//#include "mtq.h"
//#include "ertc.h"
//#include "gyro.h"
//#include "naviguider.h"
//#include "housekeeping.h"
#include "common.h"
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>


//Sub-Systems Include
#include "BQ25672.h"                                                            //EPS Controller DRIVER
#include "ina226.h"                                                             //Current Sensor Controller DRIVER

extern uint8_t g_rbt_cause;         // restart_cause() in init 

extern irqmgr_s g_irqmgr;               // Interrupts manager
extern cmdmgr_s g_cmdmgr;               // Commands manager
//extern flashmgr_s g_flashmgr;           // Flash manager
//extern scheduler_s g_scheduler;         // Schedules manager
//extern ertc_s g_ertc;                   // External RTC (on motherboard)
//extern mtq_s g_mtq;                     // Magnetorquer
//extern gyro_s g_gyro;                   // Gyroscope (x3)
//extern nvg_s g_nvg;                     // Naviguider

void cmdimpl_init(void) {
    hashtable_s* ht = &g_cmdmgr.cmdimpls;
    char cmd_ppm_set_time[] = "ppm_set_time";
    char cmd_ppm_get_time[] = "ppm_get_time";
    char cmd_com_ping[] = "com_ping";
    char cmd_ppm_delay[] = "ppm_delay";
    char cmd_ppm_clear_bufs[] = "ppm_clear_bufs";
    
    char cmd_eps_housekeeping[] = "eps_hk";
    char cmd_eps_switch[] = "eps_sw";
    char cmd_eps_cut[] = "eps_cut";
    char cmd_eps_state[] = "eps_state";
    char cmd_eps_mode[] = "eps_mode";
    
    
    ht_set(ht, cmd_ppm_set_time, (cmdimpl_f) cmdimpl_ppm_set_time);
    ht_set(ht, cmd_ppm_get_time, (cmdimpl_f) cmdimpl_ppm_get_time);
    ht_set(ht, cmd_com_ping, (cmdimpl_f) cmdimpl_com_ping);
    ht_set(ht, cmd_ppm_delay, (cmdimpl_f) cmdimpl_ppm_delay);
    ht_set(ht, cmd_ppm_clear_bufs, (cmdimpl_f) cmdimpl_ppm_clear_bufs);
    
    ht_set(ht, cmd_eps_housekeeping, (cmdimpl_f) cmdimpl_eps_housekeeping);
    ht_set(ht, cmd_eps_switch, (cmdimpl_f) cmdimpl_eps_switch);
    ht_set(ht, cmd_eps_cut, (cmdimpl_f) cmdimpl_eps_cut);
    ht_set(ht, cmd_eps_state, (cmdimpl_f) cmdimpl_eps_state);
    ht_set(ht, cmd_eps_mode, (cmdimpl_f) cmdimpl_eps_mode);

    //ht_set(ht, "tlm_get_data", (cmdimpl_f) cmdimpl_tlm_get_data);

    //ht_set(ht, "mtq_get_conf", (cmdimpl_f) cmdimpl_mtq_get_conf);
    //ht_set(ht, "mtq_set_conf", (cmdimpl_f) cmdimpl_mtq_set_conf);
    //ht_set(ht, "mtq_get_datetime", (cmdimpl_f) cmdimpl_mtq_get_datetime);
    //ht_set(ht, "mtq_set_datetime", (cmdimpl_f) cmdimpl_mtq_set_datetime);
    //ht_set(ht, "mtq_get_tle", (cmdimpl_f) cmdimpl_mtq_get_tle);
    //ht_set(ht, "mtq_set_tle", (cmdimpl_f) cmdimpl_mtq_set_tle);
    //ht_set(ht, "mtq_get_paxs", (cmdimpl_f) cmdimpl_mtq_get_paxs);
    //ht_set(ht, "mtq_set_paxs", (cmdimpl_f) cmdimpl_mtq_set_paxs);
    //ht_set(ht, "mtq_get_mtquser", (cmdimpl_f) cmdimpl_mtq_get_mtquser);
    //ht_set(ht, "mtq_set_mtquser", (cmdimpl_f) cmdimpl_mtq_set_mtquser);
    //ht_set(ht, "mtq_reset", (cmdimpl_f) cmdimpl_mtq_reset);
    //ht_set(ht, "mtq_get_stat", (cmdimpl_f) cmdimpl_mtq_get_stat);
}

// COMMAND IMPLEMENTATIONS

void cmdimpl_ppm_set_time(cmdpkt_s* pkt) {
    char* p = pkt->args;
    rtc_time_t time;
    time.tm_wday = strtoul(p, &p, 10); // Other options: strtok(), strtod(), strotol()
    time.tm_mon = strtoul(p, &p, 10); 
    time.tm_mday = strtoul(p, &p, 10);
    time.tm_year = strtoul(p, &p, 10);
    time.tm_hour = strtoul(p, &p, 10);
    time.tm_min = strtoul(p, &p, 10);
    time.tm_sec = strtoul(p, &p, 10);
    
    uint64_t oldtime = systime_epoch_ms();
    //ertc_set_time(&g_ertc, time);
    systime_sync();
    //scheduler_refresh_all(&g_scheduler, oldtime);

    //mtq_set_datetime(&g_mtq, time);
    //cmd_dispatch(NODE, NODE_UPPM, 0, REQ, "ppm_set_time", pkt->args);

    //sprintf(LOGBUF, "cmdimpl_ppm_set_time '%s' [ %02u, %02u/%02u/20%02u %02u:%02u:%02u ]", pkt->args
    //        g_ertc.time.tm_wday, g_ertc.time.tm_mon, g_ertc.time.tm_mday, g_ertc.time.tm_year, 
    //        g_ertc.time.tm_hour, g_ertc.time.tm_min, g_ertc.time.tm_sec); log_info();
}

void cmdimpl_ppm_get_time(cmdpkt_s* pkt) {
    char cmd_ppm_get_time[] = "ppm_get_time";
    
    if (pkt->ptype != REQ) return;
    // Only implement REQ here because LPPM should never be replacing its time from another subsystem
    char tm_str[32] = {0};  
    //rtc_to_str(g_ertc.time, tm_str);
    cmd_dispatch(NODE, pkt->orgn, pkt->echo, RES, cmd_ppm_get_time, tm_str);
    //sprintf(LOGBUF, "cmdimpl_ppm_get_time REQ '%s'", tm_str); log_info();
    break;
}

void cmdimpl_com_ping(cmdpkt_s* pkt) {
    char cmd_com_ping[] = "com_ping";
    char cmd_ans_pong[] = "pong";
    // sprintf(LOGBUF, "cmdimpl_ppm_ping '%s'", pkt->args); log_info();
    fprintf(COM_A, "EPS com_ping impl\r\n");
    if (pkt->ptype == REQ) {
        fprintf(COM_A, "EPS com_ping dispatch\r\n");
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, RES, cmd_com_ping, cmd_ans_pong);
    }
}

void cmdimpl_ppm_delay(cmdpkt_s* pkt) {
    char* p = pkt->args;
    char cmd_ppm_delay[] = "ppm_delay";
    char cmd_empty[] = "";
    uint32_t delay = strtoul(p, &p, 10);
    //sprintf(LOGBUF, "cmdimpl_ppm_delay d=%u", delay); log_info();
    if (delay < 60000) {
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, ACK, cmd_ppm_delay, cmd_empty);
        delay_ms(delay);
    } else {
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, NACK, cmd_ppm_delay, cmd_empty);
    }
}

void cmdimpl_ppm_clear_bufs(cmdpkt_s* pkt) {
    char* p = pkt->args;
    uint8_t mode = strtoul(p, &p, 10);
    //sprintf(LOGBUF, "cmdimpl_ppm_clear_bufs m=%u", mode); log_info();
    switch (mode) {
        case 0:
            irqmgr_clear(&g_irqmgr);
            break;
        case 1:
            cmdmgr_clear(&g_cmdmgr);
            break;
    }
}

void cmdimpl_eps_housekeeping(cmdpkt_s* pkt){
if (pkt->ptype != REQ) return;

    extern int16 I_BUS;
    extern int16 I_BAT;
    extern int16 V_BUS;
    extern int16 V_AC1;
    extern int16 V_AC2;
    extern int16 V_BAT;
    extern int16 V_SYS;
    extern int16 TS_ADC;
    extern int16 T_DIE;
    //int16 D_P;
    //int16 D_M;
    extern int16 V3V3;
    extern int16 I3V3;
    extern int16 P3V3;
    extern int16 V5V0;
    extern int16 I5V0;
    extern int16 P5V0;
    extern int16 VOUT1;
    extern int16 IOUT1;
    extern int16 POUT1;
    extern int16 VOUT2;
    extern int16 IOUT2;
    extern int16 POUT2;
    extern int16 VOUT3;
    extern int16 IOUT3;
    extern int16 POUT3;
    extern int16 VOUT4;
    extern int16 IOUT4;
    extern int16 POUT4;
    extern int16 VOUT5;
    extern int16 IOUT5;
    extern int16 POUT5;
    extern int16 VOUT6;
    extern int16 IOUT6;
    extern int16 POUT6;
    extern int16 VBRN1;
    extern int16 IBRN1;
    extern int16 PBRN1;
    extern int16 VBRN2;
    extern int16 IBRN2;
    extern int16 PBRN2;
    extern int16 VSIN1;
    extern int16 ISIN1;
    extern int16 PSIN1;
    extern int16 VSIN2;
    extern int16 ISIN2;
    extern int16 PSIN2;
    extern int16 VSIN3;
    extern int16 ISIN3;
    extern int16 PSIN3;
    
    char* p = pkt->args;
    char cmd_eps_housekeeping[] = "eps_hk";
    char eps_ans_hk[] = ""; 
    int16 hk_values[48];
    fprintf(COM_A,"V BUS %Lu\n\r", V_BUS);
    hk_values[0] = I_BUS;
    hk_values[1] = I_BAT;
    hk_values[2] = V_BUS;
    hk_values[3] = V_AC1;
    hk_values[4] = V_AC2;
    hk_values[5] = V_BAT;
    hk_values[6] = V_SYS;
    hk_values[7] = TS_ADC;
    hk_values[8] = T_DIE;
    hk_values[9] = V3V3;
    hk_values[10] = I3V3;
    hk_values[11] = P3V3;
    hk_values[12] = V5V0;
    hk_values[13] = I5V0;
    hk_values[14] = P5V0; 
    hk_values[15] = VOUT1;
    hk_values[16] = IOUT1;
    hk_values[17] = POUT1;
    hk_values[18] = VOUT2;
    hk_values[19] = IOUT2;
    hk_values[20] = POUT2;
    hk_values[21] = VOUT3;
    hk_values[22] = IOUT3;
    hk_values[23] = POUT3;
    hk_values[24] = VOUT4;
    hk_values[25] = IOUT4;
    hk_values[26] = POUT4;
    hk_values[27] = VOUT5;
    hk_values[28] = IOUT5;
    hk_values[29] = POUT5;
    hk_values[30] = VOUT6;
    hk_values[31] = IOUT6;
    hk_values[32] = POUT6; 
    hk_values[33] = VBRN1;
    hk_values[34] = IBRN1;
    hk_values[35] = PBRN1;
    hk_values[36] = VBRN2;
    hk_values[37] = IBRN2;
    hk_values[38] = PBRN2;
    hk_values[39] = VSIN1;
    hk_values[40] = ISIN1;
    hk_values[41] = PSIN1;
    hk_values[42] = VSIN2;
    hk_values[43] = ISIN2;
    hk_values[44] = PSIN2;
    hk_values[45] = VSIN3;
    hk_values[46] = ISIN3;
    hk_values[47] = PSIN3;
    fprintf(COM_A,"V BUS %02x\n\r", hk_values[2]);
    char *charPtr = (char *)hk_values;
    fprintf(COM_A,charPtr);
//    memcpy(eps_ans_hk, hk_values, 96);
    //char *eps_ans_hk = (char *)hk_values;
    cmd_dispatch(NODE, pkt->orgn, pkt->echo, RES, cmd_eps_housekeeping, hk_values,96);
}

void cmdimpl_eps_switch(cmdpkt_s* pkt){
if (pkt->ptype != REQ) return;
    char* p = pkt->args;
    char cmd_eps_switch[] = "eps_sw";
	//unsigned int8 add;
    int pin;
    char cmd_ans_switch[]="0";
    
    uint8_t eps_output = strtoul(p, &p, 10);
    uint8_t eps_state = strtoul(p, &p, 10);
    
	switch(eps_output)
	{
		case 1:
			pin = EPS_SW1;
			//add =  INA226_ADDRESS_1;
			break;
		case 2:
			pin = EPS_SW2;
			//add =  INA226_ADDRESS_2;
			break;
		case 3:
			pin = EPS_SW3;
			//add =  INA226_ADDRESS_3;
			break;
		case 4:
			pin = EPS_SW4;
			//add =  INA226_ADDRESS_4;
			break;
		case 5:
			pin = EPS_SW5;
			//add =  INA226_ADDRESS_5;
			break;
		case 6:
			pin = PIN_D6;
			//add =  INA226_ADDRESS_6;
			break;
		default:
			pin = 0;
            cmd_ans_switch = "0";
			break;
	}

	if (pin)
	{
		//output_high(pin);
		switch(eps_state)
		{
			case 0:
				output_low(pin);
				break;
			case 1:
				output_high(pin);
				break;
			default:
				break;
		}
       cmd_ans_switch = "1"; 
	}
    cmd_dispatch(NODE, pkt->orgn, pkt->echo, RES, cmd_eps_switch, cmd_ans_switch);
}

void cmdimpl_eps_cut(cmdpkt_s* pkt){
if (pkt->ptype != REQ) return;
    char* p = pkt->args;
    char cmd_eps_cut[] = "eps_cut";
    char cmd_ans_cut[] = "";
	//unsigned int8 add;
    int16 shunt_voltage, bus_voltage, power, current;
	unsigned int8 add, sw_state;
	int pin, sw_pin, dp_pin, dp_flag;
    
    uint8_t cut_output = strtoul(p, &p, 10);
    uint8_t cut_state = strtoul(p, &p, 10);
    uint8_t cut_time = strtoul(p, &p, 10);

	switch(cut_output)
	{
		case 1:
			pin = PIN_E8;
			sw_pin = PIN_B9;
			dp_pin = PIN_B13;
			add =  INA226_ADDRESS_12;
			break;
		case 2:
			pin = PIN_E9;
			sw_pin = PIN_B10;
			dp_pin = PIN_B12;
			add =  INA226_ADDRESS_13;
			break;
		default:
			fprintf(COM_A,"CUT Output Error\n\r");
			pin = 0;
			break;
	}

	if (pin)
	{	
		switch(cut_state)
		{
			case 0:
				output_low(dp_pin);
				dp_flag = 1;
				break;
			case 1:
				output_high(dp_pin);
				dp_flag = 1;
				break;
			default:
				dp_flag = 0;
				fprintf(COM_A,"Switch Read Error\n\r");
				break;
		}

		
		if (dp_flag)
		{
			output_high(pin);
			delay_ms(100);
			sw_state = input(sw_pin);

			delay_ms(1000*cut_time);
		
			sw_state = input(sw_pin);
			ina226_read_data(add, &shunt_voltage, &bus_voltage, &power, &current);
		
			output_low(pin);
		
			delay_ms(100);

            sprintf(cmd_ans_cut,"%2X %2X %Ld %Ld %d\n\r", pin, dp_pin, bus_voltage, current, sw_state);
			fprintf(COM_A,cmd_ans_cut);
            //fprintf(COM_A,"ct,%2X,%2X,%Ld,%Ld,%d\n", pin, dp_pin, bus_voltage, current, sw_state);
            cmd_dispatch(NODE, pkt->orgn, pkt->echo, RES, cmd_eps_cut, cmd_ans_cut);
		}
		
	}
}

void cmdimpl_eps_state(cmdpkt_s* pkt){
if (pkt->ptype != REQ) return;
    char* p = pkt->args;
    uint8_t eps_in_state = strtoul(p, &p, 10);
  
}

void cmdimpl_eps_mode(cmdpkt_s* pkt){
if (pkt->ptype != REQ) return;
    char* p = pkt->args;
    uint8_t eps_in_mode = strtoul(p, &p, 10);
}
/*
void cmdimpl_tlm_get_data(cmdpkt_s* pkt) {
    if (pkt->ptype != REQ) return;
    sprintf(LOGBUF, "cmdimpl_tlm_get_data REQ"); log_info();
    char args[CMD_MAX_ARGS_LEN] = {0};
    sprintf(args, "%u %u", g_flashmgr.rbt_cnt, g_rbt_cause);
    cmd_dispatch(NODE, pkt->orgn, pkt->echo, RES, "tlm_get_data", args);
}

void cmdimpl_mtq_get_conf(cmdpkt_s* pkt) {
    if (pkt->ptype != REQ) return;
    uint8_t conf[4] = {0};
    if (mtq_get_data(&g_mtq, MTQ_CONF, conf) == SUCCESS) {
        char args[32] = {0};
        sprintf(args, "%u", conf[0]);
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, RES, "mtq_get_conf", args);
    } else {
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, NACK, "mtq_get_conf", "");
    }
}

void cmdimpl_mtq_set_conf(cmdpkt_s* pkt) {
    if (pkt->ptype != REQ) return;
    char* p = pkt->args;
    uint8_t data[4] = {0};
    data[3] = strtoul(p, &p, 10);
    data[2] = 0;
    data[1] = 0;
    data[0] = strtoul(p, &p, 10);
    
    if (mtq_write_start(&g_mtq, MTQ_CONF, data) == SUCCESS) { 
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, ACK, "mtq_set_conf", "");
    } else {
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, NACK, "mtq_set_conf", "");
    }
}

void cmdimpl_mtq_get_datetime(cmdpkt_s* pkt) {
    if (pkt->ptype != REQ) return;
    uint8_t date[4] = {0};
    uint8_t time[4] = {0};
    if (mtq_get_data(&g_mtq, MTQ_DATE, date) == SUCCESS && mtq_get_data(&g_mtq, MTQ_TIME, time) == SUCCESS) {
        char args[32] = {0}; 
        sprintf(args, "%u %u %u %u %u %u %u", 
                bcdtohex(date[0]), bcdtohex(date[2]), bcdtohex(date[1]), bcdtohex(date[3]),
                bcdtohex(time[3]), bcdtohex(time[2]), bcdtohex(time[1]));
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, RES, "mtq_get_datetime", args);
    } else {
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, NACK, "mtq_get_datetime", "");
    }
}

void cmdimpl_mtq_set_datetime(cmdpkt_s* pkt) {
    if (pkt->ptype != REQ) return;
    char* p = pkt->args;
    rtc_time_t time;
    time.tm_wday = strtoul(p, &p, 10); // Other options: strtok(), strtod(), strotol()
    time.tm_mon = strtoul(p, &p, 10); 
    time.tm_mday = strtoul(p, &p, 10);
    time.tm_year = strtoul(p, &p, 10);
    time.tm_hour = strtoul(p, &p, 10);
    time.tm_min = strtoul(p, &p, 10);
    time.tm_sec = strtoul(p, &p, 10);
    
    if (mtq_set_datetime(&g_mtq, &time) == SUCCESS) { 
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, ACK, "mtq_set_datetime", "");
    } else {
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, NACK, "mtq_set_datetime", "");
    }
}

void cmdimpl_mtq_get_tle(cmdpkt_s* pkt) {
    if (pkt->ptype != REQ) return;
    char tle[140] = {0};
    if (mtq_get_data(&g_mtq, MTQ_TLE, tle) == SUCCESS) {
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, RES, "mtq_get_tle", tle);
    } else {
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, NACK, "mtq_get_tle", "");
    }
}

void cmdimpl_mtq_set_tle(cmdpkt_s* pkt) {
    if (pkt->ptype != REQ) return;
    if ((mtq_write_start(&g_mtq, MTQ_TLE, pkt->args)) == SUCCESS) { 
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, ACK, "mtq_set_tle", "");
    } else {
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, NACK, "mtq_set_tle", "");
    }
}

void cmdimpl_mtq_get_paxs(cmdpkt_s* pkt) {
    if (pkt->ptype != REQ) return;
    float paxs[3] = {0};
    if (mtq_get_data(&g_mtq, MTQ_POINTING_AXIS, paxs) == SUCCESS) {
        char args[32] = {0};
        sprintf(args, "%.3f %.3f %.3f", paxs[0], paxs[1], paxs[2]);
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, RES, "mtq_get_paxs", args);
    } else {
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, NACK, "mtq_get_paxs", "");
    }
}

void cmdimpl_mtq_set_paxs(cmdpkt_s* pkt) {
    if (pkt->ptype != REQ) return;
    char* p = pkt->args;
    float data[3] = {0};
    data[0] = strtof(p, &p);
    data[1] = strtof(p, &p);
    data[2] = strtof(p, &p);
    
    if (mtq_write_start(&g_mtq, MTQ_POINTING_AXIS, data) == SUCCESS) { 
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, ACK, "mtq_set_paxs", "");
    } else {
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, NACK, "mtq_set_paxs", "");
    }
}

void cmdimpl_mtq_get_mtquser(cmdpkt_s* pkt) {
    if (pkt->ptype != REQ) return;
    float mtquser[3] = {0};
    if (mtq_get_data(&g_mtq, MTQ_MTQ_USER, mtquser) == SUCCESS) {
        char args[32] = {0};
        sprintf(args, "%.3f %.3f %.3f", mtquser[0], mtquser[1], mtquser[2]);
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, RES, "mtq_get_mtquser", args);
    } else {
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, NACK, "mtq_get_mtquser", "");
    }
}

void cmdimpl_mtq_set_mtquser(cmdpkt_s* pkt) {
    if (pkt->ptype != REQ) return;
    char* p = pkt->args;
    float data[3] = {0};
    data[0] = strtof(p, &p);
    data[1] = strtof(p, &p);
    data[2] = strtof(p, &p);
    
    if (mtq_write_start(&g_mtq, MTQ_MTQ_USER, data) == SUCCESS) { 
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, ACK, "mtq_set_mtquser", "");
    } else {
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, NACK, "mtq_set_mtquser", "");
    }
}

void cmdimpl_mtq_reset(cmdpkt_s* pkt) {
    if (pkt->ptype != REQ) return;
    if (mtq_reset(&g_mtq) == SUCCESS) {
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, ACK, "mtq_reset", "");
    } else {
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, NACK, "mtq_reset", "");
    }
}

void cmdimpl_mtq_get_stat(cmdpkt_s* pkt) {
    if (pkt->ptype != REQ) return;
    uint8_t stat[4] = {0};
    if (mtq_get_data(&g_mtq, MTQ_STAT, stat) == SUCCESS) {
        char args[32] = {0};
        sprintf(args, "%u %u %u %u", stat[0], stat[1], stat[2], stat[3]);
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, RES, "mtq_get_stat", args);
    } else {
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, NACK, "mtq_get_stat", "");
    }
}
*/