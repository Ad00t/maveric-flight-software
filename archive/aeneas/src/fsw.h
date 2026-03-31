// Helper File for Autocode
// TODO:  
//   Put library in standard format.  
/*

EVEN NEWER LIST OF POSTPROCESSING MODS TO FSW.C
Use this as of 10-21-2011.
- Moving of Definitions such as SCHEDULER_FREQ and NTASKS to this file.
- Move the globals ExtIn, ExtOut and ELAPSED_TIME to this file.
- Change value: const RT_DURATION TIME = SUBSYS_TIME[1]; --->  const RT_DURATION TIME  = 3.0;
- Global search/replace of "= -" with "= (-1.0)*" to avoid ocmpiler error
- Change the only call of Update_DS_With_Externals() to "if(TIME_COUNT>0.5) Update_DS_With_Externals();"
- Change all instances of R_P[x] to *(R_P+x) to avoid ridiculous "can't assign a 1" bug


NEW LIST OF POSTPROCESSING MODS TO FSW.C
Use this list if you used the c_idac.tpl template file for Autocoding as of 09-07-2011
- Moving of Definitions such as SCHEDULER_FREQ and NTASKS to this file.
- Move static RT_DURATION ELAPSED_TIME; above the #module tag and change RT_DURATION to double.
- Change value: const RT_DURATION TIME = SUBSYS_TIME[1]; --->  const RT_DURATION TIME  = 3.0;
- Commented out many Xtemp,i,j,k variables
- Global search/replace of "= -" with "= (-1.0)*" to avoid ocmpiler error
- Global search/replace of "RT_INTEGER INIT = iinfo[1];" with "RT_INTEGER INIT; INIT = iinfo[1];"  (Use search term "INIT = iinfo[1];" replace with "INIT; INIT = iinfo[1];"
- Global search/replace of "RT_INTEGER *iinfo = &I->iinfo[0];" with "RT_INTEGER *iinfo; iinfo = &I->iinfo[0];"
- Global search/replace of "iinfo[1] = 0;" with "//iinfo[1] = 0;"


LIST OF MODS TO FSW.C

- #include "fsw.h" added to top
- Addition of #module to top
- ExtIn and ExtOut declarations moved to top (above #module)
- Deletion of MAX,MIN,ROUND functions because they are in this file.
- Moving of Definitions such as SCHEDULER_FREQ and NTASKS to this file.
- const RT_DURATION TIME --->  static RT_DURATION TIME
- Commented out many Xtemp,i,j,k variables
- Global commenting out of extern declarations, because they do not have correct arguments
- Global search/replace of "= -" with "= (-1.0)*" to avoid ocmpiler error
- Global search/replace of "RT_INTEGER INIT = iinfo[1];" with "RT_INTEGER INIT; INIT = iinfo[1];"
- Global search/replace of "RT_INTEGER *iinfo = &I->iinfo[0];" with "RT_INTEGER *iinfo; iinfo = &I->iinfo[0];"
TODO:
- Replace static subsys_1_in/out with local versions.


Nathaniel's Mods:

Starting at FSW.c:8827

Changed really, really, ridiculously long line into more simple lines for the compiler.
This seemed to have to effect on fixing things.
We were fixing Body_{x,y,z}, but the value that has problems is Body_{x,y,z}_1...
Body_{x,y,z} seem to always be zero for the whole simulation, which must be a 
problem with the attitude control system that iControl folks mentioned.

*/



#ifndef __AENEAS__
#define __AENEAS__

#include "debug.h"
#include "ports.h"

//==============================================
//  				Public Functions
//==============================================
void initModel(void);
void SCHEDULER();
void GNCPrintData(char which=1);


//==============================================
//  				Definitions
//==============================================
// These come from FSW.c
// Move me to fsw.h --------------------------------------
#define SCHEDULER_FREQ                4.0
#define NTASKS                        1
#define NUMIN                         90
#define NUMOUT                        81
// -------------------------------------------------------


//=============================================
//                  Globals
//=============================================
float					ExtIn[NUMIN];
float					ExtOut[NUMOUT];
static double           ELAPSED_TIME;


// Input
// These come from "SystemBuild Vector Descriptions Helper.xls" in the Analysis section of SVN
#define Mode_Override_IN                            0
#define Mode_Limit_IN                               1
#define Min_Omega_IN                                2
#define Max_Omega_IN                                3
#define Sun_Search_Slew_Transition_IN               4
#define Min_Sun_Line_Trans_IN                       5
#define Min_Pitch_Trans_IN                          6
#define SP_Time_Limit_IN                            7
#define Enable_Momemtum_Dump_IN                     8
#define kpx_IN                                      9
#define kpy_IN                                      10
#define kpz_IN                                      11
#define krx_IN                                      12
#define kry_IN                                      13
#define krz_IN                                      14
#define Ixx_IN                                      15
#define Ixy_IN                                      16
#define Ixz_IN                                      17
#define Iyy_IN                                      18
#define Iyz_IN                                      19
#define Izz_IN                                      20
#define Slew_to_PD_Trans_IN                         21
#define Commanded_Aqsn_Slew_Rate_IN                 22
#define Rate_Limit_IN                               23
#define Q_Limit_IN                                  24
#define Wheel_Sat_Limit_IN                          25
#define kMOMx_IN                                    26
#define kMOMy_IN                                    27
#define kMOMz_IN                                    28
#define Ixx_Wheel_IN                                29
#define Ixy_Wheel_IN                                30
#define Ixz_Wheel_IN                                31
#define Iyy_Wheel_IN                                32
#define Iyz_Wheel_IN                                33
#define Izz_Wheel_IN                                34
#define Wheel_Enable_IN                             35
#define AD_Filter_Gain_Kf_IN                        36
#define AD_Filter_Pole_IN                           37
#define Whl_Filter_Gain_IN                          38
#define Whl_Filter_Pole_IN                          39
#define Reset_Estimator_IN                          40
#define q1_M_LV_IN                                  41
#define q2_M_LV_IN                                  42
#define q3_M_LV_IN                                  43
#define q4_M_LV_IN                                  44
#define Target_Lat_IN                               45
#define Target_Long_IN                              46
#define Target_Alt_IN                               47
#define Slew_Angle_IN                               48
#define SS_MisAln_x_IN                              49
#define SS_MisAln_y_IN                              50
#define SS_MisAln_z_IN                              51
#define Gyro_MisAln_x_IN                            52
#define Gyro_MisAln_y_IN                            53
#define Gyro_MisAln_z_IN                            54
#define Gyro_Drift_x_IN                             55
#define Gyro_Drift_y_IN                             56
#define Gyro_Drift_z_IN                             57
#define Gyro_AutoCal_IN                             58
#define MTR_Enable_Mode4_IN                         59
#define Drag_Coeff_IN                               60
#define SUNVEC0_IN                                  61
#define SUNVEC1_IN                                  62
#define SUNVEC2_IN                                  63
#define ECI_POS_X_IN                                64
#define ECI_POS_Y_IN                                65
#define ECI_POS_Z_IN                                66
#define ECI_VEL_X_IN                                67
#define ECI_VEL_Y_IN                                68
#define ECI_VEL_Z_IN                                69
#define GHA_IN                                      70
#define nT_ECI_x_IN                                 71
#define nT_ECI_y_IN                                 72
#define nT_ECI_z_IN                                 73
#define Measured_Gyro_X_IN                          74
#define Measured_Gyro_Y_IN                          75
#define Measured_Gyro_Z_IN                          76
#define Measured_Mag_X_IN                           77
#define Measured_Mag_Y_IN                           78
#define Measured_Mag_Z_IN                           79
#define Measured_SS_x_IN                            80
#define Measured_SS_y_IN                            81
#define Measured_SS_z_IN                            82
#define Sun_Present_Flag_IN                         83
#define Measured_RW_omega_x_IN                      84
#define Measured_RW_omega_y_IN                      85
#define Measured_RW_omega_z_IN                      86
#define Wheel_Disable_Mode5_IN                      87
#define Spare2_IN                                   88
#define Spare3_IN                                   89



// Output
// These come from "SystemBuild Vector Descriptions Helper.xls" in the Analysis section of SVN
#define Meas_Sun_Vector_B_x_OUT                      0
#define Meas_Sun_Vector_B_y_OUT                      1
#define Meas_Sun_Vector_B_z_OUT                      2
#define Sun_Sensor_Processing_7_1_OUT                3
#define Filt_Omega_B_N_B_x_OUT                       4
#define Filt_Omega_B_N_B_y_OUT                       5
#define Filt_Omega_B_N_B_z_OUT                       6
#define meas_mag_body_x_OUT                          7
#define meas_mag_body_y_OUT                          8
#define meas_mag_body_z_OUT                          9
#define Meas_Wheel_omega_rad_s_x_OUT                 10
#define Meas_Wheel_omega_rad_s_y_OUT                 11
#define Meas_Wheel_omega_rad_s_z_OUT                 12
#define SP_check_failed_OUT                          13
#define Rate_too_high_OUT                            14
#define True_Last_3_Samples_2_OUT                    15
#define True_Last_3_Samples_1_OUT                    16
#define Wheel_Hx_Body_Frame_OUT                      17
#define Wheel_Hy_Body_Frame_OUT                      18
#define Wheel_Hz_Body_Frame_OUT                      19
#define Mode_1_Idle_OUT                              20
#define Mode_2_Inertial_Capture_OUT                  21
#define Mode_3_Sun_Search_OUT                        22
#define Mode_4_Sun_Pointing_OUT                      23
#define Mode_5_Eclipse_Hold_OUT                      24
#define Mode_6_Quat_Hold_OUT                         25
#define Mode_7_Surface_Track_OUT                     26
#define Mode_8_Test_OUT                              27
#define Mode_9_Momentum_Dump_OUT                     28
#define True_Last_3_Samples_OUT                      29
#define q1_err_OUT                                   30
#define q2_err_OUT                                   31
#define q3_err_OUT                                   32
#define q4_err_OUT                                   33
#define wx_err_OUT                                   34
#define wy_err_OUT                                   35
#define wz_err_OUT                                   36
#define Torque_Rod_Enable_OUT                        37
#define Reset_Estimator_OUT                          38
#define Latch_2_1_OUT                                39
#define Save_Sun_Quaternion_OUT                      40
#define Approx_Angular_Err_frm_Sun_Lin_OUT           41
#define q1_OUT                                       42
#define q2_OUT                                       43
#define q3_OUT                                       44
#define q4_OUT                                       45
#define nadir_vect_x_OUT                             46
#define nadir_vect_y_OUT                             47
#define nadir_vect_z_OUT                             48
#define Body_Hx_ECI_Frame_OUT                        49
#define Body_Hy_ECI_Frame_OUT                        50
#define Body_Hz_ECI_Frame_OUT                        51
#define Mode_OUT                                     52
#define dzero_OUT                                    53
#define dzero_OUT                                    54
#define dzero_OUT                                    55
#define gyro_offset_x_OUT                            56
#define gyro_offset_y_OUT                            57
#define gyro_offset_z_OUT                            58
#define overwritepulse_OUT                           59
#define Requested_Torque_B_x_OUT                     60
#define Requested_Torque_B_y_OUT                     61
#define Requested_Torque_B_z_OUT                     62
#define PD_Control_OUT                               63
#define Mode_5_Wheel_Enable_OUT                      64
#define dzero_OUT                                    65
#define dzero_OUT                                    66
#define dzero_OUT                                    67
#define dzero_OUT                                    68
#define Cmded_Whl_Torque_x_OUT                       69
#define Cmded_Whl_Torque_y_OUT                       70
#define Cmded_Whl_Torque_z_OUT                       71
#define Cmd_Dipole_X_OUT                             72
#define Cmd_Dipole_Y_OUT                             73
#define Cmd_Dipole_Z_OUT                             74
#define Total_Hx_OUT                                 75
#define Total_Hy_OUT                                 76
#define Total_Hz_OUT                                 77
#define Cmd_MTR_counts_X_OUT                         78
#define Cmd_MTR_counts_Y_OUT                         79
#define Cmd_MTR_counts_Z_OUT                         80



/*
** Math Functions for Cubesat
*/

float MIN(float A, float B)
{
if(A<B)return A;
else return B;
}

float MAX(float A, float B)
{
if(A>B)return A;
else return B;
}

float ROUND(float A)   // need to double check for negative A.
{
	if(A>0)
		return (float)(long)(A+0.5);
	else
		return (float)(long)(A-0.5);
}


// Checks to see if the attitude quaternion has exploded and notifies the USER_PORT
ReturnErr_t safetyCheck()
{
	float norm;
//	const float LIMIT = 0.1;
	static int resetcounter=0;
	const int MAX_ALLOWED = 10;

	norm = sqrt(ExtOut[q1_OUT]*ExtOut[q1_OUT] + 
				ExtOut[q2_OUT]*ExtOut[q2_OUT] + 
				ExtOut[q3_OUT]*ExtOut[q3_OUT] + 
				ExtOut[q4_OUT]*ExtOut[q4_OUT]);
//	check = fabs(1-norm);
	
	if(norm==0.0) 
	{
		sendDBGALL(USER_PORT, "\r\nWarning! Quaternion error! ");
		resetcounter++;
		sprintf(dbgbuf,"%d of %d MAX!",resetcounter,MAX_ALLOWED);
		sendDBGALL(USER_PORT,dbgbuf);
		if(resetcounter>MAX_ALLOWED) 
		{
			sendDBGALL(USER_PORT, "\r\nRESET COMMANDED!");
			delay_ms(500);
			reset_cpu();
		}
		return FAILURE;
	}

	return SUCCESS;

}

#endif
