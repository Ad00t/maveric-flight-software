/****************************************************************************
|                   iDAC (TM) AutoCode(TM) Generated Code                   |
|                    ICONTROL INC., Santa Clara, CALIFORNIA                    |
*****************************************************************************
Dodona GNC Flight Code 1/14/19
Author: KC & JJA

Changes from Aeneas: 
- Adds max/min torque limiter 
- Fixes Controller signs for reaction wheels
- Adds Bdot controller for detumbling

Best of luck Dodona - Fly Safe

rtf filename           : FSW.rtf
Filename               : FSW.c
Dac filename           : c_idac.dac
Generated on           : Mon Jan 14 13:47:40 2019
Dac file created on    : Mon Jan 14 13:47:40 2019
TPL file SCCS version  : 1.2
--
--   Number of External Inputs : 90
--   Number of External Outputs: 81
--
--   Scheduler Frequency:    4.0
--
--   SUBSYSTEM  FREQUENCY  TIME_SKEW  OUTPUT_TIME  TASK_TYPE
--   ---------  ---------  ---------  -----------  ---------
--   1          4.0        0.0        0.0          PERIODIC


*/
#include "fsw.h"

#module


/*
** typedefs
*/
typedef int   RT_INTEGER;
typedef char  RT_BOOLEAN;
typedef float RT_FLOAT;
typedef double RT_DURATION;
typedef float VAR_FLOAT;


typedef struct STATUS_RECORD { 
        RT_INTEGER      ERROR;
        RT_INTEGER      INIT;
        RT_INTEGER      STATES;
        RT_INTEGER      OUTPUTS;
} RT_STATUS_RECORD;

/*
** Globals
*/
RT_INTEGER cnt;











/*** System Data ***/

/******* Structure to drive disconnected input/output. *******/
struct _DcZero {
   RT_FLOAT dzero;
};


static const struct _DcZero dczero = {0.0};




#define EPSILON                      1.4901161193847656E-008
#define EPS                          (4.0 * EPSILON)
#define ABSTOL                       EPSILON
#define XREMAP						 1


enum TASK_STATE_TYPE { IDLE, RUNNING, BLOCKED, UNALLOCATED };

static RT_INTEGER                     ERROR_FLAG  [NTASKS+1];
static RT_BOOLEAN                     SUBSYS_PREINIT [NTASKS+1];
static RT_BOOLEAN                     SUBSYS_INIT [NTASKS+1];
static RT_DURATION                    SUBSYS_TIME [NTASKS+1];
static enum TASK_STATE_TYPE           TASK_STATE  [NTASKS+1];

/******* DataStore type declarations. *******/
struct _System_Parameters_1 {
   RT_FLOAT Mode_Override;
   RT_FLOAT Mode_Limit;
   RT_FLOAT Min_Omega;
   RT_FLOAT Max_Omega;
   RT_FLOAT Sun_Search_Slew_Trans;
   RT_FLOAT Min_Sun_Line_Trans;
   RT_FLOAT Min_Pitch_Trans;
   RT_FLOAT SP_time_limit;
   RT_FLOAT Enable_Momentum_Dump;
   RT_FLOAT Kpx;
   RT_FLOAT Kpy;
   RT_FLOAT Kpz;
   RT_FLOAT Krx;
   RT_FLOAT Kry;
   RT_FLOAT Krz;
   RT_FLOAT Ixx;
   RT_FLOAT Ixy;
   RT_FLOAT Ixz;
   RT_FLOAT Iyy;
   RT_FLOAT Iyz;
   RT_FLOAT Izz;
   RT_FLOAT Ixx_Depl;
   RT_FLOAT Ixy_Depl;
   RT_FLOAT Ixz_Depl;
   RT_FLOAT Iyy_Depl;
   RT_FLOAT Iyz_Depl;
   RT_FLOAT Izz_Depl;
   RT_FLOAT Slew_to_PD_Trans;
   RT_FLOAT Commanded_Aqsn_Slew_Rate;
   RT_FLOAT Rate_limit;
   RT_FLOAT Q_limit;
   RT_FLOAT Wheel_Sat_Limit;
   RT_FLOAT kMOMx;
   RT_FLOAT kMOMy;
   RT_FLOAT kMOMz;
   RT_FLOAT cgx_Depl;
   RT_FLOAT cgy_Depl;
   RT_FLOAT cgz_Depl;
   RT_FLOAT Deploy;
};

struct _System_Parameters_2 {
   RT_FLOAT On_Time_Limit;
   RT_FLOAT Kr_Wheel_X;
   RT_FLOAT Kr_Wheel_Y;
   RT_FLOAT Kr_Wheel_Z;
   RT_FLOAT Set_Wheel_Speed_X;
   RT_FLOAT Set_Wheel_Speed_Y;
   RT_FLOAT Set_Wheel_Speed_Z;
   RT_FLOAT Max_Wheel_Speed_X;
   RT_FLOAT Max_Wheel_Speed_Y;
   RT_FLOAT Max_Wheel_Speed_Z;
   RT_FLOAT I_Wheel_xx;
   RT_FLOAT I_Wheel_xy;
   RT_FLOAT I_Wheel_xz;
   RT_FLOAT I_Wheel_yy;
   RT_FLOAT I_Wheel_yz;
   RT_FLOAT I_Wheel_zz;
   RT_FLOAT Inv_I_Wheel_11;
   RT_FLOAT Inv_I_Wheel_12;
   RT_FLOAT Inv_I_Wheel_13;
   RT_FLOAT Inv_I_Wheel_22;
   RT_FLOAT Inv_I_Wheel_23;
   RT_FLOAT Inv_I_Wheel_33;
   RT_FLOAT TRBW11;
   RT_FLOAT TRBW12;
   RT_FLOAT TRBW13;
   RT_FLOAT TRBW21;
   RT_FLOAT TRBW22;
   RT_FLOAT TRBW23;
   RT_FLOAT TRBW31;
   RT_FLOAT TRBW32;
   RT_FLOAT TRBW33;
   RT_FLOAT TRWB11;
   RT_FLOAT TRWB12;
   RT_FLOAT TRWB13;
   RT_FLOAT TRWB21;
   RT_FLOAT TRWB22;
   RT_FLOAT TRWB23;
   RT_FLOAT TRWB31;
   RT_FLOAT TRWB32;
   RT_FLOAT TRWB33;
   RT_FLOAT Wheel_Enable;
   RT_FLOAT Wheel_Speed_Reset;
   RT_FLOAT Kwi;
};

struct _System_Parameters_3 {
   RT_FLOAT AD_Filter_Gain_Kf;
   RT_FLOAT AD_Filter_Pole_pf;
   RT_FLOAT Whl_Filter_Gain_Kf;
   RT_FLOAT Whl_Filter_Pole_pf;
   RT_FLOAT Reset_Estimator;
   RT_FLOAT Enable_AD_Pos_Updates;
   RT_FLOAT Wheel_Disable_Mode5;
   RT_FLOAT q1_M_LV;
   RT_FLOAT q2_M_LV;
   RT_FLOAT q3_M_LV;
   RT_FLOAT q4_M_LV;
   RT_FLOAT Spare2;
   RT_FLOAT q1_M_ECI;
   RT_FLOAT q2_M_ECI;
   RT_FLOAT q3_M_ECI;
   RT_FLOAT q4_M_ECI;
   RT_FLOAT Spare3;
   RT_FLOAT Target_Longitude;
   RT_FLOAT Target_Latitude;
   RT_FLOAT Target_Altitude;
   RT_FLOAT Gimble_Calculation_Enable;
   RT_FLOAT Spare4;
   RT_FLOAT Slew_Acceleration;
   RT_FLOAT Slew_Rate;
   RT_FLOAT Slew_Angle;
   RT_FLOAT Euler_Slew_Axis_x;
   RT_FLOAT Euler_Slew_Axis_y;
   RT_FLOAT Euler_Slew_Axis_z;
   RT_FLOAT Spare5;
   RT_FLOAT Sun_Sensor_MisAlmt_x;
   RT_FLOAT Sun_Sensor_MisAlmt_y;
   RT_FLOAT Sun_Sensor_MisAlmt_z;
   RT_FLOAT Spare6;
   RT_FLOAT Magnetometer_Mis_x;
   RT_FLOAT Magnetometer_Mis_y;
   RT_FLOAT Magnetometer_Mis_z;
   RT_FLOAT Magnetometer_Bias_x;
   RT_FLOAT Magnetometer_Bias_y;
   RT_FLOAT Magnetometer_Bias_z;
   RT_FLOAT Orbit_Rate;
   RT_FLOAT Gyro_MisAlmt_x;
   RT_FLOAT Gyro_MisAlmt_y;
   RT_FLOAT Gyro_MisAlmt_z;
   RT_FLOAT Gyro_Drift_x;
   RT_FLOAT Gyro_Drift_y;
   RT_FLOAT Gyro_Drift_z;
   RT_FLOAT Gyro_AutoCalibrate;
   RT_FLOAT Mode4_MTR_Enable;
   RT_FLOAT Spare8;
   RT_FLOAT Spare9;
   RT_FLOAT Drag_Coef;
   RT_FLOAT Cross_Section_Area_XY;
   RT_FLOAT Cross_Section_Area_Antenna;
   RT_FLOAT Half_MTR_Dipole_Am2;
};

struct _gyro {
   RT_FLOAT gyro_offset_x;
   RT_FLOAT gyro_offset_y;
   RT_FLOAT gyro_offset_z;
   RT_FLOAT n;
   RT_FLOAT overwritepulse;
};

struct _Internal {
   RT_FLOAT Saved_Sun_Vector_q1;
   RT_FLOAT Saved_Sun_Vector_q2;
   RT_FLOAT Saved_Sun_Vector_q3;
   RT_FLOAT Saved_Sun_Vector_q4;
};

/****** System Ext I/O and Sample-Hold type declarations.******/
struct _Sys_ExtOut {
   RT_FLOAT Meas_Sun_Vector_B_x;
   RT_FLOAT Meas_Sun_Vector_B_y;
   RT_FLOAT Meas_Sun_Vector_B_z;
   RT_FLOAT Sun_Sensor_Processing_7_1;
   RT_FLOAT Filt_Omega_B_N_B_x;
   RT_FLOAT Filt_Omega_B_N_B_y;
   RT_FLOAT Filt_Omega_B_N_B_z;
   RT_FLOAT meas_mag_body_x;
   RT_FLOAT meas_mag_body_y;
   RT_FLOAT meas_mag_body_z;
   RT_FLOAT Meas_Wheel_omega_rad_s_x;
   RT_FLOAT Meas_Wheel_omega_rad_s_y;
   RT_FLOAT Meas_Wheel_omega_rad_s_z;
   RT_FLOAT SP_check_failed;
   RT_FLOAT Rate_too_high;
   RT_FLOAT True_Last_3_Samples;
   RT_FLOAT True_Last_3_Samples_1;
   RT_FLOAT Wheel_Hx_Body_Frame;
   RT_FLOAT Wheel_Hy_Body_Frame;
   RT_FLOAT Wheel_Hz_Body_Frame;
   RT_FLOAT Mode_1_Idle;
   RT_FLOAT Mode_2_Inertial_Capture;
   RT_FLOAT Mode_3_Sun_Search;
   RT_FLOAT Mode_4_Sun_Pointing;
   RT_FLOAT Mode_5_Eclipse_Hold;
   RT_FLOAT Mode_6_Quat_Hold;
   RT_FLOAT Mode_7_Surface_Track;
   RT_FLOAT Mode_8_Test;
   RT_FLOAT Mode_9_Momentum_Dump;
   RT_FLOAT True_Last_3_Samples_2;
   RT_FLOAT q1_err;
   RT_FLOAT q2_err;
   RT_FLOAT q3_err;
   RT_FLOAT q4_err;
   RT_FLOAT wx_err;
   RT_FLOAT wy_err;
   RT_FLOAT wz_err;
   RT_FLOAT Torque_Rod_Enable;
   RT_FLOAT Reset_Estimator;
   RT_FLOAT Latch_2_1;
   RT_FLOAT Save_Sun_Quaternion;
   RT_FLOAT Approx_Angular_Err_frm_Sun_Lin;
   RT_FLOAT q1;
   RT_FLOAT q2;
   RT_FLOAT q3;
   RT_FLOAT q4;
   RT_FLOAT nadir_vect_x;
   RT_FLOAT nadir_vect_y;
   RT_FLOAT nadir_vect_z;
   RT_FLOAT Body_Hx_ECI_Frame;
   RT_FLOAT Body_Hy_ECI_Frame;
   RT_FLOAT Body_Hz_ECI_Frame;
   RT_FLOAT Mode;
   RT_FLOAT dzero;
   RT_FLOAT dzero_1;
   RT_FLOAT dzero_2;
   RT_FLOAT gyro_offset_x;
   RT_FLOAT gyro_offset_y;
   RT_FLOAT gyro_offset_z;
   RT_FLOAT overwritepulse;
   RT_FLOAT Requested_Torque_B_x;
   RT_FLOAT Requested_Torque_B_y;
   RT_FLOAT Requested_Torque_B_z;
   RT_FLOAT PD_Control;
   RT_FLOAT Mode_5_Wheel_Enable;
   RT_FLOAT dzero_3;
   RT_FLOAT dzero_4;
   RT_FLOAT dzero_5;
   RT_FLOAT dzero_6;
   RT_FLOAT Cmded_Whl_Torque_x;
   RT_FLOAT Cmded_Whl_Torque_y;
   RT_FLOAT Cmded_Whl_Torque_z;
   RT_FLOAT Cmd_Dipole_X;
   RT_FLOAT Cmd_Dipole_Y;
   RT_FLOAT Cmd_Dipole_Z;
   RT_FLOAT Total_Hx;
   RT_FLOAT Total_Hy;
   RT_FLOAT Total_Hz;
   RT_FLOAT Cmd_MTR_counts_X;
   RT_FLOAT Cmd_MTR_counts_Y;
   RT_FLOAT Cmd_MTR_counts_Z;
};

struct _Sys_ExtIn {
   RT_FLOAT Mode_Override;
   RT_FLOAT Mode_Limit;
   RT_FLOAT Min_Omega;
   RT_FLOAT Max_Omega;
   RT_FLOAT Sun_Search_Slew_Transition;
   RT_FLOAT Min_Sun_Line_Trans;
   RT_FLOAT Min_Pitch_Trans;
   RT_FLOAT SP_Time_Limit;
   RT_FLOAT Enable_Momemtum_Dump;
   RT_FLOAT kpx;
   RT_FLOAT kpy;
   RT_FLOAT kpz;
   RT_FLOAT krx;
   RT_FLOAT kry;
   RT_FLOAT krz;
   RT_FLOAT Ixx;
   RT_FLOAT Ixy;
   RT_FLOAT Ixz;
   RT_FLOAT Iyy;
   RT_FLOAT Iyz;
   RT_FLOAT Izz;
   RT_FLOAT Slew_to_PD_Trans;
   RT_FLOAT Commanded_Aqsn_Slew_Rate;
   RT_FLOAT Rate_Limit;
   RT_FLOAT Q_Limit;
   RT_FLOAT Wheel_Sat_Limit;
   RT_FLOAT kMOMx;
   RT_FLOAT kMOMy;
   RT_FLOAT kMOMz;
   RT_FLOAT Ixx_Wheel;
   RT_FLOAT Ixy_Wheel;
   RT_FLOAT Ixz_Wheel;
   RT_FLOAT Iyy_Wheel;
   RT_FLOAT Iyz_Wheel;
   RT_FLOAT Izz_Wheel;
   RT_FLOAT Wheel_Enable;
   RT_FLOAT AD_Filter_Gain_Kf;
   RT_FLOAT AD_Filter_Pole;
   RT_FLOAT Whl_Filter_Gain;
   RT_FLOAT Whl_Filter_Pole;
   RT_FLOAT Reset_Estimator;
   RT_FLOAT q1_M_LV;
   RT_FLOAT q2_M_LV;
   RT_FLOAT q3_M_LV;
   RT_FLOAT q4_M_LV;
   RT_FLOAT Target_Lat;
   RT_FLOAT Target_Long;
   RT_FLOAT Target_Alt;
   RT_FLOAT Slew_Angle;
   RT_FLOAT SS_MisAln_x;
   RT_FLOAT SS_MisAln_y;
   RT_FLOAT SS_MisAln_z;
   RT_FLOAT Gyro_MisAln_x;
   RT_FLOAT Gyro_MisAln_y;
   RT_FLOAT Gyro_MisAln_z;
   RT_FLOAT Gyro_Drift_x;
   RT_FLOAT Gyro_Drift_y;
   RT_FLOAT Gyro_Drift_z;
   RT_FLOAT Gyro_AutoCal;
   RT_FLOAT MTR_Enable_Mode4;
   RT_FLOAT Drag_Coeff;
   RT_FLOAT SUNVEC0;
   RT_FLOAT SUNVEC1;
   RT_FLOAT SUNVEC2;
   RT_FLOAT ECI_POS_X;
   RT_FLOAT ECI_POS_Y;
   RT_FLOAT ECI_POS_Z;
   RT_FLOAT ECI_VEL_X;
   RT_FLOAT ECI_VEL_Y;
   RT_FLOAT ECI_VEL_Z;
   RT_FLOAT GHA;
   RT_FLOAT nT_ECI_x;
   RT_FLOAT nT_ECI_y;
   RT_FLOAT nT_ECI_z;
   RT_FLOAT Measured_Gyro_X;
   RT_FLOAT Measured_Gyro_Y;
   RT_FLOAT Measured_Gyro_Z;
   RT_FLOAT Measured_Mag_X;
   RT_FLOAT Measured_Mag_Y;
   RT_FLOAT Measured_Mag_Z;
   RT_FLOAT Measured_SS_x;
   RT_FLOAT Measured_SS_y;
   RT_FLOAT Measured_SS_z;
   RT_FLOAT Sun_Present_Flag;
   RT_FLOAT Measured_RW_omega_x;
   RT_FLOAT Measured_RW_omega_y;
   RT_FLOAT Measured_RW_omega_z;
   RT_FLOAT Wheel_Disable_Mode5;
   RT_FLOAT Spare2;
   RT_FLOAT Spare3;
};

struct _Subsys_1_in {
   RT_FLOAT MTR_Enable_Mode4;
   RT_FLOAT SUNVEC0;
   RT_FLOAT SUNVEC1;
   RT_FLOAT SUNVEC2;
   RT_FLOAT ECI_POS_X;
   RT_FLOAT ECI_POS_Y;
   RT_FLOAT ECI_POS_Z;
   RT_FLOAT ECI_VEL_X;
   RT_FLOAT ECI_VEL_Y;
   RT_FLOAT ECI_VEL_Z;
   RT_FLOAT GHA;
   RT_FLOAT nT_ECI_x;
   RT_FLOAT nT_ECI_y;
   RT_FLOAT nT_ECI_z;
   RT_FLOAT Measured_Gyro_X;
   RT_FLOAT Measured_Gyro_Y;
   RT_FLOAT Measured_Gyro_Z;
   RT_FLOAT Measured_Mag_X;
   RT_FLOAT Measured_Mag_Y;
   RT_FLOAT Measured_Mag_Z;
   RT_FLOAT Measured_SS_x;
   RT_FLOAT Measured_SS_y;
   RT_FLOAT Measured_SS_z;
   RT_FLOAT Sun_Present_Flag;
   RT_FLOAT Measured_RW_omega_x;
   RT_FLOAT Measured_RW_omega_y;
   RT_FLOAT Measured_RW_omega_z;
   RT_FLOAT Mode_Override;
   RT_FLOAT Mode_Limit;
   RT_FLOAT Min_Omega;
   RT_FLOAT Max_Omega;
   RT_FLOAT Sun_Search_Slew_Trans;
   RT_FLOAT Min_Sun_Line_Trans;
   RT_FLOAT SP_time_limit;
   RT_FLOAT Enable_Momentum_Dump;
   RT_FLOAT Kpx;
   RT_FLOAT Kpy;
   RT_FLOAT Kpz;
   RT_FLOAT Krx;
   RT_FLOAT Kry;
   RT_FLOAT Krz;
   RT_FLOAT Ixx;
   RT_FLOAT Ixy;
   RT_FLOAT Ixz;
   RT_FLOAT Iyy;
   RT_FLOAT Iyz;
   RT_FLOAT Izz;
   RT_FLOAT Ixx_Depl;
   RT_FLOAT Ixy_Depl;
   RT_FLOAT Ixz_Depl;
   RT_FLOAT Iyy_Depl;
   RT_FLOAT Iyz_Depl;
   RT_FLOAT Izz_Depl;
   RT_FLOAT Slew_to_PD_Trans;
   RT_FLOAT Commanded_Aqsn_Slew_Rate;
   RT_FLOAT Rate_limit;
   RT_FLOAT Q_limit;
   RT_FLOAT Wheel_Sat_Limit;
   RT_FLOAT kMOMx;
   RT_FLOAT kMOMy;
   RT_FLOAT kMOMz;
   RT_FLOAT Deploy;
   RT_FLOAT I_Wheel_xx;
   RT_FLOAT I_Wheel_yy;
   RT_FLOAT I_Wheel_zz;
   RT_FLOAT Wheel_Enable;
   RT_FLOAT AD_Filter_Gain_Kf;
   RT_FLOAT AD_Filter_Pole_pf;
   RT_FLOAT Reset_Estimator;
   RT_FLOAT Wheel_Disable_Mode5;
   RT_FLOAT q1_M_LV;
   RT_FLOAT q2_M_LV;
   RT_FLOAT q3_M_LV;
   RT_FLOAT q4_M_LV;
   RT_FLOAT Target_Longitude;
   RT_FLOAT Target_Latitude;
   RT_FLOAT Target_Altitude;
   RT_FLOAT Slew_Angle;
   RT_FLOAT Sun_Sensor_MisAlmt_x;
   RT_FLOAT Sun_Sensor_MisAlmt_y;
   RT_FLOAT Sun_Sensor_MisAlmt_z;
   RT_FLOAT Gyro_MisAlmt_x;
   RT_FLOAT Gyro_MisAlmt_y;
   RT_FLOAT Gyro_MisAlmt_z;
   RT_FLOAT Gyro_Drift_x;
   RT_FLOAT Gyro_Drift_y;
   RT_FLOAT Gyro_Drift_z;
   RT_FLOAT Gyro_AutoCalibrate;
   RT_FLOAT gyro_offset_x;
   RT_FLOAT gyro_offset_y;
   RT_FLOAT gyro_offset_z;
   RT_FLOAT n;
   RT_FLOAT overwritepulse;
   RT_FLOAT Saved_Sun_Vector_q1;
   RT_FLOAT Saved_Sun_Vector_q2;
   RT_FLOAT Saved_Sun_Vector_q3;
   RT_FLOAT Saved_Sun_Vector_q4;
};

struct _Subsys_1_out {
   RT_FLOAT PD_Control;
   RT_FLOAT Approx_Angular_Err_frm_Sun_Lin;
   RT_FLOAT Filt_Omega_B_N_B_x;
   RT_FLOAT Filt_Omega_B_N_B_y;
   RT_FLOAT Filt_Omega_B_N_B_z;
   RT_FLOAT Meas_Sun_Vector_B_x;
   RT_FLOAT Meas_Sun_Vector_B_y;
   RT_FLOAT Meas_Sun_Vector_B_z;
   RT_FLOAT Sun_Sensor_Processing_7_1;
   RT_FLOAT meas_mag_body_x;
   RT_FLOAT meas_mag_body_y;
   RT_FLOAT meas_mag_body_z;
   RT_FLOAT Meas_Wheel_omega_rad_s_x;
   RT_FLOAT Meas_Wheel_omega_rad_s_y;
   RT_FLOAT Meas_Wheel_omega_rad_s_z;
   RT_FLOAT Wheel_Hx_Body_Frame;
   RT_FLOAT Wheel_Hy_Body_Frame;
   RT_FLOAT Wheel_Hz_Body_Frame;
   RT_FLOAT True_Last_3_Samples;
   RT_FLOAT Compute_Gyro_Offset_21_1;
   RT_FLOAT Compute_Gyro_Offset_21_2;
   RT_FLOAT Compute_Gyro_Offset_21_3;
   RT_FLOAT Compute_Gyro_Offset_21_4;
   RT_FLOAT Compute_Gyro_Offset_21_5;
   RT_FLOAT SP_check_failed;
   RT_FLOAT True_Last_3_Samples_1;
   RT_FLOAT Mode_1_Idle;
   RT_FLOAT Mode_2_Inertial_Capture;
   RT_FLOAT Mode_3_Sun_Search;
   RT_FLOAT Mode_4_Sun_Pointing;
   RT_FLOAT Mode_5_Eclipse_Hold;
   RT_FLOAT Mode_6_Quat_Hold;
   RT_FLOAT Mode_7_Surface_Track;
   RT_FLOAT Mode_8_Test;
   RT_FLOAT Mode_9_Momentum_Dump;
   RT_FLOAT Latch_2_1;
   RT_FLOAT q1;
   RT_FLOAT q2;
   RT_FLOAT q3;
   RT_FLOAT q4;
   RT_FLOAT q1_err;
   RT_FLOAT q2_err;
   RT_FLOAT q3_err;
   RT_FLOAT q4_err;
   RT_FLOAT wx_err;
   RT_FLOAT wy_err;
   RT_FLOAT wz_err;
   RT_FLOAT Torque_Rod_Enable;
   RT_FLOAT Body_Hx_ECI_Frame;
   RT_FLOAT Body_Hy_ECI_Frame;
   RT_FLOAT Body_Hz_ECI_Frame;
   RT_FLOAT Total_Hx;
   RT_FLOAT Total_Hy;
   RT_FLOAT Total_Hz;
   RT_FLOAT Requested_Torque_B_x;
   RT_FLOAT Requested_Torque_B_y;
   RT_FLOAT Requested_Torque_B_z;
   RT_FLOAT nadir_vect_x;
   RT_FLOAT nadir_vect_y;
   RT_FLOAT nadir_vect_z;
   RT_FLOAT Save_Sun_Quaternion;
   RT_FLOAT Reset_Estimator;
   RT_FLOAT Mode_5_Wheel_Enable;
   RT_FLOAT Cmded_Whl_Torque_x;
   RT_FLOAT Cmded_Whl_Torque_y;
   RT_FLOAT Cmded_Whl_Torque_z;
   RT_FLOAT Cmd_Dipole_X;
   RT_FLOAT Cmd_Dipole_Y;
   RT_FLOAT Cmd_Dipole_Z;
   RT_FLOAT Cmd_MTR_counts_X;
   RT_FLOAT Cmd_MTR_counts_Y;
   RT_FLOAT Cmd_MTR_counts_Z;
   RT_FLOAT Mode;
   RT_FLOAT Rate_too_high;
   RT_FLOAT True_Last_3_Samples_2;
   RT_FLOAT FSW_93_1;
   RT_FLOAT FSW_93_2;
};


/******* DataStore definitions. *******/
/* System Parameters 1 */
struct _System_Parameters_1 System_Parameters_1;

/* System Parameters 2 */
struct _System_Parameters_2 System_Parameters_2;

/* System Parameters 3 */
struct _System_Parameters_3 System_Parameters_3;

/* gyro */
struct _gyro gyro;

/* Internal */
struct _Internal Internal;

/**** System Ext I/O and Subsystem I/O type definitions and     ****
 **** Pointers to SubSystem Outputs ReadOnly/Work areas.        ****/
struct _Sys_ExtOut sys_extout;
struct _Sys_ExtIn sys_extin;
struct _Subsys_1_in subsys_1_in;
struct _Subsys_1_out subsys_1_out;







/******** Procedures' declarations ********/

/******* Procedure: Toggle_Down_To_Pulse *******/

/***** Inputs type declaration. *****/
struct _Toggle_Down_To_Pulse_u {
   RT_FLOAT overwritepulse;
};


/***** Outputs type declaration. *****/
struct _Toggle_Down_To_Pulse_y {
   RT_FLOAT Toggle_Down_To_Pulse_13_1;
};


/***** Private States type declaration. *****/
struct _Toggle_Down_To_Pulse_ps {
   RT_FLOAT Toggle_Down_To_Pulse_20_S1;
};


/***** States type declaration. *****/
struct _Toggle_Down_To_Pulse_s {
   struct _Toggle_Down_To_Pulse_ps Toggle_Down_To_Pulse_ps[2];
   RT_INTEGER Toggle_Down_To_Pulse_x;
};



//extern void Toggle_Down_To_Pulse(); //jja commented out b/c pic wants function arguments even for externs

/******* Procedure: Compute_Bogus_Sun_Quaternion *******/

/***** Inputs type declaration. *****/
struct _Compute_Bogus_Sun_Quaternion_u {
   RT_FLOAT Sine_of_Euler_Rotation_Angle;
   RT_FLOAT Meas_Sun_Vector_B_x;
   RT_FLOAT Meas_Sun_Vector_B_y;
};


/***** Outputs type declaration. *****/
struct _Compute_Bogus_Sun_Quaternion_y {
   RT_FLOAT q1_Bogus_Sun_N_B;
   RT_FLOAT q2_Bogus_Sun_N_B;
   RT_FLOAT q3_Bogus_Sun_N_B;
   RT_FLOAT q4_Bogus_Sun_N_B;
};



//extern void Compute_Bogus_Sun_Quaternion(); //jja commented out b/c pic wants function arguments even for externs

/******* Procedure: mth_quat_norm *******/

/***** Inputs type declaration. *****/
struct _mth_quat_norm_u {
   RT_FLOAT DQ1;
   RT_FLOAT DQ2;
   RT_FLOAT DQ3;
   RT_FLOAT DQ4;
};


/***** Outputs type declaration. *****/
struct _mth_quat_norm_y {
   RT_FLOAT mth_quat_norm_4_1;
   RT_FLOAT mth_quat_norm_4_2;
   RT_FLOAT mth_quat_norm_4_3;
   RT_FLOAT mth_quat_norm_4_4;
};



//extern void mth_quat_norm(); //jja commented out b/c pic wants function arguments even for externs

/******* Procedure: Quaternion_Difference_P *******/

/***** Inputs type declaration. *****/
struct _Quaternion_Difference_P_u {
   RT_FLOAT Q1_actual;
   RT_FLOAT Q2_actual;
   RT_FLOAT Q3_actual;
   RT_FLOAT Q4_actual;
   RT_FLOAT Q1_desired;
   RT_FLOAT Q2_desired;
   RT_FLOAT Q3_desired;
   RT_FLOAT Q4_desired;
};


/***** Outputs type declaration. *****/
struct _Quaternion_Difference_P_y {
   RT_FLOAT mth_quat_norm_99_1;
   RT_FLOAT mth_quat_norm_99_2;
   RT_FLOAT mth_quat_norm_99_3;
   RT_FLOAT mth_quat_norm_99_4;
};



//extern void Quaternion_Difference_P(); //jja commented out b/c pic wants function arguments even for externs

/******* Procedure: mth_vect_norm *******/

/***** Inputs type declaration. *****/
struct _mth_vect_norm_u {
   RT_FLOAT Measured_magnetometer_body_x;
   RT_FLOAT Measured_magnetometer_body_y;
   RT_FLOAT Measured_magnetometer_body_z;
};


/***** Outputs type declaration. *****/
struct _mth_vect_norm_y {
   RT_FLOAT X_1;
   RT_FLOAT Y_1;
   RT_FLOAT Z;
   RT_FLOAT Magnitude;
};



//extern void mth_vect_norm(); //jja commented out b/c pic wants function arguments even for externs

/******* Procedure: Compute_Gyro_Offset *******/

/***** Inputs type declaration. *****/
struct _Compute_Gyro_Offset_u {
   RT_FLOAT Meas_Sun_Vector_B_x;
   RT_FLOAT Meas_Sun_Vector_B_y;
   RT_FLOAT gyro_offset_x;
   RT_FLOAT gyro_offset_y;
   RT_FLOAT gyro_offset_z;
   RT_FLOAT Filt_Omega_B_N_B_x;
   RT_FLOAT Filt_Omega_B_N_B_y;
   RT_FLOAT Filt_Omega_B_N_B_z;
   RT_FLOAT n;
   RT_FLOAT Enable_1;
   RT_FLOAT Meas_Sun_Vector_B_z;
};


/***** Outputs type declaration. *****/
struct _Compute_Gyro_Offset_y {
   RT_FLOAT Compute_Gyro_Offset_19_1;
   RT_FLOAT Compute_Gyro_Offset_19_2;
   RT_FLOAT Compute_Gyro_Offset_19_3;
   RT_FLOAT Compute_Gyro_Offset_19_4;
   RT_FLOAT Compute_Gyro_Offset_99_1;
};


/***** Private States type declaration. *****/
struct _Compute_Gyro_Offset_ps {
   RT_FLOAT Compute_Gyro_Offset_1_S1;
   RT_FLOAT Compute_Gyro_Offset_1_S2;
   RT_FLOAT Compute_Gyro_Offset_1_S3;
   RT_FLOAT Compute_Gyro_Offset_1_S4;
   RT_FLOAT Old_Difference_S1;
   RT_FLOAT Old_Difference_S2;
   RT_FLOAT Old_Difference_S3;
   RT_FLOAT Compute_Gyro_Offset_20_S1;
   RT_FLOAT Previous_S1;
};


/***** States type declaration. *****/
struct _Compute_Gyro_Offset_s {
   struct _Compute_Gyro_Offset_ps Compute_Gyro_Offset_ps[2];
   RT_INTEGER Compute_Gyro_Offset_x;
};



//extern void Compute_Gyro_Offset(); //jja commented out b/c pic wants function arguments even for externs

/******* Procedure: Quaternion_Transformation_P *******/

/***** Inputs type declaration. *****/
struct _Quaternion_Transformation_P_u {
   RT_FLOAT q1_B_N;
   RT_FLOAT q2_B_N;
   RT_FLOAT q3_B_N;
   RT_FLOAT q4_B_N;
   RT_FLOAT Inertial_x;
   RT_FLOAT Inertial_y;
   RT_FLOAT Inertial_z;
};


/***** Outputs type declaration. *****/
struct _Quaternion_Transformation_P_y {
   RT_FLOAT Body_x;
   RT_FLOAT Body_y;
   RT_FLOAT Body_z;
};



//extern void Quaternion_Transformation_P(); //jja commented out b/c pic wants function arguments even for externs

/******* Procedure: Velocity_Track *******/

/***** Inputs type declaration. *****/
struct _Velocity_Track_u {
   RT_FLOAT ECI_POS_X;
   RT_FLOAT ECI_POS_Y;
   RT_FLOAT ECI_POS_Z;
   RT_FLOAT ECI_VEL_X;
   RT_FLOAT ECI_VEL_Y;
   RT_FLOAT ECI_VEL_Z;
   RT_FLOAT Estimated_q1_B_M;
   RT_FLOAT Estimated_q2_B_M;
   RT_FLOAT Estimated_q3_B_M;
   RT_FLOAT Estimated_q4_B_M;
   RT_FLOAT Filt_Omega_B_N_B_x;
   RT_FLOAT Filt_Omega_B_N_B_y;
   RT_FLOAT Filt_Omega_B_N_B_z;
   RT_FLOAT Enable_1;
};


/***** Outputs type declaration. *****/
struct _Velocity_Track_y {
   RT_FLOAT q1_Bogus_Sun_N_B;
   RT_FLOAT q2_Bogus_Sun_N_B;
   RT_FLOAT q3_Bogus_Sun_N_B;
   RT_FLOAT q4_Bogus_Sun_N_B;
   RT_FLOAT Flip_Omegas_1;
   RT_FLOAT Flip_Omegas_2;
   RT_FLOAT Flip_Omegas_3;
   RT_FLOAT MTR_Enable;
};



//extern void Velocity_Track(); //jja commented out b/c pic wants function arguments even for externs

/******* Procedure: Sun_Pointing_Mode *******/

/***** Inputs type declaration. *****/
struct _Sun_Pointing_Mode_u {
   RT_FLOAT Mode_9_Momentum_Dump;
   RT_FLOAT Meas_Sun_Vector_B_x;
   RT_FLOAT Meas_Sun_Vector_B_y;
   RT_FLOAT Meas_Sun_Vector_B_z;
   RT_FLOAT Filt_Omega_B_N_B_x;
   RT_FLOAT Filt_Omega_B_N_B_y;
   RT_FLOAT Filt_Omega_B_N_B_z;
};


/***** Outputs type declaration. *****/
struct _Sun_Pointing_Mode_y {
   RT_FLOAT q1_Bogus_Sun_N_B;
   RT_FLOAT q2_Bogus_Sun_N_B;
   RT_FLOAT q3_Bogus_Sun_N_B;
   RT_FLOAT q4_Bogus_Sun_N_B;
   RT_FLOAT Sun_Pointing_Mode_98_1;
   RT_FLOAT Sun_Pointing_Mode_98_2;
   RT_FLOAT Sun_Pointing_Mode_98_3;
   RT_FLOAT MTR_Enable;
};



//extern void Sun_Pointing_Mode(); //jja commented out b/c pic wants function arguments even for externs

/******* Procedure: Idle_Mode *******/

/***** Outputs type declaration. *****/
struct _Idle_Mode_y {
   RT_FLOAT Zero_outputs_1;
   RT_FLOAT Zero_outputs_2;
   RT_FLOAT Zero_outputs_3;
   RT_FLOAT Zero_outputs_4;
   RT_FLOAT Zero_outputs_5;
   RT_FLOAT Zero_outputs_6;
   RT_FLOAT Zero_outputs_7;
   RT_FLOAT Zero_outputs_1_1;
};



//extern void Idle_Mode(); //jja commented out b/c pic wants function arguments even for externs

/******* Procedure: Sun_Search_Mode *******/

/***** Inputs type declaration. *****/
struct _Sun_Search_Mode_u {
   RT_FLOAT Commanded_Aqsn_Slew_Rate;
   RT_FLOAT Switch_to_Yaw;
   RT_FLOAT Filt_Omega_B_N_B_x;
   RT_FLOAT Filt_Omega_B_N_B_y;
   RT_FLOAT Filt_Omega_B_N_B_z;
};


/***** Outputs type declaration. *****/
struct _Sun_Search_Mode_y {
   RT_FLOAT Position_Error_1;
   RT_FLOAT Position_Error_2;
   RT_FLOAT Position_Error_3;
   RT_FLOAT Position_Error_4;
   RT_FLOAT Rate_Error_1;
   RT_FLOAT Rate_Error_2;
   RT_FLOAT Rate_Error_3;
   RT_FLOAT Torque_Rod_Enable_1;
};



//extern void Sun_Search_Mode(); //jja commented out b/c pic wants function arguments even for externs

/******* Procedure: Sun_Lock_Mode *******/

/***** Inputs type declaration. *****/
struct _Sun_Lock_Mode_u {
   RT_FLOAT Estimated_q1_B_M;
   RT_FLOAT Estimated_q2_B_M;
   RT_FLOAT Estimated_q3_B_M;
   RT_FLOAT Estimated_q4_B_M;
   RT_FLOAT Filt_Omega_B_N_B_x;
   RT_FLOAT Filt_Omega_B_N_B_y;
   RT_FLOAT Filt_Omega_B_N_B_z;
   RT_FLOAT Saved_Sun_Vector_q1;
   RT_FLOAT Saved_Sun_Vector_q2;
   RT_FLOAT Saved_Sun_Vector_q3;
   RT_FLOAT Saved_Sun_Vector_q4;
};


/***** Outputs type declaration. *****/
struct _Sun_Lock_Mode_y {
   RT_FLOAT Quaternion_Difference_P_12_1;
   RT_FLOAT Quaternion_Difference_P_12_2;
   RT_FLOAT Quaternion_Difference_P_12_3;
   RT_FLOAT Quaternion_Difference_P_12_4;
   RT_FLOAT Flip_Omegas_1;
   RT_FLOAT Flip_Omegas_2;
   RT_FLOAT Flip_Omegas_3;
   RT_FLOAT Zero;
};



//extern void Sun_Lock_Mode(); //jja commented out b/c pic wants function arguments even for externs

/******* Procedure: Mission_Lock_Mode *******/

/***** Inputs type declaration. *****/
struct _Mission_Lock_Mode_u {
   RT_FLOAT Estimated_q1_B_M;
   RT_FLOAT Estimated_q2_B_M;
   RT_FLOAT Estimated_q3_B_M;
   RT_FLOAT Estimated_q4_B_M;
   RT_FLOAT Filt_Omega_B_N_B_x;
   RT_FLOAT Filt_Omega_B_N_B_y;
   RT_FLOAT Filt_Omega_B_N_B_z;
   RT_FLOAT q1_M_LV;
   RT_FLOAT q2_M_LV;
   RT_FLOAT q3_M_LV;
   RT_FLOAT q4_M_LV;
};


/***** Outputs type declaration. *****/
struct _Mission_Lock_Mode_y {
   RT_FLOAT Quaternion_Difference_P_12_1;
   RT_FLOAT Quaternion_Difference_P_12_2;
   RT_FLOAT Quaternion_Difference_P_12_3;
   RT_FLOAT Quaternion_Difference_P_12_4;
   RT_FLOAT Flip_Omegas_1;
   RT_FLOAT Flip_Omegas_2;
   RT_FLOAT Flip_Omegas_3;
   RT_FLOAT Zero;
};



//extern void Mission_Lock_Mode(); //jja commented out b/c pic wants function arguments even for externs

/******* Procedure: Lat_Long_to_ECI *******/

/***** Inputs type declaration. *****/
struct _Lat_Long_to_ECI_u {
   RT_FLOAT Target_Longitude;
   RT_FLOAT Target_Latitude;
   RT_FLOAT Target_Altitude;
   RT_FLOAT Greenwich_Hour_Angle;
};


/***** Outputs type declaration. *****/
struct _Lat_Long_to_ECI_y {
   RT_FLOAT Target_X_ECI;
   RT_FLOAT Target_Y_ECI;
   RT_FLOAT Target_Z_ECI;
};



//extern void Lat_Long_to_ECI(); //jja commented out b/c pic wants function arguments even for externs

/******* Procedure: Calculate_Surface_Point_DCM *******/

/***** Inputs type declaration. *****/
struct _Calculate_Surface_Point_DCM_u {
   RT_FLOAT Target_LOS_x_ECI;
   RT_FLOAT Target_LOS_y_ECI;
   RT_FLOAT Target_LOS_z_ECI;
   RT_FLOAT X_1;
   RT_FLOAT Y_1;
   RT_FLOAT Z;
};


/***** Outputs type declaration. *****/
struct _Calculate_Surface_Point_DCM_y {
   RT_FLOAT mth_vect_norm_2_1;
   RT_FLOAT mth_vect_norm_2_2;
   RT_FLOAT mth_vect_norm_2_3;
   RT_FLOAT mth_vect_norm_14_1;
   RT_FLOAT mth_vect_norm_14_2;
   RT_FLOAT mth_vect_norm_14_3;
   RT_FLOAT mth_vect_norm_12_1;
   RT_FLOAT mth_vect_norm_12_2;
   RT_FLOAT mth_vect_norm_12_3;
};



//extern void Calculate_Surface_Point_DCM(); //jja commented out b/c pic wants function arguments even for externs

/******* Procedure: Quaternion_Require_Continuous_P *******/

/***** Inputs type declaration. *****/
struct _Quaternion_Require_Continuous_P_u {
   RT_FLOAT Q1_desired;
   RT_FLOAT Q2_desired;
   RT_FLOAT Q3_desired;
   RT_FLOAT Q4_desired;
};


/***** Outputs type declaration. *****/
struct _Quaternion_Require_Continuous_P_y {
   RT_FLOAT q1;
   RT_FLOAT q2;
   RT_FLOAT q3;
   RT_FLOAT q4;
};


/***** Private States type declaration. *****/
struct _Quaternion_Require_Continuous_P_ps {
   RT_FLOAT One_Delay_4_Channels_S1;
   RT_FLOAT One_Delay_4_Channels_S2;
   RT_FLOAT One_Delay_4_Channels_S3;
   RT_FLOAT One_Delay_4_Channels_S4;
   RT_FLOAT Flip_Flop_P_S1;
   RT_INTEGER Flip_Flop_P_S2;
   RT_INTEGER Flip_Flop_P_S3;
};


/***** States type declaration. *****/
struct _Quaternion_Require_Continuous_P_s {
   struct _Quaternion_Require_Continuous_P_ps Quaternion_Require_Continuous_P_ps[2];
   RT_INTEGER Quaternion_Require_Continuous_P_x;
};



//extern void Quaternion_Require_Continuous_P(); //jja commented out b/c pic wants function arguments even for externs

/******* Procedure: Quaternion_From_DC_P *******/

/***** Inputs type declaration. *****/
struct _Quaternion_From_DC_P_u {
   RT_FLOAT A11;
   RT_FLOAT A12;
   RT_FLOAT A13;
   RT_FLOAT A21;
   RT_FLOAT A22;
   RT_FLOAT A23;
   RT_FLOAT A31;
   RT_FLOAT A32;
   RT_FLOAT A33;
};


/***** Outputs type declaration. *****/
struct _Quaternion_From_DC_P_y {
   RT_FLOAT q1;
   RT_FLOAT q2;
   RT_FLOAT q3;
   RT_FLOAT q4;
   RT_FLOAT Q1_desired;
   RT_FLOAT Q2_desired;
   RT_FLOAT Q3_desired;
   RT_FLOAT Q4_desired;
};


/***** States type declaration. *****/
struct _Quaternion_From_DC_P_s {
   struct _Quaternion_Require_Continuous_P_s Quaternion_Require_Continuous_P_16_s;
};


/***** Info type declaration. *****/
struct _Quaternion_From_DC_P_info {
   RT_INTEGER iinfo[5];
   RT_FLOAT RP[4];
};



//extern void Quaternion_From_DC_P(); //jja commented out b/c pic wants function arguments even for externs

/******* Procedure: Quaternion_Sign_Flip_P *******/

/***** Inputs type declaration. *****/
struct _Quaternion_Sign_Flip_P_u {
   RT_FLOAT q1_Error;
   RT_FLOAT q2_Error;
   RT_FLOAT q3_Error;
   RT_FLOAT q4_Error;
};


/***** Outputs type declaration. *****/
struct _Quaternion_Sign_Flip_P_y {
   RT_FLOAT Quaternion_Sign_Flip_P_6_1;
   RT_FLOAT Quaternion_Sign_Flip_P_6_2;
   RT_FLOAT Quaternion_Sign_Flip_P_6_3;
   RT_FLOAT Quaternion_Sign_Flip_P_6_4;
};



//extern void Quaternion_Sign_Flip_P(); //jja commented out b/c pic wants function arguments even for externs

/******* Procedure: Surface_Point_Track_rate *******/

/***** Inputs type declaration. *****/
struct _Surface_Point_Track_rate_u {
   RT_FLOAT q1_P_ECI;
   RT_FLOAT q2_P_ECI;
   RT_FLOAT q3_P_ECI;
   RT_FLOAT q4_P_ECI;
};


/***** Outputs type declaration. *****/
struct _Surface_Point_Track_rate_y {
   RT_FLOAT Omega_P_ECI_P_x;
   RT_FLOAT Omega_P_ECI_P_y;
   RT_FLOAT Omega_P_ECI_P_z;
};


/***** Private States type declaration. *****/
struct _Surface_Point_Track_rate_ps {
   RT_FLOAT Four_Channel_Single_Delay_S1;
   RT_FLOAT Four_Channel_Single_Delay_S2;
   RT_FLOAT Four_Channel_Single_Delay_S3;
   RT_FLOAT Four_Channel_Single_Delay_S4;
};


/***** States type declaration. *****/
struct _Surface_Point_Track_rate_s {
   struct _Surface_Point_Track_rate_ps Surface_Point_Track_rate_ps[2];
   RT_INTEGER Surface_Point_Track_rate_x;
};



//extern void Surface_Point_Track_rate(); //jja commented out b/c pic wants function arguments even for externs

/******* Procedure: Quaternion_Multiply *******/

/***** Inputs type declaration. *****/
struct _Quaternion_Multiply_u {
   RT_FLOAT q1_A_B;
   RT_FLOAT q2_A_B;
   RT_FLOAT q3_A_B;
   RT_FLOAT q4_A_B;
   RT_FLOAT q1_B_C;
   RT_FLOAT q2_B_C;
   RT_FLOAT q3_B_C;
   RT_FLOAT q4_B_C;
};


/***** Outputs type declaration. *****/
struct _Quaternion_Multiply_y {
   RT_FLOAT q1_A_C;
   RT_FLOAT q2_A_C;
   RT_FLOAT q3_A_C;
   RT_FLOAT q4_A_C;
};



//extern void Quaternion_Multiply(); //jja commented out b/c pic wants function arguments even for externs

/******* Procedure: Mission_Surface_Track *******/

/***** Inputs type declaration. *****/
struct _Mission_Surface_Track_u {
   RT_FLOAT Greenwich_Hour_Angle;
   RT_FLOAT Pos_ECIo_LVo_ECI_x;
   RT_FLOAT Pos_ECIo_LVo_ECI_y;
   RT_FLOAT Pos_ECIo_LVo_ECI_z;
   RT_FLOAT Sun_in_ECI_x;
   RT_FLOAT Sun_in_ECI_y;
   RT_FLOAT Sun_in_ECI_z;
   RT_FLOAT Target_Longitude;
   RT_FLOAT Target_Latitude;
   RT_FLOAT Target_Altitude;
   RT_FLOAT q1;
   RT_FLOAT q2;
   RT_FLOAT q3;
   RT_FLOAT q4;
   RT_FLOAT omega_x;
   RT_FLOAT omega_y;
   RT_FLOAT omega_z;
};


/***** Outputs type declaration. *****/
struct _Mission_Surface_Track_y {
   RT_FLOAT Quaternion_Difference_P_4_1;
   RT_FLOAT Quaternion_Difference_P_4_2;
   RT_FLOAT Quaternion_Difference_P_4_3;
   RT_FLOAT Quaternion_Difference_P_4_4;
   RT_FLOAT Mission_Surface_Track_26_1;
   RT_FLOAT Mission_Surface_Track_26_2;
   RT_FLOAT Mission_Surface_Track_26_3;
   RT_FLOAT Zero;
};


/***** States type declaration. *****/
struct _Mission_Surface_Track_s {
   struct _Quaternion_From_DC_P_s Quaternion_From_DC_P_25_s;
   struct _Surface_Point_Track_rate_s Surface_Point_Track_rate_9_s;
};


/***** Info type declaration. *****/
struct _Mission_Surface_Track_info {
   RT_INTEGER iinfo[5];
   struct _Quaternion_From_DC_P_info Quaternion_From_DC_P_25_i;
};



//extern void Mission_Surface_Track(); //jja commented out b/c pic wants function arguments even for externs

/******* Procedure: Inertial_Capture *******/

/***** Inputs type declaration. *****/
struct _Inertial_Capture_u {
   RT_FLOAT Filt_Omega_B_N_B_x;
   RT_FLOAT Filt_Omega_B_N_B_y;
   RT_FLOAT Filt_Omega_B_N_B_z;
};


/***** Outputs type declaration. *****/
struct _Inertial_Capture_y {
   RT_FLOAT q1_err;
   RT_FLOAT q2_err;
   RT_FLOAT q3_err;
   RT_FLOAT q4_err;
   RT_FLOAT wx_Error;
   RT_FLOAT wy_Error;
   RT_FLOAT wz_Error;
   RT_FLOAT Zero;
};



//extern void Inertial_Capture(); //jja commented out b/c pic wants function arguments even for externs

/******* Procedure: Quaternion_From_DC_No_Flip *******/

/***** Inputs type declaration. *****/
struct _Quaternion_From_DC_No_Flip_u {
   RT_FLOAT A11;
   RT_FLOAT A12;
   RT_FLOAT A13;
   RT_FLOAT A21;
   RT_FLOAT A22;
   RT_FLOAT A23;
   RT_FLOAT A31;
   RT_FLOAT A32;
   RT_FLOAT A33;
};


/***** Outputs type declaration. *****/
struct _Quaternion_From_DC_No_Flip_y {
   RT_FLOAT Q1_desired;
   RT_FLOAT Q2_desired;
   RT_FLOAT Q3_desired;
   RT_FLOAT Q4_desired;
};


/***** Info type declaration. *****/
struct _Quaternion_From_DC_No_Flip_info {
   RT_INTEGER iinfo[5];
   RT_FLOAT RP[4];
};



//extern void Quaternion_From_DC_No_Flip(); //jja commented out b/c pic wants function arguments even for externs


/******** Procedures' definitions ********/

/******* Procedure: Toggle_Down_To_Pulse *******/
void Toggle_Down_To_Pulse(   struct _Toggle_Down_To_Pulse_u *U
   ,struct _Toggle_Down_To_Pulse_y *Y
   ,struct _Toggle_Down_To_Pulse_s *S
   ,RT_INTEGER INIT
)
{
      
      /***** Current and Next States Pointers. *****/

      struct _Toggle_Down_To_Pulse_ps *X = &S->Toggle_Down_To_Pulse_ps[S->Toggle_Down_To_Pulse_x];
      struct _Toggle_Down_To_Pulse_ps *XD = &S->Toggle_Down_To_Pulse_ps[1-S->Toggle_Down_To_Pulse_x];


      /***** Local Block Outputs. *****/

      RT_FLOAT Toggle_Down_To_Pulse_20_1;
      RT_FLOAT Toggle_Down_To_Pulse_31_1;
      RT_FLOAT Toggle_Down_To_Pulse_23_1;

      /******* Initialization. *******/

      if( INIT ) {
         X->Toggle_Down_To_Pulse_20_S1 = 0.0;
         XD->Toggle_Down_To_Pulse_20_S1 = 0.0;
      }


      /***** Output Update. *****/
      /* ----------------------------  Time Delay */
      /* {Toggle Down To Pulse..20} */
      if (INIT) {
         X->Toggle_Down_To_Pulse_20_S1 = 0.0; 
      }
      Toggle_Down_To_Pulse_20_1 = X->Toggle_Down_To_Pulse_20_S1; 
      /* ----------------------------  Summer */
      /* {Toggle Down To Pulse..31} */
      Toggle_Down_To_Pulse_31_1 = (-1.0)*U->overwritepulse; 
      Toggle_Down_To_Pulse_31_1 = Toggle_Down_To_Pulse_31_1 + Toggle_Down_To_Pulse_20_1; 
      /* ----------------------------  Algebraic Expression */
      /* {Toggle Down To Pulse..23} */
      Toggle_Down_To_Pulse_23_1 = 0.1; 
      /* ----------------------------  Relational Operator -- LT-EQ-GT */
      /* {Toggle Down To Pulse..13} */
      if (Toggle_Down_To_Pulse_31_1 > Toggle_Down_To_Pulse_23_1) {
         Y->Toggle_Down_To_Pulse_13_1 = 1.0; 
      }
      else {
         Y->Toggle_Down_To_Pulse_13_1 = 0.0; 
      }

      /***** State Update. *****/
      /* ----------------------------  Time Delay */
      /* {Toggle Down To Pulse..20} */
      XD->Toggle_Down_To_Pulse_20_S1 = U->overwritepulse; 


      //////iinfo[1] = 0;
EXEC_ERROR: return;
}

/******* Procedure: Compute_Bogus_Sun_Quaternion *******/
void Compute_Bogus_Sun_Quaternion(   struct _Compute_Bogus_Sun_Quaternion_u *U
   ,struct _Compute_Bogus_Sun_Quaternion_y *Y
)
{
      
      /***** Local Block Outputs. *****/

      RT_FLOAT Compute_Bogus_Sun_Quaternion_1_1;
      RT_FLOAT Find_Euler_Axis_Rotation_Angle_1;
      RT_FLOAT Compute_Bogus_Sun_Quaternion_3_1;
      RT_FLOAT Compute_Bogus_Sun_Quaternion_24_1;
      RT_FLOAT Compute_Bogus_Sun_Quaternion_99_1;
      RT_FLOAT q3_Sun_B_N;

      /***** Output Update. *****/
      /* ----------------------------  Saturation */
      /* {Compute Bogus Sun Quaternion..1} */
      Compute_Bogus_Sun_Quaternion_1_1 = MIN(MAX(-1.0,U->Sine_of_Euler_Rotation_Angle),1.0); 
      /* ----------------------------  Arcsine( u ) */
      /* {Compute Bogus Sun Quaternion.Find Euler Axis Rotation Angle.14} */
      Find_Euler_Axis_Rotation_Angle_1 = asin(Compute_Bogus_Sun_Quaternion_1_1); 
      /* ----------------------------  Gain Block */
      /* {Compute Bogus Sun Quaternion..3} */
      Compute_Bogus_Sun_Quaternion_3_1 = 0.5*Find_Euler_Axis_Rotation_Angle_1; 
      /* ----------------------------  Sine( u ) */
      /* {Compute Bogus Sun Quaternion..24} */
      Compute_Bogus_Sun_Quaternion_24_1 = sin(Compute_Bogus_Sun_Quaternion_3_1); 
      /* ----------------------------  ElementDivision */
      /* {Compute Bogus Sun Quaternion..99} */
      Compute_Bogus_Sun_Quaternion_99_1 = Compute_Bogus_Sun_Quaternion_24_1/U->Sine_of_Euler_Rotation_Angle; 
      /* ----------------------------  ElementProduct */
      /* {Compute Bogus Sun Quaternion..4} */
      Y->q1_Bogus_Sun_N_B = Compute_Bogus_Sun_Quaternion_99_1*U->Meas_Sun_Vector_B_y; 
      /* ----------------------------  ElementProduct */
      /* {Compute Bogus Sun Quaternion..15} */
      q3_Sun_B_N = Compute_Bogus_Sun_Quaternion_99_1*U->Meas_Sun_Vector_B_x; 
      /* ----------------------------  Algebraic Expression */
      /* {Compute Bogus Sun Quaternion..25} */
      Y->q3_Bogus_Sun_N_B = 0.0; 
      /* ----------------------------  Cosine( u ) */
      /* {Compute Bogus Sun Quaternion..34} */
      Y->q4_Bogus_Sun_N_B = cos(Compute_Bogus_Sun_Quaternion_3_1); 
      /* ----------------------------  Gain Block */
      /* {Compute Bogus Sun Quaternion.q3 Bogus Sun B N.6} */
      Y->q2_Bogus_Sun_N_B = (-1.0)*q3_Sun_B_N; 


      //////iinfo[1] = 0;
EXEC_ERROR: return;
}

/******* Procedure: mth_quat_norm *******/
void mth_quat_norm(   struct _mth_quat_norm_u *U
   ,struct _mth_quat_norm_y *Y
)
{
      
      /***** Local Block Outputs. *****/

      RT_FLOAT mth_quat_norm_13_1;
      RT_FLOAT mth_quat_norm_14_1;

      /***** Output Update. *****/
      /* ----------------------------  Algebraic Expression */
      /* {mth quat norm..13} */
      mth_quat_norm_13_1 = U->DQ1*U->DQ1 + U->DQ2*U->DQ2 + U->DQ3*U->DQ3 + U->DQ4*U->DQ4 + 1.0E-6; 
      /* ----------------------------  Square Root */
      /* {mth quat norm..14} */
      mth_quat_norm_14_1 = sqrt(mth_quat_norm_13_1); 
      /* ----------------------------  Algebraic Expression */
      /* {mth quat norm..4} */
      Y->mth_quat_norm_4_1 = U->DQ1/mth_quat_norm_14_1; 
      Y->mth_quat_norm_4_2 = U->DQ2/mth_quat_norm_14_1; 
      Y->mth_quat_norm_4_3 = U->DQ3/mth_quat_norm_14_1; 
      Y->mth_quat_norm_4_4 = U->DQ4/mth_quat_norm_14_1; 


      //////iinfo[1] = 0;
EXEC_ERROR: return;
}

/******* Procedure: Quaternion_Difference_P *******/
void Quaternion_Difference_P(   struct _Quaternion_Difference_P_u *U
   ,struct _Quaternion_Difference_P_y *Y
)
{
      
      /***** Local Block Outputs. *****/

      RT_FLOAT Q1_ACTUAL_CONJ;
      RT_FLOAT Q2_ACTUAL_CONJ;
      RT_FLOAT Q3_ACTUAL_CONJ;
      RT_FLOAT Q4_ACTUAL_CONJ;
      RT_FLOAT DQ1;
      RT_FLOAT DQ2;
      RT_FLOAT DQ3;
      RT_FLOAT DQ4;
      struct _mth_quat_norm_u mth_quat_norm_99_u;
      struct _mth_quat_norm_y mth_quat_norm_99_y;

      /***** Output Update. *****/
      /* ----------------------------  Algebraic Expression */
      /* {Quaternion Difference P.Q CONJUGATE.6} */
      Q1_ACTUAL_CONJ = (-1.0)*U->Q1_desired; 
      Q2_ACTUAL_CONJ = (-1.0)*U->Q2_desired; 
      Q3_ACTUAL_CONJ = (-1.0)*U->Q3_desired; 
      Q4_ACTUAL_CONJ = U->Q4_desired; 
      /* ----------------------------  Algebraic Expression */
      /* {Quaternion Difference P.Q MULT CALC.7} */
      DQ1 = U->Q4_actual*Q1_ACTUAL_CONJ + U->Q3_actual*Q2_ACTUAL_CONJ - U->Q2_actual*Q3_ACTUAL_CONJ + U->Q1_actual*Q4_ACTUAL_CONJ; 
      DQ2 = (-1.0)*U->Q3_actual*Q1_ACTUAL_CONJ + U->Q4_actual*Q2_ACTUAL_CONJ + U->Q1_actual*Q3_ACTUAL_CONJ + U->Q2_actual*Q4_ACTUAL_CONJ; 
      DQ3 = U->Q2_actual*Q1_ACTUAL_CONJ - U->Q1_actual*Q2_ACTUAL_CONJ + U->Q4_actual*Q3_ACTUAL_CONJ + U->Q3_actual*Q4_ACTUAL_CONJ; 
      DQ4 = (-1.0)*U->Q1_actual*Q1_ACTUAL_CONJ - U->Q2_actual*Q2_ACTUAL_CONJ - U->Q3_actual*Q3_ACTUAL_CONJ + U->Q4_actual*Q4_ACTUAL_CONJ; 
      /* ----------------------------  Procedure Super Block */
      /* {mth quat norm.99} */
      mth_quat_norm_99_u.DQ1 = DQ1;
      mth_quat_norm_99_u.DQ2 = DQ2;
      mth_quat_norm_99_u.DQ3 = DQ3;
      mth_quat_norm_99_u.DQ4 = DQ4;
      mth_quat_norm(&mth_quat_norm_99_u, &mth_quat_norm_99_y);
      Y->mth_quat_norm_99_1 = mth_quat_norm_99_y.mth_quat_norm_4_1;
      Y->mth_quat_norm_99_2 = mth_quat_norm_99_y.mth_quat_norm_4_2;
      Y->mth_quat_norm_99_3 = mth_quat_norm_99_y.mth_quat_norm_4_3;
      Y->mth_quat_norm_99_4 = mth_quat_norm_99_y.mth_quat_norm_4_4;


      ////iinfo[1] = 0;
EXEC_ERROR: return;
}

/******* Procedure: mth_vect_norm *******/
void mth_vect_norm(   struct _mth_vect_norm_u *U
   ,struct _mth_vect_norm_y *Y
)
{
      
      /***** Local Block Outputs. *****/

      RT_FLOAT mth_vect_norm_13_1;

      /***** Output Update. *****/
      /* ----------------------------  Algebraic Expression */
      /* {mth vect norm..13} */
      mth_vect_norm_13_1 = U->Measured_magnetometer_body_x*U->Measured_magnetometer_body_x + U->Measured_magnetometer_body_y*U->Measured_magnetometer_body_y + U->Measured_magnetometer_body_z*U->Measured_magnetometer_body_z + 1.0E-10; 
      /* ----------------------------  Square Root */
      /* {mth vect norm..14} */
      Y->Magnitude = sqrt(mth_vect_norm_13_1); 
      /* ----------------------------  Algebraic Expression */
      /* {mth vect norm..23} */
      Y->X_1 = U->Measured_magnetometer_body_x/Y->Magnitude; 
      Y->Y_1 = U->Measured_magnetometer_body_y/Y->Magnitude; 
      Y->Z = U->Measured_magnetometer_body_z/Y->Magnitude; 


      ////iinfo[1] = 0;
EXEC_ERROR: return;
}

/******* Procedure: Compute_Gyro_Offset *******/
void Compute_Gyro_Offset(   struct _Compute_Gyro_Offset_u *U
   ,struct _Compute_Gyro_Offset_y *Y
   ,struct _Compute_Gyro_Offset_s *S
   ,RT_INTEGER INIT
)
{
      
      /***** Current and Next States Pointers. *****/

      struct _Compute_Gyro_Offset_ps *X = &S->Compute_Gyro_Offset_ps[S->Compute_Gyro_Offset_x];
      struct _Compute_Gyro_Offset_ps *XD = &S->Compute_Gyro_Offset_ps[1-S->Compute_Gyro_Offset_x];


      /***** Local Block Outputs. *****/

      RT_FLOAT Compute_Gyro_Offset_1_1;
      RT_FLOAT Compute_Gyro_Offset_1_2;
      RT_FLOAT Compute_Gyro_Offset_1_3;
      RT_FLOAT Compute_Gyro_Offset_1_4;
      RT_FLOAT wx_old;
      RT_FLOAT wy_old;
      RT_FLOAT wz_old;
      RT_FLOAT Compute_Gyro_Offset_20_1;
      RT_FLOAT Compute_Gyro_Offset_16_1;
      RT_FLOAT omega_prev;
      RT_FLOAT timestep;
      RT_FLOAT zero;
      RT_FLOAT zero_1;
      RT_FLOAT zero_2;
      RT_FLOAT Compute_Gyro_Offset_14_1;
      RT_FLOAT Compute_Gyro_Offset_10_1;
      RT_FLOAT Sine_of_Euler_Rotation_Angle;
      RT_FLOAT q1_Bogus_Sun_N_B;
      RT_FLOAT q2_Bogus_Sun_N_B;
      RT_FLOAT q3_Bogus_Sun_N_B;
      RT_FLOAT q4_Bogus_Sun_N_B;
      RT_FLOAT Quaternion_Difference_P_2_1;
      RT_FLOAT Quaternion_Difference_P_2_2;
      RT_FLOAT Quaternion_Difference_P_2_3;
      RT_FLOAT Quaternion_Difference_P_2_4;
      RT_FLOAT X_1;
      RT_FLOAT Y_1;
      RT_FLOAT Z;
      RT_FLOAT Magnitude;
      RT_FLOAT acos_s;
      RT_FLOAT omega;
      RT_FLOAT vx;
      RT_FLOAT vy;
      RT_FLOAT vz;
      RT_FLOAT mag_v;
      RT_FLOAT wx;
      RT_FLOAT wy;
      RT_FLOAT wz;
      RT_FLOAT Compute_Gyro_Offset_30_1;
      RT_FLOAT scaled_omega;
      RT_FLOAT Scale_zero_to_two_pi_1;
      RT_FLOAT wz_1;
      RT_FLOAT n;
      RT_FLOAT Compute_Gyro_Offset_36_1;
      RT_FLOAT Compute_Gyro_Offset_44_1;
      RT_FLOAT Compute_Gyro_Offset_26_1;
      RT_FLOAT Scale_zero_to_two_pi_1_1;
      RT_FLOAT wx_1;
      RT_FLOAT wy_1;
      RT_FLOAT wz_2;
      RT_FLOAT wxdiff;
      RT_FLOAT wydiff;
      RT_FLOAT wzdiff;
      RT_FLOAT wx_2;
      RT_FLOAT wy_2;
      RT_FLOAT wz_3;
      RT_FLOAT wx_3;
      RT_FLOAT wy_3;
      RT_FLOAT wz_4;
      RT_FLOAT n_1;
      RT_FLOAT Reset_Values_1;
      RT_FLOAT Reset_Values_2;
      RT_FLOAT Reset_Values_3;
      RT_FLOAT Reset_Values_4;
      RT_FLOAT Compute_Gyro_Offset_31_1;
      RT_FLOAT Compute_Gyro_Offset_39_1;
      RT_FLOAT Compute_Gyro_Offset_97_1;
      RT_FLOAT Compute_Gyro_Offset_35_1;
      RT_FLOAT Compute_Gyro_Offset_98_1;

      /***** Algorithmic Local Variables. *****/

      RT_BOOLEAN test;
      struct _Compute_Bogus_Sun_Quaternion_u Compute_Bogus_Sun_Quaternion_8_u;
      struct _Compute_Bogus_Sun_Quaternion_y Compute_Bogus_Sun_Quaternion_8_y;
      struct _Quaternion_Difference_P_u Quaternion_Difference_P_2_u;
      struct _Quaternion_Difference_P_y Quaternion_Difference_P_2_y;
      struct _mth_vect_norm_u mth_vect_norm_13_u;
      struct _mth_vect_norm_y mth_vect_norm_13_y;

      /******* Initialization. *******/

      if( INIT ) {
         X->Compute_Gyro_Offset_1_S1 = 0.0;
         X->Compute_Gyro_Offset_1_S2 = 0.0;
         X->Compute_Gyro_Offset_1_S3 = 0.0;
         X->Compute_Gyro_Offset_1_S4 = 0.0;
         X->Old_Difference_S1 = 0.0;
         X->Old_Difference_S2 = 0.0;
         X->Old_Difference_S3 = 0.0;
         X->Compute_Gyro_Offset_20_S1 = 0.0;
         X->Previous_S1 = 0.0;
         XD->Compute_Gyro_Offset_1_S1 = 0.0;
         XD->Compute_Gyro_Offset_1_S2 = 0.0;
         XD->Compute_Gyro_Offset_1_S3 = 0.0;
         XD->Compute_Gyro_Offset_1_S4 = 0.0;
         XD->Old_Difference_S1 = 0.0;
         XD->Old_Difference_S2 = 0.0;
         XD->Old_Difference_S3 = 0.0;
         XD->Compute_Gyro_Offset_20_S1 = 0.0;
         XD->Previous_S1 = 0.0;
      }


      /***** Output Update. *****/
      /* ----------------------------  Time Delay */
      /* {Compute_Gyro_Offset..1} */
      if (INIT) {
         X->Compute_Gyro_Offset_1_S1 = 0.0; 
         X->Compute_Gyro_Offset_1_S2 = 0.0; 
         X->Compute_Gyro_Offset_1_S3 = 0.0; 
         X->Compute_Gyro_Offset_1_S4 = 1.0; 
      }
      Compute_Gyro_Offset_1_1 = X->Compute_Gyro_Offset_1_S1; 
      Compute_Gyro_Offset_1_2 = X->Compute_Gyro_Offset_1_S2; 
      Compute_Gyro_Offset_1_3 = X->Compute_Gyro_Offset_1_S3; 
      Compute_Gyro_Offset_1_4 = X->Compute_Gyro_Offset_1_S4; 
      /* ----------------------------  Time Delay */
      /* {Compute_Gyro_Offset.Old Difference.9} */
      if (INIT) {
         X->Old_Difference_S1 = 0.0; 
         X->Old_Difference_S2 = 0.0; 
         X->Old_Difference_S3 = 0.0; 
      }
      wx_old = X->Old_Difference_S1; 
      wy_old = X->Old_Difference_S2; 
      wz_old = X->Old_Difference_S3; 
      /* ----------------------------  Time Delay */
      /* {Compute_Gyro_Offset..20} */
      if (INIT) {
         X->Compute_Gyro_Offset_20_S1 = 0.0; 
      }
      Compute_Gyro_Offset_20_1 = X->Compute_Gyro_Offset_20_S1; 
      /* ----------------------------  Algebraic Expression */
      /* {Compute_Gyro_Offset..16} */
      Compute_Gyro_Offset_16_1 = 1.5; 
      /* ----------------------------  Time Delay */
      /* {Compute_Gyro_Offset.Previous.23} */
      if (INIT) {
         X->Previous_S1 = 0.0; 
      }
      omega_prev = X->Previous_S1; 
      /* ----------------------------  Algebraic Expression */
      /* {Compute_Gyro_Offset..43} */
      timestep = 0.25; 
      /* ----------------------------  Algebraic Expression */
      /* {Compute_Gyro_Offset..24} */
      zero = 0.0; 
      /* ----------------------------  Algebraic Expression */
      /* {Compute_Gyro_Offset.Maximum Rate.27} */
      zero_1 = 0.5; 
      /* ----------------------------  Algebraic Expression */
      /* {Compute_Gyro_Offset..28} */
      zero_2 = 0.0; 
      /* ----------------------------  Algebraic Expression */
      /* {Compute_Gyro_Offset..14} */
      Compute_Gyro_Offset_14_1 = U->Meas_Sun_Vector_B_x*U->Meas_Sun_Vector_B_x + U->Meas_Sun_Vector_B_y*U->Meas_Sun_Vector_B_y; 
      /* ----------------------------  Square Root */
      /* {Compute_Gyro_Offset..10} */
      Compute_Gyro_Offset_10_1 = sqrt(Compute_Gyro_Offset_14_1); 
      /* ----------------------------  Algebraic Expression */
      /* {Compute_Gyro_Offset..11} */
      Sine_of_Euler_Rotation_Angle = Compute_Gyro_Offset_10_1 + 1.0E-010; 
      /* ----------------------------  Procedure Super Block */
      /* {Compute Bogus Sun Quaternion.8} */
      Compute_Bogus_Sun_Quaternion_8_u.Sine_of_Euler_Rotation_Angle = Sine_of_Euler_Rotation_Angle;
      Compute_Bogus_Sun_Quaternion_8_u.Meas_Sun_Vector_B_x = U->Meas_Sun_Vector_B_x;
      Compute_Bogus_Sun_Quaternion_8_u.Meas_Sun_Vector_B_y = U->Meas_Sun_Vector_B_y;
      Compute_Bogus_Sun_Quaternion(&Compute_Bogus_Sun_Quaternion_8_u, &Compute_Bogus_Sun_Quaternion_8_y);
      q1_Bogus_Sun_N_B = Compute_Bogus_Sun_Quaternion_8_y.q1_Bogus_Sun_N_B;
      q2_Bogus_Sun_N_B = Compute_Bogus_Sun_Quaternion_8_y.q2_Bogus_Sun_N_B;
      q3_Bogus_Sun_N_B = Compute_Bogus_Sun_Quaternion_8_y.q3_Bogus_Sun_N_B;
      q4_Bogus_Sun_N_B = Compute_Bogus_Sun_Quaternion_8_y.q4_Bogus_Sun_N_B;
      /* ----------------------------  Procedure Super Block */
      /* {Quaternion Difference P.2} */
      Quaternion_Difference_P_2_u.Q1_actual = Compute_Gyro_Offset_1_1;
      Quaternion_Difference_P_2_u.Q2_actual = Compute_Gyro_Offset_1_2;
      Quaternion_Difference_P_2_u.Q3_actual = Compute_Gyro_Offset_1_3;
      Quaternion_Difference_P_2_u.Q4_actual = Compute_Gyro_Offset_1_4;
      Quaternion_Difference_P_2_u.Q1_desired = q1_Bogus_Sun_N_B;
      Quaternion_Difference_P_2_u.Q2_desired = q2_Bogus_Sun_N_B;
      Quaternion_Difference_P_2_u.Q3_desired = q3_Bogus_Sun_N_B;
      Quaternion_Difference_P_2_u.Q4_desired = q4_Bogus_Sun_N_B;
      Quaternion_Difference_P(&Quaternion_Difference_P_2_u, &Quaternion_Difference_P_2_y);
      Quaternion_Difference_P_2_1 = Quaternion_Difference_P_2_y.mth_quat_norm_99_1;
      Quaternion_Difference_P_2_2 = Quaternion_Difference_P_2_y.mth_quat_norm_99_2;
      Quaternion_Difference_P_2_3 = Quaternion_Difference_P_2_y.mth_quat_norm_99_3;
      Quaternion_Difference_P_2_4 = Quaternion_Difference_P_2_y.mth_quat_norm_99_4;
      /* ----------------------------  Procedure Super Block */
      /* {mth vect norm.13} */
      mth_vect_norm_13_u.Measured_magnetometer_body_x = Quaternion_Difference_P_2_1;
      mth_vect_norm_13_u.Measured_magnetometer_body_y = Quaternion_Difference_P_2_2;
      mth_vect_norm_13_u.Measured_magnetometer_body_z = Quaternion_Difference_P_2_3;
      mth_vect_norm(&mth_vect_norm_13_u, &mth_vect_norm_13_y);
      X_1 = mth_vect_norm_13_y.X_1;
      Y_1 = mth_vect_norm_13_y.Y_1;
      Z = mth_vect_norm_13_y.Z;
      Magnitude = mth_vect_norm_13_y.Magnitude;
      /* ----------------------------  Arccosine( u ) */
      /* {Compute_Gyro_Offset..3} */
      acos_s = acos(Quaternion_Difference_P_2_4); 
      /* ----------------------------  Arctangent2 (u1 u2) */
      /* {Compute_Gyro_Offset..21} */
      omega = atan2(U->Meas_Sun_Vector_B_y,U->Meas_Sun_Vector_B_x); 
      /* ----------------------------  Gain Block */
      /* {Compute_Gyro_Offset.nameChanger.4} */
      vx = Quaternion_Difference_P_2_1; 
      vy = Quaternion_Difference_P_2_2; 
      vz = Quaternion_Difference_P_2_3; 
      /* ----------------------------  Gain Block */
      /* {Compute_Gyro_Offset.nameChanger.5} */
      mag_v = Magnitude; 
      /* ----------------------------  Algebraic Expression */
      /* {Compute_Gyro_Offset.Compute Solar Omega XY.12} */
      wx = 2.0*vx*acos_s/mag_v; 
      wy = 2.0*vy*acos_s/mag_v; 
      wz = 2.0*vz*acos_s/mag_v; 
      /* ----------------------------  Relational Operator -- LT-EQ-GT */
      /* {Compute_Gyro_Offset..30} */
      if (omega < zero) {
         Compute_Gyro_Offset_30_1 = 1.0; 
      }
      else {
         Compute_Gyro_Offset_30_1 = 0.0; 
      }
      /* ----------------------------  Algebraic Expression */
      /* {Compute_Gyro_Offset..94} */
      scaled_omega = omega + 6.28318; 
      /* ----------------------------  Data Path Switch */
      /* {Compute_Gyro_Offset.Scale zero to two_pi.95} */
      if (Compute_Gyro_Offset_30_1 > 0.0) {
         Scale_zero_to_two_pi_1 = scaled_omega; 
      }
      else {
         Scale_zero_to_two_pi_1 = omega; 
      }
      /* ----------------------------  Algebraic Expression */
      /* {Compute_Gyro_Offset.Compute Solar Omega wZ.29} */
      wz_1 = (-1.0)*(Scale_zero_to_two_pi_1 - omega_prev)/timestep; 
      /* ----------------------------  Algebraic Expression */
      /* {Compute_Gyro_Offset..22} */
      n = U->n + 1.0; 
      /* ----------------------------  Relational Operator -- LT-EQ-GT */
      /* {Compute_Gyro_Offset..36} */
      if (n < Compute_Gyro_Offset_16_1) {
         Compute_Gyro_Offset_36_1 = 1.0; 
      }
      else {
         Compute_Gyro_Offset_36_1 = 0.0; 
      }
      /* ----------------------------  Absolute Value */
      /* {Compute_Gyro_Offset..44} */
      Compute_Gyro_Offset_44_1 = fabs(wz_1); 
      /* ----------------------------  Relational Operator -- LT-EQ-GT */
      /* {Compute_Gyro_Offset..26} */
      if (Compute_Gyro_Offset_44_1 < zero_1) {
         Compute_Gyro_Offset_26_1 = 1.0; 
      }
      else {
         Compute_Gyro_Offset_26_1 = 0.0; 
      }
      /* ----------------------------  Data Path Switch */
      /* {Compute_Gyro_Offset.Scale zero to two_pi.25} */
      if (Compute_Gyro_Offset_26_1 > 0.0) {
         Scale_zero_to_two_pi_1_1 = wz_1; 
      }
      else {
         Scale_zero_to_two_pi_1_1 = zero_2; 
      }
      /* ----------------------------  Gain Block */
      /* {Compute_Gyro_Offset.TimeGain.6} */
      wx_1 = wx; 
      wy_1 = wy; 
      wz_2 = Scale_zero_to_two_pi_1_1; 
      /* ----------------------------  Summer */
      /* {Compute_Gyro_Offset.Difference Solar Measured.17} */
      wxdiff = (-1.0)*wx_1 + U->Filt_Omega_B_N_B_x; 
      wydiff = (-1.0)*wy_1 + U->Filt_Omega_B_N_B_y; 
      wzdiff = (-1.0)*wz_2 + U->Filt_Omega_B_N_B_z; 
      /* ----------------------------  Data Path Switch */
      /* {Compute_Gyro_Offset..15} */
      if (Compute_Gyro_Offset_36_1 > 0.0) {
         wx_2 = U->gyro_offset_x; 
         wy_2 = U->gyro_offset_y; 
         wz_3 = U->gyro_offset_z; 
      }
      else {
         wx_2 = wxdiff; 
         wy_2 = wydiff; 
         wz_3 = wzdiff; 
      }
      /* ----------------------------  Algebraic Expression */
      /* {Compute_Gyro_Offset.Average of Differences.7} */
      wx_3 = (wx_old*(n - 1.0) + wx_2)/n; 
      wy_3 = (wy_old*(n - 1.0) + wy_2)/n; 
      wz_4 = (wz_old*(n - 1.0) + wz_3)/n; 
      n_1 = n; 
      /* ----------------------------  Algebraic Expression */
      /* {Compute_Gyro_Offset.Reset Values.18} */
      Reset_Values_1 = U->gyro_offset_x; 
      Reset_Values_2 = U->gyro_offset_y; 
      Reset_Values_3 = U->gyro_offset_z; 
      Reset_Values_4 = 0.0; 
      /* ----------------------------  Summer */
      /* {Compute_Gyro_Offset..31} */
      Compute_Gyro_Offset_31_1 = U->Enable_1 - Compute_Gyro_Offset_20_1; 
      /* ----------------------------  Logical Operator -- AND-OR-NOT */
      /* {Compute_Gyro_Offset..39} */
      if ((U->Enable_1 > 0.0)) {
         Compute_Gyro_Offset_39_1 = 0.0; 
      }
      else {
         Compute_Gyro_Offset_39_1 = 1.0; 
      }
      /* ----------------------------  L - U  Bounded Limit (Limiter) */
      /* {Compute_Gyro_Offset..97} */
      Compute_Gyro_Offset_97_1 = MIN(MAX(0.0,Compute_Gyro_Offset_31_1),2.0); 
      /* ----------------------------  Logical Operator -- AND-OR-NOT */
      /* {Compute_Gyro_Offset..35} */
      test = Compute_Gyro_Offset_39_1 > 0.0; 
      test = test || Compute_Gyro_Offset_97_1 > 0.0; 
      if (test) {
         Compute_Gyro_Offset_35_1 = 1.0; 
      }
      else {
         Compute_Gyro_Offset_35_1 = 0.0; 
      }
      /* ----------------------------  Data Path Switch */
      /* {Compute_Gyro_Offset..19} */
      if (Compute_Gyro_Offset_35_1 > 0.0) {
         Y->Compute_Gyro_Offset_19_1 = Reset_Values_1; 
         Y->Compute_Gyro_Offset_19_2 = Reset_Values_2; 
         Y->Compute_Gyro_Offset_19_3 = Reset_Values_3; 
         Y->Compute_Gyro_Offset_19_4 = Reset_Values_4; 
      }
      else {
         Y->Compute_Gyro_Offset_19_1 = wx_3; 
         Y->Compute_Gyro_Offset_19_2 = wy_3; 
         Y->Compute_Gyro_Offset_19_3 = wz_4; 
         Y->Compute_Gyro_Offset_19_4 = n_1; 
      }
      /* ----------------------------  Algebraic Expression */
      /* {Compute_Gyro_Offset..98} */
      Compute_Gyro_Offset_98_1 = 5000.0; 
      /* ----------------------------  Relational Operator -- LT-EQ-GT */
      /* {Compute_Gyro_Offset..99} */
      if (n_1 > Compute_Gyro_Offset_98_1) {
         Y->Compute_Gyro_Offset_99_1 = 1.0; 
      }
      else {
         Y->Compute_Gyro_Offset_99_1 = 0.0; 
      }

      /***** State Update. *****/
      /* ----------------------------  Time Delay */
      /* {Compute_Gyro_Offset..1} */
      XD->Compute_Gyro_Offset_1_S1 = q1_Bogus_Sun_N_B; 
      XD->Compute_Gyro_Offset_1_S2 = q2_Bogus_Sun_N_B; 
      XD->Compute_Gyro_Offset_1_S3 = q3_Bogus_Sun_N_B; 
      XD->Compute_Gyro_Offset_1_S4 = q4_Bogus_Sun_N_B; 
      /* ----------------------------  Time Delay */
      /* {Compute_Gyro_Offset.Old Difference.9} */
      XD->Old_Difference_S1 = wx_3; 
      XD->Old_Difference_S2 = wy_3; 
      XD->Old_Difference_S3 = wz_4; 
      /* ----------------------------  Time Delay */
      /* {Compute_Gyro_Offset..20} */
      XD->Compute_Gyro_Offset_20_S1 = U->Enable_1; 
      /* ----------------------------  Time Delay */
      /* {Compute_Gyro_Offset.Previous.23} */
      XD->Previous_S1 = Scale_zero_to_two_pi_1; 


      ////iinfo[1] = 0;
EXEC_ERROR: return;
}

/******* Procedure: Quaternion_Transformation_P *******/
void Quaternion_Transformation_P(   struct _Quaternion_Transformation_P_u *U
   ,struct _Quaternion_Transformation_P_y *Y
)
{
      
      /***** Output Update. *****/
      /* ----------------------------  Algebraic Expression */
      /* {Quaternion Transformation P.Transform.13} */
      Y->Body_x = (U->q4_B_N*U->q4_B_N + U->q1_B_N*U->q1_B_N - 0.5)*U->Inertial_x*2.0 + (U->q1_B_N*U->q2_B_N + U->q4_B_N*U->q3_B_N)*U->Inertial_y*2.0 + (U->q1_B_N*U->q3_B_N - U->q4_B_N*U->q2_B_N)*U->Inertial_z*2.0; 
      Y->Body_y = (U->q1_B_N*U->q2_B_N - U->q4_B_N*U->q3_B_N)*U->Inertial_x*2.0 + (U->q4_B_N*U->q4_B_N + U->q2_B_N*U->q2_B_N - 0.5)*U->Inertial_y*2.0 + (U->q2_B_N*U->q3_B_N + U->q4_B_N*U->q1_B_N)*U->Inertial_z*2.0; 
      Y->Body_z = (U->q1_B_N*U->q3_B_N + U->q4_B_N*U->q2_B_N)*U->Inertial_x*2.0 + (U->q2_B_N*U->q3_B_N - U->q4_B_N*U->q1_B_N)*U->Inertial_y*2.0 + (U->q4_B_N*U->q4_B_N + U->q3_B_N*U->q3_B_N - 0.5)*U->Inertial_z*2.0; 


      ////iinfo[1] = 0;
EXEC_ERROR: return;
}

/******* Procedure: Velocity_Track *******/
void Velocity_Track(   struct _Velocity_Track_u *U
   ,struct _Velocity_Track_y *Y
)
{
      
      /* Local Model variable declarations. */

      VAR_FLOAT vel_vec[3];

      /***** Local Block Outputs. *****/

      RT_FLOAT Body_x;
      RT_FLOAT Body_y;
      RT_FLOAT Body_z;
      RT_FLOAT Velocity_Track_25_1;
      RT_FLOAT Velocity_Track_25_2;
      RT_FLOAT Velocity_Track_25_3;
      RT_FLOAT X_1;
      RT_FLOAT Y_1;
      RT_FLOAT Z;
      RT_FLOAT Magnitude;
      RT_FLOAT Zero_1;
      RT_FLOAT X_2;
      RT_FLOAT Y_2;
      RT_FLOAT Z_1;
      RT_FLOAT Magnitude_1;
      RT_FLOAT Gain_1_1;
      RT_FLOAT Gain_2_1;
      RT_FLOAT Gain_3_1;
      RT_FLOAT Gain_1;
      RT_FLOAT Gain_2;
      RT_FLOAT Gain_3;
      struct _Quaternion_Transformation_P_u Quaternion_Transformation_P_2_u;
      struct _Quaternion_Transformation_P_y Quaternion_Transformation_P_2_y;
      struct _mth_vect_norm_u mth_vect_norm_1_u;
      struct _mth_vect_norm_y mth_vect_norm_1_y;
      struct _mth_vect_norm_u mth_vect_norm_3_u;
      struct _mth_vect_norm_y mth_vect_norm_3_y;
      struct _Compute_Bogus_Sun_Quaternion_u Compute_Bogus_Sun_Quaternion_52_u;
      struct _Compute_Bogus_Sun_Quaternion_y Compute_Bogus_Sun_Quaternion_52_y;

      /***** Output Update. *****/
      /* ----------------------------  Procedure Super Block */
      /* {Quaternion Transformation P.2} */
      Quaternion_Transformation_P_2_u.q1_B_N = U->Estimated_q1_B_M;
      Quaternion_Transformation_P_2_u.q2_B_N = U->Estimated_q2_B_M;
      Quaternion_Transformation_P_2_u.q3_B_N = U->Estimated_q3_B_M;
      Quaternion_Transformation_P_2_u.q4_B_N = U->Estimated_q4_B_M;
      Quaternion_Transformation_P_2_u.Inertial_x = U->ECI_VEL_X;
      Quaternion_Transformation_P_2_u.Inertial_y = U->ECI_VEL_Y;
      Quaternion_Transformation_P_2_u.Inertial_z = U->ECI_VEL_Z;
      Quaternion_Transformation_P(&Quaternion_Transformation_P_2_u, &Quaternion_Transformation_P_2_y);
      Body_x = Quaternion_Transformation_P_2_y.Body_x;
      Body_y = Quaternion_Transformation_P_2_y.Body_y;
      Body_z = Quaternion_Transformation_P_2_y.Body_z;
      /* ----------------------------  Algebraic Expression */
      /* {Velocity Track.MTR Enable.44} */
      Y->MTR_Enable = U->Enable_1; 

      /* ----------------------------  IfThenElse */
      /* {Velocity Track..99} */
      if( Body_z < 0.0 ) {
         
         /* ----------------------------  Gain Block */
         /* {Velocity Track.Gain.5} */
         Gain_1 = Body_x; 
         Gain_2 = Body_y; 
         Gain_3 = Body_z; 
         /* ----------------------------  Write to Variable */
         /* {Velocity Track..98} */
         vel_vec[0] = Gain_1;
         vel_vec[1] = Gain_2;
         vel_vec[2] = Gain_3;

      }
      else {
         
         /* ----------------------------  Gain Block */
         /* {Velocity Track.Gain.4} */
         Gain_1_1 = (-1.0)*Body_x; 
         Gain_2_1 = (-1.0)*Body_y; 
         Gain_3_1 = (-1.0)*Body_z; 
         /* ----------------------------  Write to Variable */
         /* {Velocity Track..6} */
         vel_vec[0] = Gain_1_1;
         vel_vec[1] = Gain_2_1;
         vel_vec[2] = Gain_3_1;

      }
      /* ----------------------------  Gain Block */
      /* {Velocity Track.Flip Omegas.22} */
      Y->Flip_Omegas_1 = (-1.0)*U->Filt_Omega_B_N_B_x; 
      Y->Flip_Omegas_2 = (-1.0)*U->Filt_Omega_B_N_B_y; 
      Y->Flip_Omegas_3 = (-1.0)*U->Filt_Omega_B_N_B_z; 
      /* ----------------------------  Read from Variable */
      /* {Velocity Track..25} */
      Velocity_Track_25_1 = vel_vec[0];
      Velocity_Track_25_2 = vel_vec[1];
      Velocity_Track_25_3 = vel_vec[2];
      /* ----------------------------  Procedure Super Block */
      /* {mth vect norm.1} */
      mth_vect_norm_1_u.Measured_magnetometer_body_x = Velocity_Track_25_1;
      mth_vect_norm_1_u.Measured_magnetometer_body_y = Velocity_Track_25_2;
      mth_vect_norm_1_u.Measured_magnetometer_body_z = Velocity_Track_25_3;
      mth_vect_norm(&mth_vect_norm_1_u, &mth_vect_norm_1_y);
      X_1 = mth_vect_norm_1_y.X_1;
      Y_1 = mth_vect_norm_1_y.Y_1;
      Z = mth_vect_norm_1_y.Z;
      Magnitude = mth_vect_norm_1_y.Magnitude;
      /* ----------------------------  Algebraic Expression */
      /* {Velocity Track.Zero.7} */
      Zero_1 = 0.0; 
      /* ----------------------------  Procedure Super Block */
      /* {mth vect norm.3} */
      mth_vect_norm_3_u.Measured_magnetometer_body_x = X_1;
      mth_vect_norm_3_u.Measured_magnetometer_body_y = Y_1;
      mth_vect_norm_3_u.Measured_magnetometer_body_z = Zero_1;
      mth_vect_norm(&mth_vect_norm_3_u, &mth_vect_norm_3_y);
      X_2 = mth_vect_norm_3_y.X_1;
      Y_2 = mth_vect_norm_3_y.Y_1;
      Z_1 = mth_vect_norm_3_y.Z;
      Magnitude_1 = mth_vect_norm_3_y.Magnitude;
      /* ----------------------------  Procedure Super Block */
      /* {Compute Bogus Sun Quaternion.52} */
      Compute_Bogus_Sun_Quaternion_52_u.Sine_of_Euler_Rotation_Angle = Magnitude_1;
      Compute_Bogus_Sun_Quaternion_52_u.Meas_Sun_Vector_B_x = X_1;
      Compute_Bogus_Sun_Quaternion_52_u.Meas_Sun_Vector_B_y = Y_1;
      Compute_Bogus_Sun_Quaternion(&Compute_Bogus_Sun_Quaternion_52_u, &Compute_Bogus_Sun_Quaternion_52_y);
      Y->q1_Bogus_Sun_N_B = Compute_Bogus_Sun_Quaternion_52_y.q1_Bogus_Sun_N_B;
      Y->q2_Bogus_Sun_N_B = Compute_Bogus_Sun_Quaternion_52_y.q2_Bogus_Sun_N_B;
      Y->q3_Bogus_Sun_N_B = Compute_Bogus_Sun_Quaternion_52_y.q3_Bogus_Sun_N_B;
      Y->q4_Bogus_Sun_N_B = Compute_Bogus_Sun_Quaternion_52_y.q4_Bogus_Sun_N_B;


      ////iinfo[1] = 0;
EXEC_ERROR: return;
}

/******* Procedure: Sun_Pointing_Mode *******/
void Sun_Pointing_Mode(   struct _Sun_Pointing_Mode_u *U
   ,struct _Sun_Pointing_Mode_y *Y
)
{
      
      /***** Local Block Outputs. *****/

      RT_FLOAT Sun_Pointing_Mode_2_1;
      RT_FLOAT Sun_Pointing_Mode_10_1;
      RT_FLOAT Sine_of_Euler_Rotation_Angle;
      struct _Compute_Bogus_Sun_Quaternion_u Compute_Bogus_Sun_Quaternion_7_u;
      struct _Compute_Bogus_Sun_Quaternion_y Compute_Bogus_Sun_Quaternion_7_y;

      /***** Output Update. *****/
      /* ----------------------------  Algebraic Expression */
      /* {Sun Pointing Mode..2} */
      Sun_Pointing_Mode_2_1 = U->Meas_Sun_Vector_B_x*U->Meas_Sun_Vector_B_x + U->Meas_Sun_Vector_B_y*U->Meas_Sun_Vector_B_y; 
      /* ----------------------------  Square Root */
      /* {Sun Pointing Mode..10} */
      Sun_Pointing_Mode_10_1 = sqrt(Sun_Pointing_Mode_2_1); 
      /* ----------------------------  Algebraic Expression */
      /* {Sun Pointing Mode..1} */
      Sine_of_Euler_Rotation_Angle = Sun_Pointing_Mode_10_1 + 1.0E-010; 
      /* ----------------------------  Procedure Super Block */
      /* {Compute Bogus Sun Quaternion.7} */
      Compute_Bogus_Sun_Quaternion_7_u.Sine_of_Euler_Rotation_Angle = Sine_of_Euler_Rotation_Angle;
      Compute_Bogus_Sun_Quaternion_7_u.Meas_Sun_Vector_B_x = U->Meas_Sun_Vector_B_x;
      Compute_Bogus_Sun_Quaternion_7_u.Meas_Sun_Vector_B_y = U->Meas_Sun_Vector_B_y;
      Compute_Bogus_Sun_Quaternion(&Compute_Bogus_Sun_Quaternion_7_u, &Compute_Bogus_Sun_Quaternion_7_y);
      Y->q1_Bogus_Sun_N_B = Compute_Bogus_Sun_Quaternion_7_y.q1_Bogus_Sun_N_B;
      Y->q2_Bogus_Sun_N_B = Compute_Bogus_Sun_Quaternion_7_y.q2_Bogus_Sun_N_B;
      Y->q3_Bogus_Sun_N_B = Compute_Bogus_Sun_Quaternion_7_y.q3_Bogus_Sun_N_B;
      Y->q4_Bogus_Sun_N_B = Compute_Bogus_Sun_Quaternion_7_y.q4_Bogus_Sun_N_B;
      /* ----------------------------  Gain Block */
      /* {Sun Pointing Mode..98} */
      Y->Sun_Pointing_Mode_98_1 = (-1.0)*U->Filt_Omega_B_N_B_x; 
      Y->Sun_Pointing_Mode_98_2 = (-1.0)*U->Filt_Omega_B_N_B_y; 
      Y->Sun_Pointing_Mode_98_3 = (-1.0)*U->Filt_Omega_B_N_B_z; 
      /* ----------------------------  Algebraic Expression */
      /* {Sun Pointing Mode..22} */
      Y->MTR_Enable = U->Mode_9_Momentum_Dump; 


      ////iinfo[1] = 0;
EXEC_ERROR: return;
}

/******* Procedure: Idle_Mode *******/
void Idle_Mode(   struct _Idle_Mode_y *Y
)
{
      
      /***** Output Update. *****/
      /* ----------------------------  Algebraic Expression */
      /* {Idle Mode.Zero outputs.44} */
      Y->Zero_outputs_1_1 = 0.0; 
      /* ----------------------------  Algebraic Expression */
      /* {Idle Mode.Zero outputs.24} */
      Y->Zero_outputs_1 = 0.0; 
      Y->Zero_outputs_2 = 0.0; 
      Y->Zero_outputs_3 = 0.0; 
      Y->Zero_outputs_4 = 1.0; 
      Y->Zero_outputs_5 = 0.0; 
      Y->Zero_outputs_6 = 0.0; 
      Y->Zero_outputs_7 = 0.0; 


      ////iinfo[1] = 0;
EXEC_ERROR: return;
}

/******* Procedure: Sun_Search_Mode *******/
void Sun_Search_Mode(   struct _Sun_Search_Mode_u *U
   ,struct _Sun_Search_Mode_y *Y
)
{
      
      /***** Local Block Outputs. *****/

      RT_FLOAT Sun_Search_Mode_8_1;
      RT_FLOAT Sun_Search_Mode_8_2;
      RT_FLOAT Sun_Search_Mode_8_3;
      RT_FLOAT Sun_Search_Mode_11_1;
      RT_FLOAT Sun_Search_Mode_11_2;
      RT_FLOAT Sun_Search_Mode_11_3;
      RT_FLOAT Sun_Search_Mode_73_1;
      RT_FLOAT Sun_Search_Mode_73_2;
      RT_FLOAT Sun_Search_Mode_73_3;

      /***** Output Update. *****/
      /* ----------------------------  Algebraic Expression */
      /* {Sun Search Mode.Torque Rod Enable.5} */
      Y->Torque_Rod_Enable_1 = 0.0; 
      /* ----------------------------  Algebraic Expression */
      /* {Sun Search Mode.Position Error.7} */
      Y->Position_Error_1 = 0.0; 
      Y->Position_Error_2 = 0.0; 
      Y->Position_Error_3 = 0.0; 
      Y->Position_Error_4 = 1.0; 
      /* ----------------------------  Algebraic Expression */
      /* {Sun Search Mode..8} */
      Sun_Search_Mode_8_1 = U->Commanded_Aqsn_Slew_Rate; 
      Sun_Search_Mode_8_2 = 0.0; 
      Sun_Search_Mode_8_3 = 0.0; 
      /* ----------------------------  Algebraic Expression */
      /* {Sun Search Mode..11} */
      Sun_Search_Mode_11_1 = 0.0; 
      Sun_Search_Mode_11_2 = U->Commanded_Aqsn_Slew_Rate; 
      Sun_Search_Mode_11_3 = 0.0; 
      /* ----------------------------  Data Path Switch */
      /* {Sun Search Mode..73} */
      if (U->Switch_to_Yaw > 0.0) {
         Sun_Search_Mode_73_1 = Sun_Search_Mode_11_1; 
         Sun_Search_Mode_73_2 = Sun_Search_Mode_11_2; 
         Sun_Search_Mode_73_3 = Sun_Search_Mode_11_3; 
      }
      else {
         Sun_Search_Mode_73_1 = Sun_Search_Mode_8_1; 
         Sun_Search_Mode_73_2 = Sun_Search_Mode_8_2; 
         Sun_Search_Mode_73_3 = Sun_Search_Mode_8_3; 
      }
      /* ----------------------------  Summer */
      /* {Sun Search Mode.Rate Error.75} */
      Y->Rate_Error_1 = Sun_Search_Mode_73_1 - U->Filt_Omega_B_N_B_x; 
      Y->Rate_Error_2 = Sun_Search_Mode_73_2 - U->Filt_Omega_B_N_B_y; 
      Y->Rate_Error_3 = Sun_Search_Mode_73_3 - U->Filt_Omega_B_N_B_z; 


      ////iinfo[1] = 0;
EXEC_ERROR: return;
}

/******* Procedure: Sun_Lock_Mode *******/
void Sun_Lock_Mode(   struct _Sun_Lock_Mode_u *U
   ,struct _Sun_Lock_Mode_y *Y
)
{
      struct _Quaternion_Difference_P_u Quaternion_Difference_P_12_u;
      struct _Quaternion_Difference_P_y Quaternion_Difference_P_12_y;

      /***** Output Update. *****/
      /* ----------------------------  Procedure Super Block */
      /* {Quaternion Difference P.12} */
      Quaternion_Difference_P_12_u.Q1_actual = U->Estimated_q1_B_M;
      Quaternion_Difference_P_12_u.Q2_actual = U->Estimated_q2_B_M;
      Quaternion_Difference_P_12_u.Q3_actual = U->Estimated_q3_B_M;
      Quaternion_Difference_P_12_u.Q4_actual = U->Estimated_q4_B_M;
      Quaternion_Difference_P_12_u.Q1_desired = U->Saved_Sun_Vector_q1;
      Quaternion_Difference_P_12_u.Q2_desired = U->Saved_Sun_Vector_q2;
      Quaternion_Difference_P_12_u.Q3_desired = U->Saved_Sun_Vector_q3;
      Quaternion_Difference_P_12_u.Q4_desired = U->Saved_Sun_Vector_q4;
      Quaternion_Difference_P(&Quaternion_Difference_P_12_u, &Quaternion_Difference_P_12_y);
      Y->Quaternion_Difference_P_12_1 = Quaternion_Difference_P_12_y.mth_quat_norm_99_1;
      Y->Quaternion_Difference_P_12_2 = Quaternion_Difference_P_12_y.mth_quat_norm_99_2;
      Y->Quaternion_Difference_P_12_3 = Quaternion_Difference_P_12_y.mth_quat_norm_99_3;
      Y->Quaternion_Difference_P_12_4 = Quaternion_Difference_P_12_y.mth_quat_norm_99_4;
      /* ----------------------------  Gain Block */
      /* {Sun Lock Mode.Flip Omegas.22} */
      Y->Flip_Omegas_1 = (-1.0)*U->Filt_Omega_B_N_B_x; 
      Y->Flip_Omegas_2 = (-1.0)*U->Filt_Omega_B_N_B_y; 
      Y->Flip_Omegas_3 = (-1.0)*U->Filt_Omega_B_N_B_z; 
      /* ----------------------------  Algebraic Expression */
      /* {Sun Lock Mode..3} */
      Y->Zero = 0.0; 


      ////iinfo[1] = 0;
EXEC_ERROR: return;
}

/******* Procedure: Mission_Lock_Mode *******/
void Mission_Lock_Mode(   struct _Mission_Lock_Mode_u *U
   ,struct _Mission_Lock_Mode_y *Y
)
{
      struct _Quaternion_Difference_P_u Quaternion_Difference_P_12_u;
      struct _Quaternion_Difference_P_y Quaternion_Difference_P_12_y;

      /***** Output Update. *****/
      /* ----------------------------  Procedure Super Block */
      /* {Quaternion Difference P.12} */
      Quaternion_Difference_P_12_u.Q1_actual = U->Estimated_q1_B_M;
      Quaternion_Difference_P_12_u.Q2_actual = U->Estimated_q2_B_M;
      Quaternion_Difference_P_12_u.Q3_actual = U->Estimated_q3_B_M;
      Quaternion_Difference_P_12_u.Q4_actual = U->Estimated_q4_B_M;
      Quaternion_Difference_P_12_u.Q1_desired = U->q1_M_LV;
      Quaternion_Difference_P_12_u.Q2_desired = U->q2_M_LV;
      Quaternion_Difference_P_12_u.Q3_desired = U->q3_M_LV;
      Quaternion_Difference_P_12_u.Q4_desired = U->q4_M_LV;
      Quaternion_Difference_P(&Quaternion_Difference_P_12_u, &Quaternion_Difference_P_12_y);
      Y->Quaternion_Difference_P_12_1 = Quaternion_Difference_P_12_y.mth_quat_norm_99_1;
      Y->Quaternion_Difference_P_12_2 = Quaternion_Difference_P_12_y.mth_quat_norm_99_2;
      Y->Quaternion_Difference_P_12_3 = Quaternion_Difference_P_12_y.mth_quat_norm_99_3;
      Y->Quaternion_Difference_P_12_4 = Quaternion_Difference_P_12_y.mth_quat_norm_99_4;
      /* ----------------------------  Gain Block */
      /* {Mission Lock Mode.Flip Omegas.22} */
      Y->Flip_Omegas_1 = (-1.0)*U->Filt_Omega_B_N_B_x; 
      Y->Flip_Omegas_2 = (-1.0)*U->Filt_Omega_B_N_B_y; 
      Y->Flip_Omegas_3 = (-1.0)*U->Filt_Omega_B_N_B_z; 
      /* ----------------------------  Algebraic Expression */
      /* {Mission Lock Mode..3} */
      Y->Zero = 0.0; 


      ////iinfo[1] = 0;
EXEC_ERROR: return;
}

/******* Procedure: Lat_Long_to_ECI *******/
void Lat_Long_to_ECI(   struct _Lat_Long_to_ECI_u *U
   ,struct _Lat_Long_to_ECI_y *Y
)
{
      
      /***** Local Block Outputs. *****/

      RT_FLOAT longitude_radian;
      RT_FLOAT latitude_radians;
      RT_FLOAT Lat_Long_to_ECI_3_1;
      RT_FLOAT Lat_Long_to_ECI_13_1;
      RT_FLOAT Z_ECF;
      RT_FLOAT target_radius;
      RT_FLOAT Lat_Long_to_ECI_99_1;
      RT_FLOAT X_ECF;
      RT_FLOAT Lat_Long_to_ECI_2_1;
      RT_FLOAT Y_ECF;
      RT_FLOAT X_ECF_1;
      RT_FLOAT Y_ECF_1;
      RT_FLOAT Z_ECF_1;
      RT_FLOAT Lat_Long_to_ECI_15_1;
      RT_FLOAT Lat_Long_to_ECI_5_1;
      RT_FLOAT A11;
      RT_FLOAT A12;
      RT_FLOAT A13;
      RT_FLOAT A21;
      RT_FLOAT A22;
      RT_FLOAT A23;
      RT_FLOAT A31;
      RT_FLOAT A32;
      RT_FLOAT A33;

      /***** Output Update. *****/
      /* ----------------------------  Gain Block */
      /* {Lat Long to ECI.degrees to radians.4} */
      longitude_radian = 0.017453292520882225*U->Target_Longitude; 
      latitude_radians = 0.017453292520882225*U->Target_Latitude; 
      /* ----------------------------  Cosine( u ) */
      /* {Lat Long to ECI..3} */
      Lat_Long_to_ECI_3_1 = cos(longitude_radian); 
      /* ----------------------------  Sine( u ) */
      /* {Lat Long to ECI..13} */
      Lat_Long_to_ECI_13_1 = sin(longitude_radian); 
      /* ----------------------------  Sine( u ) */
      /* {Lat Long to ECI..23} */
      Z_ECF = sin(latitude_radians); 
      /* ----------------------------  Algebraic Expression */
      /* {Lat Long to ECI..12} */
      target_radius = U->Target_Altitude + 6.371E+6; 
      /* ----------------------------  Cosine( u ) */
      /* {Lat Long to ECI..99} */
      Lat_Long_to_ECI_99_1 = cos(latitude_radians); 
      /* ----------------------------  ElementProduct */
      /* {Lat Long to ECI..7} */
      X_ECF = Lat_Long_to_ECI_99_1*Lat_Long_to_ECI_3_1; 
      /* ----------------------------  Cosine( u ) */
      /* {Lat Long to ECI..2} */
      Lat_Long_to_ECI_2_1 = cos(latitude_radians); 
      /* ----------------------------  ElementProduct */
      /* {Lat Long to ECI..98} */
      Y_ECF = Lat_Long_to_ECI_2_1*Lat_Long_to_ECI_13_1; 
      /* ----------------------------  ElementProduct */
      /* {Lat Long to ECI..14} */
      X_ECF_1 = X_ECF*target_radius; 
      Y_ECF_1 = Y_ECF*target_radius; 
      Z_ECF_1 = Z_ECF*target_radius; 
      /* ----------------------------  Sine( u ) */
      /* {Lat Long to ECI..15} */
      Lat_Long_to_ECI_15_1 = sin(U->Greenwich_Hour_Angle); 
      /* ----------------------------  Cosine( u ) */
      /* {Lat Long to ECI..5} */
      Lat_Long_to_ECI_5_1 = cos(U->Greenwich_Hour_Angle); 
      /* ----------------------------  Algebraic Expression */
      /* {Lat Long to ECI.ECF to ECI.6} */
      A11 = Lat_Long_to_ECI_5_1; 
      A12 = (-1.0)*Lat_Long_to_ECI_15_1; 
      A13 = 0.0; 
      A21 = Lat_Long_to_ECI_15_1; 
      A22 = Lat_Long_to_ECI_5_1; 
      A23 = 0.0; 
      A31 = 0.0; 
      A32 = 0.0; 
      A33 = 1.0; 
      /* ----------------------------  Dot or Inner Product */
      /* {Lat Long to ECI..24} */
      Y->Target_X_ECI = X_ECF_1*A11 + Y_ECF_1*A12 + Z_ECF_1*A13; 
      /* ----------------------------  Dot or Inner Product */
      /* {Lat Long to ECI..16} */
      Y->Target_Y_ECI = X_ECF_1*A21 + Y_ECF_1*A22 + Z_ECF_1*A23; 
      /* ----------------------------  Dot or Inner Product */
      /* {Lat Long to ECI..17} */
      Y->Target_Z_ECI = X_ECF_1*A31 + Y_ECF_1*A32 + Z_ECF_1*A33; 


      ////iinfo[1] = 0;
EXEC_ERROR: return;
}

/******* Procedure: Calculate_Surface_Point_DCM *******/
void Calculate_Surface_Point_DCM(   struct _Calculate_Surface_Point_DCM_u *U
   ,struct _Calculate_Surface_Point_DCM_y *Y
)
{
      
      /***** Local Block Outputs. *****/

      RT_FLOAT mth_vect_norm_12_4;
      RT_FLOAT A11;
      RT_FLOAT A12;
      RT_FLOAT A13;
      RT_FLOAT mth_vect_norm_2_4;
      RT_FLOAT A11_1;
      RT_FLOAT A12_1;
      RT_FLOAT A13_1;
      RT_FLOAT mth_vect_norm_14_4;
      struct _mth_vect_norm_u mth_vect_norm_12_u;
      struct _mth_vect_norm_y mth_vect_norm_12_y;
      struct _mth_vect_norm_u mth_vect_norm_2_u;
      struct _mth_vect_norm_y mth_vect_norm_2_y;
      struct _mth_vect_norm_u mth_vect_norm_14_u;
      struct _mth_vect_norm_y mth_vect_norm_14_y;

      /***** Output Update. *****/
      /* ----------------------------  Procedure Super Block */
      /* {mth vect norm.12} */
      mth_vect_norm_12_u.Measured_magnetometer_body_x = U->Target_LOS_x_ECI;
      mth_vect_norm_12_u.Measured_magnetometer_body_y = U->Target_LOS_y_ECI;
      mth_vect_norm_12_u.Measured_magnetometer_body_z = U->Target_LOS_z_ECI;
      mth_vect_norm(&mth_vect_norm_12_u, &mth_vect_norm_12_y);
      Y->mth_vect_norm_12_1 = mth_vect_norm_12_y.X_1;
      Y->mth_vect_norm_12_2 = mth_vect_norm_12_y.Y_1;
      Y->mth_vect_norm_12_3 = mth_vect_norm_12_y.Z;
      mth_vect_norm_12_4 = mth_vect_norm_12_y.Magnitude;
      /* ----------------------------  Cross Product */
      /* {Calculate Surface Point DCM..25} */
      A11 = U->Y_1*Y->mth_vect_norm_12_3 - U->Z*Y->mth_vect_norm_12_2; 
      A12 = U->Z*Y->mth_vect_norm_12_1 - U->X_1*Y->mth_vect_norm_12_3; 
      A13 = U->X_1*Y->mth_vect_norm_12_2 - U->Y_1*Y->mth_vect_norm_12_1; 
      /* ----------------------------  Procedure Super Block */
      /* {mth vect norm.2} */
      mth_vect_norm_2_u.Measured_magnetometer_body_x = A11;
      mth_vect_norm_2_u.Measured_magnetometer_body_y = A12;
      mth_vect_norm_2_u.Measured_magnetometer_body_z = A13;
      mth_vect_norm(&mth_vect_norm_2_u, &mth_vect_norm_2_y);
      Y->mth_vect_norm_2_1 = mth_vect_norm_2_y.X_1;
      Y->mth_vect_norm_2_2 = mth_vect_norm_2_y.Y_1;
      Y->mth_vect_norm_2_3 = mth_vect_norm_2_y.Z;
      mth_vect_norm_2_4 = mth_vect_norm_2_y.Magnitude;
      /* ----------------------------  Cross Product */
      /* {Calculate Surface Point DCM..4} */
      A11_1 = Y->mth_vect_norm_12_2*Y->mth_vect_norm_2_3 - Y->mth_vect_norm_12_3*Y->mth_vect_norm_2_2; 
      A12_1 = Y->mth_vect_norm_12_3*Y->mth_vect_norm_2_1 - Y->mth_vect_norm_12_1*Y->mth_vect_norm_2_3; 
      A13_1 = Y->mth_vect_norm_12_1*Y->mth_vect_norm_2_2 - Y->mth_vect_norm_12_2*Y->mth_vect_norm_2_1; 
      /* ----------------------------  Procedure Super Block */
      /* {mth vect norm.14} */
      mth_vect_norm_14_u.Measured_magnetometer_body_x = A11_1;
      mth_vect_norm_14_u.Measured_magnetometer_body_y = A12_1;
      mth_vect_norm_14_u.Measured_magnetometer_body_z = A13_1;
      mth_vect_norm(&mth_vect_norm_14_u, &mth_vect_norm_14_y);
      Y->mth_vect_norm_14_1 = mth_vect_norm_14_y.X_1;
      Y->mth_vect_norm_14_2 = mth_vect_norm_14_y.Y_1;
      Y->mth_vect_norm_14_3 = mth_vect_norm_14_y.Z;
      mth_vect_norm_14_4 = mth_vect_norm_14_y.Magnitude;


      ////iinfo[1] = 0;
EXEC_ERROR: return;
}

/******* Procedure: Quaternion_Require_Continuous_P *******/
void Quaternion_Require_Continuous_P(   struct _Quaternion_Require_Continuous_P_u *U
   ,struct _Quaternion_Require_Continuous_P_y *Y
   ,struct _Quaternion_Require_Continuous_P_s *S
   ,RT_INTEGER INIT
)
{
      
      /***** Current and Next States Pointers. *****/

      struct _Quaternion_Require_Continuous_P_ps *X = &S->Quaternion_Require_Continuous_P_ps[S->Quaternion_Require_Continuous_P_x];
      struct _Quaternion_Require_Continuous_P_ps *XD = &S->Quaternion_Require_Continuous_P_ps[1-S->Quaternion_Require_Continuous_P_x];


      /***** Local Block Outputs. *****/

      RT_FLOAT One_Delay_4_Channels_1;
      RT_FLOAT One_Delay_4_Channels_2;
      RT_FLOAT One_Delay_4_Channels_3;
      RT_FLOAT One_Delay_4_Channels_4;
      RT_FLOAT Quaternion_Require_Continuous_P_15_1;
      RT_FLOAT Quaternion_Require_Continuous_P_15_2;
      RT_FLOAT Quaternion_Require_Continuous_P_15_3;
      RT_FLOAT Quaternion_Require_Continuous_P_15_4;
      RT_FLOAT Norm_1;
      RT_FLOAT Quaternion_Require_Continuous_P_5_1;
      RT_FLOAT Quaternion_Require_Continuous_P_13_1;
      RT_FLOAT Flip_Flop_P_1;
      RT_FLOAT Quaternion_Require_Continuous_P_26_1;
      RT_FLOAT Quaternion_Require_Continuous_P_26_2;
      RT_FLOAT Quaternion_Require_Continuous_P_26_3;
      RT_FLOAT Quaternion_Require_Continuous_P_26_4;

      /***** Algorithmic Local Variables. *****/

//      RT_INTEGER i;
//      RT_INTEGER j;
//      RT_INTEGER k;
      RT_INTEGER KState;

      /******* Initialization. *******/

      if( INIT ) {
         X->One_Delay_4_Channels_S1 = 0.0;
         X->One_Delay_4_Channels_S2 = 0.0;
         X->One_Delay_4_Channels_S3 = 0.0;
         X->One_Delay_4_Channels_S4 = 0.0;
         X->Flip_Flop_P_S1 = 0.0;
         X->Flip_Flop_P_S2 = 0;
         X->Flip_Flop_P_S3 = 0;
         XD->One_Delay_4_Channels_S1 = 0.0;
         XD->One_Delay_4_Channels_S2 = 0.0;
         XD->One_Delay_4_Channels_S3 = 0.0;
         XD->One_Delay_4_Channels_S4 = 0.0;
         XD->Flip_Flop_P_S1 = 0.0;
         XD->Flip_Flop_P_S2 = 0;
         XD->Flip_Flop_P_S3 = 0;
         X->One_Delay_4_Channels_S4 = 1.0;
         X->Flip_Flop_P_S2 = 1;
      }


      /***** Output Update. *****/
      /* ----------------------------  State-Space System */
      /* {Quaternion Require Continuous P.One Delay 4 Channels.7} */
      One_Delay_4_Channels_1 = 0.0; 
      One_Delay_4_Channels_2 = 0.0; 
      One_Delay_4_Channels_3 = 0.0; 
      One_Delay_4_Channels_4 = 0.0; 
      One_Delay_4_Channels_1 = One_Delay_4_Channels_1 + X->One_Delay_4_Channels_S1; 
      One_Delay_4_Channels_2 = One_Delay_4_Channels_2 + X->One_Delay_4_Channels_S2; 
      One_Delay_4_Channels_3 = One_Delay_4_Channels_3 + X->One_Delay_4_Channels_S3; 
      One_Delay_4_Channels_4 = One_Delay_4_Channels_4 + X->One_Delay_4_Channels_S4; 
      /* ----------------------------  Summer */
      /* {Quaternion Require Continuous P..15} */
      Quaternion_Require_Continuous_P_15_1 = U->Q1_desired; 
      Quaternion_Require_Continuous_P_15_1 = Quaternion_Require_Continuous_P_15_1 - One_Delay_4_Channels_1; 
      Quaternion_Require_Continuous_P_15_2 = U->Q2_desired; 
      Quaternion_Require_Continuous_P_15_2 = Quaternion_Require_Continuous_P_15_2 - One_Delay_4_Channels_2; 
      Quaternion_Require_Continuous_P_15_3 = U->Q3_desired; 
      Quaternion_Require_Continuous_P_15_3 = Quaternion_Require_Continuous_P_15_3 - One_Delay_4_Channels_3; 
      Quaternion_Require_Continuous_P_15_4 = U->Q4_desired; 
      Quaternion_Require_Continuous_P_15_4 = Quaternion_Require_Continuous_P_15_4 - One_Delay_4_Channels_4; 
      /* ----------------------------  Algebraic Expression */
      /* {Quaternion Require Continuous P.Norm.14} */
      Norm_1 = Quaternion_Require_Continuous_P_15_1*Quaternion_Require_Continuous_P_15_1 + Quaternion_Require_Continuous_P_15_2*Quaternion_Require_Continuous_P_15_2 + Quaternion_Require_Continuous_P_15_3*Quaternion_Require_Continuous_P_15_3 + Quaternion_Require_Continuous_P_15_4*Quaternion_Require_Continuous_P_15_4; 
      /* ----------------------------  Square Root */
      /* {Quaternion Require Continuous P..5} */
      Quaternion_Require_Continuous_P_5_1 = sqrt(Norm_1); 
      /* ----------------------------  Logical Expression */
      /* {Quaternion Require Continuous P..13} */
      if (Quaternion_Require_Continuous_P_5_1 > 1.0) {
         Quaternion_Require_Continuous_P_13_1 = 1.0; 
      }
      else {
         Quaternion_Require_Continuous_P_13_1 = 0.0; 
      }
      /* ----------------------------  State Transition Diagram */
      /* {Quaternion Require Continuous P.Flip Flop P.99} */
      XD->Flip_Flop_P_S1 = X->Flip_Flop_P_S1; 
      XD->Flip_Flop_P_S2 = X->Flip_Flop_P_S2; 
      if (!INIT) {
         KState = X->Flip_Flop_P_S2; 
         switch (KState)
         {
         case 1:
            if (Quaternion_Require_Continuous_P_13_1 > 0.5) {
               XD->Flip_Flop_P_S2 = 2; 
            }
            break;
         case 2:
            if (Quaternion_Require_Continuous_P_13_1 > 0.5) {
               XD->Flip_Flop_P_S2 = 1; 
            }
            break;
         default:
            break;
         }
      }
      Flip_Flop_P_1 = XD->Flip_Flop_P_S1; 
      KState = XD->Flip_Flop_P_S2; 
      switch (KState)
      {
      case 2:
         Flip_Flop_P_1 = 1.0; 
         break;
      default:
         break;
      }
      /* ----------------------------  Gain Block */
      /* {Quaternion Require Continuous P..26} */
      Quaternion_Require_Continuous_P_26_1 = (-1.0)*U->Q1_desired; 
      Quaternion_Require_Continuous_P_26_2 = (-1.0)*U->Q2_desired; 
      Quaternion_Require_Continuous_P_26_3 = (-1.0)*U->Q3_desired; 
      Quaternion_Require_Continuous_P_26_4 = (-1.0)*U->Q4_desired; 
      /* ----------------------------  Data Path Switch */
      /* {Quaternion Require Continuous P..12} */
      if (Flip_Flop_P_1 > 0.0) {
         Y->q1 = Quaternion_Require_Continuous_P_26_1; 
         Y->q2 = Quaternion_Require_Continuous_P_26_2; 
         Y->q3 = Quaternion_Require_Continuous_P_26_3; 
         Y->q4 = Quaternion_Require_Continuous_P_26_4; 
      }
      else {
         Y->q1 = U->Q1_desired; 
         Y->q2 = U->Q2_desired; 
         Y->q3 = U->Q3_desired; 
         Y->q4 = U->Q4_desired; 
      }

      /***** State Update. *****/
      /* ----------------------------  State-Space System */
      /* {Quaternion Require Continuous P.One Delay 4 Channels.7} */
      XD->One_Delay_4_Channels_S1 = 0.0; 
      XD->One_Delay_4_Channels_S2 = 0.0; 
      XD->One_Delay_4_Channels_S3 = 0.0; 
      XD->One_Delay_4_Channels_S4 = 0.0; 
      XD->One_Delay_4_Channels_S1 = XD->One_Delay_4_Channels_S1 + U->Q1_desired; 
      XD->One_Delay_4_Channels_S2 = XD->One_Delay_4_Channels_S2 + U->Q2_desired; 
      XD->One_Delay_4_Channels_S3 = XD->One_Delay_4_Channels_S3 + U->Q3_desired; 
      XD->One_Delay_4_Channels_S4 = XD->One_Delay_4_Channels_S4 + U->Q4_desired; 


      ////iinfo[1] = 0;
EXEC_ERROR: return;
}

/******* Procedure: Quaternion_From_DC_P *******/
void Quaternion_From_DC_P(   struct _Quaternion_From_DC_P_u *U
   ,struct _Quaternion_From_DC_P_y *Y
   ,struct _Quaternion_From_DC_P_s *S
   ,struct _Quaternion_From_DC_P_info *I
)
{
      RT_INTEGER *iinfo = &I->iinfo[0];
      RT_INTEGER INIT = iinfo[1];

      /***** Parameters. *****/

      RT_FLOAT *R_P = &I->RP[0];
      RT_INTEGER cnt;
      static const RT_FLOAT _R_P[4] = {0.0, 0.0, 1.0, 0.0};

      /***** Local Block Outputs. *****/

      RT_FLOAT SUM1;
      RT_FLOAT SUM2;
      RT_FLOAT SUM3;
      RT_FLOAT SUM4;
      RT_FLOAT Quaternion_From_DC_P_3_1;
      RT_FLOAT Quaternion_From_DC_P_3_2;
      RT_FLOAT Quaternion_From_DC_P_3_3;
      RT_FLOAT Quaternion_From_DC_P_3_4;
      RT_FLOAT ENABLE_LOGIC_1;
      RT_FLOAT ENABLE_LOGIC_2;
      RT_FLOAT ENABLE_LOGIC_3;
      RT_FLOAT ENABLE_LOGIC_4;
      RT_FLOAT Quaternion_From_DC_P_99_1;
      RT_FLOAT Quaternion_From_DC_P_99_2;
      RT_FLOAT Quaternion_From_DC_P_99_3;
      RT_FLOAT Quaternion_From_DC_P_99_4;
      RT_FLOAT Q1;
      RT_FLOAT Q2;
      RT_FLOAT Q3;
      RT_FLOAT Q4;
      RT_FLOAT Q2_1;
      RT_FLOAT Q1_1;
      RT_FLOAT Q3_1;
      RT_FLOAT Q4_1;
      RT_FLOAT Q3_2;
      RT_FLOAT Q1_2;
      RT_FLOAT Q2_2;
      RT_FLOAT Q4_2;
      RT_FLOAT Q4_3;
      RT_FLOAT Q1_3;
      RT_FLOAT Q2_3;
      RT_FLOAT Q3_3;
      struct _Quaternion_Require_Continuous_P_u Quaternion_Require_Continuous_P_16_u;
      struct _Quaternion_Require_Continuous_P_y Quaternion_Require_Continuous_P_16_y;

      /******* Initialization. *******/

      if( INIT ) {
         S->Quaternion_Require_Continuous_P_16_s.Quaternion_Require_Continuous_P_x = 0;
         for( cnt=0;cnt<4;cnt++ ) {
            R_P[cnt] = _R_P[cnt];
         }
      }


      /***** Output Update. *****/
      /* ----------------------------  Algebraic Expression */
      /* {Quaternion From DC P.SUMMER.6} */
      SUM1 = 1.0 + U->A11 - U->A22 - U->A33; 
      SUM2 = 1.0 - U->A11 + U->A22 - U->A33; 
      SUM3 = 1.0 - U->A11 - U->A22 + U->A33; 
      SUM4 = 1.0 + U->A11 + U->A22 + U->A33; 
      /* ----------------------------  Absolute Value */
      /* {Quaternion From DC P..3} */
      Quaternion_From_DC_P_3_1 = fabs(SUM1); 
      Quaternion_From_DC_P_3_2 = fabs(SUM2); 
      Quaternion_From_DC_P_3_3 = fabs(SUM3); 
      Quaternion_From_DC_P_3_4 = fabs(SUM4); 
      /* ----------------------------  Logical Expression */
      /* {Quaternion From DC P.ENABLE LOGIC.93} */
      if (Quaternion_From_DC_P_3_1 >= Quaternion_From_DC_P_3_2 && Quaternion_From_DC_P_3_1 >= Quaternion_From_DC_P_3_3 && Quaternion_From_DC_P_3_1 >= Quaternion_From_DC_P_3_4) {
         *R_P = 1.0; 
      }
      else {
         *R_P = 0.0; 
      }
      if (Quaternion_From_DC_P_3_2 >= Quaternion_From_DC_P_3_1 && Quaternion_From_DC_P_3_2 >= Quaternion_From_DC_P_3_3 && Quaternion_From_DC_P_3_2 >= Quaternion_From_DC_P_3_4) {
         *(R_P+1) = 1.0; 
      }
      else {
         *(R_P+1) = 0.0; 
      }
      if (Quaternion_From_DC_P_3_3 >= Quaternion_From_DC_P_3_1 && Quaternion_From_DC_P_3_3 >= Quaternion_From_DC_P_3_2 && Quaternion_From_DC_P_3_3 >= Quaternion_From_DC_P_3_4) {
         *(R_P+2) = 1.0; 
      }
      else {
         *(R_P+2) = 0.0; 
      }
      if (Quaternion_From_DC_P_3_4 >= Quaternion_From_DC_P_3_1 && Quaternion_From_DC_P_3_4 >= Quaternion_From_DC_P_3_2 && Quaternion_From_DC_P_3_4 >= Quaternion_From_DC_P_3_3) {
         *(R_P+3) = 1.0; 
      }
      else {
         *(R_P+3) = 0.0; 
      }
      if (*R_P > 0.0) {
         ENABLE_LOGIC_1 = 1.0; 
      }
      else {
         ENABLE_LOGIC_1 = 0.0; 
      }
      if (*(R_P+1) > 0.0 && ENABLE_LOGIC_1 <= 0.0) {
         ENABLE_LOGIC_2 = 1.0; 
      }
      else {
         ENABLE_LOGIC_2 = 0.0; 
      }
      if (*(R_P+2) > 0.0 && ENABLE_LOGIC_1 <= 0.0 && ENABLE_LOGIC_2 <= 0.0) {
         ENABLE_LOGIC_3 = 1.0; 
      }
      else {
         ENABLE_LOGIC_3 = 0.0; 
      }
      if (*(R_P+3) > 0.0 && ENABLE_LOGIC_1 <= 0.0 && ENABLE_LOGIC_2 <= 0.0 && ENABLE_LOGIC_3 <= 0.0) {
         ENABLE_LOGIC_4 = 1.0; 
      }
      else {
         ENABLE_LOGIC_4 = 0.0; 
      }
      /* ----------------------------  Square Root */
      /* {Quaternion From DC P..99} */
      Quaternion_From_DC_P_99_1 = sqrt(Quaternion_From_DC_P_3_1); 
      Quaternion_From_DC_P_99_2 = sqrt(Quaternion_From_DC_P_3_2); 
      Quaternion_From_DC_P_99_3 = sqrt(Quaternion_From_DC_P_3_3); 
      Quaternion_From_DC_P_99_4 = sqrt(Quaternion_From_DC_P_3_4); 
      /* ----------------------------  Algebraic Expression */
      /* {Quaternion From DC P..2} */
      Q1 = 0.5*Quaternion_From_DC_P_99_1 + 1.0E-7; 
      Q2 = 0.25*(U->A12 + U->A21)/Q1; 
      Q3 = 0.25*(U->A13 + U->A31)/Q1; 
      Q4 = 0.25*(U->A23 - U->A32)/Q1; 
      /* ----------------------------  Algebraic Expression */
      /* {Quaternion From DC P..8} */
      Q2_1 = 0.5*Quaternion_From_DC_P_99_2 + 1.0E-7; 
      Q1_1 = 0.25*(U->A12 + U->A21)/Q2_1; 
      Q3_1 = 0.25*(U->A23 + U->A32)/Q2_1; 
      Q4_1 = 0.25*(U->A31 - U->A13)/Q2_1; 
      /* ----------------------------  Algebraic Expression */
      /* {Quaternion From DC P..4} */
      Q3_2 = 0.5*Quaternion_From_DC_P_99_3 + 1.0E-7; 
      Q1_2 = 0.25*(U->A13 + U->A31)/Q3_2; 
      Q2_2 = 0.25*(U->A23 + U->A32)/Q3_2; 
      Q4_2 = 0.25*(U->A12 - U->A21)/Q3_2; 
      /* ----------------------------  Algebraic Expression */
      /* {Quaternion From DC P..5} */
      Q4_3 = 0.5*Quaternion_From_DC_P_99_4 + 1.0E-7; 
      Q1_3 = 0.25*(U->A23 - U->A32)/Q4_3; 
      Q2_3 = 0.25*(U->A31 - U->A13)/Q4_3; 
      Q3_3 = 0.25*(U->A12 - U->A21)/Q4_3; 
      /* ----------------------------  Algebraic Expression */
      /* {Quaternion From DC P.Switch.7} */
      Y->Q1_desired = ENABLE_LOGIC_1*Q1 + ENABLE_LOGIC_2*Q1_1 + ENABLE_LOGIC_3*Q1_2 + ENABLE_LOGIC_4*Q1_3; 
      Y->Q2_desired = ENABLE_LOGIC_1*Q2 + ENABLE_LOGIC_2*Q2_1 + ENABLE_LOGIC_3*Q2_2 + ENABLE_LOGIC_4*Q2_3; 
      Y->Q3_desired = ENABLE_LOGIC_1*Q3 + ENABLE_LOGIC_2*Q3_1 + ENABLE_LOGIC_3*Q3_2 + ENABLE_LOGIC_4*Q3_3; 
      Y->Q4_desired = ENABLE_LOGIC_1*Q4 + ENABLE_LOGIC_2*Q4_1 + ENABLE_LOGIC_3*Q4_2 + ENABLE_LOGIC_4*Q4_3; 
      /* ----------------------------  Procedure Super Block */
      /* {Quaternion Require Continuous P.16} */
      Quaternion_Require_Continuous_P_16_u.Q1_desired = Y->Q1_desired;
      Quaternion_Require_Continuous_P_16_u.Q2_desired = Y->Q2_desired;
      Quaternion_Require_Continuous_P_16_u.Q3_desired = Y->Q3_desired;
      Quaternion_Require_Continuous_P_16_u.Q4_desired = Y->Q4_desired;
      Quaternion_Require_Continuous_P(&Quaternion_Require_Continuous_P_16_u, &Quaternion_Require_Continuous_P_16_y, &S->Quaternion_Require_Continuous_P_16_s, INIT);
      S->Quaternion_Require_Continuous_P_16_s.Quaternion_Require_Continuous_P_x = 1 - S->Quaternion_Require_Continuous_P_16_s.Quaternion_Require_Continuous_P_x;
      Y->q1 = Quaternion_Require_Continuous_P_16_y.q1;
      Y->q2 = Quaternion_Require_Continuous_P_16_y.q2;
      Y->q3 = Quaternion_Require_Continuous_P_16_y.q3;
      Y->q4 = Quaternion_Require_Continuous_P_16_y.q4;


      ////iinfo[1] = 0;
EXEC_ERROR: return;
}

/******* Procedure: Quaternion_Sign_Flip_P *******/
void Quaternion_Sign_Flip_P(   struct _Quaternion_Sign_Flip_P_u *U
   ,struct _Quaternion_Sign_Flip_P_y *Y
)
{
      
      /***** Local Block Outputs. *****/

      RT_FLOAT Quaternion_Sign_Flip_P_4_1;
      RT_FLOAT Quaternion_Sign_Flip_P_4_2;
      RT_FLOAT Quaternion_Sign_Flip_P_4_3;
      RT_FLOAT Quaternion_Sign_Flip_P_4_4;
      RT_FLOAT Quaternion_Sign_Flip_P_5_1;

      /***** Output Update. *****/
      /* ----------------------------  Gain Block */
      /* {Quaternion Sign Flip P..4} */
      Quaternion_Sign_Flip_P_4_1 = (-1.0)*U->q1_Error; 
      Quaternion_Sign_Flip_P_4_2 = (-1.0)*U->q2_Error; 
      Quaternion_Sign_Flip_P_4_3 = (-1.0)*U->q3_Error; 
      Quaternion_Sign_Flip_P_4_4 = (-1.0)*U->q4_Error; 
      /* ----------------------------  Logical Expression */
      /* {Quaternion Sign Flip P..5} */
      if (U->q4_Error < 0.0) {
         Quaternion_Sign_Flip_P_5_1 = 1.0; 
      }
      else {
         Quaternion_Sign_Flip_P_5_1 = 0.0; 
      }
      /* ----------------------------  Data Path Switch */
      /* {Quaternion Sign Flip P..6} */
      if (Quaternion_Sign_Flip_P_5_1 > 0.0) {
         Y->Quaternion_Sign_Flip_P_6_1 = Quaternion_Sign_Flip_P_4_1; 
         Y->Quaternion_Sign_Flip_P_6_2 = Quaternion_Sign_Flip_P_4_2; 
         Y->Quaternion_Sign_Flip_P_6_3 = Quaternion_Sign_Flip_P_4_3; 
         Y->Quaternion_Sign_Flip_P_6_4 = Quaternion_Sign_Flip_P_4_4; 
      }
      else {
         Y->Quaternion_Sign_Flip_P_6_1 = U->q1_Error; 
         Y->Quaternion_Sign_Flip_P_6_2 = U->q2_Error; 
         Y->Quaternion_Sign_Flip_P_6_3 = U->q3_Error; 
         Y->Quaternion_Sign_Flip_P_6_4 = U->q4_Error; 
      }


      ////iinfo[1] = 0;
EXEC_ERROR: return;
}

/******* Procedure: Surface_Point_Track_rate *******/
void Surface_Point_Track_rate(   struct _Surface_Point_Track_rate_u *U
   ,struct _Surface_Point_Track_rate_y *Y
   ,struct _Surface_Point_Track_rate_s *S
   ,RT_INTEGER INIT
)
{
      
      /***** Current and Next States Pointers. *****/

      struct _Surface_Point_Track_rate_ps *X = &S->Surface_Point_Track_rate_ps[S->Surface_Point_Track_rate_x];
      struct _Surface_Point_Track_rate_ps *XD = &S->Surface_Point_Track_rate_ps[1-S->Surface_Point_Track_rate_x];


      /***** Local Block Outputs. *****/

      RT_FLOAT Four_Channel_Single_Delay_1;
      RT_FLOAT Four_Channel_Single_Delay_2;
      RT_FLOAT Four_Channel_Single_Delay_3;
      RT_FLOAT Four_Channel_Single_Delay_4;
      RT_FLOAT q1_Error;
      RT_FLOAT q2_Error;
      RT_FLOAT q3_Error;
      RT_FLOAT q4_Error;
      RT_FLOAT Quaternion_Sign_Flip_P_14_1;
      RT_FLOAT Quaternion_Sign_Flip_P_14_2;
      RT_FLOAT Quaternion_Sign_Flip_P_14_3;
      RT_FLOAT Quaternion_Sign_Flip_P_14_4;
      RT_FLOAT convert_to_delta_angles_1;
      RT_FLOAT convert_to_delta_angles_2;
      RT_FLOAT convert_to_delta_angles_3;

      /***** Algorithmic Local Variables. *****/

//      RT_INTEGER i;
//      RT_INTEGER j;
//      RT_INTEGER k;
      struct _Quaternion_Difference_P_u Quaternion_Difference_P_23_u;
      struct _Quaternion_Difference_P_y Quaternion_Difference_P_23_y;
      struct _Quaternion_Sign_Flip_P_u Quaternion_Sign_Flip_P_14_u;
      struct _Quaternion_Sign_Flip_P_y Quaternion_Sign_Flip_P_14_y;

      /******* Initialization. *******/

      if( INIT ) {
         X->Four_Channel_Single_Delay_S1 = 0.0;
         X->Four_Channel_Single_Delay_S2 = 0.0;
         X->Four_Channel_Single_Delay_S3 = 0.0;
         X->Four_Channel_Single_Delay_S4 = 0.0;
         XD->Four_Channel_Single_Delay_S1 = 0.0;
         XD->Four_Channel_Single_Delay_S2 = 0.0;
         XD->Four_Channel_Single_Delay_S3 = 0.0;
         XD->Four_Channel_Single_Delay_S4 = 0.0;
      }


      /***** Output Update. *****/
      /* ----------------------------  State-Space System */
      /* {Surface Point Track rate.Four Channel Single Delay.13} */
      Four_Channel_Single_Delay_1 = 0.0; 
      Four_Channel_Single_Delay_2 = 0.0; 
      Four_Channel_Single_Delay_3 = 0.0; 
      Four_Channel_Single_Delay_4 = 0.0; 
      Four_Channel_Single_Delay_1 = Four_Channel_Single_Delay_1 + X->Four_Channel_Single_Delay_S1; 
      Four_Channel_Single_Delay_2 = Four_Channel_Single_Delay_2 + X->Four_Channel_Single_Delay_S2; 
      Four_Channel_Single_Delay_3 = Four_Channel_Single_Delay_3 + X->Four_Channel_Single_Delay_S3; 
      Four_Channel_Single_Delay_4 = Four_Channel_Single_Delay_4 + X->Four_Channel_Single_Delay_S4; 
      /* ----------------------------  Procedure Super Block */
      /* {Quaternion Difference P.23} */
      Quaternion_Difference_P_23_u.Q1_actual = U->q1_P_ECI;
      Quaternion_Difference_P_23_u.Q2_actual = U->q2_P_ECI;
      Quaternion_Difference_P_23_u.Q3_actual = U->q3_P_ECI;
      Quaternion_Difference_P_23_u.Q4_actual = U->q4_P_ECI;
      Quaternion_Difference_P_23_u.Q1_desired = Four_Channel_Single_Delay_1;
      Quaternion_Difference_P_23_u.Q2_desired = Four_Channel_Single_Delay_2;
      Quaternion_Difference_P_23_u.Q3_desired = Four_Channel_Single_Delay_3;
      Quaternion_Difference_P_23_u.Q4_desired = Four_Channel_Single_Delay_4;
      Quaternion_Difference_P(&Quaternion_Difference_P_23_u, &Quaternion_Difference_P_23_y);
      q1_Error = Quaternion_Difference_P_23_y.mth_quat_norm_99_1;
      q2_Error = Quaternion_Difference_P_23_y.mth_quat_norm_99_2;
      q3_Error = Quaternion_Difference_P_23_y.mth_quat_norm_99_3;
      q4_Error = Quaternion_Difference_P_23_y.mth_quat_norm_99_4;
      /* ----------------------------  Procedure Super Block */
      /* {Quaternion Sign Flip P.14} */
      Quaternion_Sign_Flip_P_14_u.q1_Error = q1_Error;
      Quaternion_Sign_Flip_P_14_u.q2_Error = q2_Error;
      Quaternion_Sign_Flip_P_14_u.q3_Error = q3_Error;
      Quaternion_Sign_Flip_P_14_u.q4_Error = q4_Error;
      Quaternion_Sign_Flip_P(&Quaternion_Sign_Flip_P_14_u, &Quaternion_Sign_Flip_P_14_y);
      Quaternion_Sign_Flip_P_14_1 = Quaternion_Sign_Flip_P_14_y.Quaternion_Sign_Flip_P_6_1;
      Quaternion_Sign_Flip_P_14_2 = Quaternion_Sign_Flip_P_14_y.Quaternion_Sign_Flip_P_6_2;
      Quaternion_Sign_Flip_P_14_3 = Quaternion_Sign_Flip_P_14_y.Quaternion_Sign_Flip_P_6_3;
      Quaternion_Sign_Flip_P_14_4 = Quaternion_Sign_Flip_P_14_y.Quaternion_Sign_Flip_P_6_4;
      /* ----------------------------  Gain Block */
      /* {Surface Point Track rate.convert to delta angles.1} */
      convert_to_delta_angles_1 = 2.0*Quaternion_Sign_Flip_P_14_1; 
      convert_to_delta_angles_2 = 2.0*Quaternion_Sign_Flip_P_14_2; 
      convert_to_delta_angles_3 = 2.0*Quaternion_Sign_Flip_P_14_3; 
      /* ----------------------------  Gain Block */
      /* {Surface Point Track rate.one over delta t.2} */
      Y->Omega_P_ECI_P_x = 5.0*convert_to_delta_angles_1; 
      Y->Omega_P_ECI_P_y = 5.0*convert_to_delta_angles_2; 
      Y->Omega_P_ECI_P_z = 5.0*convert_to_delta_angles_3; 

      /***** State Update. *****/
      /* ----------------------------  State-Space System */
      /* {Surface Point Track rate.Four Channel Single Delay.13} */
      XD->Four_Channel_Single_Delay_S1 = 0.0; 
      XD->Four_Channel_Single_Delay_S2 = 0.0; 
      XD->Four_Channel_Single_Delay_S3 = 0.0; 
      XD->Four_Channel_Single_Delay_S4 = 0.0; 
      XD->Four_Channel_Single_Delay_S1 = XD->Four_Channel_Single_Delay_S1 + U->q1_P_ECI; 
      XD->Four_Channel_Single_Delay_S2 = XD->Four_Channel_Single_Delay_S2 + U->q2_P_ECI; 
      XD->Four_Channel_Single_Delay_S3 = XD->Four_Channel_Single_Delay_S3 + U->q3_P_ECI; 
      XD->Four_Channel_Single_Delay_S4 = XD->Four_Channel_Single_Delay_S4 + U->q4_P_ECI; 


      ////iinfo[1] = 0;
EXEC_ERROR: return;
}

/******* Procedure: Quaternion_Multiply *******/
void Quaternion_Multiply(   struct _Quaternion_Multiply_u *U
   ,struct _Quaternion_Multiply_y *Y
)
{
      
      /***** Output Update. *****/
      /* ----------------------------  Algebraic Expression */
      /* {Quaternion Multiply.Q MULT CALC.1} */
      Y->q1_A_C = U->q4_A_B*U->q1_B_C + U->q3_A_B*U->q2_B_C - U->q2_A_B*U->q3_B_C + U->q1_A_B*U->q4_B_C; 
      Y->q2_A_C = (-1.0)*U->q3_A_B*U->q1_B_C + U->q4_A_B*U->q2_B_C + U->q1_A_B*U->q3_B_C + U->q2_A_B*U->q4_B_C; 
      Y->q3_A_C = U->q2_A_B*U->q1_B_C - U->q1_A_B*U->q2_B_C + U->q4_A_B*U->q3_B_C + U->q3_A_B*U->q4_B_C; 
      Y->q4_A_C = (-1.0)*U->q1_A_B*U->q1_B_C - U->q2_A_B*U->q2_B_C - U->q3_A_B*U->q3_B_C + U->q4_A_B*U->q4_B_C; 


      ////iinfo[1] = 0;
EXEC_ERROR: return;
}

/******* Procedure: Mission_Surface_Track *******/
void Mission_Surface_Track(   struct _Mission_Surface_Track_u *U
   ,struct _Mission_Surface_Track_y *Y
   ,struct _Mission_Surface_Track_s *S
   ,struct _Mission_Surface_Track_info *I
)
{
      RT_INTEGER *iinfo = &I->iinfo[0];
      RT_INTEGER INIT = iinfo[1];

      /***** Local Block Outputs. *****/

      RT_FLOAT Target_X_ECI;
      RT_FLOAT Target_Y_ECI;
      RT_FLOAT Target_Z_ECI;
      RT_FLOAT Target_LOS_x_ECI;
      RT_FLOAT Target_LOS_y_ECI;
      RT_FLOAT Target_LOS_z_ECI;
      RT_FLOAT Mission_Surface_Track_2_1;
      RT_FLOAT Mission_Surface_Track_2_2;
      RT_FLOAT Mission_Surface_Track_2_3;
      RT_FLOAT X_1;
      RT_FLOAT Y_1;
      RT_FLOAT Z;
      RT_FLOAT Magnitude;
      RT_FLOAT X_2;
      RT_FLOAT Y_2;
      RT_FLOAT Z_1;
      RT_FLOAT Magnitude_1;
      RT_FLOAT Mission_Surface_Track_11_1;
      RT_FLOAT Mission_Surface_Track_11_2;
      RT_FLOAT Mission_Surface_Track_11_3;
      RT_FLOAT X_3;
      RT_FLOAT Y_3;
      RT_FLOAT Z_2;
      RT_FLOAT Magnitude_2;
      RT_FLOAT Calculate_Surface_Point_DCM_8_1;
      RT_FLOAT Calculate_Surface_Point_DCM_8_2;
      RT_FLOAT Calculate_Surface_Point_DCM_8_3;
      RT_FLOAT Calculate_Surface_Point_DCM_8_4;
      RT_FLOAT Calculate_Surface_Point_DCM_8_5;
      RT_FLOAT Calculate_Surface_Point_DCM_8_6;
      RT_FLOAT Calculate_Surface_Point_DCM_8_7;
      RT_FLOAT Calculate_Surface_Point_DCM_8_8;
      RT_FLOAT Calculate_Surface_Point_DCM_8_9;
      RT_FLOAT q1_P_ECI;
      RT_FLOAT q2_P_ECI;
      RT_FLOAT q3_P_ECI;
      RT_FLOAT q4_P_ECI;
      RT_FLOAT Quaternion_From_DC_P_25_5;
      RT_FLOAT Quaternion_From_DC_P_25_6;
      RT_FLOAT Quaternion_From_DC_P_25_7;
      RT_FLOAT Quaternion_From_DC_P_25_8;
      RT_FLOAT Omega_P_ECI_P_x;
      RT_FLOAT Omega_P_ECI_P_y;
      RT_FLOAT Omega_P_ECI_P_z;
      RT_FLOAT Mission_Surface_Track_99_1;
      RT_FLOAT Mission_Surface_Track_99_2;
      RT_FLOAT Mission_Surface_Track_99_3;
      RT_FLOAT Mission_Surface_Track_99_4;
      RT_FLOAT q1_A_C;
      RT_FLOAT q2_A_C;
      RT_FLOAT q3_A_C;
      RT_FLOAT q4_A_C;
      RT_FLOAT Target_q1;
      RT_FLOAT Target_q2;
      RT_FLOAT Target_q3;
      RT_FLOAT Target_q4;
      struct _Lat_Long_to_ECI_u Lat_Long_to_ECI_7_u;
      struct _Lat_Long_to_ECI_y Lat_Long_to_ECI_7_y;
      struct _mth_vect_norm_u mth_vect_norm_21_u;
      struct _mth_vect_norm_y mth_vect_norm_21_y;
      struct _mth_vect_norm_u mth_vect_norm_12_u;
      struct _mth_vect_norm_y mth_vect_norm_12_y;
      struct _mth_vect_norm_u mth_vect_norm_1_u;
      struct _mth_vect_norm_y mth_vect_norm_1_y;
      struct _Calculate_Surface_Point_DCM_u Calculate_Surface_Point_DCM_8_u;
      struct _Calculate_Surface_Point_DCM_y Calculate_Surface_Point_DCM_8_y;
      struct _Quaternion_From_DC_P_u Quaternion_From_DC_P_25_u;
      struct _Quaternion_From_DC_P_y Quaternion_From_DC_P_25_y;
      struct _Surface_Point_Track_rate_u Surface_Point_Track_rate_9_u;
      struct _Surface_Point_Track_rate_y Surface_Point_Track_rate_9_y;
      struct _Quaternion_Multiply_u Quaternion_Multiply_3_u;
      struct _Quaternion_Multiply_y Quaternion_Multiply_3_y;
      struct _mth_quat_norm_u mth_quat_norm_6_u;
      struct _mth_quat_norm_y mth_quat_norm_6_y;
      struct _Quaternion_Difference_P_u Quaternion_Difference_P_4_u;
      struct _Quaternion_Difference_P_y Quaternion_Difference_P_4_y;

      /******* Initialization. *******/

      if( INIT ) {
         S->Surface_Point_Track_rate_9_s.Surface_Point_Track_rate_x = 0;
         I->Quaternion_From_DC_P_25_i.iinfo[0] = iinfo[0];
         I->Quaternion_From_DC_P_25_i.iinfo[1] = iinfo[1];
         I->Quaternion_From_DC_P_25_i.iinfo[3] = iinfo[3];
      }


      /***** Output Update. *****/
      /* ----------------------------  Procedure Super Block */
      /* {Lat Long to ECI.7} */
      Lat_Long_to_ECI_7_u.Target_Longitude = U->Target_Longitude;
      Lat_Long_to_ECI_7_u.Target_Latitude = U->Target_Latitude;
      Lat_Long_to_ECI_7_u.Target_Altitude = U->Target_Altitude;
      Lat_Long_to_ECI_7_u.Greenwich_Hour_Angle = U->Greenwich_Hour_Angle;
      Lat_Long_to_ECI(&Lat_Long_to_ECI_7_u, &Lat_Long_to_ECI_7_y);
      Target_X_ECI = Lat_Long_to_ECI_7_y.Target_X_ECI;
      Target_Y_ECI = Lat_Long_to_ECI_7_y.Target_Y_ECI;
      Target_Z_ECI = Lat_Long_to_ECI_7_y.Target_Z_ECI;
      /* ----------------------------  Summer */
      /* {Mission Surface Track.Target minus SV Position.23} */
      Target_LOS_x_ECI = Target_X_ECI - U->Pos_ECIo_LVo_ECI_x; 
      Target_LOS_y_ECI = Target_Y_ECI - U->Pos_ECIo_LVo_ECI_y; 
      Target_LOS_z_ECI = Target_Z_ECI - U->Pos_ECIo_LVo_ECI_z; 
      /* ----------------------------  Gain Block */
      /* {Mission Surface Track..2} */
      Mission_Surface_Track_2_1 = (-1.0)*U->Pos_ECIo_LVo_ECI_x; 
      Mission_Surface_Track_2_2 = (-1.0)*U->Pos_ECIo_LVo_ECI_y; 
      Mission_Surface_Track_2_3 = (-1.0)*U->Pos_ECIo_LVo_ECI_z; 
      /* ----------------------------  Procedure Super Block */
      /* {mth vect norm.21} */
      mth_vect_norm_21_u.Measured_magnetometer_body_x = Mission_Surface_Track_2_1;
      mth_vect_norm_21_u.Measured_magnetometer_body_y = Mission_Surface_Track_2_2;
      mth_vect_norm_21_u.Measured_magnetometer_body_z = Mission_Surface_Track_2_3;
      mth_vect_norm(&mth_vect_norm_21_u, &mth_vect_norm_21_y);
      X_1 = mth_vect_norm_21_y.X_1;
      Y_1 = mth_vect_norm_21_y.Y_1;
      Z = mth_vect_norm_21_y.Z;
      Magnitude = mth_vect_norm_21_y.Magnitude;
      /* ----------------------------  Procedure Super Block */
      /* {mth vect norm.12} */
      mth_vect_norm_12_u.Measured_magnetometer_body_x = U->Sun_in_ECI_x;
      mth_vect_norm_12_u.Measured_magnetometer_body_y = U->Sun_in_ECI_y;
      mth_vect_norm_12_u.Measured_magnetometer_body_z = U->Sun_in_ECI_z;
      mth_vect_norm(&mth_vect_norm_12_u, &mth_vect_norm_12_y);
      X_2 = mth_vect_norm_12_y.X_1;
      Y_2 = mth_vect_norm_12_y.Y_1;
      Z_1 = mth_vect_norm_12_y.Z;
      Magnitude_1 = mth_vect_norm_12_y.Magnitude;
      /* ----------------------------  Cross Product */
      /* {Mission Surface Track..11} */
      Mission_Surface_Track_11_1 = Y_1*Z_1 - Z*Y_2; 
      Mission_Surface_Track_11_2 = Z*X_2 - X_1*Z_1; 
      Mission_Surface_Track_11_3 = X_1*Y_2 - Y_1*X_2; 
      /* ----------------------------  Procedure Super Block */
      /* {mth vect norm.1} */
      mth_vect_norm_1_u.Measured_magnetometer_body_x = Mission_Surface_Track_11_1;
      mth_vect_norm_1_u.Measured_magnetometer_body_y = Mission_Surface_Track_11_2;
      mth_vect_norm_1_u.Measured_magnetometer_body_z = Mission_Surface_Track_11_3;
      mth_vect_norm(&mth_vect_norm_1_u, &mth_vect_norm_1_y);
      X_3 = mth_vect_norm_1_y.X_1;
      Y_3 = mth_vect_norm_1_y.Y_1;
      Z_2 = mth_vect_norm_1_y.Z;
      Magnitude_2 = mth_vect_norm_1_y.Magnitude;
      /* ----------------------------  Procedure Super Block */
      /* {Calculate Surface Point DCM.8} */
      Calculate_Surface_Point_DCM_8_u.Target_LOS_x_ECI = Target_LOS_x_ECI;
      Calculate_Surface_Point_DCM_8_u.Target_LOS_y_ECI = Target_LOS_y_ECI;
      Calculate_Surface_Point_DCM_8_u.Target_LOS_z_ECI = Target_LOS_z_ECI;
      Calculate_Surface_Point_DCM_8_u.X_1 = X_3;
      Calculate_Surface_Point_DCM_8_u.Y_1 = Y_3;
      Calculate_Surface_Point_DCM_8_u.Z = Z_2;
      Calculate_Surface_Point_DCM(&Calculate_Surface_Point_DCM_8_u, &Calculate_Surface_Point_DCM_8_y);
      Calculate_Surface_Point_DCM_8_1 = Calculate_Surface_Point_DCM_8_y.mth_vect_norm_2_1;
      Calculate_Surface_Point_DCM_8_2 = Calculate_Surface_Point_DCM_8_y.mth_vect_norm_2_2;
      Calculate_Surface_Point_DCM_8_3 = Calculate_Surface_Point_DCM_8_y.mth_vect_norm_2_3;
      Calculate_Surface_Point_DCM_8_4 = Calculate_Surface_Point_DCM_8_y.mth_vect_norm_14_1;
      Calculate_Surface_Point_DCM_8_5 = Calculate_Surface_Point_DCM_8_y.mth_vect_norm_14_2;
      Calculate_Surface_Point_DCM_8_6 = Calculate_Surface_Point_DCM_8_y.mth_vect_norm_14_3;
      Calculate_Surface_Point_DCM_8_7 = Calculate_Surface_Point_DCM_8_y.mth_vect_norm_12_1;
      Calculate_Surface_Point_DCM_8_8 = Calculate_Surface_Point_DCM_8_y.mth_vect_norm_12_2;
      Calculate_Surface_Point_DCM_8_9 = Calculate_Surface_Point_DCM_8_y.mth_vect_norm_12_3;
      /* ----------------------------  Procedure Super Block */
      /* {Quaternion From DC P.25} */
      Quaternion_From_DC_P_25_u.A11 = Calculate_Surface_Point_DCM_8_1;
      Quaternion_From_DC_P_25_u.A12 = Calculate_Surface_Point_DCM_8_2;
      Quaternion_From_DC_P_25_u.A13 = Calculate_Surface_Point_DCM_8_3;
      Quaternion_From_DC_P_25_u.A21 = Calculate_Surface_Point_DCM_8_4;
      Quaternion_From_DC_P_25_u.A22 = Calculate_Surface_Point_DCM_8_5;
      Quaternion_From_DC_P_25_u.A23 = Calculate_Surface_Point_DCM_8_6;
      Quaternion_From_DC_P_25_u.A31 = Calculate_Surface_Point_DCM_8_7;
      Quaternion_From_DC_P_25_u.A32 = Calculate_Surface_Point_DCM_8_8;
      Quaternion_From_DC_P_25_u.A33 = Calculate_Surface_Point_DCM_8_9;
      Quaternion_From_DC_P(&Quaternion_From_DC_P_25_u, &Quaternion_From_DC_P_25_y, &S->Quaternion_From_DC_P_25_s, &I->Quaternion_From_DC_P_25_i);
      q1_P_ECI = Quaternion_From_DC_P_25_y.q1;
      q2_P_ECI = Quaternion_From_DC_P_25_y.q2;
      q3_P_ECI = Quaternion_From_DC_P_25_y.q3;
      q4_P_ECI = Quaternion_From_DC_P_25_y.q4;
      Quaternion_From_DC_P_25_5 = Quaternion_From_DC_P_25_y.Q1_desired;
      Quaternion_From_DC_P_25_6 = Quaternion_From_DC_P_25_y.Q2_desired;
      Quaternion_From_DC_P_25_7 = Quaternion_From_DC_P_25_y.Q3_desired;
      Quaternion_From_DC_P_25_8 = Quaternion_From_DC_P_25_y.Q4_desired;
      iinfo[0] = I->Quaternion_From_DC_P_25_i.iinfo[0];
      if( iinfo[0] != 0 ) {
         I->Quaternion_From_DC_P_25_i.iinfo[0] = 0; goto EXEC_ERROR;
      }
      /* ----------------------------  Procedure Super Block */
      /* {Surface Point Track rate.9} */
      Surface_Point_Track_rate_9_u.q1_P_ECI = q1_P_ECI;
      Surface_Point_Track_rate_9_u.q2_P_ECI = q2_P_ECI;
      Surface_Point_Track_rate_9_u.q3_P_ECI = q3_P_ECI;
      Surface_Point_Track_rate_9_u.q4_P_ECI = q4_P_ECI;
      Surface_Point_Track_rate(&Surface_Point_Track_rate_9_u, &Surface_Point_Track_rate_9_y, &S->Surface_Point_Track_rate_9_s, INIT);
      S->Surface_Point_Track_rate_9_s.Surface_Point_Track_rate_x = 1 - S->Surface_Point_Track_rate_9_s.Surface_Point_Track_rate_x;
      Omega_P_ECI_P_x = Surface_Point_Track_rate_9_y.Omega_P_ECI_P_x;
      Omega_P_ECI_P_y = Surface_Point_Track_rate_9_y.Omega_P_ECI_P_y;
      Omega_P_ECI_P_z = Surface_Point_Track_rate_9_y.Omega_P_ECI_P_z;
      /* ----------------------------  Algebraic Expression */
      /* {Mission Surface Track..99} */
      Mission_Surface_Track_99_1 = 0.0; 
      Mission_Surface_Track_99_2 = 0.0; 
      Mission_Surface_Track_99_3 = 0.3927; 
      Mission_Surface_Track_99_4 = (-1.0)*0.91967; 
      /* ----------------------------  Procedure Super Block */
      /* {Quaternion Multiply.3} */
      Quaternion_Multiply_3_u.q1_A_B = Mission_Surface_Track_99_1;
      Quaternion_Multiply_3_u.q2_A_B = Mission_Surface_Track_99_2;
      Quaternion_Multiply_3_u.q3_A_B = Mission_Surface_Track_99_3;
      Quaternion_Multiply_3_u.q4_A_B = Mission_Surface_Track_99_4;
      Quaternion_Multiply_3_u.q1_B_C = q1_P_ECI;
      Quaternion_Multiply_3_u.q2_B_C = q2_P_ECI;
      Quaternion_Multiply_3_u.q3_B_C = q3_P_ECI;
      Quaternion_Multiply_3_u.q4_B_C = q4_P_ECI;
      Quaternion_Multiply(&Quaternion_Multiply_3_u, &Quaternion_Multiply_3_y);
      q1_A_C = Quaternion_Multiply_3_y.q1_A_C;
      q2_A_C = Quaternion_Multiply_3_y.q2_A_C;
      q3_A_C = Quaternion_Multiply_3_y.q3_A_C;
      q4_A_C = Quaternion_Multiply_3_y.q4_A_C;
      /* ----------------------------  Procedure Super Block */
      /* {mth quat norm.6} */
      mth_quat_norm_6_u.DQ1 = q1_A_C;
      mth_quat_norm_6_u.DQ2 = q2_A_C;
      mth_quat_norm_6_u.DQ3 = q3_A_C;
      mth_quat_norm_6_u.DQ4 = q4_A_C;
      mth_quat_norm(&mth_quat_norm_6_u, &mth_quat_norm_6_y);
      Target_q1 = mth_quat_norm_6_y.mth_quat_norm_4_1;
      Target_q2 = mth_quat_norm_6_y.mth_quat_norm_4_2;
      Target_q3 = mth_quat_norm_6_y.mth_quat_norm_4_3;
      Target_q4 = mth_quat_norm_6_y.mth_quat_norm_4_4;
      /* ----------------------------  Procedure Super Block */
      /* {Quaternion Difference P.4} */
      Quaternion_Difference_P_4_u.Q1_actual = U->q1;
      Quaternion_Difference_P_4_u.Q2_actual = U->q2;
      Quaternion_Difference_P_4_u.Q3_actual = U->q3;
      Quaternion_Difference_P_4_u.Q4_actual = U->q4;
      Quaternion_Difference_P_4_u.Q1_desired = Target_q1;
      Quaternion_Difference_P_4_u.Q2_desired = Target_q2;
      Quaternion_Difference_P_4_u.Q3_desired = Target_q3;
      Quaternion_Difference_P_4_u.Q4_desired = Target_q4;
      Quaternion_Difference_P(&Quaternion_Difference_P_4_u, &Quaternion_Difference_P_4_y);
      Y->Quaternion_Difference_P_4_1 = Quaternion_Difference_P_4_y.mth_quat_norm_99_1;
      Y->Quaternion_Difference_P_4_2 = Quaternion_Difference_P_4_y.mth_quat_norm_99_2;
      Y->Quaternion_Difference_P_4_3 = Quaternion_Difference_P_4_y.mth_quat_norm_99_3;
      Y->Quaternion_Difference_P_4_4 = Quaternion_Difference_P_4_y.mth_quat_norm_99_4;
      /* ----------------------------  Summer */
      /* {Mission Surface Track..26} */
      Y->Mission_Surface_Track_26_1 = Omega_P_ECI_P_x - U->omega_x; 
      Y->Mission_Surface_Track_26_2 = Omega_P_ECI_P_y - U->omega_y; 
      Y->Mission_Surface_Track_26_3 = Omega_P_ECI_P_z - U->omega_z; 
      /* ----------------------------  Algebraic Expression */
      /* {Mission Surface Track..5} */
      Y->Zero = 0.0; 


      ////iinfo[1] = 0;
EXEC_ERROR: return;
}

/******* Procedure: Inertial_Capture *******/
void Inertial_Capture(   struct _Inertial_Capture_u *U
   ,struct _Inertial_Capture_y *Y
)
{
      
      /***** Output Update. *****/
      /* ----------------------------  Gain Block */
      /* {Inertial Capture..98} */
      Y->wx_Error = (-1.0)*U->Filt_Omega_B_N_B_x; 
      Y->wy_Error = (-1.0)*U->Filt_Omega_B_N_B_y; 
      Y->wz_Error = (-1.0)*U->Filt_Omega_B_N_B_z; 
      /* ----------------------------  Algebraic Expression */
      /* {Inertial Capture.Zero Position Error q.13} */
      Y->q1_err = 0.0; 
      Y->q2_err = 0.0; 
      Y->q3_err = 0.0; 
      Y->q4_err = 1.0; 
      /* ----------------------------  Algebraic Expression */
      /* {Inertial Capture..3} */
      Y->Zero = 1.0; 


      ////iinfo[1] = 0;
EXEC_ERROR: return;
}

/******* Procedure: Quaternion_From_DC_No_Flip *******/
void Quaternion_From_DC_No_Flip(   struct _Quaternion_From_DC_No_Flip_u *U
   ,struct _Quaternion_From_DC_No_Flip_y *Y
   ,struct _Quaternion_From_DC_No_Flip_info *I
)
{
      RT_INTEGER *iinfo = &I->iinfo[0];
      RT_INTEGER INIT = iinfo[1];

      /***** Parameters. *****/

      RT_FLOAT *R_P = &I->RP[0];
      RT_INTEGER cnt;
      static const RT_FLOAT _R_P[4] = {0.0, 0.0, 1.0, 0.0};

      /***** Local Block Outputs. *****/

      RT_FLOAT SUM1;
      RT_FLOAT SUM2;
      RT_FLOAT SUM3;
      RT_FLOAT SUM4;
      RT_FLOAT Quaternion_From_DC_No_Flip_3_1;
      RT_FLOAT Quaternion_From_DC_No_Flip_3_2;
      RT_FLOAT Quaternion_From_DC_No_Flip_3_3;
      RT_FLOAT Quaternion_From_DC_No_Flip_3_4;
      RT_FLOAT ENABLE_LOGIC_1;
      RT_FLOAT ENABLE_LOGIC_2;
      RT_FLOAT ENABLE_LOGIC_3;
      RT_FLOAT ENABLE_LOGIC_4;
      RT_FLOAT Quaternion_From_DC_No_Flip_97_1;
      RT_FLOAT Q1;
      RT_FLOAT Q2;
      RT_FLOAT Q3;
      RT_FLOAT Q4;
      RT_FLOAT Quaternion_From_DC_No_Flip_98_1;
      RT_FLOAT Q2_1;
      RT_FLOAT Q1_1;
      RT_FLOAT Q3_1;
      RT_FLOAT Q4_1;
      RT_FLOAT Quaternion_From_DC_No_Flip_1_1;
      RT_FLOAT Q3_2;
      RT_FLOAT Q1_2;
      RT_FLOAT Q2_2;
      RT_FLOAT Q4_2;
      RT_FLOAT Quaternion_From_DC_No_Flip_99_1;
      RT_FLOAT Q4_3;
      RT_FLOAT Q1_3;
      RT_FLOAT Q2_3;
      RT_FLOAT Q3_3;

      /******* Initialization. *******/

      if( INIT ) {
         for( cnt=0;cnt<4;cnt++ ) {
            R_P[cnt] = _R_P[cnt];
         }
      }


      /***** Output Update. *****/
      /* ----------------------------  Algebraic Expression */
      /* {Quaternion From DC No Flip.SUMMER.6} */
      SUM1 = 1.0 + U->A11 - U->A22 - U->A33; 
      SUM2 = 1.0 - U->A11 + U->A22 - U->A33; 
      SUM3 = 1.0 - U->A11 - U->A22 + U->A33; 
      SUM4 = 1.0 + U->A11 + U->A22 + U->A33; 
      /* ----------------------------  Absolute Value */
      /* {Quaternion From DC No Flip..3} */
      Quaternion_From_DC_No_Flip_3_1 = fabs(SUM1); 
      Quaternion_From_DC_No_Flip_3_2 = fabs(SUM2); 
      Quaternion_From_DC_No_Flip_3_3 = fabs(SUM3); 
      Quaternion_From_DC_No_Flip_3_4 = fabs(SUM4); 
      /* ----------------------------  Logical Expression */
      /* {Quaternion From DC No Flip.ENABLE LOGIC.93} */
      if (Quaternion_From_DC_No_Flip_3_1 >= Quaternion_From_DC_No_Flip_3_2 && Quaternion_From_DC_No_Flip_3_1 >= Quaternion_From_DC_No_Flip_3_3 && Quaternion_From_DC_No_Flip_3_1 >= Quaternion_From_DC_No_Flip_3_4) {
         *R_P = 1.0; 
      }
      else {
         *R_P = 0.0; 
      }
      if (Quaternion_From_DC_No_Flip_3_2 >= Quaternion_From_DC_No_Flip_3_1 && Quaternion_From_DC_No_Flip_3_2 >= Quaternion_From_DC_No_Flip_3_3 && Quaternion_From_DC_No_Flip_3_2 >= Quaternion_From_DC_No_Flip_3_4) {
         *(R_P+1) = 1.0; 
      }
      else {
         *(R_P+1) = 0.0; 
      }
      if (Quaternion_From_DC_No_Flip_3_3 >= Quaternion_From_DC_No_Flip_3_1 && Quaternion_From_DC_No_Flip_3_3 >= Quaternion_From_DC_No_Flip_3_2 && Quaternion_From_DC_No_Flip_3_3 >= Quaternion_From_DC_No_Flip_3_4) {
         *(R_P+2) = 1.0; 
      }
      else {
         *(R_P+2) = 0.0; 
      }
      if (Quaternion_From_DC_No_Flip_3_4 >= Quaternion_From_DC_No_Flip_3_1 && Quaternion_From_DC_No_Flip_3_4 >= Quaternion_From_DC_No_Flip_3_2 && Quaternion_From_DC_No_Flip_3_4 >= Quaternion_From_DC_No_Flip_3_3) {
         *(R_P+3) = 1.0; 
      }
      else {
         *(R_P+3) = 0.0; 
      }
      if (*R_P > 0.0) {
         ENABLE_LOGIC_1 = 1.0; 
      }
      else {
         ENABLE_LOGIC_1 = 0.0; 
      }
      if (*(R_P+1) > 0.0 && ENABLE_LOGIC_1 <= 0.0) {
         ENABLE_LOGIC_2 = 1.0; 
      }
      else {
         ENABLE_LOGIC_2 = 0.0; 
      }
      if (*(R_P+2) > 0.0 && ENABLE_LOGIC_1 <= 0.0 && ENABLE_LOGIC_2 <= 0.0) {
         ENABLE_LOGIC_3 = 1.0; 
      }
      else {
         ENABLE_LOGIC_3 = 0.0; 
      }
      if (*(R_P+3) > 0.0 && ENABLE_LOGIC_1 <= 0.0 && ENABLE_LOGIC_2 <= 0.0 && ENABLE_LOGIC_3 <= 0.0) {
         ENABLE_LOGIC_4 = 1.0; 
      }
      else {
         ENABLE_LOGIC_4 = 0.0; 
      }
      /* ----------------------------  Square Root */
      /* {Quaternion From DC No Flip..97} */
      Quaternion_From_DC_No_Flip_97_1 = sqrt(Quaternion_From_DC_No_Flip_3_1); 
      /* ----------------------------  Algebraic Expression */
      /* {Quaternion From DC No Flip..2} */
      Q1 = 0.5*Quaternion_From_DC_No_Flip_97_1 + 1.0E-7; 
      Q2 = 0.25*(U->A12 + U->A21)/Q1; 
      Q3 = 0.25*(U->A13 + U->A31)/Q1; 
      Q4 = 0.25*(U->A23 - U->A32)/Q1; 
      /* ----------------------------  Square Root */
      /* {Quaternion From DC No Flip..98} */
      Quaternion_From_DC_No_Flip_98_1 = sqrt(Quaternion_From_DC_No_Flip_3_2); 
      /* ----------------------------  Algebraic Expression */
      /* {Quaternion From DC No Flip..8} */
      Q2_1 = 0.5*Quaternion_From_DC_No_Flip_98_1 + 1.0E-7; 
      Q1_1 = 0.25*(U->A12 + U->A21)/Q2_1; 
      Q3_1 = 0.25*(U->A23 + U->A32)/Q2_1; 
      Q4_1 = 0.25*(U->A31 - U->A13)/Q2_1; 
      /* ----------------------------  Square Root */
      /* {Quaternion From DC No Flip..1} */
      Quaternion_From_DC_No_Flip_1_1 = sqrt(Quaternion_From_DC_No_Flip_3_3); 
      /* ----------------------------  Algebraic Expression */
      /* {Quaternion From DC No Flip..4} */
      Q3_2 = 0.5*Quaternion_From_DC_No_Flip_1_1 + 1.0E-7; 
      Q1_2 = 0.25*(U->A13 + U->A31)/Q3_2; 
      Q2_2 = 0.25*(U->A23 + U->A32)/Q3_2; 
      Q4_2 = 0.25*(U->A12 - U->A21)/Q3_2; 
      /* ----------------------------  Square Root */
      /* {Quaternion From DC No Flip..99} */
      Quaternion_From_DC_No_Flip_99_1 = sqrt(Quaternion_From_DC_No_Flip_3_4); 
      /* ----------------------------  Algebraic Expression */
      /* {Quaternion From DC No Flip..5} */
      Q4_3 = 0.5*Quaternion_From_DC_No_Flip_99_1 + 1.0E-7; 
      Q1_3 = 0.25*(U->A23 - U->A32)/Q4_3; 
      Q2_3 = 0.25*(U->A31 - U->A13)/Q4_3; 
      Q3_3 = 0.25*(U->A12 - U->A21)/Q4_3; 
      /* ----------------------------  Algebraic Expression */
      /* {Quaternion From DC No Flip.Switch.7} */
      Y->Q1_desired = ENABLE_LOGIC_1*Q1 + ENABLE_LOGIC_2*Q1_1 + ENABLE_LOGIC_3*Q1_2 + ENABLE_LOGIC_4*Q1_3; 
      Y->Q2_desired = ENABLE_LOGIC_1*Q2 + ENABLE_LOGIC_2*Q2_1 + ENABLE_LOGIC_3*Q2_2 + ENABLE_LOGIC_4*Q2_3; 
      Y->Q3_desired = ENABLE_LOGIC_1*Q3 + ENABLE_LOGIC_2*Q3_1 + ENABLE_LOGIC_3*Q3_2 + ENABLE_LOGIC_4*Q3_3; 
      Y->Q4_desired = ENABLE_LOGIC_1*Q4 + ENABLE_LOGIC_2*Q4_1 + ENABLE_LOGIC_3*Q4_2 + ENABLE_LOGIC_4*Q4_3; 


      ////iinfo[1] = 0;
EXEC_ERROR: return;
}



/******** Tasks declarations ********/

/******* Subsystem 1  *******/
//extern void subsys_1(); //jja commented out b/c pic wants function arguments even for externs



/******** Tasks code ********/


/******* Subsystem 1  *******/


/***** States type declaration. *****/
struct _Subsys_1_states {
   RT_FLOAT Gyro_Quantization_Filter_34_S1;
   RT_FLOAT Gyro_Quantization_Filter_34_S2;
   RT_FLOAT Gyro_Quantization_Filter_34_S3;
   RT_FLOAT Attitude_Control_28_S1;
   RT_FLOAT Control_Modes_6_S1;
   RT_FLOAT Control_Modes_6_S2;
   RT_FLOAT Control_Modes_14_S1;
   RT_FLOAT Control_Modes_14_S2;
   RT_FLOAT Control_Modes_12_S1;
   RT_FLOAT Control_Modes_12_S2;
   RT_FLOAT Control_Modes_1_S1;
   RT_FLOAT Control_Modes_1_S2;
   RT_FLOAT Control_Modes_3_S1;
   RT_FLOAT Control_Modes_3_S2;
   RT_FLOAT Control_Modes_23_S1;
   RT_FLOAT Control_Modes_23_S2;
   RT_FLOAT Control_Modes_9_S1;
   RT_FLOAT Control_Modes_9_S2;
   RT_FLOAT Control_Modes_19_S1;
   RT_FLOAT Control_Modes_19_S2;
   RT_FLOAT Mode_3_Timer_4_S1;
   RT_FLOAT Mode_3_Timer_99_S1;
   RT_FLOAT PAST_QUATERNION_S1;
   RT_FLOAT PAST_QUATERNION_S2;
   RT_FLOAT PAST_QUATERNION_S3;
   RT_FLOAT PAST_QUATERNION_S4;
   RT_FLOAT Quaternion_Require_Continuous_16_S1;
   RT_FLOAT Quaternion_Require_Continuous_16_S2;
   RT_FLOAT Quaternion_Require_Continuous_16_S3;
   RT_FLOAT Quaternion_Require_Continuous_16_S4;
   RT_FLOAT Enable_and_Initialize_96_S1;
   RT_FLOAT Toggle_2_Pulse_20_S1;
   RT_FLOAT Persistance_Test_Up_and_Down_12_S1;
   RT_FLOAT Persistance_Test_Up_and_Down_13_S1;
   RT_FLOAT Toggle_2_Pulse_5_20_S1;
   RT_FLOAT Toggle_2_Pulse_5_20_S2;
   RT_FLOAT Toggle_2_Pulse_5_20_S3;
   RT_FLOAT Toggle_2_Pulse_5_20_S4;
   RT_FLOAT Toggle_2_Pulse_20_S1_1;
   RT_FLOAT Persistance_Test_Up_Only_12_S1;
   RT_FLOAT Persistance_Test_Up_Only_13_S1;
   RT_FLOAT Persistance_Test_Up_Only_12_S1_1;
   RT_FLOAT Persistance_Test_Up_Only_13_S1_1;
   RT_FLOAT Persistance_Test_Up_Only_12_S1_2;
   RT_FLOAT Persistance_Test_Up_Only_13_S1_2;
   RT_FLOAT Avoid_Algebraic_Loop_S1;
   RT_FLOAT Persistance_Test_Up_Only_12_S1_3;
   RT_FLOAT Persistance_Test_Up_Only_13_S1_3;
   RT_FLOAT Persistance_Test_Up_Only_12_S1_4;
   RT_FLOAT Persistance_Test_Up_Only_13_S1_4;
   RT_FLOAT Safe_Sun_Presence_Check_4_S1;
   RT_FLOAT Persistance_Test_Up_Only_12_S1_5;
   RT_FLOAT Persistance_Test_Up_Only_13_S1_5;
   RT_FLOAT Persistance_Test_Up_Only_12_S1_6;
   RT_FLOAT Persistance_Test_Up_Only_13_S1_6;
   RT_FLOAT Persistance_Test_Up_Only_12_S1_7;
   RT_FLOAT Persistance_Test_Up_Only_13_S1_7;
   RT_FLOAT Persistance_Test_Up_Only_12_S1_8;
   RT_FLOAT Persistance_Test_Up_Only_13_S1_8;
   RT_FLOAT Persistance_Test_Up_Only_12_S1_9;
   RT_FLOAT Persistance_Test_Up_Only_13_S1_9;
   RT_FLOAT Persistance_Test_Up_Only_12_S1_10;
   RT_FLOAT Persistance_Test_Up_Only_13_S1_10;
   RT_FLOAT Safe_Sun_Presence_Check_2_S1;
   RT_FLOAT Mode_1;
   RT_FLOAT Mode_2;
   RT_FLOAT Mode_3;
   RT_FLOAT Mode_4;
   RT_FLOAT Mode_5;
   RT_FLOAT Mode_6;
   RT_FLOAT Mode_7;
   RT_FLOAT Copy_of_Mode_Controller_S8;
   RT_FLOAT Copy_of_Mode_Controller_S9;
   RT_INTEGER Copy_of_Mode_Controller_S10;
   RT_INTEGER Copy_of_Mode_Controller_S11;
   RT_FLOAT Latch_2_S1;
   RT_INTEGER Latch_2_S2;
   RT_INTEGER Latch_2_S3;
   RT_FLOAT Mode_3_Timer_2_S1;
   RT_FLOAT Flip_Flop_S1;
   RT_INTEGER Flip_Flop_S2;
   RT_INTEGER Flip_Flop_S3;
};

void subsys_1(   struct _Subsys_1_in *U
   ,struct _Subsys_1_out *Y
)
{
      
      /***** States Array. *****/

      static struct _Subsys_1_states ss_1_states[2];

      /***** Current and Next States Pointers. *****/

      static struct _Subsys_1_states *X;
      static struct _Subsys_1_states *XD;
      static struct _Subsys_1_states *XTMP;
      static RT_INTEGER iinfo[4];
      static RT_INTEGER INIT;
      const RT_DURATION TIME  = 3.0;
      const RT_DURATION TSAMP = 0.25;

      /***** Parameters. *****/

      static RT_FLOAT R_P[2];
      RT_INTEGER cnt;
      static const RT_FLOAT _R_P[2] = {0.0};

      /* Local Model variable declarations. */

      VAR_FLOAT error_signal[8];

      /***** Local Block Outputs. *****/

      RT_FLOAT Gyro_Quantization_Filter_34_1;
      RT_FLOAT Gyro_Quantization_Filter_34_2;
      RT_FLOAT Gyro_Quantization_Filter_34_3;
      RT_FLOAT Mode_2_or_3_Enable;
      RT_FLOAT Enable_1;
      RT_FLOAT Enable_2;
      RT_FLOAT Enable_3;
      RT_FLOAT Enable_4;
      RT_FLOAT Enable_5;
      RT_FLOAT Enable_6;
      RT_FLOAT Enable_7;
      RT_FLOAT Reset_timer_to_this_value_1;
      RT_FLOAT Delayed_Sun_not_present;
      RT_FLOAT Mode_3_Timer_99_1;
      RT_FLOAT PAST_QUATERNION_1;
      RT_FLOAT PAST_QUATERNION_2;
      RT_FLOAT PAST_QUATERNION_3;
      RT_FLOAT PAST_QUATERNION_4;
      RT_FLOAT Quaternion_Require_Continuous_16_1;
      RT_FLOAT Quaternion_Require_Continuous_16_2;
      RT_FLOAT Quaternion_Require_Continuous_16_3;
      RT_FLOAT Quaternion_Require_Continuous_16_4;
      RT_FLOAT Enable_and_Initialize_96_1;
      RT_FLOAT Toggle_2_Pulse_20_1;
      RT_FLOAT Persistance_Test_Up_and_Down_12_1;
      RT_FLOAT Persistance_Test_Up_and_Down_13_1;
      RT_FLOAT Toggle_2_Pulse_5_20_1;
      RT_FLOAT Toggle_2_Pulse_5_20_2;
      RT_FLOAT Toggle_2_Pulse_5_20_3;
      RT_FLOAT Toggle_2_Pulse_5_20_4;
      RT_FLOAT Toggle_2_Pulse_20_1_1;
      RT_FLOAT Persistance_Test_Up_Only_12_1;
      RT_FLOAT Persistance_Test_Up_Only_13_1;
      RT_FLOAT Persistance_Test_Up_Only_12_1_1;
      RT_FLOAT Persistance_Test_Up_Only_13_1_1;
      RT_FLOAT Persistance_Test_Up_Only_12_1_2;
      RT_FLOAT Persistance_Test_Up_Only_13_1_2;
      RT_FLOAT Persistance_Test_Up_Only_12_1_3;
      RT_FLOAT Persistance_Test_Up_Only_13_1_3;
      RT_FLOAT Persistance_Test_Up_Only_12_1_4;
      RT_FLOAT Persistance_Test_Up_Only_13_1_4;
      RT_FLOAT Sun_Line_Error_11_1;
      RT_FLOAT Reset_timer_to_this_value_1_1;
      RT_FLOAT Delayed_Sun_not_present_1;
      RT_FLOAT Persistance_Test_Up_Only_12_1_5;
      RT_FLOAT Persistance_Test_Up_Only_13_1_5;
      RT_FLOAT Persistance_Test_Up_Only_12_1_6;
      RT_FLOAT Persistance_Test_Up_Only_13_1_6;
      RT_FLOAT Persistance_Test_Up_Only_12_1_7;
      RT_FLOAT Persistance_Test_Up_Only_13_1_7;
      RT_FLOAT Persistance_Test_Up_Only_12_1_8;
      RT_FLOAT Persistance_Test_Up_Only_13_1_8;
      RT_FLOAT Persistance_Test_Up_Only_12_1_9;
      RT_FLOAT Persistance_Test_Up_Only_13_1_9;
      RT_FLOAT Persistance_Test_Up_Only_12_1_10;
      RT_FLOAT Persistance_Test_Up_Only_13_1_10;
      RT_FLOAT mth_vect_norm_30_1;
      RT_FLOAT mth_vect_norm_30_2;
      RT_FLOAT mth_vect_norm_30_3;
      RT_FLOAT mth_vect_norm_30_4;
      RT_FLOAT mth_vect_norm_12_1;
      RT_FLOAT mth_vect_norm_12_2;
      RT_FLOAT mth_vect_norm_12_3;
      RT_FLOAT mth_vect_norm_12_4;
      RT_FLOAT Algebraically_Compute_q_B_M_1_1;
      RT_FLOAT Algebraically_Compute_q_B_M_1_2;
      RT_FLOAT Algebraically_Compute_q_B_M_1_3;
      RT_FLOAT mth_vect_norm_11_1;
      RT_FLOAT mth_vect_norm_11_2;
      RT_FLOAT mth_vect_norm_11_3;
      RT_FLOAT mth_vect_norm_11_4;
      RT_FLOAT MB_13;
      RT_FLOAT MB_23;
      RT_FLOAT MB_33;
      RT_FLOAT Gyro_Quantization_Filter_44_1;
      RT_FLOAT Gyro_Quantization_Filter_44_2;
      RT_FLOAT Gyro_Quantization_Filter_44_3;
      RT_FLOAT Reset_Estimator;
      RT_FLOAT Correct_Drift_and_Misalign_13_1;
      RT_FLOAT Correct_Drift_and_Misalign_13_2;
      RT_FLOAT Correct_Drift_and_Misalign_13_3;
      RT_FLOAT Meas_Omega_B_N_B_x;
      RT_FLOAT Meas_Omega_B_N_B_y;
      RT_FLOAT Meas_Omega_B_N_B_z;
      RT_FLOAT Gyro_Quantization_Filter_35_1;
      RT_FLOAT Gyro_Quantization_Filter_35_2;
      RT_FLOAT Gyro_Quantization_Filter_35_3;
      RT_FLOAT Norm_Omega_B_N_B;
      RT_FLOAT mth_vect_norm_3_1;
      RT_FLOAT mth_vect_norm_3_2;
      RT_FLOAT mth_vect_norm_3_3;
      RT_FLOAT mth_vect_norm_3_4;
      RT_FLOAT mth_vect_norm_31_1;
      RT_FLOAT mth_vect_norm_31_2;
      RT_FLOAT mth_vect_norm_31_3;
      RT_FLOAT mth_vect_norm_31_4;
      RT_FLOAT Algebraically_Compute_q_B_M_22_1;
      RT_FLOAT Algebraically_Compute_q_B_M_22_2;
      RT_FLOAT Algebraically_Compute_q_B_M_22_3;
      RT_FLOAT mth_vect_norm_4_1;
      RT_FLOAT mth_vect_norm_4_2;
      RT_FLOAT mth_vect_norm_4_3;
      RT_FLOAT mth_vect_norm_4_4;
      RT_FLOAT MB_13_1;
      RT_FLOAT MB_23_1;
      RT_FLOAT MB_33_1;
      RT_FLOAT A11;
      RT_FLOAT A12;
      RT_FLOAT A13;
      RT_FLOAT A21;
      RT_FLOAT A22;
      RT_FLOAT A23;
      RT_FLOAT A31;
      RT_FLOAT A32;
      RT_FLOAT A33;
      RT_FLOAT q1_B_M_Estimated;
      RT_FLOAT q2_B_M_Estimated;
      RT_FLOAT q3_B_M_Estimated;
      RT_FLOAT q4_B_M_Estimated;
      RT_FLOAT Avoid_Numerical_Error_1;
      RT_FLOAT Mode_Selection_95_1;
      RT_FLOAT Omega_B_N_B_is_Small;
      RT_FLOAT True_Last_3_Samples;
      RT_FLOAT Omega_B_N_B_is_Large;
      RT_FLOAT True_Last_3_Samples_1;
      RT_FLOAT Sun_Line_Error_Small;
      RT_FLOAT True_Last_3_Samples_2;
      RT_FLOAT Sun_Line_Error_21_1;
      RT_FLOAT Sun_Line_Error_Small_1;
      RT_FLOAT True_Last_3_Samples_3;
      RT_FLOAT Reset_Trigger;
      RT_FLOAT Sun_not_present;
      RT_FLOAT Time_since_Sun_not_present;
      RT_FLOAT Whel_Sat_Lower_Limit;
      RT_FLOAT Delta_T_Over_2_1;
      RT_FLOAT Delta_T_Over_2_2;
      RT_FLOAT Delta_T_Over_2_3;
      RT_FLOAT Quaternion_Update_1;
      RT_FLOAT Quaternion_Update_2;
      RT_FLOAT Quaternion_Update_3;
      RT_FLOAT Quaternion_Update_4;
      RT_FLOAT Quaternion_Update_5;
      RT_FLOAT Accumulate_Gyros_33_1;
      RT_FLOAT Accumulate_Gyros_34_1;
      RT_FLOAT q1_B_LV_Estimated;
      RT_FLOAT q2_B_LV_Estimated;
      RT_FLOAT q3_B_LV_Estimated;
      RT_FLOAT q4_B_LV_Estimated;
      RT_FLOAT X_1;
      RT_FLOAT Y_1;
      RT_FLOAT Z;
      RT_FLOAT Magnitude;
      RT_FLOAT Saturated;
      RT_FLOAT Desaturated;
      RT_FLOAT True_Last_3_Samples_4;
      RT_FLOAT Mode_Override;
      RT_FLOAT Omega_Small;
      RT_FLOAT Sun_present;
      RT_FLOAT Sun_line_error_small;
      RT_FLOAT Sun_line_error_large;
      RT_FLOAT Mode_limit;
      RT_FLOAT Omega_Large;
      RT_FLOAT Wheel_Sat;
      RT_FLOAT Slew_Angle;
      RT_FLOAT Sun_gone_for_a_while;
      RT_FLOAT Wheel_desaturated;
      RT_FLOAT Enable_Momentum_Dump;
      RT_FLOAT Enable_AD;
      RT_FLOAT True_Last_3_Samples_5;
      RT_FLOAT Reset_Trigger_1;
      RT_FLOAT False_Last_3_Samples;
      RT_FLOAT Mode_3_Timer_53_1;
      RT_FLOAT Time_since_Mode_3_Start;
      RT_FLOAT Mode_3_Limit_Reached;
      RT_FLOAT Switch_to_Yaw;
      RT_FLOAT Safe_Rate_Check_12_1;
      RT_FLOAT Safe_Rate_Check_12_2;
      RT_FLOAT Safe_Rate_Check_12_3;
      RT_FLOAT Estimated_q1_B_ECI;
      RT_FLOAT Estimated_q2_B_ECI;
      RT_FLOAT Estimated_q3_B_ECI;
      RT_FLOAT Estimated_q4_B_ECI;
      RT_FLOAT Quaternion_Require_Continuous_15_1;
      RT_FLOAT Quaternion_Require_Continuous_15_2;
      RT_FLOAT Quaternion_Require_Continuous_15_3;
      RT_FLOAT Quaternion_Require_Continuous_15_4;
      RT_FLOAT Norm_1;
      RT_FLOAT Quaternion_Require_Continuous_5_1;
      RT_FLOAT Quaternion_Require_Continuous_13_1;
      RT_FLOAT Flip_Flop_1;
      RT_FLOAT Quaternion_Require_Continuous_26_1;
      RT_FLOAT Quaternion_Require_Continuous_26_2;
      RT_FLOAT Quaternion_Require_Continuous_26_3;
      RT_FLOAT Quaternion_Require_Continuous_26_4;
      RT_FLOAT Attitude_Control_1_1;
      RT_FLOAT Norm_1_1;
      RT_FLOAT Approx_Angular_Sun_Line_Error_1;
      RT_FLOAT Sun_Line_Error_10_1;
      RT_FLOAT Sun_Line_Error_96_1;
      RT_FLOAT Safe_Q_Check_1_1;
      RT_FLOAT Safe_Q_Check_34_1;
      RT_FLOAT Safe_Q_Check_4_1;
      RT_FLOAT Mission_Pointing_Error_3_Axis;
      RT_FLOAT Safe_Q_Check_14_1;
      RT_FLOAT Attitude_Control_98_1;
      RT_FLOAT Attitude_Control_8_1;
      RT_FLOAT SCALAR_MAG;
      RT_FLOAT Slew_Inactive;
      RT_FLOAT Toggle_Down_To_Pulse_94_1;
      RT_FLOAT Parameters_6_12_1;
      RT_FLOAT Parameters_6_12_2;
      RT_FLOAT Parameters_6_12_3;
      RT_FLOAT Parameters_6_12_4;
      RT_FLOAT Parameters_6_12_5;
      RT_FLOAT Parameters_6_12_6;
      RT_FLOAT Parameters_6_12_1_1;
      RT_FLOAT Parameters_6_12_2_1;
      RT_FLOAT Parameters_6_12_3_1;
      RT_FLOAT Parameters_6_12_4_1;
      RT_FLOAT Parameters_6_12_5_1;
      RT_FLOAT Parameters_6_12_6_1;
      RT_FLOAT Zero_4;
      RT_FLOAT FSW_96_1;
      RT_FLOAT Attitude_Control_6_1;
      RT_FLOAT Attitude_Control_6_2;
      RT_FLOAT Attitude_Control_6_3;
      RT_FLOAT Attitude_Control_10_1;
      RT_FLOAT Attitude_Control_21_1;
      RT_FLOAT Euler_Axis_Rate_Controlled_Slew_1;
      RT_FLOAT Euler_Axis_Rate_Controlled_Slew_2;
      RT_FLOAT Euler_Axis_Rate_Controlled_Slew_3;
      RT_FLOAT Euler_Axis_Rate_Controlled_Slew_4;
      RT_FLOAT Commanded_Slew_Rate_1;
      RT_FLOAT Commanded_Slew_Rate_2;
      RT_FLOAT Commanded_Slew_Rate_3;
      RT_FLOAT Attitude_Control_9_1;
      RT_FLOAT Attitude_Control_9_2;
      RT_FLOAT Attitude_Control_9_3;
      RT_FLOAT Attitude_Control_99_1;
      RT_FLOAT Attitude_Control_99_2;
      RT_FLOAT Attitude_Control_99_3;
      RT_FLOAT Small_Euler_Angles_1;
      RT_FLOAT Small_Euler_Angles_2;
      RT_FLOAT Small_Euler_Angles_3;
      RT_FLOAT PD_or_Rate_Control_1;
      RT_FLOAT PD_or_Rate_Control_2;
      RT_FLOAT PD_or_Rate_Control_3;
      RT_FLOAT PD_or_Rate_Control_4;
      RT_FLOAT PD_or_Rate_Control_5;
      RT_FLOAT PD_or_Rate_Control_6;
      RT_FLOAT Position_Gain_1;
      RT_FLOAT Position_Gain_2;
      RT_FLOAT Position_Gain_3;
      RT_FLOAT Rate_Gain_1;
      RT_FLOAT Rate_Gain_2;
      RT_FLOAT Rate_Gain_3;
      RT_FLOAT Requested_Alpha_B_X_B_x;
      RT_FLOAT Requested_Alpha_B_X_B_y;
      RT_FLOAT Requested_Alpha_B_X_B_z;
      RT_FLOAT Parameters_6_12_1_2;
      RT_FLOAT Parameters_6_12_2_2;
      RT_FLOAT Parameters_6_12_3_2;
      RT_FLOAT Parameters_6_12_4_2;
      RT_FLOAT Parameters_6_12_5_2;
      RT_FLOAT Parameters_6_12_6_2;
      RT_FLOAT Attitude_Control_17_1;
      RT_FLOAT Attitude_Control_17_2;
      RT_FLOAT Attitude_Control_17_3;
      RT_FLOAT Requested_Torque_x;
      RT_FLOAT Requested_Torque_y;
      RT_FLOAT Requested_Torque_z;
      RT_FLOAT Mom_torque_x;
      RT_FLOAT Mom_torque_y;
      RT_FLOAT Mom_torque_z;
      RT_FLOAT X_2;
      RT_FLOAT Y_2;
      RT_FLOAT Z_1;
      RT_FLOAT Magnitude_1;
      RT_FLOAT Earth_Nadir_in_ECI_x;
      RT_FLOAT Earth_Nadir_in_ECI_y;
      RT_FLOAT Earth_Nadir_in_ECI_z;
      RT_FLOAT Body_x;
      RT_FLOAT Body_y;
      RT_FLOAT Body_z;
      RT_FLOAT Zero_5;
      RT_FLOAT Toggle_2_Pulse_31_1;
      RT_FLOAT Toggle_2_Pulse_21_1;
      RT_FLOAT Toggle_2_Pulse_5_31_1;
      RT_FLOAT Toggle_2_Pulse_5_31_2;
      RT_FLOAT Toggle_2_Pulse_5_31_3;
      RT_FLOAT Toggle_2_Pulse_5_31_4;
      RT_FLOAT Toggle_2_Pulse_5_21_1;
      RT_FLOAT Toggle_2_Pulse_5_21_2;
      RT_FLOAT Toggle_2_Pulse_5_21_3;
      RT_FLOAT Toggle_2_Pulse_5_21_4;
      RT_FLOAT Toggle_2_Pulse_5_32_1;
      RT_FLOAT Toggle_2_Pulse_5_23_1;
      RT_FLOAT Toggle_2_Pulse_31_1_1;
      RT_FLOAT Toggle_2_Pulse_21_1_1;
      RT_FLOAT Wheel_Enable;
      RT_FLOAT Requested_Torque_x_1;
      RT_FLOAT Requested_Torque_y_1;
      RT_FLOAT Requested_Torque_z_1;
      RT_FLOAT Wheel_Disable_Mode_5;
      RT_FLOAT Wheel_Disable_Mode_5_1;
      RT_FLOAT Mode_5_On;
      RT_FLOAT Reaction_Wheel_Control_98_1;
      RT_FLOAT Requested_Torque_x_2;
      RT_FLOAT Requested_Torque_y_2;
      RT_FLOAT Requested_Torque_z_2;
      RT_FLOAT Reaction_Wheel_Control_96_1;
      RT_FLOAT Reaction_Wheel_Control_97_1;
      RT_FLOAT Requested_Torque_x_3;
      RT_FLOAT Requested_Torque_y_3;
      RT_FLOAT Requested_Torque_z_3;
      RT_FLOAT Cmded_Body_Torque_x;
      RT_FLOAT Cmded_Body_Torque_y;
      RT_FLOAT Cmded_Body_Torque_z;
      RT_FLOAT Momentum_Error_X_Nms;
      RT_FLOAT Momentum_Error_Y_Nms;
      RT_FLOAT Momentum_Error_Z_Nms;
      RT_FLOAT X_3;
      RT_FLOAT Y_3;
      RT_FLOAT Z_2;
      RT_FLOAT Magnitude_2;
      RT_FLOAT Mx;
      RT_FLOAT My;
      RT_FLOAT Mz;
      RT_FLOAT Torque_Rod_Control_5_1;
      RT_FLOAT Torque_Rod_Control_5_2;
      RT_FLOAT Torque_Rod_Control_5_3;
      RT_FLOAT Requested_Dipole_X;
      RT_FLOAT Requested_Dipole_Y;
      RT_FLOAT Requested_Dipole_Z;
      RT_FLOAT Cmd_MTR_x_counts;
      RT_FLOAT Cmd_MTR_y_counts;
      RT_FLOAT Cmd_MTR_z_counts;
      RT_FLOAT Zero_1_1;
      RT_FLOAT Torque_Rod_Control_25_1;
      RT_FLOAT Torque_Rod_Control_25_2;
      RT_FLOAT Torque_Rod_Control_25_3;
      RT_FLOAT Torque_Rod_Control_26_1;
      RT_FLOAT Torque_Rod_Control_16_1;
      RT_FLOAT Torque_Rod_Control_2_1;
      RT_FLOAT Torque_Rod_Control_10_1;
      RT_FLOAT Scaling_Factor;
      RT_FLOAT Scaled_MTR_Count_X;
      RT_FLOAT Scaled_MTR_Count_Y;
      RT_FLOAT Scaled_MTR_Count_Z;
      RT_FLOAT Cmd_MTR_Am2_X;
      RT_FLOAT Cmd_MTR_Am2_Y;
      RT_FLOAT Cmd_MTR_Am2_Z;
      RT_FLOAT Torque_Rod_Control_97_1;
      RT_FLOAT Torque_Rod_Control_97_2;
      RT_FLOAT Torque_Rod_Control_98_1;
      RT_FLOAT Cmd_Dipole_X;
      RT_FLOAT Cmd_Dipole_Y;
      RT_FLOAT Cmd_Dipole_Z;
      RT_FLOAT Cmd_MTR_counts_X;
      RT_FLOAT Cmd_MTR_counts_Y;
      RT_FLOAT Cmd_MTR_counts_Z;
      RT_FLOAT Avoid_Eps_Bug_1;
      RT_FLOAT Avoid_Eps_Bug_2;
      RT_FLOAT Activation_Check_1;
      RT_FLOAT Bdot_x;
      RT_FLOAT Bdot_y;
      RT_FLOAT Bdot_z;
      RT_FLOAT BdotPreCalcGain_1;
      RT_FLOAT Bdot_X;
      RT_FLOAT Bdot_Y;
      RT_FLOAT Bdot_Z;
      RT_FLOAT X_4;
      RT_FLOAT Y_4;
      RT_FLOAT Z_3;
      RT_FLOAT Magnitude_3;
      RT_FLOAT Cmd_Dipole_X_1;
      RT_FLOAT Cmd_Dipole_Y_1;
      RT_FLOAT Cmd_Dipole_Z_1;
      RT_FLOAT Cmd_MTR_x_counts_1;
      RT_FLOAT Cmd_MTR_y_counts_1;
      RT_FLOAT Cmd_MTR_z_counts_1;
      RT_FLOAT Bdot_Controller_25_1;
      RT_FLOAT Bdot_Controller_25_2;
      RT_FLOAT Bdot_Controller_25_3;
      RT_FLOAT Bdot_Controller_26_1;
      RT_FLOAT Bdot_Controller_16_1;
      RT_FLOAT Bdot_Controller_6_1;
      RT_FLOAT Bdot_Controller_10_1;
      RT_FLOAT Scaling_Factor_1;
      RT_FLOAT Scaled_MTR_Count_X_1;
      RT_FLOAT Scaled_MTR_Count_Y_1;
      RT_FLOAT Scaled_MTR_Count_Z_1;
      RT_FLOAT Cmd_MTR_Am2_X_1;
      RT_FLOAT Cmd_MTR_Am2_Y_1;
      RT_FLOAT Cmd_MTR_Am2_Z_1;
      RT_FLOAT Zero_1_2;
      RT_FLOAT Cmd_Dipole_X_2;
      RT_FLOAT Cmd_Dipole_Y_2;
      RT_FLOAT Cmd_Dipole_Z_2;
      RT_FLOAT Cmd_MTR_counts_X_1;
      RT_FLOAT Cmd_MTR_counts_Y_1;
      RT_FLOAT Cmd_MTR_counts_Z_1;
      RT_FLOAT True_Last_3_Samples_6;
      RT_FLOAT True_Last_3_Samples_7;
      RT_FLOAT True_Last_3_Samples_8;
      RT_FLOAT FSW_93_3;
      RT_FLOAT q1_Bogus_Sun_N_B_1;
      RT_FLOAT q2_Bogus_Sun_N_B_1;
      RT_FLOAT q3_Bogus_Sun_N_B_1;
      RT_FLOAT q4_Bogus_Sun_N_B_1;
      RT_FLOAT Velocity_Track_91_5;
      RT_FLOAT Velocity_Track_91_6;
      RT_FLOAT Velocity_Track_91_7;
      RT_FLOAT MTR_Enable_1;
      RT_FLOAT q1_Bogus_Sun_N_B_2;
      RT_FLOAT q2_Bogus_Sun_N_B_2;
      RT_FLOAT q3_Bogus_Sun_N_B_2;
      RT_FLOAT q4_Bogus_Sun_N_B_2;
      RT_FLOAT Sun_Pointing_Mode_11_5;
      RT_FLOAT Sun_Pointing_Mode_11_6;
      RT_FLOAT Sun_Pointing_Mode_11_7;
      RT_FLOAT MTR_Enable_2;
      RT_FLOAT Idle_Mode_88_1;
      RT_FLOAT Idle_Mode_88_2;
      RT_FLOAT Idle_Mode_88_3;
      RT_FLOAT Idle_Mode_88_4;
      RT_FLOAT Idle_Mode_88_5;
      RT_FLOAT Idle_Mode_88_6;
      RT_FLOAT Idle_Mode_88_7;
      RT_FLOAT Idle_Mode_88_8;
      RT_FLOAT Sun_Search_Mode_97_1;
      RT_FLOAT Sun_Search_Mode_97_2;
      RT_FLOAT Sun_Search_Mode_97_3;
      RT_FLOAT Sun_Search_Mode_97_4;
      RT_FLOAT Sun_Search_Mode_97_5;
      RT_FLOAT Sun_Search_Mode_97_6;
      RT_FLOAT Sun_Search_Mode_97_7;
      RT_FLOAT Sun_Search_Mode_97_8;
      RT_FLOAT q1_Bogus_Sun_N_B;
      RT_FLOAT q2_Bogus_Sun_N_B;
      RT_FLOAT q3_Bogus_Sun_N_B;
      RT_FLOAT q4_Bogus_Sun_N_B;
      RT_FLOAT Sun_Pointing_Mode_96_5;
      RT_FLOAT Sun_Pointing_Mode_96_6;
      RT_FLOAT Sun_Pointing_Mode_96_7;
      RT_FLOAT MTR_Enable;
      RT_FLOAT Sun_Lock_Mode_95_1;
      RT_FLOAT Sun_Lock_Mode_95_2;
      RT_FLOAT Sun_Lock_Mode_95_3;
      RT_FLOAT Sun_Lock_Mode_95_4;
      RT_FLOAT Sun_Lock_Mode_95_5;
      RT_FLOAT Sun_Lock_Mode_95_6;
      RT_FLOAT Sun_Lock_Mode_95_7;
      RT_FLOAT Zero_1;
      RT_FLOAT Mission_Lock_Mode_94_1;
      RT_FLOAT Mission_Lock_Mode_94_2;
      RT_FLOAT Mission_Lock_Mode_94_3;
      RT_FLOAT Mission_Lock_Mode_94_4;
      RT_FLOAT Mission_Lock_Mode_94_5;
      RT_FLOAT Mission_Lock_Mode_94_6;
      RT_FLOAT Mission_Lock_Mode_94_7;
      RT_FLOAT Zero_2;
      RT_FLOAT Mission_Surface_Track_92_1;
      RT_FLOAT Mission_Surface_Track_92_2;
      RT_FLOAT Mission_Surface_Track_92_3;
      RT_FLOAT Mission_Surface_Track_92_4;
      RT_FLOAT Mission_Surface_Track_92_5;
      RT_FLOAT Mission_Surface_Track_92_6;
      RT_FLOAT Mission_Surface_Track_92_7;
      RT_FLOAT Zero_3;
      RT_FLOAT q1_err;
      RT_FLOAT q2_err;
      RT_FLOAT q3_err;
      RT_FLOAT q4_err;
      RT_FLOAT wx_Error;
      RT_FLOAT wy_Error;
      RT_FLOAT wz_Error;
      RT_FLOAT Zero;

      /***** Algorithmic Local Variables. *****/

      RT_BOOLEAN test;
      RT_FLOAT ulast;
      RT_INTEGER ireset;
      RT_BOOLEAN first_point;
      RT_INTEGER KState;
      static struct _mth_vect_norm_u mth_vect_norm_30_u;
      static struct _mth_vect_norm_y mth_vect_norm_30_y;
      static struct _mth_vect_norm_u mth_vect_norm_12_u;
      static struct _mth_vect_norm_y mth_vect_norm_12_y;
      static struct _mth_vect_norm_u mth_vect_norm_11_u;
      static struct _mth_vect_norm_y mth_vect_norm_11_y;
      static struct _mth_vect_norm_u mth_vect_norm_3_u;
      static struct _mth_vect_norm_y mth_vect_norm_3_y;
      static struct _mth_vect_norm_u mth_vect_norm_31_u;
      static struct _mth_vect_norm_y mth_vect_norm_31_y;
      static struct _mth_vect_norm_u mth_vect_norm_4_u;
      static struct _mth_vect_norm_y mth_vect_norm_4_y;
      static struct _Quaternion_From_DC_No_Flip_u Quaternion_From_DC_No_Flip_16_u;
      static struct _Quaternion_From_DC_No_Flip_y Quaternion_From_DC_No_Flip_16_y;
      static struct _Quaternion_From_DC_No_Flip_info Quaternion_From_DC_No_Flip_16_i;
      static struct _Compute_Gyro_Offset_u Compute_Gyro_Offset_21_u;
      static struct _Compute_Gyro_Offset_y Compute_Gyro_Offset_21_y;
      static struct _Compute_Gyro_Offset_s Compute_Gyro_Offset_21_s;
      static struct _mth_vect_norm_u mth_vect_norm_21_u;
      static struct _mth_vect_norm_y mth_vect_norm_21_y;
      static struct _Inertial_Capture_u Inertial_Capture_8_u;
      static struct _Inertial_Capture_y Inertial_Capture_8_y;
      static struct _Sun_Search_Mode_u Sun_Search_Mode_97_u;
      static struct _Sun_Search_Mode_y Sun_Search_Mode_97_y;
      static struct _Sun_Pointing_Mode_u Sun_Pointing_Mode_96_u;
      static struct _Sun_Pointing_Mode_y Sun_Pointing_Mode_96_y;
      static struct _Sun_Lock_Mode_u Sun_Lock_Mode_95_u;
      static struct _Sun_Lock_Mode_y Sun_Lock_Mode_95_y;
      static struct _Mission_Lock_Mode_u Mission_Lock_Mode_94_u;
      static struct _Mission_Lock_Mode_y Mission_Lock_Mode_94_y;
      static struct _Mission_Surface_Track_u Mission_Surface_Track_92_u;
      static struct _Mission_Surface_Track_y Mission_Surface_Track_92_y;
      static struct _Mission_Surface_Track_s Mission_Surface_Track_92_s;
      static struct _Mission_Surface_Track_info Mission_Surface_Track_92_i;
      static struct _Velocity_Track_u Velocity_Track_91_u;
      static struct _Velocity_Track_y Velocity_Track_91_y;
      static struct _Sun_Pointing_Mode_u Sun_Pointing_Mode_11_u;
      static struct _Sun_Pointing_Mode_y Sun_Pointing_Mode_11_y;
      static struct _Idle_Mode_y Idle_Mode_88_y;
      static struct _Toggle_Down_To_Pulse_u Toggle_Down_To_Pulse_94_u;
      static struct _Toggle_Down_To_Pulse_y Toggle_Down_To_Pulse_94_y;
      static struct _Toggle_Down_To_Pulse_s Toggle_Down_To_Pulse_94_s;
      static struct _mth_vect_norm_u mth_vect_norm_96_u;
      static struct _mth_vect_norm_y mth_vect_norm_96_y;
      static struct _mth_vect_norm_u mth_vect_norm_13_u;
      static struct _mth_vect_norm_y mth_vect_norm_13_y;
      static struct _mth_vect_norm_u mth_vect_norm_4_u_1;
      static struct _mth_vect_norm_y mth_vect_norm_4_y_1;

      /******* Initialization. *******/

      if (SUBSYS_PREINIT[1]) {
         iinfo[0] = 0; 
         iinfo[1] = 1; 
         iinfo[2] = 1; 
         iinfo[3] = 1; 
         INIT = 1; 
         X = &ss_1_states[0];
         XD = &ss_1_states[1];
         X->Gyro_Quantization_Filter_34_S1 = 0.0;
         X->Gyro_Quantization_Filter_34_S2 = 0.0;
         X->Gyro_Quantization_Filter_34_S3 = 0.0;
         X->Attitude_Control_28_S1 = 0.0;
         X->Control_Modes_6_S1 = 0.0;
         X->Control_Modes_6_S2 = 0.0;
         X->Control_Modes_14_S1 = 0.0;
         X->Control_Modes_14_S2 = 0.0;
         X->Control_Modes_12_S1 = 0.0;
         X->Control_Modes_12_S2 = 0.0;
         X->Control_Modes_1_S1 = 0.0;
         X->Control_Modes_1_S2 = 0.0;
         X->Control_Modes_3_S1 = 0.0;
         X->Control_Modes_3_S2 = 0.0;
         X->Control_Modes_23_S1 = 0.0;
         X->Control_Modes_23_S2 = 0.0;
         X->Control_Modes_9_S1 = 0.0;
         X->Control_Modes_9_S2 = 0.0;
         X->Control_Modes_19_S1 = 0.0;
         X->Control_Modes_19_S2 = 0.0;
         X->Mode_3_Timer_4_S1 = 0.0;
         X->Mode_3_Timer_99_S1 = 0.0;
         X->PAST_QUATERNION_S1 = 0.0;
         X->PAST_QUATERNION_S2 = 0.0;
         X->PAST_QUATERNION_S3 = 0.0;
         X->PAST_QUATERNION_S4 = 0.0;
         X->Quaternion_Require_Continuous_16_S1 = 0.0;
         X->Quaternion_Require_Continuous_16_S2 = 0.0;
         X->Quaternion_Require_Continuous_16_S3 = 0.0;
         X->Quaternion_Require_Continuous_16_S4 = 0.0;
         X->Enable_and_Initialize_96_S1 = 0.0;
         X->Toggle_2_Pulse_20_S1 = 0.0;
         X->Persistance_Test_Up_and_Down_12_S1 = 0.0;
         X->Persistance_Test_Up_and_Down_13_S1 = 0.0;
         X->Toggle_2_Pulse_5_20_S1 = 0.0;
         X->Toggle_2_Pulse_5_20_S2 = 0.0;
         X->Toggle_2_Pulse_5_20_S3 = 0.0;
         X->Toggle_2_Pulse_5_20_S4 = 0.0;
         X->Toggle_2_Pulse_20_S1_1 = 0.0;
         X->Persistance_Test_Up_Only_12_S1 = 0.0;
         X->Persistance_Test_Up_Only_13_S1 = 0.0;
         X->Persistance_Test_Up_Only_12_S1_1 = 0.0;
         X->Persistance_Test_Up_Only_13_S1_1 = 0.0;
         X->Persistance_Test_Up_Only_12_S1_2 = 0.0;
         X->Persistance_Test_Up_Only_13_S1_2 = 0.0;
         X->Avoid_Algebraic_Loop_S1 = 0.0;
         X->Persistance_Test_Up_Only_12_S1_3 = 0.0;
         X->Persistance_Test_Up_Only_13_S1_3 = 0.0;
         X->Persistance_Test_Up_Only_12_S1_4 = 0.0;
         X->Persistance_Test_Up_Only_13_S1_4 = 0.0;
         X->Safe_Sun_Presence_Check_4_S1 = 0.0;
         X->Persistance_Test_Up_Only_12_S1_5 = 0.0;
         X->Persistance_Test_Up_Only_13_S1_5 = 0.0;
         X->Persistance_Test_Up_Only_12_S1_6 = 0.0;
         X->Persistance_Test_Up_Only_13_S1_6 = 0.0;
         X->Persistance_Test_Up_Only_12_S1_7 = 0.0;
         X->Persistance_Test_Up_Only_13_S1_7 = 0.0;
         X->Persistance_Test_Up_Only_12_S1_8 = 0.0;
         X->Persistance_Test_Up_Only_13_S1_8 = 0.0;
         X->Persistance_Test_Up_Only_12_S1_9 = 0.0;
         X->Persistance_Test_Up_Only_13_S1_9 = 0.0;
         X->Persistance_Test_Up_Only_12_S1_10 = 0.0;
         X->Persistance_Test_Up_Only_13_S1_10 = 0.0;
         X->Safe_Sun_Presence_Check_2_S1 = 0.0;
         X->Mode_1 = 0.0;
         X->Mode_2 = 0.0;
         X->Mode_3 = 0.0;
         X->Mode_4 = 0.0;
         X->Mode_5 = 0.0;
         X->Mode_6 = 0.0;
         X->Mode_7 = 0.0;
         X->Copy_of_Mode_Controller_S8 = 0.0;
         X->Copy_of_Mode_Controller_S9 = 0.0;
         X->Copy_of_Mode_Controller_S10 = 0;
         X->Copy_of_Mode_Controller_S11 = 0;
         X->Latch_2_S1 = 0.0;
         X->Latch_2_S2 = 0;
         X->Latch_2_S3 = 0;
         X->Mode_3_Timer_2_S1 = 0.0;
         X->Flip_Flop_S1 = 0.0;
         X->Flip_Flop_S2 = 0;
         X->Flip_Flop_S3 = 0;
         XD->Gyro_Quantization_Filter_34_S1 = 0.0;
         XD->Gyro_Quantization_Filter_34_S2 = 0.0;
         XD->Gyro_Quantization_Filter_34_S3 = 0.0;
         XD->Attitude_Control_28_S1 = 0.0;
         XD->Control_Modes_6_S1 = 0.0;
         XD->Control_Modes_6_S2 = 0.0;
         XD->Control_Modes_14_S1 = 0.0;
         XD->Control_Modes_14_S2 = 0.0;
         XD->Control_Modes_12_S1 = 0.0;
         XD->Control_Modes_12_S2 = 0.0;
         XD->Control_Modes_1_S1 = 0.0;
         XD->Control_Modes_1_S2 = 0.0;
         XD->Control_Modes_3_S1 = 0.0;
         XD->Control_Modes_3_S2 = 0.0;
         XD->Control_Modes_23_S1 = 0.0;
         XD->Control_Modes_23_S2 = 0.0;
         XD->Control_Modes_9_S1 = 0.0;
         XD->Control_Modes_9_S2 = 0.0;
         XD->Control_Modes_19_S1 = 0.0;
         XD->Control_Modes_19_S2 = 0.0;
         XD->Mode_3_Timer_4_S1 = 0.0;
         XD->Mode_3_Timer_99_S1 = 0.0;
         XD->PAST_QUATERNION_S1 = 0.0;
         XD->PAST_QUATERNION_S2 = 0.0;
         XD->PAST_QUATERNION_S3 = 0.0;
         XD->PAST_QUATERNION_S4 = 0.0;
         XD->Quaternion_Require_Continuous_16_S1 = 0.0;
         XD->Quaternion_Require_Continuous_16_S2 = 0.0;
         XD->Quaternion_Require_Continuous_16_S3 = 0.0;
         XD->Quaternion_Require_Continuous_16_S4 = 0.0;
         XD->Enable_and_Initialize_96_S1 = 0.0;
         XD->Toggle_2_Pulse_20_S1 = 0.0;
         XD->Persistance_Test_Up_and_Down_12_S1 = 0.0;
         XD->Persistance_Test_Up_and_Down_13_S1 = 0.0;
         XD->Toggle_2_Pulse_5_20_S1 = 0.0;
         XD->Toggle_2_Pulse_5_20_S2 = 0.0;
         XD->Toggle_2_Pulse_5_20_S3 = 0.0;
         XD->Toggle_2_Pulse_5_20_S4 = 0.0;
         XD->Toggle_2_Pulse_20_S1_1 = 0.0;
         XD->Persistance_Test_Up_Only_12_S1 = 0.0;
         XD->Persistance_Test_Up_Only_13_S1 = 0.0;
         XD->Persistance_Test_Up_Only_12_S1_1 = 0.0;
         XD->Persistance_Test_Up_Only_13_S1_1 = 0.0;
         XD->Persistance_Test_Up_Only_12_S1_2 = 0.0;
         XD->Persistance_Test_Up_Only_13_S1_2 = 0.0;
         XD->Avoid_Algebraic_Loop_S1 = 0.0;
         XD->Persistance_Test_Up_Only_12_S1_3 = 0.0;
         XD->Persistance_Test_Up_Only_13_S1_3 = 0.0;
         XD->Persistance_Test_Up_Only_12_S1_4 = 0.0;
         XD->Persistance_Test_Up_Only_13_S1_4 = 0.0;
         XD->Safe_Sun_Presence_Check_4_S1 = 0.0;
         XD->Persistance_Test_Up_Only_12_S1_5 = 0.0;
         XD->Persistance_Test_Up_Only_13_S1_5 = 0.0;
         XD->Persistance_Test_Up_Only_12_S1_6 = 0.0;
         XD->Persistance_Test_Up_Only_13_S1_6 = 0.0;
         XD->Persistance_Test_Up_Only_12_S1_7 = 0.0;
         XD->Persistance_Test_Up_Only_13_S1_7 = 0.0;
         XD->Persistance_Test_Up_Only_12_S1_8 = 0.0;
         XD->Persistance_Test_Up_Only_13_S1_8 = 0.0;
         XD->Persistance_Test_Up_Only_12_S1_9 = 0.0;
         XD->Persistance_Test_Up_Only_13_S1_9 = 0.0;
         XD->Persistance_Test_Up_Only_12_S1_10 = 0.0;
         XD->Persistance_Test_Up_Only_13_S1_10 = 0.0;
         XD->Safe_Sun_Presence_Check_2_S1 = 0.0;
         XD->Mode_1 = 0.0;
         XD->Mode_2 = 0.0;
         XD->Mode_3 = 0.0;
         XD->Mode_4 = 0.0;
         XD->Mode_5 = 0.0;
         XD->Mode_6 = 0.0;
         XD->Mode_7 = 0.0;
         XD->Copy_of_Mode_Controller_S8 = 0.0;
         XD->Copy_of_Mode_Controller_S9 = 0.0;
         XD->Copy_of_Mode_Controller_S10 = 0;
         XD->Copy_of_Mode_Controller_S11 = 0;
         XD->Latch_2_S1 = 0.0;
         XD->Latch_2_S2 = 0;
         XD->Latch_2_S3 = 0;
         XD->Mode_3_Timer_2_S1 = 0.0;
         XD->Flip_Flop_S1 = 0.0;
         XD->Flip_Flop_S2 = 0;
         XD->Flip_Flop_S3 = 0;
         Compute_Gyro_Offset_21_s.Compute_Gyro_Offset_x = 0;
         Toggle_Down_To_Pulse_94_s.Toggle_Down_To_Pulse_x = 0;
         Quaternion_From_DC_No_Flip_16_i.iinfo[0] = iinfo[0];
         Quaternion_From_DC_No_Flip_16_i.iinfo[1] = iinfo[1];
         Quaternion_From_DC_No_Flip_16_i.iinfo[3] = iinfo[3];
         Mission_Surface_Track_92_i.iinfo[0] = iinfo[0];
         Mission_Surface_Track_92_i.iinfo[1] = iinfo[1];
         Mission_Surface_Track_92_i.iinfo[3] = iinfo[3];
         for( cnt=0;cnt<2;cnt++ ) {
            R_P[cnt] = _R_P[cnt];
         }
         X->Copy_of_Mode_Controller_S10 = 1;
         X->Latch_2_S2 = 1;
         X->Flip_Flop_S2 = 1;
         SUBSYS_PREINIT[1] = FALSE; 
         return;
      }

      /***** Output Update. *****/

      /* ----------------------------  IfThenElse */
      /* {FSW..57} */
      if( U->Gyro_AutoCalibrate < 0.5 ) {
         

      }
      /* ----------------------------  Time Delay */
      /* {Gyro Quantization Filter..34} */
      if (INIT) {
         X->Gyro_Quantization_Filter_34_S1 = 0.0; 
         X->Gyro_Quantization_Filter_34_S2 = 0.0; 
         X->Gyro_Quantization_Filter_34_S3 = 0.0; 
      }
      Gyro_Quantization_Filter_34_1 = X->Gyro_Quantization_Filter_34_S1; 
      Gyro_Quantization_Filter_34_2 = X->Gyro_Quantization_Filter_34_S2; 
      Gyro_Quantization_Filter_34_3 = X->Gyro_Quantization_Filter_34_S3; 
      /* ----------------------------  Time Delay */
      /* {Attitude Control..28} */
      if (INIT) {
         X->Attitude_Control_28_S1 = 0.0; 
      }
      Y->PD_Control = X->Attitude_Control_28_S1; 
      /* ----------------------------  Time Delay */
      /* {Control Modes..6} */
      if (INIT) {
         X->Control_Modes_6_S1 = 0.0; 
         X->Control_Modes_6_S2 = 0.0; 
      }
      Mode_2_or_3_Enable = X->Control_Modes_6_S1; 
      /* ----------------------------  Time Delay */
      /* {Control Modes..14} */
      if (INIT) {
         X->Control_Modes_14_S1 = 0.0; 
         X->Control_Modes_14_S2 = 0.0; 
      }
      Enable_1 = X->Control_Modes_14_S1; 
      /* ----------------------------  Time Delay */
      /* {Control Modes..12} */
      if (INIT) {
         X->Control_Modes_12_S1 = 0.0; 
         X->Control_Modes_12_S2 = 0.0; 
      }
      Enable_2 = X->Control_Modes_12_S1; 
      /* ----------------------------  Time Delay */
      /* {Control Modes..1} */
      if (INIT) {
         X->Control_Modes_1_S1 = 0.0; 
         X->Control_Modes_1_S2 = 0.0; 
      }
      Enable_3 = X->Control_Modes_1_S1; 
      /* ----------------------------  Time Delay */
      /* {Control Modes..3} */
      if (INIT) {
         X->Control_Modes_3_S1 = 0.0; 
         X->Control_Modes_3_S2 = 0.0; 
      }
      Enable_4 = X->Control_Modes_3_S1; 
      /* ----------------------------  Time Delay */
      /* {Control Modes..23} */
      if (INIT) {
         X->Control_Modes_23_S1 = 0.0; 
         X->Control_Modes_23_S2 = 0.0; 
      }
      Enable_5 = X->Control_Modes_23_S1; 
      /* ----------------------------  Time Delay */
      /* {Control Modes..9} */
      if (INIT) {
         X->Control_Modes_9_S1 = 0.0; 
         X->Control_Modes_9_S2 = 0.0; 
      }
      Enable_6 = X->Control_Modes_9_S1; 
      /* ----------------------------  Time Delay */
      /* {Control Modes..19} */
      if (INIT) {
         X->Control_Modes_19_S1 = 0.0; 
         X->Control_Modes_19_S2 = 0.0; 
      }
      Enable_7 = X->Control_Modes_19_S1; 
      /* ----------------------------  Algebraic Expression */
      /* {Mode 3 Timer.Reset timer to this value.1} */
      Reset_timer_to_this_value_1 = 0.0; 
      /* ----------------------------  Time Delay */
      /* {Mode 3 Timer..4} */
      if (INIT) {
         X->Mode_3_Timer_4_S1 = 0.0; 
      }
      Delayed_Sun_not_present = X->Mode_3_Timer_4_S1; 
      /* ----------------------------  Time Delay */
      /* {Mode 3 Timer..99} */
      if (INIT) {
         X->Mode_3_Timer_99_S1 = 0.0; 
      }
      Mode_3_Timer_99_1 = X->Mode_3_Timer_99_S1; 
      /* ----------------------------  Time Delay */
      /* {Accumulate Gyros.PAST QUATERNION.14} */
      if (INIT) {
         X->PAST_QUATERNION_S1 = 0.0; 
         X->PAST_QUATERNION_S2 = 0.0; 
         X->PAST_QUATERNION_S3 = 0.0; 
         X->PAST_QUATERNION_S4 = 1.0; 
      }
      PAST_QUATERNION_1 = X->PAST_QUATERNION_S1; 
      PAST_QUATERNION_2 = X->PAST_QUATERNION_S2; 
      PAST_QUATERNION_3 = X->PAST_QUATERNION_S3; 
      PAST_QUATERNION_4 = X->PAST_QUATERNION_S4; 
      /* ----------------------------  Time Delay */
      /* {Quaternion Require Continuous..16} */
      if (INIT) {
         X->Quaternion_Require_Continuous_16_S1 = 0.0; 
         X->Quaternion_Require_Continuous_16_S2 = 0.0; 
         X->Quaternion_Require_Continuous_16_S3 = 0.0; 
         X->Quaternion_Require_Continuous_16_S4 = 1.0; 
      }
      Quaternion_Require_Continuous_16_1 = X->Quaternion_Require_Continuous_16_S1; 
      Quaternion_Require_Continuous_16_2 = X->Quaternion_Require_Continuous_16_S2; 
      Quaternion_Require_Continuous_16_3 = X->Quaternion_Require_Continuous_16_S3; 
      Quaternion_Require_Continuous_16_4 = X->Quaternion_Require_Continuous_16_S4; 
      /* ----------------------------  Time Delay */
      /* {Enable and Initialize..96} */
      if (INIT) {
         X->Enable_and_Initialize_96_S1 = 0.0; 
      }
      Enable_and_Initialize_96_1 = X->Enable_and_Initialize_96_S1; 
      /* ----------------------------  Time Delay */
      /* {Toggle 2 Pulse..20} */
      if (INIT) {
         X->Toggle_2_Pulse_20_S1 = 0.0; 
      }
      Toggle_2_Pulse_20_1 = X->Toggle_2_Pulse_20_S1; 
      /* ----------------------------  Time Delay */
      /* {Persistance Test Up and Down..12} */
      if (INIT) {
         X->Persistance_Test_Up_and_Down_12_S1 = 0.0; 
      }
      Persistance_Test_Up_and_Down_12_1 = X->Persistance_Test_Up_and_Down_12_S1; 
      /* ----------------------------  Time Delay */
      /* {Persistance Test Up and Down..13} */
      if (INIT) {
         X->Persistance_Test_Up_and_Down_13_S1 = 0.0; 
      }
      Persistance_Test_Up_and_Down_13_1 = X->Persistance_Test_Up_and_Down_13_S1; 
      /* ----------------------------  Time Delay */
      /* {Toggle 2 Pulse 5..20} */
      if (INIT) {
         X->Toggle_2_Pulse_5_20_S1 = 0.0; 
         X->Toggle_2_Pulse_5_20_S2 = 0.0; 
         X->Toggle_2_Pulse_5_20_S3 = 0.0; 
         X->Toggle_2_Pulse_5_20_S4 = 0.0; 
      }
      Toggle_2_Pulse_5_20_1 = X->Toggle_2_Pulse_5_20_S1; 
      Toggle_2_Pulse_5_20_2 = X->Toggle_2_Pulse_5_20_S2; 
      Toggle_2_Pulse_5_20_3 = X->Toggle_2_Pulse_5_20_S3; 
      Toggle_2_Pulse_5_20_4 = X->Toggle_2_Pulse_5_20_S4; 
      /* ----------------------------  Time Delay */
      /* {Toggle 2 Pulse..20} */
      if (INIT) {
         X->Toggle_2_Pulse_20_S1_1 = 0.0; 
      }
      Toggle_2_Pulse_20_1_1 = X->Toggle_2_Pulse_20_S1_1; 
      /* ----------------------------  Time Delay */
      /* {Persistance Test Up Only..12} */
      if (INIT) {
         X->Persistance_Test_Up_Only_12_S1 = 0.0; 
      }
      Persistance_Test_Up_Only_12_1 = X->Persistance_Test_Up_Only_12_S1; 
      /* ----------------------------  Time Delay */
      /* {Persistance Test Up Only..13} */
      if (INIT) {
         X->Persistance_Test_Up_Only_13_S1 = 0.0; 
      }
      Persistance_Test_Up_Only_13_1 = X->Persistance_Test_Up_Only_13_S1; 
      /* ----------------------------  Time Delay */
      /* {Persistance Test Up Only..12} */
      if (INIT) {
         X->Persistance_Test_Up_Only_12_S1_1 = 0.0; 
      }
      Persistance_Test_Up_Only_12_1_1 = X->Persistance_Test_Up_Only_12_S1_1; 
      /* ----------------------------  Time Delay */
      /* {Persistance Test Up Only..13} */
      if (INIT) {
         X->Persistance_Test_Up_Only_13_S1_1 = 0.0; 
      }
      Persistance_Test_Up_Only_13_1_1 = X->Persistance_Test_Up_Only_13_S1_1; 
      /* ----------------------------  Time Delay */
      /* {Persistance Test Up Only..12} */
      if (INIT) {
         X->Persistance_Test_Up_Only_12_S1_2 = 0.0; 
      }
      Persistance_Test_Up_Only_12_1_2 = X->Persistance_Test_Up_Only_12_S1_2; 
      /* ----------------------------  Time Delay */
      /* {Persistance Test Up Only..13} */
      if (INIT) {
         X->Persistance_Test_Up_Only_13_S1_2 = 0.0; 
      }
      Persistance_Test_Up_Only_13_1_2 = X->Persistance_Test_Up_Only_13_S1_2; 
      /* ----------------------------  Time Delay */
      /* {Sun Line Error.Avoid Algebraic Loop.32} */
      if (INIT) {
         X->Avoid_Algebraic_Loop_S1 = 0.0; 
      }
      Y->Approx_Angular_Err_frm_Sun_Lin = X->Avoid_Algebraic_Loop_S1; 
      /* ----------------------------  Time Delay */
      /* {Persistance Test Up Only..12} */
      if (INIT) {
         X->Persistance_Test_Up_Only_12_S1_3 = 0.0; 
      }
      Persistance_Test_Up_Only_12_1_3 = X->Persistance_Test_Up_Only_12_S1_3; 
      /* ----------------------------  Time Delay */
      /* {Persistance Test Up Only..13} */
      if (INIT) {
         X->Persistance_Test_Up_Only_13_S1_3 = 0.0; 
      }
      Persistance_Test_Up_Only_13_1_3 = X->Persistance_Test_Up_Only_13_S1_3; 
      /* ----------------------------  Time Delay */
      /* {Persistance Test Up Only..12} */
      if (INIT) {
         X->Persistance_Test_Up_Only_12_S1_4 = 0.0; 
      }
      Persistance_Test_Up_Only_12_1_4 = X->Persistance_Test_Up_Only_12_S1_4; 
      /* ----------------------------  Time Delay */
      /* {Persistance Test Up Only..13} */
      if (INIT) {
         X->Persistance_Test_Up_Only_13_S1_4 = 0.0; 
      }
      Persistance_Test_Up_Only_13_1_4 = X->Persistance_Test_Up_Only_13_S1_4; 
      /* ----------------------------  Algebraic Expression */
      /* {Sun Line Error..11} */
      Sun_Line_Error_11_1 = 0.06; 
      /* ----------------------------  Algebraic Expression */
      /* {Safe Sun Presence Check.Reset timer to this value.1} */
      Reset_timer_to_this_value_1_1 = 0.0; 
      /* ----------------------------  Time Delay */
      /* {Safe Sun Presence Check..4} */
      if (INIT) {
         X->Safe_Sun_Presence_Check_4_S1 = 0.0; 
      }
      Delayed_Sun_not_present_1 = X->Safe_Sun_Presence_Check_4_S1; 
      /* ----------------------------  Time Delay */
      /* {Persistance Test Up Only..12} */
      if (INIT) {
         X->Persistance_Test_Up_Only_12_S1_5 = 0.0; 
      }
      Persistance_Test_Up_Only_12_1_5 = X->Persistance_Test_Up_Only_12_S1_5; 
      /* ----------------------------  Time Delay */
      /* {Persistance Test Up Only..13} */
      if (INIT) {
         X->Persistance_Test_Up_Only_13_S1_5 = 0.0; 
      }
      Persistance_Test_Up_Only_13_1_5 = X->Persistance_Test_Up_Only_13_S1_5; 
      /* ----------------------------  Time Delay */
      /* {Persistance Test Up Only..12} */
      if (INIT) {
         X->Persistance_Test_Up_Only_12_S1_6 = 0.0; 
      }
      Persistance_Test_Up_Only_12_1_6 = X->Persistance_Test_Up_Only_12_S1_6; 
      /* ----------------------------  Time Delay */
      /* {Persistance Test Up Only..13} */
      if (INIT) {
         X->Persistance_Test_Up_Only_13_S1_6 = 0.0; 
      }
      Persistance_Test_Up_Only_13_1_6 = X->Persistance_Test_Up_Only_13_S1_6; 
      /* ----------------------------  Time Delay */
      /* {Persistance Test Up Only..12} */
      if (INIT) {
         X->Persistance_Test_Up_Only_12_S1_7 = 0.0; 
      }
      Persistance_Test_Up_Only_12_1_7 = X->Persistance_Test_Up_Only_12_S1_7; 
      /* ----------------------------  Time Delay */
      /* {Persistance Test Up Only..13} */
      if (INIT) {
         X->Persistance_Test_Up_Only_13_S1_7 = 0.0; 
      }
      Persistance_Test_Up_Only_13_1_7 = X->Persistance_Test_Up_Only_13_S1_7; 
      /* ----------------------------  Time Delay */
      /* {Persistance Test Up Only..12} */
      if (INIT) {
         X->Persistance_Test_Up_Only_12_S1_8 = 0.0; 
      }
      Persistance_Test_Up_Only_12_1_8 = X->Persistance_Test_Up_Only_12_S1_8; 
      /* ----------------------------  Time Delay */
      /* {Persistance Test Up Only..13} */
      if (INIT) {
         X->Persistance_Test_Up_Only_13_S1_8 = 0.0; 
      }
      Persistance_Test_Up_Only_13_1_8 = X->Persistance_Test_Up_Only_13_S1_8; 
      /* ----------------------------  Time Delay */
      /* {Persistance Test Up Only..12} */
      if (INIT) {
         X->Persistance_Test_Up_Only_12_S1_9 = 0.0; 
      }
      Persistance_Test_Up_Only_12_1_9 = X->Persistance_Test_Up_Only_12_S1_9; 
      /* ----------------------------  Time Delay */
      /* {Persistance Test Up Only..13} */
      if (INIT) {
         X->Persistance_Test_Up_Only_13_S1_9 = 0.0; 
      }
      Persistance_Test_Up_Only_13_1_9 = X->Persistance_Test_Up_Only_13_S1_9; 
      /* ----------------------------  Time Delay */
      /* {Persistance Test Up Only..12} */
      if (INIT) {
         X->Persistance_Test_Up_Only_12_S1_10 = 0.0; 
      }
      Persistance_Test_Up_Only_12_1_10 = X->Persistance_Test_Up_Only_12_S1_10; 
      /* ----------------------------  Time Delay */
      /* {Persistance Test Up Only..13} */
      if (INIT) {
         X->Persistance_Test_Up_Only_13_S1_10 = 0.0; 
      }
      Persistance_Test_Up_Only_13_1_10 = X->Persistance_Test_Up_Only_13_S1_10; 
      /* ----------------------------  Procedure Super Block */
      /* {mth vect norm.30} */
      mth_vect_norm_30_u.Measured_magnetometer_body_x = U->nT_ECI_x;
      mth_vect_norm_30_u.Measured_magnetometer_body_y = U->nT_ECI_y;
      mth_vect_norm_30_u.Measured_magnetometer_body_z = U->nT_ECI_z;
      mth_vect_norm(&mth_vect_norm_30_u, &mth_vect_norm_30_y);
      mth_vect_norm_30_1 = mth_vect_norm_30_y.X_1;
      mth_vect_norm_30_2 = mth_vect_norm_30_y.Y_1;
      mth_vect_norm_30_3 = mth_vect_norm_30_y.Z;
      mth_vect_norm_30_4 = mth_vect_norm_30_y.Magnitude;
      /* ----------------------------  Procedure Super Block */
      /* {mth vect norm.12} */
      mth_vect_norm_12_u.Measured_magnetometer_body_x = U->SUNVEC0;
      mth_vect_norm_12_u.Measured_magnetometer_body_y = U->SUNVEC1;
      mth_vect_norm_12_u.Measured_magnetometer_body_z = U->SUNVEC2;
      mth_vect_norm(&mth_vect_norm_12_u, &mth_vect_norm_12_y);
      mth_vect_norm_12_1 = mth_vect_norm_12_y.X_1;
      mth_vect_norm_12_2 = mth_vect_norm_12_y.Y_1;
      mth_vect_norm_12_3 = mth_vect_norm_12_y.Z;
      mth_vect_norm_12_4 = mth_vect_norm_12_y.Magnitude;
      /* ----------------------------  Cross Product */
      /* {Algebraically Compute q B M..1} */
      Algebraically_Compute_q_B_M_1_1 = mth_vect_norm_12_2*mth_vect_norm_30_3 - mth_vect_norm_12_3*mth_vect_norm_30_2; 
      Algebraically_Compute_q_B_M_1_2 = mth_vect_norm_12_3*mth_vect_norm_30_1 - mth_vect_norm_12_1*mth_vect_norm_30_3; 
      Algebraically_Compute_q_B_M_1_3 = mth_vect_norm_12_1*mth_vect_norm_30_2 - mth_vect_norm_12_2*mth_vect_norm_30_1; 
      /* ----------------------------  Procedure Super Block */
      /* {mth vect norm.11} */
      mth_vect_norm_11_u.Measured_magnetometer_body_x = Algebraically_Compute_q_B_M_1_1;
      mth_vect_norm_11_u.Measured_magnetometer_body_y = Algebraically_Compute_q_B_M_1_2;
      mth_vect_norm_11_u.Measured_magnetometer_body_z = Algebraically_Compute_q_B_M_1_3;
      mth_vect_norm(&mth_vect_norm_11_u, &mth_vect_norm_11_y);
      mth_vect_norm_11_1 = mth_vect_norm_11_y.X_1;
      mth_vect_norm_11_2 = mth_vect_norm_11_y.Y_1;
      mth_vect_norm_11_3 = mth_vect_norm_11_y.Z;
      mth_vect_norm_11_4 = mth_vect_norm_11_y.Magnitude;
      /* ----------------------------  Cross Product */
      /* {Algebraically Compute q B M..2} */
      MB_13 = mth_vect_norm_11_2*mth_vect_norm_12_3 - mth_vect_norm_11_3*mth_vect_norm_12_2; 
      MB_23 = mth_vect_norm_11_3*mth_vect_norm_12_1 - mth_vect_norm_11_1*mth_vect_norm_12_3; 
      MB_33 = mth_vect_norm_11_1*mth_vect_norm_12_2 - mth_vect_norm_11_2*mth_vect_norm_12_1; 
      /* ----------------------------  ElementProduct */
      /* {Gyro Quantization Filter..44} */
      Gyro_Quantization_Filter_44_1 = Gyro_Quantization_Filter_34_1*U->AD_Filter_Pole_pf; 
      Gyro_Quantization_Filter_44_2 = Gyro_Quantization_Filter_34_2*U->AD_Filter_Pole_pf; 
      Gyro_Quantization_Filter_44_3 = Gyro_Quantization_Filter_34_3*U->AD_Filter_Pole_pf; 
      /* ----------------------------  SignalType Conversion */
      /* {Parameters 15.Avoid Epsilon Bug.6} */
      Reset_Estimator = ROUND(U->Reset_Estimator); 
      /* ----------------------------  Summer */
      /* {Correct Drift and Misalign..13} */
      Correct_Drift_and_Misalign_13_1 = U->Measured_Gyro_X - U->Gyro_Drift_x; 
      Correct_Drift_and_Misalign_13_2 = U->Measured_Gyro_Y - U->Gyro_Drift_y; 
      Correct_Drift_and_Misalign_13_3 = U->Measured_Gyro_Z - U->Gyro_Drift_z; 
      /* ----------------------------  Algebraic Expression */
      /* {Correct Drift and Misalign.Small Angle Transf IMU to Body.4} */
      Meas_Omega_B_N_B_x = Correct_Drift_and_Misalign_13_1 - Correct_Drift_and_Misalign_13_2*U->Gyro_MisAlmt_z + Correct_Drift_and_Misalign_13_3*U->Gyro_MisAlmt_y; 
      Meas_Omega_B_N_B_y = Correct_Drift_and_Misalign_13_1*U->Gyro_MisAlmt_z + Correct_Drift_and_Misalign_13_2 - Correct_Drift_and_Misalign_13_3*U->Gyro_MisAlmt_x; 
      Meas_Omega_B_N_B_z = (-1.0)*Correct_Drift_and_Misalign_13_1*U->Gyro_MisAlmt_y + Correct_Drift_and_Misalign_13_2*U->Gyro_MisAlmt_x + Correct_Drift_and_Misalign_13_3; 
      /* ----------------------------  ElementProduct */
      /* {Gyro Quantization Filter..35} */
      Gyro_Quantization_Filter_35_1 = Meas_Omega_B_N_B_x*U->AD_Filter_Gain_Kf; 
      Gyro_Quantization_Filter_35_2 = Meas_Omega_B_N_B_y*U->AD_Filter_Gain_Kf; 
      Gyro_Quantization_Filter_35_3 = Meas_Omega_B_N_B_z*U->AD_Filter_Gain_Kf; 
      /* ----------------------------  Summer */
      /* {Gyro Quantization Filter..33} */
      Y->Filt_Omega_B_N_B_x = Gyro_Quantization_Filter_35_1; 
      Y->Filt_Omega_B_N_B_x = Y->Filt_Omega_B_N_B_x + Gyro_Quantization_Filter_44_1; 
      Y->Filt_Omega_B_N_B_y = Gyro_Quantization_Filter_35_2; 
      Y->Filt_Omega_B_N_B_y = Y->Filt_Omega_B_N_B_y + Gyro_Quantization_Filter_44_2; 
      Y->Filt_Omega_B_N_B_z = Gyro_Quantization_Filter_35_3; 
      Y->Filt_Omega_B_N_B_z = Y->Filt_Omega_B_N_B_z + Gyro_Quantization_Filter_44_3; 
      /* ----------------------------  Algebraic Expression */
      /* {Mode Selection.Norm.35} */
      Norm_Omega_B_N_B = Y->Filt_Omega_B_N_B_x*Y->Filt_Omega_B_N_B_x + Y->Filt_Omega_B_N_B_y*Y->Filt_Omega_B_N_B_y + Y->Filt_Omega_B_N_B_z*Y->Filt_Omega_B_N_B_z; 
      /* ----------------------------  Algebraic Expression */
      /* {Sun Sensor Processing.Small Ang Transf Sensor to Body.95} */
      Y->Meas_Sun_Vector_B_x = U->Measured_SS_x - U->Measured_SS_y*U->Sun_Sensor_MisAlmt_z + U->Measured_SS_z*U->Sun_Sensor_MisAlmt_y; 
      Y->Meas_Sun_Vector_B_y = U->Measured_SS_x*U->Sun_Sensor_MisAlmt_z + U->Measured_SS_y - U->Measured_SS_z*U->Sun_Sensor_MisAlmt_x; 
      Y->Meas_Sun_Vector_B_z = (-1.0)*U->Measured_SS_x*U->Sun_Sensor_MisAlmt_y + U->Measured_SS_y*U->Sun_Sensor_MisAlmt_x + U->Measured_SS_z; 
      /* ----------------------------  Procedure Super Block */
      /* {mth vect norm.3} */
      mth_vect_norm_3_u.Measured_magnetometer_body_x = Y->Meas_Sun_Vector_B_x;
      mth_vect_norm_3_u.Measured_magnetometer_body_y = Y->Meas_Sun_Vector_B_y;
      mth_vect_norm_3_u.Measured_magnetometer_body_z = Y->Meas_Sun_Vector_B_z;
      mth_vect_norm(&mth_vect_norm_3_u, &mth_vect_norm_3_y);
      mth_vect_norm_3_1 = mth_vect_norm_3_y.X_1;
      mth_vect_norm_3_2 = mth_vect_norm_3_y.Y_1;
      mth_vect_norm_3_3 = mth_vect_norm_3_y.Z;
      mth_vect_norm_3_4 = mth_vect_norm_3_y.Magnitude;
      /* ----------------------------  Gain Block */
      /* {Sun Sensor Processing..7} */
      Y->Sun_Sensor_Processing_7_1 = U->Sun_Present_Flag; 
      /* ----------------------------  Gain Block */
      /* {Magnetometer Data Processing..1} */
      Y->meas_mag_body_x = U->Measured_Mag_X; 
      Y->meas_mag_body_y = U->Measured_Mag_Y; 
      Y->meas_mag_body_z = U->Measured_Mag_Z; 
      /* ----------------------------  Procedure Super Block */
      /* {mth vect norm.31} */
      mth_vect_norm_31_u.Measured_magnetometer_body_x = Y->meas_mag_body_x;
      mth_vect_norm_31_u.Measured_magnetometer_body_y = Y->meas_mag_body_y;
      mth_vect_norm_31_u.Measured_magnetometer_body_z = Y->meas_mag_body_z;
      mth_vect_norm(&mth_vect_norm_31_u, &mth_vect_norm_31_y);
      mth_vect_norm_31_1 = mth_vect_norm_31_y.X_1;
      mth_vect_norm_31_2 = mth_vect_norm_31_y.Y_1;
      mth_vect_norm_31_3 = mth_vect_norm_31_y.Z;
      mth_vect_norm_31_4 = mth_vect_norm_31_y.Magnitude;
      /* ----------------------------  Cross Product */
      /* {Algebraically Compute q B M..22} */
      Algebraically_Compute_q_B_M_22_1 = mth_vect_norm_3_2*mth_vect_norm_31_3 - mth_vect_norm_3_3*mth_vect_norm_31_2; 
      Algebraically_Compute_q_B_M_22_2 = mth_vect_norm_3_3*mth_vect_norm_31_1 - mth_vect_norm_3_1*mth_vect_norm_31_3; 
      Algebraically_Compute_q_B_M_22_3 = mth_vect_norm_3_1*mth_vect_norm_31_2 - mth_vect_norm_3_2*mth_vect_norm_31_1; 
      /* ----------------------------  Procedure Super Block */
      /* {mth vect norm.4} */
      mth_vect_norm_4_u.Measured_magnetometer_body_x = Algebraically_Compute_q_B_M_22_1;
      mth_vect_norm_4_u.Measured_magnetometer_body_y = Algebraically_Compute_q_B_M_22_2;
      mth_vect_norm_4_u.Measured_magnetometer_body_z = Algebraically_Compute_q_B_M_22_3;
      mth_vect_norm(&mth_vect_norm_4_u, &mth_vect_norm_4_y);
      mth_vect_norm_4_1 = mth_vect_norm_4_y.X_1;
      mth_vect_norm_4_2 = mth_vect_norm_4_y.Y_1;
      mth_vect_norm_4_3 = mth_vect_norm_4_y.Z;
      mth_vect_norm_4_4 = mth_vect_norm_4_y.Magnitude;
      /* ----------------------------  Cross Product */
      /* {Algebraically Compute q B M..23} */
      MB_13_1 = mth_vect_norm_4_2*mth_vect_norm_3_3 - mth_vect_norm_4_3*mth_vect_norm_3_2; 
      MB_23_1 = mth_vect_norm_4_3*mth_vect_norm_3_1 - mth_vect_norm_4_1*mth_vect_norm_3_3; 
      MB_33_1 = mth_vect_norm_4_1*mth_vect_norm_3_2 - mth_vect_norm_4_2*mth_vect_norm_3_1; 
      /* ----------------------------  Algebraic Expression */
      /* {Algebraically Compute q B M.Direction Cosine Matrix.14} */
      A11 = mth_vect_norm_3_1*mth_vect_norm_12_1 + MB_13_1*MB_13 + mth_vect_norm_4_1*mth_vect_norm_11_1; 
      A12 = mth_vect_norm_3_1*mth_vect_norm_12_2 + MB_13_1*MB_23 + mth_vect_norm_4_1*mth_vect_norm_11_2; 
      A13 = mth_vect_norm_3_1*mth_vect_norm_12_3 + MB_13_1*MB_33 + mth_vect_norm_4_1*mth_vect_norm_11_3; 
      A21 = mth_vect_norm_3_2*mth_vect_norm_12_1 + MB_23_1*MB_13 + mth_vect_norm_4_2*mth_vect_norm_11_1; 
      A22 = mth_vect_norm_3_2*mth_vect_norm_12_2 + MB_23_1*MB_23 + mth_vect_norm_4_2*mth_vect_norm_11_2; 
      A23 = mth_vect_norm_3_2*mth_vect_norm_12_3 + MB_23_1*MB_33 + mth_vect_norm_4_2*mth_vect_norm_11_3; 
      A31 = mth_vect_norm_3_3*mth_vect_norm_12_1 + MB_33_1*MB_13 + mth_vect_norm_4_3*mth_vect_norm_11_1; 
      A32 = mth_vect_norm_3_3*mth_vect_norm_12_2 + MB_33_1*MB_23 + mth_vect_norm_4_3*mth_vect_norm_11_2; 
      A33 = mth_vect_norm_3_3*mth_vect_norm_12_3 + MB_33_1*MB_33 + mth_vect_norm_4_3*mth_vect_norm_11_3; 
      /* ----------------------------  Procedure Super Block */
      /* {Quaternion From DC No Flip.16} */
      Quaternion_From_DC_No_Flip_16_u.A11 = A11;
      Quaternion_From_DC_No_Flip_16_u.A12 = A12;
      Quaternion_From_DC_No_Flip_16_u.A13 = A13;
      Quaternion_From_DC_No_Flip_16_u.A21 = A21;
      Quaternion_From_DC_No_Flip_16_u.A22 = A22;
      Quaternion_From_DC_No_Flip_16_u.A23 = A23;
      Quaternion_From_DC_No_Flip_16_u.A31 = A31;
      Quaternion_From_DC_No_Flip_16_u.A32 = A32;
      Quaternion_From_DC_No_Flip_16_u.A33 = A33;
      Quaternion_From_DC_No_Flip(&Quaternion_From_DC_No_Flip_16_u, &Quaternion_From_DC_No_Flip_16_y, &Quaternion_From_DC_No_Flip_16_i);
      q1_B_M_Estimated = Quaternion_From_DC_No_Flip_16_y.Q1_desired;
      q2_B_M_Estimated = Quaternion_From_DC_No_Flip_16_y.Q2_desired;
      q3_B_M_Estimated = Quaternion_From_DC_No_Flip_16_y.Q3_desired;
      q4_B_M_Estimated = Quaternion_From_DC_No_Flip_16_y.Q4_desired;
      iinfo[0] = Quaternion_From_DC_No_Flip_16_i.iinfo[0];
      if( iinfo[0] != 0 ) {
         Quaternion_From_DC_No_Flip_16_i.iinfo[0] = 0; goto EXEC_ERROR;
      }
      /* ----------------------------  Gain Block */
      /* {Reaction Wheel Tach Processing..99} */
      Y->Meas_Wheel_omega_rad_s_x = U->Measured_RW_omega_x; 
      Y->Meas_Wheel_omega_rad_s_y = U->Measured_RW_omega_y; 
      Y->Meas_Wheel_omega_rad_s_z = U->Measured_RW_omega_z; 
      /* ----------------------------  Algebraic Expression */
      /* {Calc Wheel Momentum..13} */
      Y->Wheel_Hx_Body_Frame = Y->Meas_Wheel_omega_rad_s_x*U->I_Wheel_xx; 
      Y->Wheel_Hy_Body_Frame = Y->Meas_Wheel_omega_rad_s_y*U->I_Wheel_yy; 
      Y->Wheel_Hz_Body_Frame = Y->Meas_Wheel_omega_rad_s_z*U->I_Wheel_zz; 
      /* ----------------------------  Logical Expression */
      /* {Mode Selection.Avoid Numerical Error.1} */
      if (Y->Sun_Sensor_Processing_7_1 > 0.5) {
         Avoid_Numerical_Error_1 = 1.0; 
      }
      else {
         Avoid_Numerical_Error_1 = 0.0; 
      }
      /* ----------------------------  Logical Operator -- AND-OR-NOT */
      /* {Persistance Test Up Only..14} */
      test = Persistance_Test_Up_Only_13_1_1 > 0.0; 
      test = test && Persistance_Test_Up_Only_12_1_1 > 0.0; 
      test = test && Avoid_Numerical_Error_1 > 0.0; 
      if (test) {
         Y->True_Last_3_Samples = 1.0; 
      }
      else {
         Y->True_Last_3_Samples = 0.0; 
      }
      /* ----------------------------  Square Root */
      /* {Mode Selection..95} */
      Mode_Selection_95_1 = sqrt(Norm_Omega_B_N_B); 
      /* ----------------------------  Logical Expression */
      /* {Mode Selection.Omega Less Than Omega Min.4} */
      if (Mode_Selection_95_1 < U->Min_Omega) {
         Omega_B_N_B_is_Small = 1.0; 
      }
      else {
         Omega_B_N_B_is_Small = 0.0; 
      }
      /* ----------------------------  Logical Operator -- AND-OR-NOT */
      /* {Persistance Test Up Only..14} */
      test = Persistance_Test_Up_Only_13_1 > 0.0; 
      test = test && Persistance_Test_Up_Only_12_1 > 0.0; 
      test = test && Omega_B_N_B_is_Small > 0.0; 
      if (test) {
         True_Last_3_Samples = 1.0; 
      }
      else {
         True_Last_3_Samples = 0.0; 
      }
      /* ----------------------------  Logical Expression */
      /* {Mode Selection.Omega Greater Than Omega Max.98} */
      if (Mode_Selection_95_1 > U->Max_Omega) {
         Omega_B_N_B_is_Large = 1.0; 
      }
      else {
         Omega_B_N_B_is_Large = 0.0; 
      }
      /* ----------------------------  Logical Operator -- AND-OR-NOT */
      /* {Persistance Test Up Only..14} */
      test = Persistance_Test_Up_Only_13_1_2 > 0.0; 
      test = test && Persistance_Test_Up_Only_12_1_2 > 0.0; 
      test = test && Omega_B_N_B_is_Large > 0.0; 
      if (test) {
         True_Last_3_Samples_1 = 1.0; 
      }
      else {
         True_Last_3_Samples_1 = 0.0; 
      }
      /* ----------------------------  Logical Expression */
      /* {Sun Line Error.Sun Line Error Small Enough.99} */
      if (Y->Approx_Angular_Err_frm_Sun_Lin < U->Min_Sun_Line_Trans) {
         Sun_Line_Error_Small = 1.0; 
      }
      else {
         Sun_Line_Error_Small = 0.0; 
      }
      /* ----------------------------  Logical Operator -- AND-OR-NOT */
      /* {Persistance Test Up Only..14} */
      test = Persistance_Test_Up_Only_13_1_4 > 0.0; 
      test = test && Persistance_Test_Up_Only_12_1_4 > 0.0; 
      test = test && Sun_Line_Error_Small > 0.0; 
      if (test) {
         True_Last_3_Samples_2 = 1.0; 
      }
      else {
         True_Last_3_Samples_2 = 0.0; 
      }
      /* ----------------------------  Summer */
      /* {Sun Line Error..21} */
      Sun_Line_Error_21_1 = U->Min_Sun_Line_Trans + Sun_Line_Error_11_1; 
      /* ----------------------------  Logical Expression */
      /* {Sun Line Error.Sun Line Error Small Enough.8} */
      if (Y->Approx_Angular_Err_frm_Sun_Lin > Sun_Line_Error_21_1) {
         Sun_Line_Error_Small_1 = 1.0; 
      }
      else {
         Sun_Line_Error_Small_1 = 0.0; 
      }
      /* ----------------------------  Logical Operator -- AND-OR-NOT */
      /* {Persistance Test Up Only..14} */
      test = Persistance_Test_Up_Only_13_1_3 > 0.0; 
      test = test && Persistance_Test_Up_Only_12_1_3 > 0.0; 
      test = test && Sun_Line_Error_Small_1 > 0.0; 
      if (test) {
         True_Last_3_Samples_3 = 1.0; 
      }
      else {
         True_Last_3_Samples_3 = 0.0; 
      }
      /* ----------------------------  Procedure Super Block */
      /* {Compute_Gyro_Offset.21} */
      Compute_Gyro_Offset_21_u.Meas_Sun_Vector_B_x = Y->Meas_Sun_Vector_B_x;
      Compute_Gyro_Offset_21_u.Meas_Sun_Vector_B_y = Y->Meas_Sun_Vector_B_y;
      Compute_Gyro_Offset_21_u.gyro_offset_x = U->gyro_offset_x;
      Compute_Gyro_Offset_21_u.gyro_offset_y = U->gyro_offset_y;
      Compute_Gyro_Offset_21_u.gyro_offset_z = U->gyro_offset_z;
      Compute_Gyro_Offset_21_u.Filt_Omega_B_N_B_x = Y->Filt_Omega_B_N_B_x;
      Compute_Gyro_Offset_21_u.Filt_Omega_B_N_B_y = Y->Filt_Omega_B_N_B_y;
      Compute_Gyro_Offset_21_u.Filt_Omega_B_N_B_z = Y->Filt_Omega_B_N_B_z;
      Compute_Gyro_Offset_21_u.n = U->n;
      Compute_Gyro_Offset_21_u.Enable_1 = Enable_1;
      Compute_Gyro_Offset_21_u.Meas_Sun_Vector_B_z = Y->Meas_Sun_Vector_B_z;
      Compute_Gyro_Offset(&Compute_Gyro_Offset_21_u, &Compute_Gyro_Offset_21_y, &Compute_Gyro_Offset_21_s, INIT);
      Compute_Gyro_Offset_21_s.Compute_Gyro_Offset_x = 1 - Compute_Gyro_Offset_21_s.Compute_Gyro_Offset_x;
      Y->Compute_Gyro_Offset_21_1 = Compute_Gyro_Offset_21_y.Compute_Gyro_Offset_19_1;
      Y->Compute_Gyro_Offset_21_2 = Compute_Gyro_Offset_21_y.Compute_Gyro_Offset_19_2;
      Y->Compute_Gyro_Offset_21_3 = Compute_Gyro_Offset_21_y.Compute_Gyro_Offset_19_3;
      Y->Compute_Gyro_Offset_21_4 = Compute_Gyro_Offset_21_y.Compute_Gyro_Offset_19_4;
      Y->Compute_Gyro_Offset_21_5 = Compute_Gyro_Offset_21_y.Compute_Gyro_Offset_99_1;
      /* ----------------------------  Summer */
      /* {Safe Sun Presence Check..3} */
      Reset_Trigger = Y->Sun_Sensor_Processing_7_1; 
      Reset_Trigger = Reset_Trigger - Delayed_Sun_not_present_1; 
      /* ----------------------------  Logical Operator -- AND-OR-NOT */
      /* {Safe Sun Presence Check..7} */
      if ((Y->Sun_Sensor_Processing_7_1 > 0.0)) {
         Sun_not_present = 0.0; 
      }
      else {
         Sun_not_present = 1.0; 
      }
      /* ----------------------------  Integrator */
      /* {Safe Sun Presence Check..2} */
      if (INIT && XREMAP) {
         X->Safe_Sun_Presence_Check_2_S1 = X->Safe_Sun_Presence_Check_2_S1 - TSAMP*Sun_not_present; 
      }
      if (INIT) {
         R_P[0] = Reset_Trigger; 
      }
      ireset = 0; 
      ulast = R_P[0]; 
      R_P[0] = Reset_Trigger; 
      if (ulast <= 0.0 && R_P[0] > 0.0) {
         ireset = 1; 
      }
      first_point = (TIME == 0.0); 
      if (ireset != 0 && !first_point) {
         if (XREMAP) {
            X->Safe_Sun_Presence_Check_2_S1 = X->Safe_Sun_Presence_Check_2_S1 + TSAMP*Sun_not_present; 
         }
         X->Safe_Sun_Presence_Check_2_S1 = Reset_timer_to_this_value_1_1; 
         if (XREMAP) {
            X->Safe_Sun_Presence_Check_2_S1 = X->Safe_Sun_Presence_Check_2_S1 - TSAMP*Sun_not_present; 
         }
      }
      Time_since_Sun_not_present = X->Safe_Sun_Presence_Check_2_S1 + TSAMP*Sun_not_present; 
      /* ----------------------------  Logical Expression */
      /* {Safe Sun Presence Check..5} */
      if (Time_since_Sun_not_present > U->SP_time_limit) {
         Y->SP_check_failed = 1.0; 
      }
      else {
         Y->SP_check_failed = 0.0; 
      }
      /* ----------------------------  Gain Block */
      /* {Safe Wheel Saturation Check..10} */
      Whel_Sat_Lower_Limit = 0.25*U->Wheel_Sat_Limit; 
      /* ----------------------------  Gain Block */
      /* {Accumulate Gyros.Delta T Over 2.22} */
      Delta_T_Over_2_1 = 0.125*Y->Filt_Omega_B_N_B_x; 
      Delta_T_Over_2_2 = 0.125*Y->Filt_Omega_B_N_B_y; 
      Delta_T_Over_2_3 = 0.125*Y->Filt_Omega_B_N_B_z; 
      /* ----------------------------  Algebraic Expression */
      /* {Accumulate Gyros.Quaternion Update.23} */
      Quaternion_Update_1 = PAST_QUATERNION_1 + PAST_QUATERNION_2*Delta_T_Over_2_3 - PAST_QUATERNION_3*Delta_T_Over_2_2 + PAST_QUATERNION_4*Delta_T_Over_2_1; 
      Quaternion_Update_2 = (-1.0)*Delta_T_Over_2_3*PAST_QUATERNION_1 + PAST_QUATERNION_2 + Delta_T_Over_2_1*PAST_QUATERNION_3 + Delta_T_Over_2_2*PAST_QUATERNION_4; 
      Quaternion_Update_3 = Delta_T_Over_2_2*PAST_QUATERNION_1 - Delta_T_Over_2_1*PAST_QUATERNION_2 + PAST_QUATERNION_3 + Delta_T_Over_2_3*PAST_QUATERNION_4; 
      Quaternion_Update_4 = (-1.0)*Delta_T_Over_2_1*PAST_QUATERNION_1 - Delta_T_Over_2_2*PAST_QUATERNION_2 - Delta_T_Over_2_3*PAST_QUATERNION_3 + PAST_QUATERNION_4; 
      Quaternion_Update_5 = Quaternion_Update_1*Quaternion_Update_1 + Quaternion_Update_2*Quaternion_Update_2 + Quaternion_Update_3*Quaternion_Update_3 + Quaternion_Update_4*Quaternion_Update_4; 
      /* ----------------------------  Square Root */
      /* {Accumulate Gyros..33} */
      Accumulate_Gyros_33_1 = sqrt(Quaternion_Update_5); 
      /* ----------------------------  Algebraic Expression */
      /* {Accumulate Gyros..34} */
      Accumulate_Gyros_34_1 = Accumulate_Gyros_33_1 + 1.0E-20; 
      /* ----------------------------  Algebraic Expression */
      /* {Accumulate Gyros.Normalized Quaternion.26} */
      q1_B_LV_Estimated = Quaternion_Update_1/Accumulate_Gyros_34_1; 
      q2_B_LV_Estimated = Quaternion_Update_2/Accumulate_Gyros_34_1; 
      q3_B_LV_Estimated = Quaternion_Update_3/Accumulate_Gyros_34_1; 
      q4_B_LV_Estimated = Quaternion_Update_4/Accumulate_Gyros_34_1; 
      /* ----------------------------  Procedure Super Block */
      /* {mth vect norm.21} */
      mth_vect_norm_21_u.Measured_magnetometer_body_x = Y->Wheel_Hx_Body_Frame;
      mth_vect_norm_21_u.Measured_magnetometer_body_y = Y->Wheel_Hy_Body_Frame;
      mth_vect_norm_21_u.Measured_magnetometer_body_z = Y->Wheel_Hz_Body_Frame;
      mth_vect_norm(&mth_vect_norm_21_u, &mth_vect_norm_21_y);
      X_1 = mth_vect_norm_21_y.X_1;
      Y_1 = mth_vect_norm_21_y.Y_1;
      Z = mth_vect_norm_21_y.Z;
      Magnitude = mth_vect_norm_21_y.Magnitude;
      /* ----------------------------  Relational Operator -- LT-EQ-GT */
      /* {Safe Wheel Saturation Check..11} */
      if (Magnitude >= U->Wheel_Sat_Limit) {
         Saturated = 1.0; 
      }
      else {
         Saturated = 0.0; 
      }
      /* ----------------------------  Logical Operator -- AND-OR-NOT */
      /* {Persistance Test Up Only..14} */
      test = Persistance_Test_Up_Only_13_1_5 > 0.0; 
      test = test && Persistance_Test_Up_Only_12_1_5 > 0.0; 
      test = test && Saturated > 0.0; 
      if (test) {
         Y->True_Last_3_Samples_1 = 1.0; 
      }
      else {
         Y->True_Last_3_Samples_1 = 0.0; 
      }
      /* ----------------------------  Relational Operator -- LT-EQ-GT */
      /* {Safe Wheel Saturation Check..14} */
      if (Magnitude <= Whel_Sat_Lower_Limit) {
         Desaturated = 1.0; 
      }
      else {
         Desaturated = 0.0; 
      }
      /* ----------------------------  Logical Operator -- AND-OR-NOT */
      /* {Persistance Test Up Only..14} */
      test = Persistance_Test_Up_Only_13_1_6 > 0.0; 
      test = test && Persistance_Test_Up_Only_12_1_6 > 0.0; 
      test = test && Desaturated > 0.0; 
      if (test) {
         True_Last_3_Samples_4 = 1.0; 
      }
      else {
         True_Last_3_Samples_4 = 0.0; 
      }
      /* ----------------------------  SignalType Conversion */
      /* {Mode Selection.Convert to Ints.6} */
      Mode_Override = ROUND(U->Mode_Override); 
      Omega_Small = ROUND(True_Last_3_Samples); 
      Sun_present = ROUND(Y->True_Last_3_Samples); 
      Sun_line_error_small = ROUND(True_Last_3_Samples_2); 
      Sun_line_error_large = ROUND(True_Last_3_Samples_3); 
      Mode_limit = ROUND(U->Mode_Limit); 
      Omega_Large = ROUND(True_Last_3_Samples_1); 
      Wheel_Sat = ROUND(Y->True_Last_3_Samples_1); 
      Slew_Angle = ROUND(U->Slew_Angle); 
      Sun_gone_for_a_while = ROUND(Y->SP_check_failed); 
      Wheel_desaturated = ROUND(True_Last_3_Samples_4); 
      Enable_Momentum_Dump = ROUND(U->Enable_Momentum_Dump); 
      /* ----------------------------  State Transition Diagram */
      /* {Mode Selection.Copy of Mode Controller.2} */
      XD->Mode_1 = X->Mode_1; 
      XD->Mode_2 = X->Mode_2; 
      XD->Mode_3 = X->Mode_3; 
      XD->Mode_4 = X->Mode_4; 
      XD->Mode_5 = X->Mode_5; 
      XD->Mode_6 = X->Mode_6; 
      XD->Mode_7 = X->Mode_7; 
      XD->Copy_of_Mode_Controller_S8 = X->Copy_of_Mode_Controller_S8; 
      XD->Copy_of_Mode_Controller_S9 = X->Copy_of_Mode_Controller_S9; 
      XD->Copy_of_Mode_Controller_S10 = X->Copy_of_Mode_Controller_S10; 
      if (!INIT) {
         KState = X->Copy_of_Mode_Controller_S10; 
         switch (KState)
         {
         case 1:
            if (Mode_Override > 1 || Mode_limit > 1 && (Mode_Override == 0)) {
               XD->Copy_of_Mode_Controller_S10 = 2; 
            }
            break;
         case 2:
            if (Omega_Small > 0.0 && Mode_limit > 2 && (Mode_Override == 0) || Mode_Override > 2) {
               XD->Copy_of_Mode_Controller_S10 = 3; 
            }
            else if (Mode_limit < 2 || (Mode_Override == 1)) {
               XD->Copy_of_Mode_Controller_S10 = 1; 
            }
            else if (Mode_Override == 9 || Wheel_Sat > 0.0 && Enable_Momentum_Dump > 0.0) {
               XD->Copy_of_Mode_Controller_S10 = 6; 
            }
            break;
         case 3:
            if (Omega_Large > 0.0 || Mode_limit < 3 || Mode_Override < 3 && !(Mode_Override == 0)) {
               XD->Copy_of_Mode_Controller_S10 = 2; 
            }
            else if (Sun_present > 0.0 && Mode_limit > 3 && (Mode_Override == 0) || Mode_Override > 3) {
               XD->Copy_of_Mode_Controller_S10 = 4; 
            }
            else if (Mode_Override == 9 || Wheel_Sat > 0.0 && Enable_Momentum_Dump > 0.0) {
               XD->Copy_of_Mode_Controller_S10 = 6; 
            }
            break;
         case 4:
            if (Sun_present <= 0.0 && Mode_limit > 4 && (Mode_Override == 0) || Mode_Override > 4) {
               XD->Copy_of_Mode_Controller_S10 = 5; 
            }
            else if (Mode_Override == 9 || Wheel_Sat > 0.0 && Enable_Momentum_Dump > 0.0) {
               XD->Copy_of_Mode_Controller_S10 = 6; 
            }
            break;
         case 5:
            if (Mode_Override == 6) {
               XD->Copy_of_Mode_Controller_S10 = 7; 
            }
            else if (Sun_present > 0.0 && !(Mode_Override > 4) || Mode_limit < 5 || Mode_Override < 5 && !(Mode_Override == 0)) {
               XD->Copy_of_Mode_Controller_S10 = 4; 
            }
            else if (Mode_Override == 7) {
               XD->Copy_of_Mode_Controller_S10 = 8; 
            }
            else if (Mode_Override == 8) {
               XD->Copy_of_Mode_Controller_S10 = 9; 
            }
            else if (Sun_gone_for_a_while > 0.0 || Mode_limit < 4 || Mode_Override < 4 && !(Mode_Override == 0) && !(Mode_Override > 3)) {
               XD->Copy_of_Mode_Controller_S10 = 3; 
            }
            else if (Mode_Override == 9 || Wheel_Sat > 0.0 && Enable_Momentum_Dump > 0.0) {
               XD->Copy_of_Mode_Controller_S10 = 6; 
            }
            break;
         case 6:
            if (Wheel_desaturated > 0.0 && !(Mode_Override == 9)) {
               XD->Copy_of_Mode_Controller_S10 = 5; 
            }
            else if (!(Mode_Override == 9) && !(Mode_Override == 0)) {
               XD->Copy_of_Mode_Controller_S10 = 1; 
            }
            break;
         case 7:
            if (!(Mode_Override == 6)) {
               XD->Copy_of_Mode_Controller_S10 = 2; 
            }
            break;
         case 8:
            if (!(Mode_Override == 7)) {
               XD->Copy_of_Mode_Controller_S10 = 2; 
            }
            break;
         case 9:
            if (!(Mode_Override == 8)) {
               XD->Copy_of_Mode_Controller_S10 = 2; 
            }
            break;
         default:
            break;
         }
      }
      Y->Mode_1_Idle = XD->Mode_1; 
      Y->Mode_2_Inertial_Capture = XD->Mode_2; 
      Y->Mode_3_Sun_Search = XD->Mode_3; 
      Y->Mode_4_Sun_Pointing = XD->Mode_4; 
      Y->Mode_5_Eclipse_Hold = XD->Mode_5; 
      Y->Mode_6_Quat_Hold = XD->Mode_6; 
      Y->Mode_7_Surface_Track = XD->Mode_7; 
      Y->Mode_8_Test = XD->Copy_of_Mode_Controller_S8; 
      Y->Mode_9_Momentum_Dump = XD->Copy_of_Mode_Controller_S9; 
      KState = XD->Copy_of_Mode_Controller_S10; 
      switch (KState)
      {
      case 1:
         Y->Mode_1_Idle = 1.0; 
         break;
      case 2:
         Y->Mode_2_Inertial_Capture = 1.0; 
         break;
      case 3:
         Y->Mode_3_Sun_Search = 1.0; 
         break;
      case 4:
         Y->Mode_4_Sun_Pointing = 1.0; 
         break;
      case 5:
         Y->Mode_5_Eclipse_Hold = 1.0; 
         break;
      case 6:
         Y->Mode_9_Momentum_Dump = 1.0; 
         break;
      case 7:
         Y->Mode_6_Quat_Hold = 1.0; 
         break;
      case 8:
         Y->Mode_7_Surface_Track = 1.0; 
         break;
      case 9:
         Y->Mode_8_Test = 1.0; 
         break;
      default:
         break;
      }
      /* ----------------------------  Logical Operator -- AND-OR-NOT */
      /* {Enable and Initialize..30} */
      test = Y->Mode_4_Sun_Pointing > 0.0; 
      test = test || Y->Mode_5_Eclipse_Hold > 0.0; 
      test = test || Y->Mode_6_Quat_Hold > 0.0; 
      test = test || Y->Mode_7_Surface_Track > 0.0; 
      if (test) {
         Enable_AD = 1.0; 
      }
      else {
         Enable_AD = 0.0; 
      }
      /* ----------------------------  Logical Operator -- AND-OR-NOT */
      /* {Persistance Test Up and Down..14} */
      test = Persistance_Test_Up_and_Down_13_1 > 0.0; 
      test = test && Persistance_Test_Up_and_Down_12_1 > 0.0; 
      test = test && Enable_AD > 0.0; 
      if (test) {
         True_Last_3_Samples_5 = 1.0; 
      }
      else {
         True_Last_3_Samples_5 = 0.0; 
      }
      /* ----------------------------  Summer */
      /* {Mode 3 Timer..3} */
      Reset_Trigger_1 = Y->Mode_3_Sun_Search - Delayed_Sun_not_present; 
      /* ----------------------------  Logical Operator -- AND-OR-NOT */
      /* {Persistance Test Up and Down..24} */
      test = Enable_AD > 0.0; 
      test = test || Persistance_Test_Up_and_Down_12_1 > 0.0; 
      test = test || Persistance_Test_Up_and_Down_13_1 > 0.0; 
      if (test) {
         False_Last_3_Samples = 1.0; 
      }
      else {
         False_Last_3_Samples = 0.0; 
      }
      /* ----------------------------  State Transition Diagram */
      /* {Persistance Test Up and Down.Latch 2.34} */
      XD->Latch_2_S1 = X->Latch_2_S1; 
      XD->Latch_2_S2 = X->Latch_2_S2; 
      if (!INIT) {
         KState = X->Latch_2_S2; 
         switch (KState)
         {
         case 1:
            if (True_Last_3_Samples_5 > 0.5) {
               XD->Latch_2_S2 = 2; 
            }
            break;
         case 2:
            if (False_Last_3_Samples < 0.5) {
               XD->Latch_2_S2 = 1; 
            }
            break;
         default:
            break;
         }
      }
      Y->Latch_2_1 = XD->Latch_2_S1; 
      KState = XD->Latch_2_S2; 
      switch (KState)
      {
      case 2:
         Y->Latch_2_1 = 1.0; 
         break;
      default:
         break;
      }
      /* ----------------------------  Logical Operator -- AND-OR-NOT */
      /* {Mode 3 Timer..53} */
      test = Reset_Trigger_1 > 0.0; 
      test = test || Mode_3_Timer_99_1 > 0.0; 
      if (test) {
         Mode_3_Timer_53_1 = 1.0; 
      }
      else {
         Mode_3_Timer_53_1 = 0.0; 
      }
      /* ----------------------------  Integrator */
      /* {Mode 3 Timer..2} */
      if (INIT && XREMAP) {
         X->Mode_3_Timer_2_S1 = X->Mode_3_Timer_2_S1 - TSAMP*Y->Mode_3_Sun_Search; 
      }
      if (INIT) {
         R_P[1] = Mode_3_Timer_53_1; 
      }
      ireset = 0; 
      ulast = R_P[1]; 
      R_P[1] = Mode_3_Timer_53_1; 
      if (ulast <= 0.0 && R_P[1] > 0.0) {
         ireset = 1; 
      }
      first_point = (TIME == 0.0); 
      if (ireset != 0 && !first_point) {
         if (XREMAP) {
            X->Mode_3_Timer_2_S1 = X->Mode_3_Timer_2_S1 + TSAMP*Y->Mode_3_Sun_Search; 
         }
         X->Mode_3_Timer_2_S1 = Reset_timer_to_this_value_1; 
         if (XREMAP) {
            X->Mode_3_Timer_2_S1 = X->Mode_3_Timer_2_S1 - TSAMP*Y->Mode_3_Sun_Search; 
         }
      }
      Time_since_Mode_3_Start = X->Mode_3_Timer_2_S1 + TSAMP*Y->Mode_3_Sun_Search; 
      /* ----------------------------  Logical Expression */
      /* {Mode 3 Timer..7} */
      if (Time_since_Mode_3_Start > 2.0*U->Sun_Search_Slew_Trans) {
         Mode_3_Limit_Reached = 1.0; 
      }
      else {
         Mode_3_Limit_Reached = 0.0; 
      }
      /* ----------------------------  Logical Expression */
      /* {Mode 3 Timer..5} */
      if (Time_since_Mode_3_Start > U->Sun_Search_Slew_Trans) {
         Switch_to_Yaw = 1.0; 
      }
      else {
         Switch_to_Yaw = 0.0; 
      }
      /* ----------------------------  Relational Operator -- LT-EQ-GT */
      /* {Safe Rate Check..12} */
      if (Y->Filt_Omega_B_N_B_x >= U->Rate_limit) {
         Safe_Rate_Check_12_1 = 1.0; 
      }
      else {
         Safe_Rate_Check_12_1 = 0.0; 
      }
      if (Y->Filt_Omega_B_N_B_y >= U->Rate_limit) {
         Safe_Rate_Check_12_2 = 1.0; 
      }
      else {
         Safe_Rate_Check_12_2 = 0.0; 
      }
      if (Y->Filt_Omega_B_N_B_z >= U->Rate_limit) {
         Safe_Rate_Check_12_3 = 1.0; 
      }
      else {
         Safe_Rate_Check_12_3 = 0.0; 
      }
      /* ----------------------------  Data Path Switch */
      /* {Accumulate Gyros..2} */
      if (Y->True_Last_3_Samples > 0.0) {
         Estimated_q1_B_ECI = q1_B_M_Estimated; 
         Estimated_q2_B_ECI = q2_B_M_Estimated; 
         Estimated_q3_B_ECI = q3_B_M_Estimated; 
         Estimated_q4_B_ECI = q4_B_M_Estimated; 
      }
      else {
         Estimated_q1_B_ECI = q1_B_LV_Estimated; 
         Estimated_q2_B_ECI = q2_B_LV_Estimated; 
         Estimated_q3_B_ECI = q3_B_LV_Estimated; 
         Estimated_q4_B_ECI = q4_B_LV_Estimated; 
      }
      /* ----------------------------  Summer */
      /* {Quaternion Require Continuous..15} */
      Quaternion_Require_Continuous_15_1 = Estimated_q1_B_ECI; 
      Quaternion_Require_Continuous_15_1 = Quaternion_Require_Continuous_15_1 - Quaternion_Require_Continuous_16_1; 
      Quaternion_Require_Continuous_15_2 = Estimated_q2_B_ECI; 
      Quaternion_Require_Continuous_15_2 = Quaternion_Require_Continuous_15_2 - Quaternion_Require_Continuous_16_2; 
      Quaternion_Require_Continuous_15_3 = Estimated_q3_B_ECI; 
      Quaternion_Require_Continuous_15_3 = Quaternion_Require_Continuous_15_3 - Quaternion_Require_Continuous_16_3; 
      Quaternion_Require_Continuous_15_4 = Estimated_q4_B_ECI; 
      Quaternion_Require_Continuous_15_4 = Quaternion_Require_Continuous_15_4 - Quaternion_Require_Continuous_16_4; 
      /* ----------------------------  Algebraic Expression */
      /* {Quaternion Require Continuous.Norm.14} */
      Norm_1 = Quaternion_Require_Continuous_15_1*Quaternion_Require_Continuous_15_1 + Quaternion_Require_Continuous_15_2*Quaternion_Require_Continuous_15_2 + Quaternion_Require_Continuous_15_3*Quaternion_Require_Continuous_15_3 + Quaternion_Require_Continuous_15_4*Quaternion_Require_Continuous_15_4; 
      /* ----------------------------  Square Root */
      /* {Quaternion Require Continuous..5} */
      Quaternion_Require_Continuous_5_1 = sqrt(Norm_1); 
      /* ----------------------------  Logical Expression */
      /* {Quaternion Require Continuous..13} */
      if (Quaternion_Require_Continuous_5_1 > 1.0) {
         Quaternion_Require_Continuous_13_1 = 1.0; 
      }
      else {
         Quaternion_Require_Continuous_13_1 = 0.0; 
      }
      /* ----------------------------  State Transition Diagram */
      /* {Quaternion Require Continuous.Flip Flop.99} */
      XD->Flip_Flop_S1 = X->Flip_Flop_S1; 
      XD->Flip_Flop_S2 = X->Flip_Flop_S2; 
      if (!INIT) {
         KState = X->Flip_Flop_S2; 
         switch (KState)
         {
         case 1:
            if (Quaternion_Require_Continuous_13_1 > 0.5) {
               XD->Flip_Flop_S2 = 2; 
            }
            break;
         case 2:
            if (Quaternion_Require_Continuous_13_1 > 0.5) {
               XD->Flip_Flop_S2 = 1; 
            }
            break;
         default:
            break;
         }
      }
      Flip_Flop_1 = XD->Flip_Flop_S1; 
      KState = XD->Flip_Flop_S2; 
      switch (KState)
      {
      case 2:
         Flip_Flop_1 = 1.0; 
         break;
      default:
         break;
      }
      /* ----------------------------  Gain Block */
      /* {Quaternion Require Continuous..26} */
      Quaternion_Require_Continuous_26_1 = (-1.0)*Estimated_q1_B_ECI; 
      Quaternion_Require_Continuous_26_2 = (-1.0)*Estimated_q2_B_ECI; 
      Quaternion_Require_Continuous_26_3 = (-1.0)*Estimated_q3_B_ECI; 
      Quaternion_Require_Continuous_26_4 = (-1.0)*Estimated_q4_B_ECI; 
      /* ----------------------------  Data Path Switch */
      /* {Quaternion Require Continuous..12} */
      if (Flip_Flop_1 > 0.0) {
         Y->q1 = Quaternion_Require_Continuous_26_1; 
         Y->q2 = Quaternion_Require_Continuous_26_2; 
         Y->q3 = Quaternion_Require_Continuous_26_3; 
         Y->q4 = Quaternion_Require_Continuous_26_4; 
      }
      else {
         Y->q1 = Estimated_q1_B_ECI; 
         Y->q2 = Estimated_q2_B_ECI; 
         Y->q3 = Estimated_q3_B_ECI; 
         Y->q4 = Estimated_q4_B_ECI; 
      }

      /* ----------------------------  IfThenElse */
      /* {Control Modes..29} */
      if( Enable_5 > 0.0 ) {
         
         /* ----------------------------  Procedure Super Block */
         /* {Inertial Capture.8} */
         Inertial_Capture_8_u.Filt_Omega_B_N_B_x = Y->Filt_Omega_B_N_B_x;
         Inertial_Capture_8_u.Filt_Omega_B_N_B_y = Y->Filt_Omega_B_N_B_y;
         Inertial_Capture_8_u.Filt_Omega_B_N_B_z = Y->Filt_Omega_B_N_B_z;
         Inertial_Capture(&Inertial_Capture_8_u, &Inertial_Capture_8_y);
         q1_err = Inertial_Capture_8_y.q1_err;
         q2_err = Inertial_Capture_8_y.q2_err;
         q3_err = Inertial_Capture_8_y.q3_err;
         q4_err = Inertial_Capture_8_y.q4_err;
         wx_Error = Inertial_Capture_8_y.wx_Error;
         wy_Error = Inertial_Capture_8_y.wy_Error;
         wz_Error = Inertial_Capture_8_y.wz_Error;
         Zero = Inertial_Capture_8_y.Zero;
         /* ----------------------------  Write to Variable */
         /* {Control Modes..28} */
         error_signal[0] = q1_err;
         error_signal[1] = q2_err;
         error_signal[2] = q3_err;
         error_signal[3] = q4_err;
         error_signal[4] = wx_Error;
         error_signal[5] = wy_Error;
         error_signal[6] = wz_Error;
         error_signal[7] = Zero;

      }
      else if( Mode_2_or_3_Enable > 0.0 ) {
         
         /* ----------------------------  Procedure Super Block */
         /* {Sun Search Mode.97} */
         Sun_Search_Mode_97_u.Commanded_Aqsn_Slew_Rate = U->Commanded_Aqsn_Slew_Rate;
         Sun_Search_Mode_97_u.Switch_to_Yaw = Switch_to_Yaw;
         Sun_Search_Mode_97_u.Filt_Omega_B_N_B_x = Y->Filt_Omega_B_N_B_x;
         Sun_Search_Mode_97_u.Filt_Omega_B_N_B_y = Y->Filt_Omega_B_N_B_y;
         Sun_Search_Mode_97_u.Filt_Omega_B_N_B_z = Y->Filt_Omega_B_N_B_z;
         Sun_Search_Mode(&Sun_Search_Mode_97_u, &Sun_Search_Mode_97_y);
         Sun_Search_Mode_97_1 = Sun_Search_Mode_97_y.Position_Error_1;
         Sun_Search_Mode_97_2 = Sun_Search_Mode_97_y.Position_Error_2;
         Sun_Search_Mode_97_3 = Sun_Search_Mode_97_y.Position_Error_3;
         Sun_Search_Mode_97_4 = Sun_Search_Mode_97_y.Position_Error_4;
         Sun_Search_Mode_97_5 = Sun_Search_Mode_97_y.Rate_Error_1;
         Sun_Search_Mode_97_6 = Sun_Search_Mode_97_y.Rate_Error_2;
         Sun_Search_Mode_97_7 = Sun_Search_Mode_97_y.Rate_Error_3;
         Sun_Search_Mode_97_8 = Sun_Search_Mode_97_y.Torque_Rod_Enable_1;
         /* ----------------------------  Write to Variable */
         /* {Control Modes..2} */
         error_signal[0] = Sun_Search_Mode_97_1;
         error_signal[1] = Sun_Search_Mode_97_2;
         error_signal[2] = Sun_Search_Mode_97_3;
         error_signal[3] = Sun_Search_Mode_97_4;
         error_signal[4] = Sun_Search_Mode_97_5;
         error_signal[5] = Sun_Search_Mode_97_6;
         error_signal[6] = Sun_Search_Mode_97_7;
         error_signal[7] = Sun_Search_Mode_97_8;

      }
      else if( Enable_1 > 0.0 ) {
         
         /* ----------------------------  Procedure Super Block */
         /* {Sun Pointing Mode.96} */
         Sun_Pointing_Mode_96_u.Mode_9_Momentum_Dump = U->MTR_Enable_Mode4;
         Sun_Pointing_Mode_96_u.Meas_Sun_Vector_B_x = Y->Meas_Sun_Vector_B_x;
         Sun_Pointing_Mode_96_u.Meas_Sun_Vector_B_y = Y->Meas_Sun_Vector_B_y;
         Sun_Pointing_Mode_96_u.Meas_Sun_Vector_B_z = Y->Meas_Sun_Vector_B_z;
         Sun_Pointing_Mode_96_u.Filt_Omega_B_N_B_x = Y->Filt_Omega_B_N_B_x;
         Sun_Pointing_Mode_96_u.Filt_Omega_B_N_B_y = Y->Filt_Omega_B_N_B_y;
         Sun_Pointing_Mode_96_u.Filt_Omega_B_N_B_z = Y->Filt_Omega_B_N_B_z;
         Sun_Pointing_Mode(&Sun_Pointing_Mode_96_u, &Sun_Pointing_Mode_96_y);
         q1_Bogus_Sun_N_B = Sun_Pointing_Mode_96_y.q1_Bogus_Sun_N_B;
         q2_Bogus_Sun_N_B = Sun_Pointing_Mode_96_y.q2_Bogus_Sun_N_B;
         q3_Bogus_Sun_N_B = Sun_Pointing_Mode_96_y.q3_Bogus_Sun_N_B;
         q4_Bogus_Sun_N_B = Sun_Pointing_Mode_96_y.q4_Bogus_Sun_N_B;
         Sun_Pointing_Mode_96_5 = Sun_Pointing_Mode_96_y.Sun_Pointing_Mode_98_1;
         Sun_Pointing_Mode_96_6 = Sun_Pointing_Mode_96_y.Sun_Pointing_Mode_98_2;
         Sun_Pointing_Mode_96_7 = Sun_Pointing_Mode_96_y.Sun_Pointing_Mode_98_3;
         MTR_Enable = Sun_Pointing_Mode_96_y.MTR_Enable;
         /* ----------------------------  Write to Variable */
         /* {Control Modes..4} */
         error_signal[0] = q1_Bogus_Sun_N_B;
         error_signal[1] = q2_Bogus_Sun_N_B;
         error_signal[2] = q3_Bogus_Sun_N_B;
         error_signal[3] = q4_Bogus_Sun_N_B;
         error_signal[4] = Sun_Pointing_Mode_96_5;
         error_signal[5] = Sun_Pointing_Mode_96_6;
         error_signal[6] = Sun_Pointing_Mode_96_7;
         error_signal[7] = MTR_Enable;

      }
      else if( Enable_4 > 0.0 ) {
         
         /* ----------------------------  Procedure Super Block */
         /* {Sun Lock Mode.95} */
         Sun_Lock_Mode_95_u.Estimated_q1_B_M = Y->q1;
         Sun_Lock_Mode_95_u.Estimated_q2_B_M = Y->q2;
         Sun_Lock_Mode_95_u.Estimated_q3_B_M = Y->q3;
         Sun_Lock_Mode_95_u.Estimated_q4_B_M = Y->q4;
         Sun_Lock_Mode_95_u.Filt_Omega_B_N_B_x = Y->Filt_Omega_B_N_B_x;
         Sun_Lock_Mode_95_u.Filt_Omega_B_N_B_y = Y->Filt_Omega_B_N_B_y;
         Sun_Lock_Mode_95_u.Filt_Omega_B_N_B_z = Y->Filt_Omega_B_N_B_z;
         Sun_Lock_Mode_95_u.Saved_Sun_Vector_q1 = U->Saved_Sun_Vector_q1;
         Sun_Lock_Mode_95_u.Saved_Sun_Vector_q2 = U->Saved_Sun_Vector_q2;
         Sun_Lock_Mode_95_u.Saved_Sun_Vector_q3 = U->Saved_Sun_Vector_q3;
         Sun_Lock_Mode_95_u.Saved_Sun_Vector_q4 = U->Saved_Sun_Vector_q4;
         Sun_Lock_Mode(&Sun_Lock_Mode_95_u, &Sun_Lock_Mode_95_y);
         Sun_Lock_Mode_95_1 = Sun_Lock_Mode_95_y.Quaternion_Difference_P_12_1;
         Sun_Lock_Mode_95_2 = Sun_Lock_Mode_95_y.Quaternion_Difference_P_12_2;
         Sun_Lock_Mode_95_3 = Sun_Lock_Mode_95_y.Quaternion_Difference_P_12_3;
         Sun_Lock_Mode_95_4 = Sun_Lock_Mode_95_y.Quaternion_Difference_P_12_4;
         Sun_Lock_Mode_95_5 = Sun_Lock_Mode_95_y.Flip_Omegas_1;
         Sun_Lock_Mode_95_6 = Sun_Lock_Mode_95_y.Flip_Omegas_2;
         Sun_Lock_Mode_95_7 = Sun_Lock_Mode_95_y.Flip_Omegas_3;
         Zero_1 = Sun_Lock_Mode_95_y.Zero;
         /* ----------------------------  Write to Variable */
         /* {Control Modes..5} */
         error_signal[0] = Sun_Lock_Mode_95_1;
         error_signal[1] = Sun_Lock_Mode_95_2;
         error_signal[2] = Sun_Lock_Mode_95_3;
         error_signal[3] = Sun_Lock_Mode_95_4;
         error_signal[4] = Sun_Lock_Mode_95_5;
         error_signal[5] = Sun_Lock_Mode_95_6;
         error_signal[6] = Sun_Lock_Mode_95_7;
         error_signal[7] = Zero_1;

      }
      else if( Enable_6 > 0.0 ) {
         
         /* ----------------------------  Procedure Super Block */
         /* {Mission Lock Mode.94} */
         Mission_Lock_Mode_94_u.Estimated_q1_B_M = Y->q1;
         Mission_Lock_Mode_94_u.Estimated_q2_B_M = Y->q2;
         Mission_Lock_Mode_94_u.Estimated_q3_B_M = Y->q3;
         Mission_Lock_Mode_94_u.Estimated_q4_B_M = Y->q4;
         Mission_Lock_Mode_94_u.Filt_Omega_B_N_B_x = Y->Filt_Omega_B_N_B_x;
         Mission_Lock_Mode_94_u.Filt_Omega_B_N_B_y = Y->Filt_Omega_B_N_B_y;
         Mission_Lock_Mode_94_u.Filt_Omega_B_N_B_z = Y->Filt_Omega_B_N_B_z;
         Mission_Lock_Mode_94_u.q1_M_LV = U->q1_M_LV;
         Mission_Lock_Mode_94_u.q2_M_LV = U->q2_M_LV;
         Mission_Lock_Mode_94_u.q3_M_LV = U->q3_M_LV;
         Mission_Lock_Mode_94_u.q4_M_LV = U->q4_M_LV;
         Mission_Lock_Mode(&Mission_Lock_Mode_94_u, &Mission_Lock_Mode_94_y);
         Mission_Lock_Mode_94_1 = Mission_Lock_Mode_94_y.Quaternion_Difference_P_12_1;
         Mission_Lock_Mode_94_2 = Mission_Lock_Mode_94_y.Quaternion_Difference_P_12_2;
         Mission_Lock_Mode_94_3 = Mission_Lock_Mode_94_y.Quaternion_Difference_P_12_3;
         Mission_Lock_Mode_94_4 = Mission_Lock_Mode_94_y.Quaternion_Difference_P_12_4;
         Mission_Lock_Mode_94_5 = Mission_Lock_Mode_94_y.Flip_Omegas_1;
         Mission_Lock_Mode_94_6 = Mission_Lock_Mode_94_y.Flip_Omegas_2;
         Mission_Lock_Mode_94_7 = Mission_Lock_Mode_94_y.Flip_Omegas_3;
         Zero_2 = Mission_Lock_Mode_94_y.Zero;
         /* ----------------------------  Write to Variable */
         /* {Control Modes..7} */
         error_signal[0] = Mission_Lock_Mode_94_1;
         error_signal[1] = Mission_Lock_Mode_94_2;
         error_signal[2] = Mission_Lock_Mode_94_3;
         error_signal[3] = Mission_Lock_Mode_94_4;
         error_signal[4] = Mission_Lock_Mode_94_5;
         error_signal[5] = Mission_Lock_Mode_94_6;
         error_signal[6] = Mission_Lock_Mode_94_7;
         error_signal[7] = Zero_2;

      }
      else if( Enable_2 > 0.0 ) {
         
         /* ----------------------------  Procedure Super Block */
         /* {Mission Surface Track.92} */
         Mission_Surface_Track_92_u.Greenwich_Hour_Angle = U->GHA;
         Mission_Surface_Track_92_u.Pos_ECIo_LVo_ECI_x = U->ECI_POS_X;
         Mission_Surface_Track_92_u.Pos_ECIo_LVo_ECI_y = U->ECI_POS_Y;
         Mission_Surface_Track_92_u.Pos_ECIo_LVo_ECI_z = U->ECI_POS_Z;
         Mission_Surface_Track_92_u.Sun_in_ECI_x = U->ECI_VEL_X;
         Mission_Surface_Track_92_u.Sun_in_ECI_y = U->ECI_VEL_Y;
         Mission_Surface_Track_92_u.Sun_in_ECI_z = U->ECI_VEL_Z;
         Mission_Surface_Track_92_u.Target_Longitude = U->Target_Longitude;
         Mission_Surface_Track_92_u.Target_Latitude = U->Target_Latitude;
         Mission_Surface_Track_92_u.Target_Altitude = U->Target_Altitude;
         Mission_Surface_Track_92_u.q1 = Y->q1;
         Mission_Surface_Track_92_u.q2 = Y->q2;
         Mission_Surface_Track_92_u.q3 = Y->q3;
         Mission_Surface_Track_92_u.q4 = Y->q4;
         Mission_Surface_Track_92_u.omega_x = Y->Filt_Omega_B_N_B_x;
         Mission_Surface_Track_92_u.omega_y = Y->Filt_Omega_B_N_B_y;
         Mission_Surface_Track_92_u.omega_z = Y->Filt_Omega_B_N_B_z;
         Mission_Surface_Track(&Mission_Surface_Track_92_u, &Mission_Surface_Track_92_y, &Mission_Surface_Track_92_s, &Mission_Surface_Track_92_i);
         Mission_Surface_Track_92_1 = Mission_Surface_Track_92_y.Quaternion_Difference_P_4_1;
         Mission_Surface_Track_92_2 = Mission_Surface_Track_92_y.Quaternion_Difference_P_4_2;
         Mission_Surface_Track_92_3 = Mission_Surface_Track_92_y.Quaternion_Difference_P_4_3;
         Mission_Surface_Track_92_4 = Mission_Surface_Track_92_y.Quaternion_Difference_P_4_4;
         Mission_Surface_Track_92_5 = Mission_Surface_Track_92_y.Mission_Surface_Track_26_1;
         Mission_Surface_Track_92_6 = Mission_Surface_Track_92_y.Mission_Surface_Track_26_2;
         Mission_Surface_Track_92_7 = Mission_Surface_Track_92_y.Mission_Surface_Track_26_3;
         Zero_3 = Mission_Surface_Track_92_y.Zero;
         iinfo[0] = Mission_Surface_Track_92_i.iinfo[0];
         if( iinfo[0] != 0 ) {
            Mission_Surface_Track_92_i.iinfo[0] = 0; goto EXEC_ERROR;
         }
         /* ----------------------------  Write to Variable */
         /* {Control Modes..10} */
         error_signal[0] = Mission_Surface_Track_92_1;
         error_signal[1] = Mission_Surface_Track_92_2;
         error_signal[2] = Mission_Surface_Track_92_3;
         error_signal[3] = Mission_Surface_Track_92_4;
         error_signal[4] = Mission_Surface_Track_92_5;
         error_signal[5] = Mission_Surface_Track_92_6;
         error_signal[6] = Mission_Surface_Track_92_7;
         error_signal[7] = Zero_3;

      }
      else if( Enable_7 > 0.0 ) {
         
         /* ----------------------------  Procedure Super Block */
         /* {Velocity Track.91} */
         Velocity_Track_91_u.ECI_POS_X = U->ECI_POS_X;
         Velocity_Track_91_u.ECI_POS_Y = U->ECI_POS_Y;
         Velocity_Track_91_u.ECI_POS_Z = U->ECI_POS_Z;
         Velocity_Track_91_u.ECI_VEL_X = U->ECI_VEL_X;
         Velocity_Track_91_u.ECI_VEL_Y = U->ECI_VEL_Y;
         Velocity_Track_91_u.ECI_VEL_Z = U->ECI_VEL_Z;
         Velocity_Track_91_u.Estimated_q1_B_M = Y->q1;
         Velocity_Track_91_u.Estimated_q2_B_M = Y->q2;
         Velocity_Track_91_u.Estimated_q3_B_M = Y->q3;
         Velocity_Track_91_u.Estimated_q4_B_M = Y->q4;
         Velocity_Track_91_u.Filt_Omega_B_N_B_x = Y->Filt_Omega_B_N_B_x;
         Velocity_Track_91_u.Filt_Omega_B_N_B_y = Y->Filt_Omega_B_N_B_y;
         Velocity_Track_91_u.Filt_Omega_B_N_B_z = Y->Filt_Omega_B_N_B_z;
         Velocity_Track_91_u.Enable_1 = Enable_3;
         Velocity_Track(&Velocity_Track_91_u, &Velocity_Track_91_y);
         q1_Bogus_Sun_N_B_1 = Velocity_Track_91_y.q1_Bogus_Sun_N_B;
         q2_Bogus_Sun_N_B_1 = Velocity_Track_91_y.q2_Bogus_Sun_N_B;
         q3_Bogus_Sun_N_B_1 = Velocity_Track_91_y.q3_Bogus_Sun_N_B;
         q4_Bogus_Sun_N_B_1 = Velocity_Track_91_y.q4_Bogus_Sun_N_B;
         Velocity_Track_91_5 = Velocity_Track_91_y.Flip_Omegas_1;
         Velocity_Track_91_6 = Velocity_Track_91_y.Flip_Omegas_2;
         Velocity_Track_91_7 = Velocity_Track_91_y.Flip_Omegas_3;
         MTR_Enable_1 = Velocity_Track_91_y.MTR_Enable;
         /* ----------------------------  Write to Variable */
         /* {Control Modes..15} */
         error_signal[0] = q1_Bogus_Sun_N_B_1;
         error_signal[1] = q2_Bogus_Sun_N_B_1;
         error_signal[2] = q3_Bogus_Sun_N_B_1;
         error_signal[3] = q4_Bogus_Sun_N_B_1;
         error_signal[4] = Velocity_Track_91_5;
         error_signal[5] = Velocity_Track_91_6;
         error_signal[6] = Velocity_Track_91_7;
         error_signal[7] = MTR_Enable_1;

      }
      else if( Enable_3 > 0.0 ) {
         
         /* ----------------------------  Procedure Super Block */
         /* {Sun Pointing Mode.11} */
         Sun_Pointing_Mode_11_u.Mode_9_Momentum_Dump = Y->Mode_9_Momentum_Dump;
         Sun_Pointing_Mode_11_u.Meas_Sun_Vector_B_x = Y->Meas_Sun_Vector_B_x;
         Sun_Pointing_Mode_11_u.Meas_Sun_Vector_B_y = Y->Meas_Sun_Vector_B_y;
         Sun_Pointing_Mode_11_u.Meas_Sun_Vector_B_z = Y->Meas_Sun_Vector_B_z;
         Sun_Pointing_Mode_11_u.Filt_Omega_B_N_B_x = Y->Filt_Omega_B_N_B_x;
         Sun_Pointing_Mode_11_u.Filt_Omega_B_N_B_y = Y->Filt_Omega_B_N_B_y;
         Sun_Pointing_Mode_11_u.Filt_Omega_B_N_B_z = Y->Filt_Omega_B_N_B_z;
         Sun_Pointing_Mode(&Sun_Pointing_Mode_11_u, &Sun_Pointing_Mode_11_y);
         q1_Bogus_Sun_N_B_2 = Sun_Pointing_Mode_11_y.q1_Bogus_Sun_N_B;
         q2_Bogus_Sun_N_B_2 = Sun_Pointing_Mode_11_y.q2_Bogus_Sun_N_B;
         q3_Bogus_Sun_N_B_2 = Sun_Pointing_Mode_11_y.q3_Bogus_Sun_N_B;
         q4_Bogus_Sun_N_B_2 = Sun_Pointing_Mode_11_y.q4_Bogus_Sun_N_B;
         Sun_Pointing_Mode_11_5 = Sun_Pointing_Mode_11_y.Sun_Pointing_Mode_98_1;
         Sun_Pointing_Mode_11_6 = Sun_Pointing_Mode_11_y.Sun_Pointing_Mode_98_2;
         Sun_Pointing_Mode_11_7 = Sun_Pointing_Mode_11_y.Sun_Pointing_Mode_98_3;
         MTR_Enable_2 = Sun_Pointing_Mode_11_y.MTR_Enable;
         /* ----------------------------  Write to Variable */
         /* {Control Modes..16} */
         error_signal[0] = q1_Bogus_Sun_N_B_2;
         error_signal[1] = q2_Bogus_Sun_N_B_2;
         error_signal[2] = q3_Bogus_Sun_N_B_2;
         error_signal[3] = q4_Bogus_Sun_N_B_2;
         error_signal[4] = Sun_Pointing_Mode_11_5;
         error_signal[5] = Sun_Pointing_Mode_11_6;
         error_signal[6] = Sun_Pointing_Mode_11_7;
         error_signal[7] = MTR_Enable_2;

      }
      else {
         
         /* ----------------------------  Procedure Super Block */
         /* {Idle Mode.88} */
         Idle_Mode(&Idle_Mode_88_y);
         Idle_Mode_88_1 = Idle_Mode_88_y.Zero_outputs_1;
         Idle_Mode_88_2 = Idle_Mode_88_y.Zero_outputs_2;
         Idle_Mode_88_3 = Idle_Mode_88_y.Zero_outputs_3;
         Idle_Mode_88_4 = Idle_Mode_88_y.Zero_outputs_4;
         Idle_Mode_88_5 = Idle_Mode_88_y.Zero_outputs_5;
         Idle_Mode_88_6 = Idle_Mode_88_y.Zero_outputs_6;
         Idle_Mode_88_7 = Idle_Mode_88_y.Zero_outputs_7;
         Idle_Mode_88_8 = Idle_Mode_88_y.Zero_outputs_1_1;
         /* ----------------------------  Write to Variable */
         /* {Control Modes..22} */
         error_signal[0] = Idle_Mode_88_1;
         error_signal[1] = Idle_Mode_88_2;
         error_signal[2] = Idle_Mode_88_3;
         error_signal[3] = Idle_Mode_88_4;
         error_signal[4] = Idle_Mode_88_5;
         error_signal[5] = Idle_Mode_88_6;
         error_signal[6] = Idle_Mode_88_7;
         error_signal[7] = Idle_Mode_88_8;

      }
      /* ----------------------------  Read from Variable */
      /* {Control Modes..98} */
      Y->q1_err = error_signal[0];
      Y->q2_err = error_signal[1];
      Y->q3_err = error_signal[2];
      Y->q4_err = error_signal[3];
      Y->wx_err = error_signal[4];
      Y->wy_err = error_signal[5];
      Y->wz_err = error_signal[6];
      Y->Torque_Rod_Enable = error_signal[7];
      /* ----------------------------  Saturation */
      /* {Attitude Control..1} */
      Attitude_Control_1_1 = MIN(MAX(-1.0,Y->q4_err),1.0); 
      /* ----------------------------  Algebraic Expression */
      /* {Safe Q Check.Norm.2} */
      Norm_1_1 = Y->q1_err*Y->q1_err + Y->q2_err*Y->q2_err + Y->q3_err*Y->q3_err; 
      /* ----------------------------  Algebraic Expression */
      /* {Sun Line Error.Approx Angular Sun Line Error.2} */
      Approx_Angular_Sun_Line_Error_1 = Y->q1_err*Y->q1_err + Y->q2_err*Y->q2_err; 
      /* ----------------------------  Square Root */
      /* {Sun Line Error..10} */
      Sun_Line_Error_10_1 = sqrt(Approx_Angular_Sun_Line_Error_1); 
      /* ----------------------------  Algebraic Expression */
      /* {Sun Line Error..96} */
      Sun_Line_Error_96_1 = Sun_Line_Error_10_1*2.0; 
      /* ----------------------------  Square Root */
      /* {Safe Q Check..1} */
      Safe_Q_Check_1_1 = sqrt(Norm_1_1); 
      /* ----------------------------  Saturation */
      /* {Safe Q Check..34} */
      Safe_Q_Check_34_1 = MIN(MAX(-1.0,Safe_Q_Check_1_1),1.0); 
      /* ----------------------------  Arcsine( u ) */
      /* {Safe Q Check..4} */
      Safe_Q_Check_4_1 = asin(Safe_Q_Check_34_1); 
      /* ----------------------------  Gain Block */
      /* {Safe Q Check..18} */
      Mission_Pointing_Error_3_Axis = 2.0*Safe_Q_Check_4_1; 
      /* ----------------------------  Relational Operator -- LT-EQ-GT */
      /* {Safe Q Check..14} */
      if (Mission_Pointing_Error_3_Axis >= U->Q_limit) {
         Safe_Q_Check_14_1 = 1.0; 
      }
      else {
         Safe_Q_Check_14_1 = 0.0; 
      }
      /* ----------------------------  Absolute Value */
      /* {Attitude Control..98} */
      Attitude_Control_98_1 = fabs(Attitude_Control_1_1); 
      /* ----------------------------  Arccosine( u ) */
      /* {Attitude Control..8} */
      Attitude_Control_8_1 = acos(Attitude_Control_98_1); 
      /* ----------------------------  Gain Block */
      /* {Attitude Control..2} */
      SCALAR_MAG = 2.0*Attitude_Control_8_1; 
      /* ----------------------------  Logical Expression */
      /* {Attitude Control.Switch to PD for Small Angles.12} */
      if (SCALAR_MAG < U->Slew_to_PD_Trans) {
         Slew_Inactive = 1.0; 
      }
      else {
         Slew_Inactive = 0.0; 
      }
      /* ----------------------------  Procedure Super Block */
      /* {Toggle Down To Pulse.94} */
      Toggle_Down_To_Pulse_94_u.overwritepulse = U->overwritepulse;
      Toggle_Down_To_Pulse(&Toggle_Down_To_Pulse_94_u, &Toggle_Down_To_Pulse_94_y, &Toggle_Down_To_Pulse_94_s, INIT);
      Toggle_Down_To_Pulse_94_s.Toggle_Down_To_Pulse_x = 1 - Toggle_Down_To_Pulse_94_s.Toggle_Down_To_Pulse_x;
      Toggle_Down_To_Pulse_94_1 = Toggle_Down_To_Pulse_94_y.Toggle_Down_To_Pulse_13_1;
      /* ----------------------------  Data Path Switch */
      /* {Parameters 6..12} */
      if (U->Deploy > 0.0) {
         Parameters_6_12_1 = U->Ixx_Depl; 
         Parameters_6_12_2 = U->Ixy_Depl; 
         Parameters_6_12_3 = U->Ixz_Depl; 
         Parameters_6_12_4 = U->Iyy_Depl; 
         Parameters_6_12_5 = U->Iyz_Depl; 
         Parameters_6_12_6 = U->Izz_Depl; 
      }
      else {
         Parameters_6_12_1 = U->Ixx; 
         Parameters_6_12_2 = U->Ixy; 
         Parameters_6_12_3 = U->Ixz; 
         Parameters_6_12_4 = U->Iyy; 
         Parameters_6_12_5 = U->Iyz; 
         Parameters_6_12_6 = U->Izz; 
      }
      /* ----------------------------  Algebraic Expression */
      /* {Calc Body Momentum..97} */
      Y->Body_Hx_ECI_Frame = Y->Filt_Omega_B_N_B_x*Parameters_6_12_1; 
      Y->Body_Hy_ECI_Frame = Y->Filt_Omega_B_N_B_y*Parameters_6_12_4; 
      Y->Body_Hz_ECI_Frame = Y->Filt_Omega_B_N_B_z*Parameters_6_12_6; 
      /* ----------------------------  Data Path Switch */
      /* {Parameters 6..12} */
      if (U->Deploy > 0.0) {
         Parameters_6_12_1_1 = U->Ixx_Depl; 
         Parameters_6_12_2_1 = U->Ixy_Depl; 
         Parameters_6_12_3_1 = U->Ixz_Depl; 
         Parameters_6_12_4_1 = U->Iyy_Depl; 
         Parameters_6_12_5_1 = U->Iyz_Depl; 
         Parameters_6_12_6_1 = U->Izz_Depl; 
      }
      else {
         Parameters_6_12_1_1 = U->Ixx; 
         Parameters_6_12_2_1 = U->Ixy; 
         Parameters_6_12_3_1 = U->Ixz; 
         Parameters_6_12_4_1 = U->Iyy; 
         Parameters_6_12_5_1 = U->Iyz; 
         Parameters_6_12_6_1 = U->Izz; 
      }
      /* ----------------------------  Algebraic Expression */
      /* {Env IC..94} */
      Zero_4 = 0.0; 
      /* ----------------------------  Logical Operator -- AND-OR-NOT */
      /* {FSW..96} */
      test = Toggle_Down_To_Pulse_94_1 > 0.0; 
      test = test && U->Gyro_AutoCalibrate > 0.0; 
      if (test) {
         FSW_96_1 = 1.0; 
      }
      else {
         FSW_96_1 = 0.0; 
      }

      /* ----------------------------  IfThenElse */
      /* {FSW..97} */
      if( FSW_96_1 > 0.0 ) {
         
         /* ----------------------------  Summer */
         /* {FSW..93} */
         Y->FSW_93_1 = U->Gyro_Drift_x + U->gyro_offset_x; 
         Y->FSW_93_2 = U->Gyro_Drift_y + U->gyro_offset_y; 
         FSW_93_3 = U->Gyro_Drift_z + U->gyro_offset_z; 

      }
      /* ----------------------------  Algebraic Expression */
      /* {Attitude Control..6} */
      Attitude_Control_6_1 = 0.0; 
      Attitude_Control_6_2 = 0.0; 
      Attitude_Control_6_3 = 0.0; 
      /* ----------------------------  Algebraic Expression */
      /* {Attitude Control..10} */
      Attitude_Control_10_1 = Y->q1_err*Y->q1_err + Y->q2_err*Y->q2_err + Y->q3_err*Y->q3_err; 
      /* ----------------------------  Square Root */
      /* {Attitude Control..21} */
      Attitude_Control_21_1 = sqrt(Attitude_Control_10_1); 
      /* ----------------------------  Algebraic Expression */
      /* {Attitude Control.Euler Axis Rate Controlled Slew.7} */
      Euler_Axis_Rate_Controlled_Slew_1 = Attitude_Control_21_1 + 1.0E-010; 
      Euler_Axis_Rate_Controlled_Slew_2 = Y->q1_err/Euler_Axis_Rate_Controlled_Slew_1; 
      Euler_Axis_Rate_Controlled_Slew_3 = Y->q2_err/Euler_Axis_Rate_Controlled_Slew_1; 
      Euler_Axis_Rate_Controlled_Slew_4 = Y->q3_err/Euler_Axis_Rate_Controlled_Slew_1; 
      /* ----------------------------  ElementProduct */
      /* {Attitude Control.Commanded Slew Rate.34} */
      Commanded_Slew_Rate_1 = Euler_Axis_Rate_Controlled_Slew_2*U->Commanded_Aqsn_Slew_Rate; 
      Commanded_Slew_Rate_2 = Euler_Axis_Rate_Controlled_Slew_3*U->Commanded_Aqsn_Slew_Rate; 
      Commanded_Slew_Rate_3 = Euler_Axis_Rate_Controlled_Slew_4*U->Commanded_Aqsn_Slew_Rate; 
      /* ----------------------------  Summer */
      /* {Attitude Control..9} */
      Attitude_Control_9_1 = Y->wx_err + Commanded_Slew_Rate_1; 
      Attitude_Control_9_2 = Y->wy_err + Commanded_Slew_Rate_2; 
      Attitude_Control_9_3 = Y->wz_err + Commanded_Slew_Rate_3; 
      /* ----------------------------  Gain Block */
      /* {Attitude Control..99} */
      Attitude_Control_99_1 = Y->wx_err; 
      Attitude_Control_99_2 = Y->wy_err; 
      Attitude_Control_99_3 = Y->wz_err; 
      /* ----------------------------  Gain Block */
      /* {Attitude Control.Small Euler Angles.11} */
      Small_Euler_Angles_1 = Y->q1_err; 
      Small_Euler_Angles_2 = Y->q2_err; 
      Small_Euler_Angles_3 = Y->q3_err; 
      /* ----------------------------  Data Path Switch */
      /* {Attitude Control.PD or Rate Control.3} */
      if (Slew_Inactive > 0.0) {
         PD_or_Rate_Control_1 = Small_Euler_Angles_1; 
         PD_or_Rate_Control_2 = Small_Euler_Angles_2; 
         PD_or_Rate_Control_3 = Small_Euler_Angles_3; 
         PD_or_Rate_Control_4 = Attitude_Control_99_1; 
         PD_or_Rate_Control_5 = Attitude_Control_99_2; 
         PD_or_Rate_Control_6 = Attitude_Control_99_3; 
      }
      else {
         PD_or_Rate_Control_1 = Attitude_Control_6_1; 
         PD_or_Rate_Control_2 = Attitude_Control_6_2; 
         PD_or_Rate_Control_3 = Attitude_Control_6_3; 
         PD_or_Rate_Control_4 = Attitude_Control_9_1; 
         PD_or_Rate_Control_5 = Attitude_Control_9_2; 
         PD_or_Rate_Control_6 = Attitude_Control_9_3; 
      }
      /* ----------------------------  ElementProduct */
      /* {Attitude Control.Position Gain.24} */
      Position_Gain_1 = PD_or_Rate_Control_1*U->Kpx; 
      Position_Gain_2 = PD_or_Rate_Control_2*U->Kpy; 
      Position_Gain_3 = PD_or_Rate_Control_3*U->Kpz; 
      /* ----------------------------  ElementProduct */
      /* {Attitude Control.Rate Gain.16} */
      Rate_Gain_1 = U->Krx*PD_or_Rate_Control_4; 
      Rate_Gain_2 = U->Kry*PD_or_Rate_Control_5; 
      Rate_Gain_3 = U->Krz*PD_or_Rate_Control_6; 
      /* ----------------------------  Summer */
      /* {Attitude Control.Sum.26} */
      Requested_Alpha_B_X_B_x = Position_Gain_1 + Rate_Gain_1; 
      Requested_Alpha_B_X_B_y = Position_Gain_2 + Rate_Gain_2; 
      Requested_Alpha_B_X_B_z = Position_Gain_3 + Rate_Gain_3; 
      /* ----------------------------  Data Path Switch */
      /* {Parameters 6..12} */
      if (U->Deploy > 0.0) {
         Parameters_6_12_1_2 = U->Ixx_Depl; 
         Parameters_6_12_2_2 = U->Ixy_Depl; 
         Parameters_6_12_3_2 = U->Ixz_Depl; 
         Parameters_6_12_4_2 = U->Iyy_Depl; 
         Parameters_6_12_5_2 = U->Iyz_Depl; 
         Parameters_6_12_6_2 = U->Izz_Depl; 
      }
      else {
         Parameters_6_12_1_2 = U->Ixx; 
         Parameters_6_12_2_2 = U->Ixy; 
         Parameters_6_12_3_2 = U->Ixz; 
         Parameters_6_12_4_2 = U->Iyy; 
         Parameters_6_12_5_2 = U->Iyz; 
         Parameters_6_12_6_2 = U->Izz; 
      }
      /* ----------------------------  Summer */
      /* {Safe Wheel Saturation Check..99} */
      Y->Total_Hx = Y->Body_Hx_ECI_Frame + Y->Wheel_Hx_Body_Frame; 
      Y->Total_Hy = Y->Body_Hy_ECI_Frame + Y->Wheel_Hy_Body_Frame; 
      Y->Total_Hz = Y->Body_Hz_ECI_Frame + Y->Wheel_Hz_Body_Frame; 
      /* ----------------------------  Cross Product */
      /* {Attitude Control..17} */
      Attitude_Control_17_1 = Y->Filt_Omega_B_N_B_y*Y->Total_Hz - Y->Filt_Omega_B_N_B_z*Y->Total_Hy; 
      Attitude_Control_17_2 = Y->Filt_Omega_B_N_B_z*Y->Total_Hx - Y->Filt_Omega_B_N_B_x*Y->Total_Hz; 
      Attitude_Control_17_3 = Y->Filt_Omega_B_N_B_x*Y->Total_Hy - Y->Filt_Omega_B_N_B_y*Y->Total_Hx; 
      /* ----------------------------  Algebraic Expression */
      /* {Attitude Control.Inertia Matrix Times Angular Acc.37} */
      Requested_Torque_x = Requested_Alpha_B_X_B_x*Parameters_6_12_1_2 + Requested_Alpha_B_X_B_y*Parameters_6_12_2_2 + Requested_Alpha_B_X_B_z*Parameters_6_12_3_2; 
      Requested_Torque_y = Requested_Alpha_B_X_B_x*Parameters_6_12_2_2 + Requested_Alpha_B_X_B_y*Parameters_6_12_4_2 + Requested_Alpha_B_X_B_z*Parameters_6_12_5_2; 
      Requested_Torque_z = Requested_Alpha_B_X_B_x*Parameters_6_12_3_2 + Requested_Alpha_B_X_B_y*Parameters_6_12_5_2 + Requested_Alpha_B_X_B_z*Parameters_6_12_6_2; 
      /* ----------------------------  ElementProduct */
      /* {Attitude Control.Momentum Gain.5} */
      Mom_torque_x = U->kMOMx*Attitude_Control_17_1; 
      Mom_torque_y = U->kMOMy*Attitude_Control_17_2; 
      Mom_torque_z = U->kMOMz*Attitude_Control_17_3; 
      /* ----------------------------  Summer */
      /* {Attitude Control..14} */
      Y->Requested_Torque_B_x = Requested_Torque_x + Mom_torque_x; 
      Y->Requested_Torque_B_y = Requested_Torque_y + Mom_torque_y; 
      Y->Requested_Torque_B_z = Requested_Torque_z + Mom_torque_z; 
      /* ----------------------------  Procedure Super Block */
      /* {mth vect norm(Normalize Position).96} */
      mth_vect_norm_96_u.Measured_magnetometer_body_x = U->ECI_POS_X;
      mth_vect_norm_96_u.Measured_magnetometer_body_y = U->ECI_POS_Y;
      mth_vect_norm_96_u.Measured_magnetometer_body_z = U->ECI_POS_Z;
      mth_vect_norm(&mth_vect_norm_96_u, &mth_vect_norm_96_y);
      X_2 = mth_vect_norm_96_y.X_1;
      Y_2 = mth_vect_norm_96_y.Y_1;
      Z_1 = mth_vect_norm_96_y.Z;
      Magnitude_1 = mth_vect_norm_96_y.Magnitude;
      /* ----------------------------  Gain Block */
      /* {Attitude Determination..97} */
      Earth_Nadir_in_ECI_x = (-1.0)*X_2; 
      Earth_Nadir_in_ECI_y = (-1.0)*Y_2; 
      Earth_Nadir_in_ECI_z = (-1.0)*Z_1; 
      /* ----------------------------  Algebraic Expression */
      /* {Quaternion Transformation.Transform.13} */
      Body_x = (Y->q4*Y->q4 + Y->q1*Y->q1 - 0.5)*Earth_Nadir_in_ECI_x*2.0 + (Y->q1*Y->q2 + Y->q4*Y->q3)*Earth_Nadir_in_ECI_y*2.0 + (Y->q1*Y->q3 - Y->q4*Y->q2)*Earth_Nadir_in_ECI_z*2.0; 
      Body_y = (Y->q1*Y->q2 - Y->q4*Y->q3)*Earth_Nadir_in_ECI_x*2.0 + (Y->q4*Y->q4 + Y->q2*Y->q2 - 0.5)*Earth_Nadir_in_ECI_y*2.0 + (Y->q2*Y->q3 + Y->q4*Y->q1)*Earth_Nadir_in_ECI_z*2.0; 
      Body_z = (Y->q1*Y->q3 + Y->q4*Y->q2)*Earth_Nadir_in_ECI_x*2.0 + (Y->q2*Y->q3 - Y->q4*Y->q1)*Earth_Nadir_in_ECI_y*2.0 + (Y->q4*Y->q4 + Y->q3*Y->q3 - 0.5)*Earth_Nadir_in_ECI_z*2.0; 
      /* ----------------------------  Algebraic Expression */
      /* {Attitude Determination..99} */
      Zero_5 = 0.0; 
      /* ----------------------------  Gain Block */
      /* {Attitude Determination.Name Change.16} */
      Y->nadir_vect_x = Body_x; 
      Y->nadir_vect_y = Body_y; 
      Y->nadir_vect_z = Body_z; 
      /* ----------------------------  Summer */
      /* {Toggle 2 Pulse..31} */
      Toggle_2_Pulse_31_1 = Y->Mode_5_Eclipse_Hold - Toggle_2_Pulse_20_1; 
      /* ----------------------------  Absolute Value */
      /* {Toggle 2 Pulse..21} */
      Toggle_2_Pulse_21_1 = fabs(Toggle_2_Pulse_31_1); 
      /* ----------------------------  Logical Operator -- AND-OR-NOT */
      /* {Enable and Initialize..2} */
      test = Toggle_2_Pulse_21_1 > 0.0; 
      test = test && Enable_and_Initialize_96_1 > 0.0; 
      if (test) {
         Y->Save_Sun_Quaternion = 1.0; 
      }
      else {
         Y->Save_Sun_Quaternion = 0.0; 
      }
      /* ----------------------------  Summer */
      /* {Toggle 2 Pulse 5..31} */
      Toggle_2_Pulse_5_31_1 = Y->Mode_4_Sun_Pointing - Toggle_2_Pulse_5_20_1; 
      Toggle_2_Pulse_5_31_2 = Y->Mode_5_Eclipse_Hold - Toggle_2_Pulse_5_20_2; 
      Toggle_2_Pulse_5_31_3 = Y->Mode_6_Quat_Hold - Toggle_2_Pulse_5_20_3; 
      Toggle_2_Pulse_5_31_4 = Y->Mode_7_Surface_Track - Toggle_2_Pulse_5_20_4; 
      /* ----------------------------  Absolute Value */
      /* {Toggle 2 Pulse 5..21} */
      Toggle_2_Pulse_5_21_1 = fabs(Toggle_2_Pulse_5_31_1); 
      Toggle_2_Pulse_5_21_2 = fabs(Toggle_2_Pulse_5_31_2); 
      Toggle_2_Pulse_5_21_3 = fabs(Toggle_2_Pulse_5_31_3); 
      Toggle_2_Pulse_5_21_4 = fabs(Toggle_2_Pulse_5_31_4); 
      /* ----------------------------  Summer */
      /* {Toggle 2 Pulse 5..32} */
      Toggle_2_Pulse_5_32_1 = Toggle_2_Pulse_5_21_1 + Toggle_2_Pulse_5_21_2; 
      Toggle_2_Pulse_5_32_1 = Toggle_2_Pulse_5_32_1 + Toggle_2_Pulse_5_21_3; 
      Toggle_2_Pulse_5_32_1 = Toggle_2_Pulse_5_32_1 + Toggle_2_Pulse_5_21_4; 
      /* ----------------------------  L - U  Bounded Limit (Limiter) */
      /* {Toggle 2 Pulse 5..23} */
      Toggle_2_Pulse_5_23_1 = MIN(MAX(0.0,Toggle_2_Pulse_5_32_1),1.0); 
      /* ----------------------------  Summer */
      /* {Toggle 2 Pulse..31} */
      Toggle_2_Pulse_31_1_1 = Reset_Estimator - Toggle_2_Pulse_20_1_1; 
      /* ----------------------------  Absolute Value */
      /* {Toggle 2 Pulse..21} */
      Toggle_2_Pulse_21_1_1 = fabs(Toggle_2_Pulse_31_1_1); 

      /* ----------------------------  IfThenElse */
      /* {Enable and Initialize..97} */
      if( Y->Save_Sun_Quaternion > 0.0 ) {
         

      }
      /* ----------------------------  Logical Operator -- AND-OR-NOT */
      /* {Enable and Initialize..40} */
      test = Toggle_2_Pulse_21_1_1 > 0.0; 
      test = test || Toggle_2_Pulse_5_23_1 > 0.0; 
      if (test) {
         Y->Reset_Estimator = 1.0; 
      }
      else {
         Y->Reset_Estimator = 0.0; 
      }
      /* ----------------------------  SignalType Conversion */
      /* {Parameters 24 Wheels.Avoid Epsilon Bug.6} */
      Wheel_Enable = ROUND(U->Wheel_Enable); 
      /* ----------------------------  ElementProduct */
      /* {Reaction Wheel Control..11} */
      Requested_Torque_x_1 = Wheel_Enable*Y->Requested_Torque_B_x; 
      Requested_Torque_y_1 = Wheel_Enable*Y->Requested_Torque_B_y; 
      Requested_Torque_z_1 = Wheel_Enable*Y->Requested_Torque_B_z; 
      /* ----------------------------  SignalType Conversion */
      /* {Parameters 24 Wheels.Avoid Epsilon Bug.1} */
      Wheel_Disable_Mode_5 = ROUND(U->Wheel_Disable_Mode5); 
      /* ----------------------------  SignalType Conversion */
      /* {Reaction Wheel Control.Avoid Epsilon Bug.3} */
      Wheel_Disable_Mode_5_1 = ROUND(Wheel_Disable_Mode_5); 
      Mode_5_On = ROUND(Y->Mode_5_Eclipse_Hold); 
      /* ----------------------------  Logical Operator -- AND-OR-NOT */
      /* {Reaction Wheel Control..98} */
      test = Wheel_Disable_Mode_5_1 > 0.0; 
      test = test && Mode_5_On > 0.0; 
      if (test) {
         Reaction_Wheel_Control_98_1 = 1.0; 
      }
      else {
         Reaction_Wheel_Control_98_1 = 0.0; 
      }
      /* ----------------------------  Logical Operator -- AND-OR-NOT */
      /* {Reaction Wheel Control..2} */
      if ((Reaction_Wheel_Control_98_1 > 0.0)) {
         Y->Mode_5_Wheel_Enable = 0.0; 
      }
      else {
         Y->Mode_5_Wheel_Enable = 1.0; 
      }
      /* ----------------------------  ElementProduct */
      /* {Reaction Wheel Control..1} */
      Requested_Torque_x_2 = Y->Mode_5_Wheel_Enable*Requested_Torque_x_1; 
      Requested_Torque_y_2 = Y->Mode_5_Wheel_Enable*Requested_Torque_y_1; 
      Requested_Torque_z_2 = Y->Mode_5_Wheel_Enable*Requested_Torque_z_1; 
      /* ----------------------------  SignalType Conversion */
      /* {Reaction Wheel Control..96} */
      Reaction_Wheel_Control_96_1 = ROUND(Y->Mode_2_Inertial_Capture); 
      /* ----------------------------  Logical Operator -- AND-OR-NOT */
      /* {Reaction Wheel Control..97} */
      if ((Reaction_Wheel_Control_96_1 > 0.0)) {
         Reaction_Wheel_Control_97_1 = 0.0; 
      }
      else {
         Reaction_Wheel_Control_97_1 = 1.0; 
      }
      /* ----------------------------  ElementProduct */
      /* {Reaction Wheel Control..4} */
      Requested_Torque_x_3 = Requested_Torque_x_2*Reaction_Wheel_Control_97_1; 
      Requested_Torque_y_3 = Requested_Torque_y_2*Reaction_Wheel_Control_97_1; 
      Requested_Torque_z_3 = Requested_Torque_z_2*Reaction_Wheel_Control_97_1; 
      /* ----------------------------  Saturation */
      /* {Reaction Wheel Control..99} */
      Cmded_Body_Torque_x = MIN(MAX(-0.0002,Requested_Torque_x_3),0.0002); 
      Cmded_Body_Torque_y = MIN(MAX(-0.0002,Requested_Torque_y_3),0.0002); 
      Cmded_Body_Torque_z = MIN(MAX(-0.0002,Requested_Torque_z_3),0.0002); 
      /* ----------------------------  Gain Block */
      /* {Reaction Wheel Control..95} */
      Y->Cmded_Whl_Torque_x = (-1.0)*Cmded_Body_Torque_x; 
      Y->Cmded_Whl_Torque_y = (-1.0)*Cmded_Body_Torque_y; 
      Y->Cmded_Whl_Torque_z = (-1.0)*Cmded_Body_Torque_z; 
      /* ----------------------------  Gain Block */
      /* {Torque Rod Control.Find Error.1} */
      Momentum_Error_X_Nms = (-1.0)*Y->Wheel_Hx_Body_Frame; 
      Momentum_Error_Y_Nms = (-1.0)*Y->Wheel_Hy_Body_Frame; 
      Momentum_Error_Z_Nms = (-1.0)*Y->Wheel_Hz_Body_Frame; 
      /* ----------------------------  Procedure Super Block */
      /* {mth vect norm(Normalize).13} */
      mth_vect_norm_13_u.Measured_magnetometer_body_x = Y->meas_mag_body_x;
      mth_vect_norm_13_u.Measured_magnetometer_body_y = Y->meas_mag_body_y;
      mth_vect_norm_13_u.Measured_magnetometer_body_z = Y->meas_mag_body_z;
      mth_vect_norm(&mth_vect_norm_13_u, &mth_vect_norm_13_y);
      X_3 = mth_vect_norm_13_y.X_1;
      Y_3 = mth_vect_norm_13_y.Y_1;
      Z_2 = mth_vect_norm_13_y.Z;
      Magnitude_2 = mth_vect_norm_13_y.Magnitude;
      /* ----------------------------  Cross Product */
      /* {Torque Rod Control..4} */
      Mx = Y->meas_mag_body_y*Momentum_Error_Z_Nms - Y->meas_mag_body_z*Momentum_Error_Y_Nms; 
      My = Y->meas_mag_body_z*Momentum_Error_X_Nms - Y->meas_mag_body_x*Momentum_Error_Z_Nms; 
      Mz = Y->meas_mag_body_x*Momentum_Error_Y_Nms - Y->meas_mag_body_y*Momentum_Error_X_Nms; 
      /* ----------------------------  Algebraic Expression */
      /* {Torque Rod Control..5} */
      Torque_Rod_Control_5_1 = 1.0/(Magnitude_2*Magnitude_2)*Mx; 
      Torque_Rod_Control_5_2 = 1.0/(Magnitude_2*Magnitude_2)*My; 
      Torque_Rod_Control_5_3 = 1.0/(Magnitude_2*Magnitude_2)*Mz; 
      /* ----------------------------  Gain Block */
      /* {Torque Rod Control..6} */
      Requested_Dipole_X = Torque_Rod_Control_5_1; 
      Requested_Dipole_Y = Torque_Rod_Control_5_2; 
      Requested_Dipole_Z = Torque_Rod_Control_5_3; 
      /* ----------------------------  Gain Block */
      /* {Torque Rod Control.Am2 to Counts.17} */
      Cmd_MTR_x_counts = 3367.0033670033699*Requested_Dipole_X; 
      Cmd_MTR_y_counts = 3367.0033670033699*Requested_Dipole_Y; 
      Cmd_MTR_z_counts = 3367.0033670033672*Requested_Dipole_Z; 
      /* ----------------------------  Algebraic Expression */
      /* {Torque Rod Control.Zero.99} */
      Zero_1_1 = 0.0; 
      /* ----------------------------  Absolute Value */
      /* {Torque Rod Control..25} */
      Torque_Rod_Control_25_1 = fabs(Cmd_MTR_x_counts); 
      Torque_Rod_Control_25_2 = fabs(Cmd_MTR_y_counts); 
      Torque_Rod_Control_25_3 = fabs(Cmd_MTR_z_counts); 
      /* ----------------------------  Relational Operator -- LT-EQ-GT */
      /* {Torque Rod Control..26} */
      if (Torque_Rod_Control_25_1 > Torque_Rod_Control_25_2) {
         Torque_Rod_Control_26_1 = 1.0; 
      }
      else {
         Torque_Rod_Control_26_1 = 0.0; 
      }
      /* ----------------------------  Data Path Switch */
      /* {Torque Rod Control..16} */
      if (Torque_Rod_Control_26_1 > 0.0) {
         Torque_Rod_Control_16_1 = Torque_Rod_Control_25_1; 
      }
      else {
         Torque_Rod_Control_16_1 = Torque_Rod_Control_25_2; 
      }
      /* ----------------------------  Relational Operator -- LT-EQ-GT */
      /* {Torque Rod Control..2} */
      if (Torque_Rod_Control_16_1 > Torque_Rod_Control_25_3) {
         Torque_Rod_Control_2_1 = 1.0; 
      }
      else {
         Torque_Rod_Control_2_1 = 0.0; 
      }
      /* ----------------------------  Data Path Switch */
      /* {Torque Rod Control..10} */
      if (Torque_Rod_Control_2_1 > 0.0) {
         Torque_Rod_Control_10_1 = Torque_Rod_Control_16_1; 
      }
      else {
         Torque_Rod_Control_10_1 = Torque_Rod_Control_25_3; 
      }
      /* ----------------------------  Algebraic Expression */
      /* {Torque Rod Control..58} */
      Scaling_Factor = 127.0/(Torque_Rod_Control_10_1 + 1.0E-9); 
      /* ----------------------------  Algebraic Expression */
      /* {Torque Rod Control..8} */
      Scaled_MTR_Count_X = Cmd_MTR_x_counts*Scaling_Factor; 
      Scaled_MTR_Count_Y = Cmd_MTR_y_counts*Scaling_Factor; 
      Scaled_MTR_Count_Z = Cmd_MTR_z_counts*Scaling_Factor; 
      /* ----------------------------  Gain Block */
      /* {Torque Rod Control.Counts to Am2.11} */
      Cmd_MTR_Am2_X = 0.000297*Scaled_MTR_Count_X; 
      Cmd_MTR_Am2_Y = 0.000297*Scaled_MTR_Count_Y; 
      Cmd_MTR_Am2_Z = 0.000297*Scaled_MTR_Count_Z; 
      /* ----------------------------  SignalType Conversion */
      /* {Torque Rod Control..97} */
      Torque_Rod_Control_97_1 = ROUND(Y->Torque_Rod_Enable); 
      Torque_Rod_Control_97_2 = ROUND(Y->Mode_9_Momentum_Dump); 
      /* ----------------------------  Logical Operator -- AND-OR-NOT */
      /* {Torque Rod Control..98} */
      test = Torque_Rod_Control_97_1 > 0.0; 
      test = test && Torque_Rod_Control_97_2 > 0.0; 
      if (test) {
         Torque_Rod_Control_98_1 = 1.0; 
      }
      else {
         Torque_Rod_Control_98_1 = 0.0; 
      }
      /* ----------------------------  Data Path Switch */
      /* {Torque Rod Control..7} */
      if (Torque_Rod_Control_98_1 > 0.0) {
         Cmd_Dipole_X = Requested_Dipole_X; 
         Cmd_Dipole_Y = Requested_Dipole_Y; 
         Cmd_Dipole_Z = Requested_Dipole_Z; 
         Cmd_MTR_counts_X = Cmd_MTR_Am2_X; 
         Cmd_MTR_counts_Y = Cmd_MTR_Am2_Y; 
         Cmd_MTR_counts_Z = Cmd_MTR_Am2_Z; 
      }
      else {
         Cmd_Dipole_X = Zero_1_1; 
         Cmd_Dipole_Y = Zero_1_1; 
         Cmd_Dipole_Z = Zero_1_1; 
         Cmd_MTR_counts_X = Zero_1_1; 
         Cmd_MTR_counts_Y = Zero_1_1; 
         Cmd_MTR_counts_Z = Zero_1_1; 
      }
      /* ----------------------------  SignalType Conversion */
      /* {Bdot Controller.Avoid Eps Bug.11} */
      Avoid_Eps_Bug_1 = ROUND(Y->Mode_2_Inertial_Capture); 
      Avoid_Eps_Bug_2 = ROUND(Y->Torque_Rod_Enable); 
      /* ----------------------------  Logical Operator -- AND-OR-NOT */
      /* {Bdot Controller.Activation Check.2} */
      test = Avoid_Eps_Bug_1 > 0.0; 
      test = test && Avoid_Eps_Bug_2 > 0.0; 
      if (test) {
         Activation_Check_1 = 1.0; 
      }
      else {
         Activation_Check_1 = 0.0; 
      }
      /* ----------------------------  Cross Product */
      /* {Bdot Controller.Calc Bdot.12} */
      Bdot_x = Y->meas_mag_body_y*Y->Filt_Omega_B_N_B_z - Y->meas_mag_body_z*Y->Filt_Omega_B_N_B_y; 
      Bdot_y = Y->meas_mag_body_z*Y->Filt_Omega_B_N_B_x - Y->meas_mag_body_x*Y->Filt_Omega_B_N_B_z; 
      Bdot_z = Y->meas_mag_body_x*Y->Filt_Omega_B_N_B_y - Y->meas_mag_body_y*Y->Filt_Omega_B_N_B_x; 
      /* ----------------------------  Gain Block */
      /* {Bdot Controller.BdotPreCalcGain.99} */
      BdotPreCalcGain_1 = (-1.0)*0.00426532*U->Izz_Depl; 
      /* ----------------------------  ElementProduct */
      /* {Bdot Controller..24} */
      Bdot_X = BdotPreCalcGain_1*Bdot_x; 
      Bdot_Y = BdotPreCalcGain_1*Bdot_y; 
      Bdot_Z = BdotPreCalcGain_1*Bdot_z; 
      /* ----------------------------  Procedure Super Block */
      /* {mth vect norm(Normalize).4} */
      mth_vect_norm_4_u_1.Measured_magnetometer_body_x = Y->meas_mag_body_x;
      mth_vect_norm_4_u_1.Measured_magnetometer_body_y = Y->meas_mag_body_y;
      mth_vect_norm_4_u_1.Measured_magnetometer_body_z = Y->meas_mag_body_z;
      mth_vect_norm(&mth_vect_norm_4_u_1, &mth_vect_norm_4_y_1);
      X_4 = mth_vect_norm_4_y_1.X_1;
      Y_4 = mth_vect_norm_4_y_1.Y_1;
      Z_3 = mth_vect_norm_4_y_1.Z;
      Magnitude_3 = mth_vect_norm_4_y_1.Magnitude;
      /* ----------------------------  Algebraic Expression */
      /* {Bdot Controller..5} */
      Cmd_Dipole_X_1 = 1.0/(Magnitude_3*Magnitude_3)*Bdot_X; 
      Cmd_Dipole_Y_1 = 1.0/(Magnitude_3*Magnitude_3)*Bdot_Y; 
      Cmd_Dipole_Z_1 = 1.0/(Magnitude_3*Magnitude_3)*Bdot_Z; 
      /* ----------------------------  Gain Block */
      /* {Bdot Controller.Am2 to Counts.17} */
      Cmd_MTR_x_counts_1 = 3367.0033670033699*Cmd_Dipole_X_1; 
      Cmd_MTR_y_counts_1 = 3367.0033670033699*Cmd_Dipole_Y_1; 
      Cmd_MTR_z_counts_1 = 3367.0033670033672*Cmd_Dipole_Z_1; 
      /* ----------------------------  Absolute Value */
      /* {Bdot Controller..25} */
      Bdot_Controller_25_1 = fabs(Cmd_MTR_x_counts_1); 
      Bdot_Controller_25_2 = fabs(Cmd_MTR_y_counts_1); 
      Bdot_Controller_25_3 = fabs(Cmd_MTR_z_counts_1); 
      /* ----------------------------  Relational Operator -- LT-EQ-GT */
      /* {Bdot Controller..26} */
      if (Bdot_Controller_25_1 > Bdot_Controller_25_2) {
         Bdot_Controller_26_1 = 1.0; 
      }
      else {
         Bdot_Controller_26_1 = 0.0; 
      }
      /* ----------------------------  Data Path Switch */
      /* {Bdot Controller..16} */
      if (Bdot_Controller_26_1 > 0.0) {
         Bdot_Controller_16_1 = Bdot_Controller_25_1; 
      }
      else {
         Bdot_Controller_16_1 = Bdot_Controller_25_2; 
      }
      /* ----------------------------  Relational Operator -- LT-EQ-GT */
      /* {Bdot Controller..6} */
      if (Bdot_Controller_16_1 > Bdot_Controller_25_3) {
         Bdot_Controller_6_1 = 1.0; 
      }
      else {
         Bdot_Controller_6_1 = 0.0; 
      }
      /* ----------------------------  Data Path Switch */
      /* {Bdot Controller..10} */
      if (Bdot_Controller_6_1 > 0.0) {
         Bdot_Controller_10_1 = Bdot_Controller_16_1; 
      }
      else {
         Bdot_Controller_10_1 = Bdot_Controller_25_3; 
      }
      /* ----------------------------  Algebraic Expression */
      /* {Bdot Controller..58} */
      Scaling_Factor_1 = 127.0/(Bdot_Controller_10_1 + 1.0E-9); 
      /* ----------------------------  Algebraic Expression */
      /* {Bdot Controller..8} */
      Scaled_MTR_Count_X_1 = Cmd_MTR_x_counts_1*Scaling_Factor_1; 
      Scaled_MTR_Count_Y_1 = Cmd_MTR_y_counts_1*Scaling_Factor_1; 
      Scaled_MTR_Count_Z_1 = Cmd_MTR_z_counts_1*Scaling_Factor_1; 
      /* ----------------------------  Gain Block */
      /* {Bdot Controller.Counts to Am2.14} */
      Cmd_MTR_Am2_X_1 = 0.000297*Scaled_MTR_Count_X_1; 
      Cmd_MTR_Am2_Y_1 = 0.000297*Scaled_MTR_Count_Y_1; 
      Cmd_MTR_Am2_Z_1 = 0.000297*Scaled_MTR_Count_Z_1; 
      /* ----------------------------  Algebraic Expression */
      /* {Bdot Controller.Zero.15} */
      Zero_1_2 = 0.0; 
      /* ----------------------------  Data Path Switch */
      /* {Bdot Controller..7} */
      if (Activation_Check_1 > 0.0) {
         Cmd_Dipole_X_2 = Cmd_Dipole_X_1; 
         Cmd_Dipole_Y_2 = Cmd_Dipole_Y_1; 
         Cmd_Dipole_Z_2 = Cmd_Dipole_Z_1; 
         Cmd_MTR_counts_X_1 = Cmd_MTR_Am2_X_1; 
         Cmd_MTR_counts_Y_1 = Cmd_MTR_Am2_Y_1; 
         Cmd_MTR_counts_Z_1 = Cmd_MTR_Am2_Z_1; 
      }
      else {
         Cmd_Dipole_X_2 = Zero_1_2; 
         Cmd_Dipole_Y_2 = Zero_1_2; 
         Cmd_Dipole_Z_2 = Zero_1_2; 
         Cmd_MTR_counts_X_1 = Zero_1_2; 
         Cmd_MTR_counts_Y_1 = Zero_1_2; 
         Cmd_MTR_counts_Z_1 = Zero_1_2; 
      }
      /* ----------------------------  Summer */
      /* {Attitude Determination and Cntrl..26} */
      Y->Cmd_Dipole_X = Cmd_Dipole_X_2 + Cmd_Dipole_X; 
      Y->Cmd_Dipole_Y = Cmd_Dipole_Y_2 + Cmd_Dipole_Y; 
      Y->Cmd_Dipole_Z = Cmd_Dipole_Z_2 + Cmd_Dipole_Z; 
      /* ----------------------------  Summer */
      /* {Attitude Determination and Cntrl..2} */
      Y->Cmd_MTR_counts_X = Cmd_MTR_counts_X_1 + Cmd_MTR_counts_X; 
      Y->Cmd_MTR_counts_Y = Cmd_MTR_counts_Y_1 + Cmd_MTR_counts_Y; 
      Y->Cmd_MTR_counts_Z = Cmd_MTR_counts_Z_1 + Cmd_MTR_counts_Z; 
      /* ----------------------------  Algebraic Expression */
      /* {Mode Selection.Mode Output.99} */
      Y->Mode = Y->Mode_1_Idle + 2.0*Y->Mode_2_Inertial_Capture + 3.0*Y->Mode_3_Sun_Search + 4.0*Y->Mode_4_Sun_Pointing + 5.0*Y->Mode_5_Eclipse_Hold + 6.0*Y->Mode_6_Quat_Hold + 7.0*Y->Mode_7_Surface_Track + 8.0*Y->Mode_8_Test + 9.0*Y->Mode_9_Momentum_Dump; 
      /* ----------------------------  Logical Operator -- AND-OR-NOT */
      /* {Persistance Test Up Only..14} */
      test = Persistance_Test_Up_Only_13_1_7 > 0.0; 
      test = test && Persistance_Test_Up_Only_12_1_7 > 0.0; 
      test = test && Safe_Rate_Check_12_1 > 0.0; 
      if (test) {
         True_Last_3_Samples_6 = 1.0; 
      }
      else {
         True_Last_3_Samples_6 = 0.0; 
      }
      /* ----------------------------  Logical Operator -- AND-OR-NOT */
      /* {Persistance Test Up Only..14} */
      test = Persistance_Test_Up_Only_13_1_8 > 0.0; 
      test = test && Persistance_Test_Up_Only_12_1_8 > 0.0; 
      test = test && Safe_Rate_Check_12_2 > 0.0; 
      if (test) {
         True_Last_3_Samples_7 = 1.0; 
      }
      else {
         True_Last_3_Samples_7 = 0.0; 
      }
      /* ----------------------------  Logical Operator -- AND-OR-NOT */
      /* {Persistance Test Up Only..14} */
      test = Persistance_Test_Up_Only_13_1_9 > 0.0; 
      test = test && Persistance_Test_Up_Only_12_1_9 > 0.0; 
      test = test && Safe_Rate_Check_12_3 > 0.0; 
      if (test) {
         True_Last_3_Samples_8 = 1.0; 
      }
      else {
         True_Last_3_Samples_8 = 0.0; 
      }
      /* ----------------------------  Logical Operator -- AND-OR-NOT */
      /* {Safe Rate Check..23} */
      test = True_Last_3_Samples_6 > 0.0; 
      test = test || True_Last_3_Samples_7 > 0.0; 
      test = test || True_Last_3_Samples_8 > 0.0; 
      if (test) {
         Y->Rate_too_high = 1.0; 
      }
      else {
         Y->Rate_too_high = 0.0; 
      }
      /* ----------------------------  Logical Operator -- AND-OR-NOT */
      /* {Persistance Test Up Only..14} */
      test = Persistance_Test_Up_Only_13_1_10 > 0.0; 
      test = test && Persistance_Test_Up_Only_12_1_10 > 0.0; 
      test = test && Safe_Q_Check_14_1 > 0.0; 
      if (test) {
         Y->True_Last_3_Samples_2 = 1.0; 
      }
      else {
         Y->True_Last_3_Samples_2 = 0.0; 
      }

      /***** State Update. *****/
      /* ----------------------------  Time Delay */
      /* {Gyro Quantization Filter..34} */
      XD->Gyro_Quantization_Filter_34_S1 = Y->Filt_Omega_B_N_B_x; 
      XD->Gyro_Quantization_Filter_34_S2 = Y->Filt_Omega_B_N_B_y; 
      XD->Gyro_Quantization_Filter_34_S3 = Y->Filt_Omega_B_N_B_z; 
      /* ----------------------------  Time Delay */
      /* {Attitude Control..28} */
      XD->Attitude_Control_28_S1 = Slew_Inactive; 
      /* ----------------------------  Time Delay */
      /* {Control Modes..6} */
      XD->Control_Modes_6_S1 = X->Control_Modes_6_S2; 
      XD->Control_Modes_6_S2 = Y->Mode_3_Sun_Search; 
      /* ----------------------------  Time Delay */
      /* {Control Modes..14} */
      XD->Control_Modes_14_S1 = X->Control_Modes_14_S2; 
      XD->Control_Modes_14_S2 = Y->Mode_4_Sun_Pointing; 
      /* ----------------------------  Time Delay */
      /* {Control Modes..12} */
      XD->Control_Modes_12_S1 = X->Control_Modes_12_S2; 
      XD->Control_Modes_12_S2 = Y->Mode_7_Surface_Track; 
      /* ----------------------------  Time Delay */
      /* {Control Modes..1} */
      XD->Control_Modes_1_S1 = X->Control_Modes_1_S2; 
      XD->Control_Modes_1_S2 = Y->Mode_9_Momentum_Dump; 
      /* ----------------------------  Time Delay */
      /* {Control Modes..3} */
      XD->Control_Modes_3_S1 = X->Control_Modes_3_S2; 
      XD->Control_Modes_3_S2 = Y->Mode_5_Eclipse_Hold; 
      /* ----------------------------  Time Delay */
      /* {Control Modes..23} */
      XD->Control_Modes_23_S1 = X->Control_Modes_23_S2; 
      XD->Control_Modes_23_S2 = Y->Mode_2_Inertial_Capture; 
      /* ----------------------------  Time Delay */
      /* {Control Modes..9} */
      XD->Control_Modes_9_S1 = X->Control_Modes_9_S2; 
      XD->Control_Modes_9_S2 = Y->Mode_6_Quat_Hold; 
      /* ----------------------------  Time Delay */
      /* {Control Modes..19} */
      XD->Control_Modes_19_S1 = X->Control_Modes_19_S2; 
      XD->Control_Modes_19_S2 = Y->Mode_8_Test; 
      /* ----------------------------  Time Delay */
      /* {Mode 3 Timer..4} */
      XD->Mode_3_Timer_4_S1 = Y->Mode_3_Sun_Search; 
      /* ----------------------------  Time Delay */
      /* {Mode 3 Timer..99} */
      XD->Mode_3_Timer_99_S1 = Mode_3_Limit_Reached; 
      /* ----------------------------  Time Delay */
      /* {Accumulate Gyros.PAST QUATERNION.14} */
      XD->PAST_QUATERNION_S1 = Estimated_q1_B_ECI; 
      XD->PAST_QUATERNION_S2 = Estimated_q2_B_ECI; 
      XD->PAST_QUATERNION_S3 = Estimated_q3_B_ECI; 
      XD->PAST_QUATERNION_S4 = Estimated_q4_B_ECI; 
      /* ----------------------------  Time Delay */
      /* {Quaternion Require Continuous..16} */
      XD->Quaternion_Require_Continuous_16_S1 = Estimated_q1_B_ECI; 
      XD->Quaternion_Require_Continuous_16_S2 = Estimated_q2_B_ECI; 
      XD->Quaternion_Require_Continuous_16_S3 = Estimated_q3_B_ECI; 
      XD->Quaternion_Require_Continuous_16_S4 = Estimated_q4_B_ECI; 
      /* ----------------------------  Time Delay */
      /* {Enable and Initialize..96} */
      XD->Enable_and_Initialize_96_S1 = Y->Mode_4_Sun_Pointing; 
      /* ----------------------------  Time Delay */
      /* {Toggle 2 Pulse..20} */
      XD->Toggle_2_Pulse_20_S1 = Y->Mode_5_Eclipse_Hold; 
      /* ----------------------------  Time Delay */
      /* {Persistance Test Up and Down..12} */
      XD->Persistance_Test_Up_and_Down_12_S1 = Enable_AD; 
      /* ----------------------------  Time Delay */
      /* {Persistance Test Up and Down..13} */
      XD->Persistance_Test_Up_and_Down_13_S1 = Persistance_Test_Up_and_Down_12_1; 
      /* ----------------------------  Time Delay */
      /* {Toggle 2 Pulse 5..20} */
      XD->Toggle_2_Pulse_5_20_S1 = Y->Mode_4_Sun_Pointing; 
      XD->Toggle_2_Pulse_5_20_S2 = Y->Mode_5_Eclipse_Hold; 
      XD->Toggle_2_Pulse_5_20_S3 = Y->Mode_6_Quat_Hold; 
      XD->Toggle_2_Pulse_5_20_S4 = Y->Mode_7_Surface_Track; 
      /* ----------------------------  Time Delay */
      /* {Toggle 2 Pulse..20} */
      XD->Toggle_2_Pulse_20_S1_1 = Reset_Estimator; 
      /* ----------------------------  Time Delay */
      /* {Persistance Test Up Only..12} */
      XD->Persistance_Test_Up_Only_12_S1 = Omega_B_N_B_is_Small; 
      /* ----------------------------  Time Delay */
      /* {Persistance Test Up Only..13} */
      XD->Persistance_Test_Up_Only_13_S1 = Persistance_Test_Up_Only_12_1; 
      /* ----------------------------  Time Delay */
      /* {Persistance Test Up Only..12} */
      XD->Persistance_Test_Up_Only_12_S1_1 = Avoid_Numerical_Error_1; 
      /* ----------------------------  Time Delay */
      /* {Persistance Test Up Only..13} */
      XD->Persistance_Test_Up_Only_13_S1_1 = Persistance_Test_Up_Only_12_1_1; 
      /* ----------------------------  Time Delay */
      /* {Persistance Test Up Only..12} */
      XD->Persistance_Test_Up_Only_12_S1_2 = Omega_B_N_B_is_Large; 
      /* ----------------------------  Time Delay */
      /* {Persistance Test Up Only..13} */
      XD->Persistance_Test_Up_Only_13_S1_2 = Persistance_Test_Up_Only_12_1_2; 
      /* ----------------------------  Time Delay */
      /* {Sun Line Error.Avoid Algebraic Loop.32} */
      XD->Avoid_Algebraic_Loop_S1 = Sun_Line_Error_96_1; 
      /* ----------------------------  Time Delay */
      /* {Persistance Test Up Only..12} */
      XD->Persistance_Test_Up_Only_12_S1_3 = Sun_Line_Error_Small_1; 
      /* ----------------------------  Time Delay */
      /* {Persistance Test Up Only..13} */
      XD->Persistance_Test_Up_Only_13_S1_3 = Persistance_Test_Up_Only_12_1_3; 
      /* ----------------------------  Time Delay */
      /* {Persistance Test Up Only..12} */
      XD->Persistance_Test_Up_Only_12_S1_4 = Sun_Line_Error_Small; 
      /* ----------------------------  Time Delay */
      /* {Persistance Test Up Only..13} */
      XD->Persistance_Test_Up_Only_13_S1_4 = Persistance_Test_Up_Only_12_1_4; 
      /* ----------------------------  Time Delay */
      /* {Safe Sun Presence Check..4} */
      XD->Safe_Sun_Presence_Check_4_S1 = Y->Sun_Sensor_Processing_7_1; 
      /* ----------------------------  Time Delay */
      /* {Persistance Test Up Only..12} */
      XD->Persistance_Test_Up_Only_12_S1_5 = Saturated; 
      /* ----------------------------  Time Delay */
      /* {Persistance Test Up Only..13} */
      XD->Persistance_Test_Up_Only_13_S1_5 = Persistance_Test_Up_Only_12_1_5; 
      /* ----------------------------  Time Delay */
      /* {Persistance Test Up Only..12} */
      XD->Persistance_Test_Up_Only_12_S1_6 = Desaturated; 
      /* ----------------------------  Time Delay */
      /* {Persistance Test Up Only..13} */
      XD->Persistance_Test_Up_Only_13_S1_6 = Persistance_Test_Up_Only_12_1_6; 
      /* ----------------------------  Time Delay */
      /* {Persistance Test Up Only..12} */
      XD->Persistance_Test_Up_Only_12_S1_7 = Safe_Rate_Check_12_1; 
      /* ----------------------------  Time Delay */
      /* {Persistance Test Up Only..13} */
      XD->Persistance_Test_Up_Only_13_S1_7 = Persistance_Test_Up_Only_12_1_7; 
      /* ----------------------------  Time Delay */
      /* {Persistance Test Up Only..12} */
      XD->Persistance_Test_Up_Only_12_S1_8 = Safe_Rate_Check_12_2; 
      /* ----------------------------  Time Delay */
      /* {Persistance Test Up Only..13} */
      XD->Persistance_Test_Up_Only_13_S1_8 = Persistance_Test_Up_Only_12_1_8; 
      /* ----------------------------  Time Delay */
      /* {Persistance Test Up Only..12} */
      XD->Persistance_Test_Up_Only_12_S1_9 = Safe_Rate_Check_12_3; 
      /* ----------------------------  Time Delay */
      /* {Persistance Test Up Only..13} */
      XD->Persistance_Test_Up_Only_13_S1_9 = Persistance_Test_Up_Only_12_1_9; 
      /* ----------------------------  Time Delay */
      /* {Persistance Test Up Only..12} */
      XD->Persistance_Test_Up_Only_12_S1_10 = Safe_Q_Check_14_1; 
      /* ----------------------------  Time Delay */
      /* {Persistance Test Up Only..13} */
      XD->Persistance_Test_Up_Only_13_S1_10 = Persistance_Test_Up_Only_12_1_10; 
      /* ----------------------------  Integrator */
      /* {Safe Sun Presence Check..2} */
      XD->Safe_Sun_Presence_Check_2_S1 = X->Safe_Sun_Presence_Check_2_S1 + TSAMP*Sun_not_present; 
      /* ----------------------------  Integrator */
      /* {Mode 3 Timer..2} */
      XD->Mode_3_Timer_2_S1 = X->Mode_3_Timer_2_S1 + TSAMP*Y->Mode_3_Sun_Search; 

      /* Copy back(s) and/or duplicate(s) */

      /***** Swap state pointers. *****/

      XTMP = X;
      X = XD;
      XD = XTMP;
      INIT = 0;


      if(iinfo[1]) {
         SUBSYS_INIT[1] = FALSE;
         ////iinfo[1] = 0;
      }
      return;
EXEC_ERROR: ERROR_FLAG[1] = iinfo[0];
      iinfo[0]=0;
}



static void Init_Application_Data ()
{

     /* Declare %var/varblk initialization data */

     /* External outputs initialization. */
     sys_extout.Meas_Sun_Vector_B_x = (-1.0)*EPSILON;
     sys_extout.Meas_Sun_Vector_B_y = (-1.0)*EPSILON;
     sys_extout.Meas_Sun_Vector_B_z = (-1.0)*EPSILON;
     sys_extout.Sun_Sensor_Processing_7_1 = (-1.0)*EPSILON;
     sys_extout.Filt_Omega_B_N_B_x = (-1.0)*EPSILON;
     sys_extout.Filt_Omega_B_N_B_y = (-1.0)*EPSILON;
     sys_extout.Filt_Omega_B_N_B_z = (-1.0)*EPSILON;
     sys_extout.meas_mag_body_x = (-1.0)*EPSILON;
     sys_extout.meas_mag_body_y = (-1.0)*EPSILON;
     sys_extout.meas_mag_body_z = (-1.0)*EPSILON;
     sys_extout.Meas_Wheel_omega_rad_s_x = (-1.0)*EPSILON;
     sys_extout.Meas_Wheel_omega_rad_s_y = (-1.0)*EPSILON;
     sys_extout.Meas_Wheel_omega_rad_s_z = (-1.0)*EPSILON;
     sys_extout.SP_check_failed = (-1.0)*EPSILON;
     sys_extout.Rate_too_high = (-1.0)*EPSILON;
     sys_extout.True_Last_3_Samples = (-1.0)*EPSILON;
     sys_extout.True_Last_3_Samples_1 = (-1.0)*EPSILON;
     sys_extout.Wheel_Hx_Body_Frame = (-1.0)*EPSILON;
     sys_extout.Wheel_Hy_Body_Frame = (-1.0)*EPSILON;
     sys_extout.Wheel_Hz_Body_Frame = (-1.0)*EPSILON;
     sys_extout.Mode_1_Idle = (-1.0)*EPSILON;
     sys_extout.Mode_2_Inertial_Capture = (-1.0)*EPSILON;
     sys_extout.Mode_3_Sun_Search = (-1.0)*EPSILON;
     sys_extout.Mode_4_Sun_Pointing = (-1.0)*EPSILON;
     sys_extout.Mode_5_Eclipse_Hold = (-1.0)*EPSILON;
     sys_extout.Mode_6_Quat_Hold = (-1.0)*EPSILON;
     sys_extout.Mode_7_Surface_Track = (-1.0)*EPSILON;
     sys_extout.Mode_8_Test = (-1.0)*EPSILON;
     sys_extout.Mode_9_Momentum_Dump = (-1.0)*EPSILON;
     sys_extout.True_Last_3_Samples_2 = (-1.0)*EPSILON;
     sys_extout.q1_err = (-1.0)*EPSILON;
     sys_extout.q2_err = (-1.0)*EPSILON;
     sys_extout.q3_err = (-1.0)*EPSILON;
     sys_extout.q4_err = (-1.0)*EPSILON;
     sys_extout.wx_err = (-1.0)*EPSILON;
     sys_extout.wy_err = (-1.0)*EPSILON;
     sys_extout.wz_err = (-1.0)*EPSILON;
     sys_extout.Torque_Rod_Enable = (-1.0)*EPSILON;
     sys_extout.Reset_Estimator = (-1.0)*EPSILON;
     sys_extout.Latch_2_1 = (-1.0)*EPSILON;
     sys_extout.Save_Sun_Quaternion = (-1.0)*EPSILON;
     sys_extout.Approx_Angular_Err_frm_Sun_Lin = (-1.0)*EPSILON;
     sys_extout.q1 = (-1.0)*EPSILON;
     sys_extout.q2 = (-1.0)*EPSILON;
     sys_extout.q3 = (-1.0)*EPSILON;
     sys_extout.q4 = (-1.0)*EPSILON;
     sys_extout.nadir_vect_x = (-1.0)*EPSILON;
     sys_extout.nadir_vect_y = (-1.0)*EPSILON;
     sys_extout.nadir_vect_z = (-1.0)*EPSILON;
     sys_extout.Body_Hx_ECI_Frame = (-1.0)*EPSILON;
     sys_extout.Body_Hy_ECI_Frame = (-1.0)*EPSILON;
     sys_extout.Body_Hz_ECI_Frame = (-1.0)*EPSILON;
     sys_extout.Mode = (-1.0)*EPSILON;
     sys_extout.dzero = (-1.0)*EPSILON;
     sys_extout.dzero_1 = (-1.0)*EPSILON;
     sys_extout.dzero_2 = (-1.0)*EPSILON;
     sys_extout.gyro_offset_x = (-1.0)*EPSILON;
     sys_extout.gyro_offset_y = (-1.0)*EPSILON;
     sys_extout.gyro_offset_z = (-1.0)*EPSILON;
     sys_extout.overwritepulse = (-1.0)*EPSILON;
     sys_extout.Requested_Torque_B_x = (-1.0)*EPSILON;
     sys_extout.Requested_Torque_B_y = (-1.0)*EPSILON;
     sys_extout.Requested_Torque_B_z = (-1.0)*EPSILON;
     sys_extout.PD_Control = (-1.0)*EPSILON;
     sys_extout.Mode_5_Wheel_Enable = (-1.0)*EPSILON;
     sys_extout.dzero_3 = (-1.0)*EPSILON;
     sys_extout.dzero_4 = (-1.0)*EPSILON;
     sys_extout.dzero_5 = (-1.0)*EPSILON;
     sys_extout.dzero_6 = (-1.0)*EPSILON;
     sys_extout.Cmded_Whl_Torque_x = (-1.0)*EPSILON;
     sys_extout.Cmded_Whl_Torque_y = (-1.0)*EPSILON;
     sys_extout.Cmded_Whl_Torque_z = (-1.0)*EPSILON;
     sys_extout.Cmd_Dipole_X = (-1.0)*EPSILON;
     sys_extout.Cmd_Dipole_Y = (-1.0)*EPSILON;
     sys_extout.Cmd_Dipole_Z = (-1.0)*EPSILON;
     sys_extout.Total_Hx = (-1.0)*EPSILON;
     sys_extout.Total_Hy = (-1.0)*EPSILON;
     sys_extout.Total_Hz = (-1.0)*EPSILON;
     sys_extout.Cmd_MTR_counts_X = (-1.0)*EPSILON;
     sys_extout.Cmd_MTR_counts_Y = (-1.0)*EPSILON;
     sys_extout.Cmd_MTR_counts_Z = (-1.0)*EPSILON;

     /* DataStore initialization. */
     System_Parameters_1.Mode_Override = 0.0;
     System_Parameters_1.Mode_Limit = 9.0;
     System_Parameters_1.Min_Omega = 0.01;
     System_Parameters_1.Max_Omega = 0.1;
     System_Parameters_1.Sun_Search_Slew_Trans = 900.0;
     System_Parameters_1.Min_Sun_Line_Trans = 0.15;
     System_Parameters_1.Min_Pitch_Trans = 0.17;
     System_Parameters_1.SP_time_limit = 2700.0;
     System_Parameters_1.Enable_Momentum_Dump = 1.0;
     System_Parameters_1.Kpx = 0.16;
     System_Parameters_1.Kpy = 0.16;
     System_Parameters_1.Kpz = 0.16;
     System_Parameters_1.Krx = 0.8;
     System_Parameters_1.Kry = 0.8;
     System_Parameters_1.Krz = 0.8;
     System_Parameters_1.Ixx = 0.1328;
     System_Parameters_1.Ixy = 0.0;
     System_Parameters_1.Ixz = 0.0;
     System_Parameters_1.Iyy = 0.13204;
     System_Parameters_1.Iyz = 0.0;
     System_Parameters_1.Izz = 0.0351;
     System_Parameters_1.Ixx_Depl = 0.1328;
     System_Parameters_1.Ixy_Depl = 0.0;
     System_Parameters_1.Ixz_Depl = 0.0;
     System_Parameters_1.Iyy_Depl = 0.13204;
     System_Parameters_1.Iyz_Depl = 0.0;
     System_Parameters_1.Izz_Depl = 0.0351;
     System_Parameters_1.Slew_to_PD_Trans = 0.25;
     System_Parameters_1.Commanded_Aqsn_Slew_Rate = 0.007;
     System_Parameters_1.Rate_limit = 20.0;
     System_Parameters_1.Q_limit = 1.0;
     System_Parameters_1.Wheel_Sat_Limit = 0.0025;
     System_Parameters_1.kMOMx = 0.0;
     System_Parameters_1.kMOMy = 0.0;
     System_Parameters_1.kMOMz = 0.0;
     System_Parameters_1.cgx_Depl = 0.0;
     System_Parameters_1.cgy_Depl = (-1.0)*0.0006;
     System_Parameters_1.cgz_Depl = (-1.0)*0.0471;
     System_Parameters_1.Deploy = 1.0;
     System_Parameters_2.On_Time_Limit = 0.0;
     System_Parameters_2.Kr_Wheel_X = 0.35;
     System_Parameters_2.Kr_Wheel_Y = 0.35;
     System_Parameters_2.Kr_Wheel_Z = 0.35;
     System_Parameters_2.Set_Wheel_Speed_X = 0.0;
     System_Parameters_2.Set_Wheel_Speed_Y = 0.0;
     System_Parameters_2.Set_Wheel_Speed_Z = 0.0;
     System_Parameters_2.Max_Wheel_Speed_X = 104.0;
     System_Parameters_2.Max_Wheel_Speed_Y = 104.0;
     System_Parameters_2.Max_Wheel_Speed_Z = 104.0;
     System_Parameters_2.I_Wheel_xx = 1.04E-005;
     System_Parameters_2.I_Wheel_xy = 0.0;
     System_Parameters_2.I_Wheel_xz = 0.0;
     System_Parameters_2.I_Wheel_yy = 1.04E-005;
     System_Parameters_2.I_Wheel_yz = 0.0;
     System_Parameters_2.I_Wheel_zz = 1.04E-005;
     System_Parameters_2.Inv_I_Wheel_11 = 96618.35749;
     System_Parameters_2.Inv_I_Wheel_12 = 0.0;
     System_Parameters_2.Inv_I_Wheel_13 = 0.0;
     System_Parameters_2.Inv_I_Wheel_22 = 96618.35749;
     System_Parameters_2.Inv_I_Wheel_23 = 0.0;
     System_Parameters_2.Inv_I_Wheel_33 = 96618.35749;
     System_Parameters_2.TRBW11 = 1.0;
     System_Parameters_2.TRBW12 = 0.0;
     System_Parameters_2.TRBW13 = 0.0;
     System_Parameters_2.TRBW21 = 0.0;
     System_Parameters_2.TRBW22 = 1.0;
     System_Parameters_2.TRBW23 = 0.0;
     System_Parameters_2.TRBW31 = 0.0;
     System_Parameters_2.TRBW32 = 0.0;
     System_Parameters_2.TRBW33 = 1.0;
     System_Parameters_2.TRWB11 = 1.0;
     System_Parameters_2.TRWB12 = 0.0;
     System_Parameters_2.TRWB13 = 0.0;
     System_Parameters_2.TRWB21 = 0.0;
     System_Parameters_2.TRWB22 = 1.0;
     System_Parameters_2.TRWB23 = 0.0;
     System_Parameters_2.TRWB31 = 0.0;
     System_Parameters_2.TRWB32 = 0.0;
     System_Parameters_2.TRWB33 = 1.0;
     System_Parameters_2.Wheel_Enable = 1.0;
     System_Parameters_2.Wheel_Speed_Reset = 1.0;
     System_Parameters_2.Kwi = 0.0;
     System_Parameters_3.AD_Filter_Gain_Kf = 0.46651;
     System_Parameters_3.AD_Filter_Pole_pf = 0.53349;
     System_Parameters_3.Whl_Filter_Gain_Kf = 0.0;
     System_Parameters_3.Whl_Filter_Pole_pf = 0.0;
     System_Parameters_3.Reset_Estimator = 0.0;
     System_Parameters_3.Enable_AD_Pos_Updates = 0.0;
     System_Parameters_3.Wheel_Disable_Mode5 = 1.0;
     System_Parameters_3.q1_M_LV = 0.5;
     System_Parameters_3.q2_M_LV = 0.5;
     System_Parameters_3.q3_M_LV = 0.0;
     System_Parameters_3.q4_M_LV = 0.707107;
     System_Parameters_3.Spare2 = 0.0;
     System_Parameters_3.q1_M_ECI = 0.0;
     System_Parameters_3.q2_M_ECI = 0.0;
     System_Parameters_3.q3_M_ECI = 0.0;
     System_Parameters_3.q4_M_ECI = 1.0;
     System_Parameters_3.Spare3 = 0.0;
     System_Parameters_3.Target_Longitude = (-1.0)*121.98;
     System_Parameters_3.Target_Latitude = 38.92;
     System_Parameters_3.Target_Altitude = 0.0;
     System_Parameters_3.Gimble_Calculation_Enable = 0.0;
     System_Parameters_3.Spare4 = 0.0;
     System_Parameters_3.Slew_Acceleration = 0.0005;
     System_Parameters_3.Slew_Rate = 0.0175;
     System_Parameters_3.Slew_Angle = 2.5;
     System_Parameters_3.Euler_Slew_Axis_x = 0.0;
     System_Parameters_3.Euler_Slew_Axis_y = 0.0;
     System_Parameters_3.Euler_Slew_Axis_z = 1.0;
     System_Parameters_3.Spare5 = 0.0;
     System_Parameters_3.Sun_Sensor_MisAlmt_x = 0.0;
     System_Parameters_3.Sun_Sensor_MisAlmt_y = 0.0;
     System_Parameters_3.Sun_Sensor_MisAlmt_z = 0.0;
     System_Parameters_3.Spare6 = 0.0;
     System_Parameters_3.Magnetometer_Mis_x = 0.0;
     System_Parameters_3.Magnetometer_Mis_y = 0.0;
     System_Parameters_3.Magnetometer_Mis_z = 0.0;
     System_Parameters_3.Magnetometer_Bias_x = 0.0;
     System_Parameters_3.Magnetometer_Bias_y = 0.0;
     System_Parameters_3.Magnetometer_Bias_z = 0.0;
     System_Parameters_3.Orbit_Rate = (-1.0)*0.0011339;
     System_Parameters_3.Gyro_MisAlmt_x = 0.0;
     System_Parameters_3.Gyro_MisAlmt_y = 0.0;
     System_Parameters_3.Gyro_MisAlmt_z = 0.0;
     System_Parameters_3.Gyro_Drift_x = 0.0;
     System_Parameters_3.Gyro_Drift_y = 0.0;
     System_Parameters_3.Gyro_Drift_z = 0.0;
//disable_interrupts(INTR_GLOBAL);
//sprintf(dbgbuf,"\r\nZeroes --> System_Parameters_3:%f,%f,%f",System_Parameters_3.Gyro_Drift_x,System_Parameters_3.Gyro_Drift_y,System_Parameters_3.Gyro_Drift_z);
//sendDBGALL(userPort,dbgbuf);
//enable_interrupts(INTR_GLOBAL);
     System_Parameters_3.Gyro_AutoCalibrate = 0.0;
     System_Parameters_3.Mode4_MTR_Enable = 0.0;
     System_Parameters_3.Spare8 = 0.0;
     System_Parameters_3.Spare9 = 0.0;
     System_Parameters_3.Drag_Coef = 2.2;
     System_Parameters_3.Cross_Section_Area_XY = 0.05405;
     System_Parameters_3.Cross_Section_Area_Antenna = 0.3977;
     System_Parameters_3.Half_MTR_Dipole_Am2 = 0.0185;
     gyro.gyro_offset_x = 0.0;
     gyro.gyro_offset_y = 0.0;
     gyro.gyro_offset_z = 0.0;
     gyro.n = 0.0;
     gyro.overwritepulse = 0.0;
     Internal.Saved_Sun_Vector_q1 = 0.0;
     Internal.Saved_Sun_Vector_q2 = 0.0;
     Internal.Saved_Sun_Vector_q3 = 0.0;
     Internal.Saved_Sun_Vector_q4 = 1.0;

     /* Subsystem outputs initialization. */
     subsys_1_out.PD_Control = (-1.0)*EPSILON;
     subsys_1_out.Approx_Angular_Err_frm_Sun_Lin = (-1.0)*EPSILON;
     subsys_1_out.Filt_Omega_B_N_B_x = (-1.0)*EPSILON;
     subsys_1_out.Filt_Omega_B_N_B_y = (-1.0)*EPSILON;
     subsys_1_out.Filt_Omega_B_N_B_z = (-1.0)*EPSILON;
     subsys_1_out.Meas_Sun_Vector_B_x = (-1.0)*EPSILON;
     subsys_1_out.Meas_Sun_Vector_B_y = (-1.0)*EPSILON;
     subsys_1_out.Meas_Sun_Vector_B_z = (-1.0)*EPSILON;
     subsys_1_out.Sun_Sensor_Processing_7_1 = (-1.0)*EPSILON;
     subsys_1_out.meas_mag_body_x = (-1.0)*EPSILON;
     subsys_1_out.meas_mag_body_y = (-1.0)*EPSILON;
     subsys_1_out.meas_mag_body_z = (-1.0)*EPSILON;
     subsys_1_out.Meas_Wheel_omega_rad_s_x = (-1.0)*EPSILON;
     subsys_1_out.Meas_Wheel_omega_rad_s_y = (-1.0)*EPSILON;
     subsys_1_out.Meas_Wheel_omega_rad_s_z = (-1.0)*EPSILON;
     subsys_1_out.Wheel_Hx_Body_Frame = (-1.0)*EPSILON;
     subsys_1_out.Wheel_Hy_Body_Frame = (-1.0)*EPSILON;
     subsys_1_out.Wheel_Hz_Body_Frame = (-1.0)*EPSILON;
     subsys_1_out.True_Last_3_Samples = (-1.0)*EPSILON;
     subsys_1_out.Compute_Gyro_Offset_21_1 = (-1.0)*EPSILON;
     subsys_1_out.Compute_Gyro_Offset_21_2 = (-1.0)*EPSILON;
     subsys_1_out.Compute_Gyro_Offset_21_3 = (-1.0)*EPSILON;
     subsys_1_out.Compute_Gyro_Offset_21_4 = (-1.0)*EPSILON;
     subsys_1_out.Compute_Gyro_Offset_21_5 = (-1.0)*EPSILON;
     subsys_1_out.SP_check_failed = (-1.0)*EPSILON;
     subsys_1_out.True_Last_3_Samples_1 = (-1.0)*EPSILON;
     subsys_1_out.Mode_1_Idle = (-1.0)*EPSILON;
     subsys_1_out.Mode_2_Inertial_Capture = (-1.0)*EPSILON;
     subsys_1_out.Mode_3_Sun_Search = (-1.0)*EPSILON;
     subsys_1_out.Mode_4_Sun_Pointing = (-1.0)*EPSILON;
     subsys_1_out.Mode_5_Eclipse_Hold = (-1.0)*EPSILON;
     subsys_1_out.Mode_6_Quat_Hold = (-1.0)*EPSILON;
     subsys_1_out.Mode_7_Surface_Track = (-1.0)*EPSILON;
     subsys_1_out.Mode_8_Test = (-1.0)*EPSILON;
     subsys_1_out.Mode_9_Momentum_Dump = (-1.0)*EPSILON;
     subsys_1_out.Latch_2_1 = (-1.0)*EPSILON;
     subsys_1_out.q1 = (-1.0)*EPSILON;
     subsys_1_out.q2 = (-1.0)*EPSILON;
     subsys_1_out.q3 = (-1.0)*EPSILON;
     subsys_1_out.q4 = (-1.0)*EPSILON;
     subsys_1_out.q1_err = (-1.0)*EPSILON;
     subsys_1_out.q2_err = (-1.0)*EPSILON;
     subsys_1_out.q3_err = (-1.0)*EPSILON;
     subsys_1_out.q4_err = (-1.0)*EPSILON;
     subsys_1_out.wx_err = (-1.0)*EPSILON;
     subsys_1_out.wy_err = (-1.0)*EPSILON;
     subsys_1_out.wz_err = (-1.0)*EPSILON;
     subsys_1_out.Torque_Rod_Enable = (-1.0)*EPSILON;
     subsys_1_out.Body_Hx_ECI_Frame = (-1.0)*EPSILON;
     subsys_1_out.Body_Hy_ECI_Frame = (-1.0)*EPSILON;
     subsys_1_out.Body_Hz_ECI_Frame = (-1.0)*EPSILON;
     subsys_1_out.Total_Hx = (-1.0)*EPSILON;
     subsys_1_out.Total_Hy = (-1.0)*EPSILON;
     subsys_1_out.Total_Hz = (-1.0)*EPSILON;
     subsys_1_out.Requested_Torque_B_x = (-1.0)*EPSILON;
     subsys_1_out.Requested_Torque_B_y = (-1.0)*EPSILON;
     subsys_1_out.Requested_Torque_B_z = (-1.0)*EPSILON;
     subsys_1_out.nadir_vect_x = (-1.0)*EPSILON;
     subsys_1_out.nadir_vect_y = (-1.0)*EPSILON;
     subsys_1_out.nadir_vect_z = (-1.0)*EPSILON;
     subsys_1_out.Save_Sun_Quaternion = (-1.0)*EPSILON;
     subsys_1_out.Reset_Estimator = (-1.0)*EPSILON;
     subsys_1_out.Mode_5_Wheel_Enable = (-1.0)*EPSILON;
     subsys_1_out.Cmded_Whl_Torque_x = (-1.0)*EPSILON;
     subsys_1_out.Cmded_Whl_Torque_y = (-1.0)*EPSILON;
     subsys_1_out.Cmded_Whl_Torque_z = (-1.0)*EPSILON;
     subsys_1_out.Cmd_Dipole_X = (-1.0)*EPSILON;
     subsys_1_out.Cmd_Dipole_Y = (-1.0)*EPSILON;
     subsys_1_out.Cmd_Dipole_Z = (-1.0)*EPSILON;
     subsys_1_out.Cmd_MTR_counts_X = (-1.0)*EPSILON;
     subsys_1_out.Cmd_MTR_counts_Y = (-1.0)*EPSILON;
     subsys_1_out.Cmd_MTR_counts_Z = (-1.0)*EPSILON;
     subsys_1_out.Mode = (-1.0)*EPSILON;
     subsys_1_out.Rate_too_high = (-1.0)*EPSILON;
     subsys_1_out.True_Last_3_Samples_2 = (-1.0)*EPSILON;
     subsys_1_out.FSW_93_1 = (-1.0)*EPSILON;
     subsys_1_out.FSW_93_2 = (-1.0)*EPSILON;

   for(  cnt=0; cnt<NUMOUT; cnt++ ){
       ExtOut[cnt] = (-1.0)*EPSILON;
   }

     SUBSYS_PREINIT[1] = SUBSYS_INIT[1];
     subsys_1(&subsys_1_in,&subsys_1_out);


}



/*---------------*
 *-- SCHEDULER --*
 *---------------*/

/*** Scheduler Data ***/

enum SUBSYSTEM_TYPE  { CONTINUOUS, PERIODIC, ENABLED_PERIODIC, TRIGGERED_ANT,
                          TRIGGERED_ATR, TRIGGERED_SAF, NONE };
static RT_DURATION              SCHEDULER_INTERVAL              =
                                      (RT_DURATION) (1.0/SCHEDULER_FREQ);
static const enum SUBSYSTEM_TYPE      TASK_TYPE            [NTASKS+1] =
  {NONE, PERIODIC};
static const enum TASK_STATE_TYPE     INITIAL_TASK_STATE   [NTASKS+1] =
  {UNALLOCATED, IDLE};
static const RT_INTEGER               START_COUNT          [NTASKS+1] =
  {0, 0};
static const RT_INTEGER               SCHEDULING_COUNT     [NTASKS+1] =
  {0, 0};
static const RT_INTEGER               OUTPUT_COUNT         [NTASKS+1] =
  {0, 0};

static long int                       TIME_COUNT;
static RT_BOOLEAN                     DISPATCH    [NTASKS+1];

struct TCB_TYPE 
   { 
     enum  SUBSYSTEM_TYPE             TASK_TYPE;
     RT_BOOLEAN                       ENABLED;
     RT_INTEGER                       START;
     RT_INTEGER                       START_COUNT;
     RT_INTEGER                       SCHEDULING_COUNT;
     RT_INTEGER                       OUTPUT;
     RT_INTEGER                       OUTPUT_COUNT;
     RT_INTEGER                       DS_UPDATE;
   }; 
static struct TCB_TYPE                TCB [NTASKS+1];

void  Update_DS_With_Externals() {
   System_Parameters_1.Mode_Override = sys_extin.Mode_Override;
      System_Parameters_1.Mode_Limit = sys_extin.Mode_Limit;
      System_Parameters_1.Min_Omega = sys_extin.Min_Omega;
      System_Parameters_1.Max_Omega = sys_extin.Max_Omega;
      System_Parameters_1.Sun_Search_Slew_Trans = sys_extin.Sun_Search_Slew_Transition;
      System_Parameters_1.Min_Sun_Line_Trans = sys_extin.Min_Sun_Line_Trans;
      System_Parameters_1.Min_Pitch_Trans = sys_extin.Min_Pitch_Trans;
      System_Parameters_1.SP_time_limit = sys_extin.SP_Time_Limit;
      System_Parameters_1.Enable_Momentum_Dump = sys_extin.Enable_Momemtum_Dump;
      System_Parameters_1.Kpx = sys_extin.kpx;
      System_Parameters_1.Kpy = sys_extin.kpy;
      System_Parameters_1.Kpz = sys_extin.kpz;
      System_Parameters_1.Krx = sys_extin.krx;
      System_Parameters_1.Kry = sys_extin.kry;
      System_Parameters_1.Krz = sys_extin.krz;
      System_Parameters_1.Ixx = sys_extin.Ixx;
      System_Parameters_1.Ixy = sys_extin.Ixy;
      System_Parameters_1.Ixz = sys_extin.Ixz;
      System_Parameters_1.Iyy = sys_extin.Iyy;
      System_Parameters_1.Iyz = sys_extin.Iyz;
      System_Parameters_1.Izz = sys_extin.Izz;
      System_Parameters_1.Slew_to_PD_Trans = sys_extin.Slew_to_PD_Trans;
      System_Parameters_1.Commanded_Aqsn_Slew_Rate = sys_extin.Commanded_Aqsn_Slew_Rate;
      System_Parameters_1.Rate_limit = sys_extin.Rate_Limit;
      System_Parameters_1.Q_limit = sys_extin.Q_Limit;
      System_Parameters_1.Wheel_Sat_Limit = sys_extin.Wheel_Sat_Limit;
      System_Parameters_1.kMOMx = sys_extin.kMOMx;
      System_Parameters_1.kMOMy = sys_extin.kMOMy;
      System_Parameters_1.kMOMz = sys_extin.kMOMz;
      System_Parameters_2.I_Wheel_xx = sys_extin.Ixx_Wheel;
      System_Parameters_2.I_Wheel_xy = sys_extin.Ixy_Wheel;
      System_Parameters_2.I_Wheel_xz = sys_extin.Ixz_Wheel;
      System_Parameters_2.I_Wheel_yy = sys_extin.Iyy_Wheel;
      System_Parameters_2.I_Wheel_yz = sys_extin.Iyz_Wheel;
      System_Parameters_2.I_Wheel_zz = sys_extin.Izz_Wheel;
      System_Parameters_2.Wheel_Enable = sys_extin.Wheel_Enable;
      System_Parameters_3.AD_Filter_Gain_Kf = sys_extin.AD_Filter_Gain_Kf;
      System_Parameters_3.AD_Filter_Pole_pf = sys_extin.AD_Filter_Pole;
      System_Parameters_3.Whl_Filter_Gain_Kf = sys_extin.Whl_Filter_Gain;
      System_Parameters_3.Whl_Filter_Pole_pf = sys_extin.Whl_Filter_Pole;
      System_Parameters_3.Reset_Estimator = sys_extin.Reset_Estimator;
      System_Parameters_3.Wheel_Disable_Mode5 = sys_extin.Wheel_Disable_Mode5;
      System_Parameters_3.q1_M_LV = sys_extin.q1_M_LV;
      System_Parameters_3.q2_M_LV = sys_extin.q2_M_LV;
      System_Parameters_3.q3_M_LV = sys_extin.q3_M_LV;
      System_Parameters_3.q4_M_LV = sys_extin.q4_M_LV;
      System_Parameters_3.Spare2 = sys_extin.Spare2;
      System_Parameters_3.Spare3 = sys_extin.Spare3;
      System_Parameters_3.Target_Longitude = sys_extin.Target_Lat;
      System_Parameters_3.Target_Latitude = sys_extin.Target_Long;
      System_Parameters_3.Target_Altitude = sys_extin.Target_Alt;
      System_Parameters_3.Slew_Angle = sys_extin.Slew_Angle;
      System_Parameters_3.Sun_Sensor_MisAlmt_x = sys_extin.SS_MisAln_x;
      System_Parameters_3.Sun_Sensor_MisAlmt_y = sys_extin.SS_MisAln_y;
      System_Parameters_3.Sun_Sensor_MisAlmt_z = sys_extin.SS_MisAln_z;
      System_Parameters_3.Gyro_MisAlmt_x = sys_extin.Gyro_MisAln_x;
      System_Parameters_3.Gyro_MisAlmt_y = sys_extin.Gyro_MisAln_y;
      System_Parameters_3.Gyro_MisAlmt_z = sys_extin.Gyro_MisAln_z;
      System_Parameters_3.Gyro_Drift_x = sys_extin.Gyro_Drift_x;
      System_Parameters_3.Gyro_Drift_y = sys_extin.Gyro_Drift_y;
      System_Parameters_3.Gyro_Drift_z = sys_extin.Gyro_Drift_z;
      System_Parameters_3.Gyro_AutoCalibrate = sys_extin.Gyro_AutoCal;
      System_Parameters_3.Mode4_MTR_Enable = sys_extin.MTR_Enable_Mode4;
      System_Parameters_3.Drag_Coef = sys_extin.Drag_Coeff;


}

void Init_Scheduler(void)
{ 
   int NTSK;
   for(  NTSK=1; NTSK<=NTASKS; NTSK++  ) { 
      TCB[NTSK].TASK_TYPE        = TASK_TYPE[NTSK];
      TCB[NTSK].ENABLED          = FALSE;
      TCB[NTSK].START            = START_COUNT[NTSK];
      TCB[NTSK].START_COUNT      = START_COUNT[NTSK];
      TCB[NTSK].SCHEDULING_COUNT = SCHEDULING_COUNT[NTSK];
      TCB[NTSK].OUTPUT           = OUTPUT_COUNT[NTSK];
      TCB[NTSK].OUTPUT_COUNT     = OUTPUT_COUNT[NTSK];
      TASK_STATE[NTSK]           = INITIAL_TASK_STATE[NTSK];
      DISPATCH[NTSK]             = FALSE;
      ERROR_FLAG[NTSK]           = 0;
      SUBSYS_INIT[NTSK]          = TRUE;

      if(TASK_TYPE[NTSK]==TRIGGERED_ATR || TASK_TYPE[NTSK]==TRIGGERED_SAF){
         TCB[NTSK].DS_UPDATE = 1;
      }
      else {
         TCB[NTSK].DS_UPDATE = 0;
      }
   }
   ERROR_FLAG[0]    = 0;
   DISPATCH[0]      = FALSE;
   SUBSYS_INIT[0]   = FALSE;
   ELAPSED_TIME     = 0.0;
   TIME_COUNT       = (-1.0)*1;
}

void queueTask(int tskIndex)
{
  DISPATCH[tskIndex] = 1;
  switch(tskIndex)
  {
    /*** Task Input Sample and Hold ***/
    case 1 : 
                  subsys_1_in.MTR_Enable_Mode4 = sys_extin.MTR_Enable_Mode4;
                  subsys_1_in.SUNVEC0 = sys_extin.SUNVEC0;
                  subsys_1_in.SUNVEC1 = sys_extin.SUNVEC1;
                  subsys_1_in.SUNVEC2 = sys_extin.SUNVEC2;
                  subsys_1_in.ECI_POS_X = sys_extin.ECI_POS_X;
                  subsys_1_in.ECI_POS_Y = sys_extin.ECI_POS_Y;
                  subsys_1_in.ECI_POS_Z = sys_extin.ECI_POS_Z;
                  subsys_1_in.ECI_VEL_X = sys_extin.ECI_VEL_X;
                  subsys_1_in.ECI_VEL_Y = sys_extin.ECI_VEL_Y;
                  subsys_1_in.ECI_VEL_Z = sys_extin.ECI_VEL_Z;
                  subsys_1_in.GHA = sys_extin.GHA;
                  subsys_1_in.nT_ECI_x = sys_extin.nT_ECI_x;
                  subsys_1_in.nT_ECI_y = sys_extin.nT_ECI_y;
                  subsys_1_in.nT_ECI_z = sys_extin.nT_ECI_z;
                  subsys_1_in.Measured_Gyro_X = sys_extin.Measured_Gyro_X;
                  subsys_1_in.Measured_Gyro_Y = sys_extin.Measured_Gyro_Y;
                  subsys_1_in.Measured_Gyro_Z = sys_extin.Measured_Gyro_Z;
                  subsys_1_in.Measured_Mag_X = sys_extin.Measured_Mag_X;
                  subsys_1_in.Measured_Mag_Y = sys_extin.Measured_Mag_Y;
                  subsys_1_in.Measured_Mag_Z = sys_extin.Measured_Mag_Z;
                  subsys_1_in.Measured_SS_x = sys_extin.Measured_SS_x;
                  subsys_1_in.Measured_SS_y = sys_extin.Measured_SS_y;
                  subsys_1_in.Measured_SS_z = sys_extin.Measured_SS_z;
                  subsys_1_in.Sun_Present_Flag = sys_extin.Sun_Present_Flag;
                  subsys_1_in.Measured_RW_omega_x = sys_extin.Measured_RW_omega_x;
                  subsys_1_in.Measured_RW_omega_y = sys_extin.Measured_RW_omega_y;
                  subsys_1_in.Measured_RW_omega_z = sys_extin.Measured_RW_omega_z;
                  subsys_1_in.Mode_Override = System_Parameters_1.Mode_Override;
                  subsys_1_in.Mode_Limit = System_Parameters_1.Mode_Limit;
                  subsys_1_in.Min_Omega = System_Parameters_1.Min_Omega;
                  subsys_1_in.Max_Omega = System_Parameters_1.Max_Omega;
                  subsys_1_in.Sun_Search_Slew_Trans = System_Parameters_1.Sun_Search_Slew_Trans;
                  subsys_1_in.Min_Sun_Line_Trans = System_Parameters_1.Min_Sun_Line_Trans;
                  subsys_1_in.SP_time_limit = System_Parameters_1.SP_time_limit;
                  subsys_1_in.Enable_Momentum_Dump = System_Parameters_1.Enable_Momentum_Dump;
                  subsys_1_in.Kpx = System_Parameters_1.Kpx;
                  subsys_1_in.Kpy = System_Parameters_1.Kpy;
                  subsys_1_in.Kpz = System_Parameters_1.Kpz;
                  subsys_1_in.Krx = System_Parameters_1.Krx;
                  subsys_1_in.Kry = System_Parameters_1.Kry;
                  subsys_1_in.Krz = System_Parameters_1.Krz;
                  subsys_1_in.Ixx = System_Parameters_1.Ixx;
                  subsys_1_in.Ixy = System_Parameters_1.Ixy;
                  subsys_1_in.Ixz = System_Parameters_1.Ixz;
                  subsys_1_in.Iyy = System_Parameters_1.Iyy;
                  subsys_1_in.Iyz = System_Parameters_1.Iyz;
                  subsys_1_in.Izz = System_Parameters_1.Izz;
                  subsys_1_in.Ixx_Depl = System_Parameters_1.Ixx_Depl;
                  subsys_1_in.Ixy_Depl = System_Parameters_1.Ixy_Depl;
                  subsys_1_in.Ixz_Depl = System_Parameters_1.Ixz_Depl;
                  subsys_1_in.Iyy_Depl = System_Parameters_1.Iyy_Depl;
                  subsys_1_in.Iyz_Depl = System_Parameters_1.Iyz_Depl;
                  subsys_1_in.Izz_Depl = System_Parameters_1.Izz_Depl;
                  subsys_1_in.Slew_to_PD_Trans = System_Parameters_1.Slew_to_PD_Trans;
                  subsys_1_in.Commanded_Aqsn_Slew_Rate = System_Parameters_1.Commanded_Aqsn_Slew_Rate;
                  subsys_1_in.Rate_limit = System_Parameters_1.Rate_limit;
                  subsys_1_in.Q_limit = System_Parameters_1.Q_limit;
                  subsys_1_in.Wheel_Sat_Limit = System_Parameters_1.Wheel_Sat_Limit;
                  subsys_1_in.kMOMx = System_Parameters_1.kMOMx;
                  subsys_1_in.kMOMy = System_Parameters_1.kMOMy;
                  subsys_1_in.kMOMz = System_Parameters_1.kMOMz;
                  subsys_1_in.Deploy = System_Parameters_1.Deploy;
                  subsys_1_in.I_Wheel_xx = System_Parameters_2.I_Wheel_xx;
                  subsys_1_in.I_Wheel_yy = System_Parameters_2.I_Wheel_yy;
                  subsys_1_in.I_Wheel_zz = System_Parameters_2.I_Wheel_zz;
                  subsys_1_in.Wheel_Enable = System_Parameters_2.Wheel_Enable;
                  subsys_1_in.AD_Filter_Gain_Kf = System_Parameters_3.AD_Filter_Gain_Kf;
                  subsys_1_in.AD_Filter_Pole_pf = System_Parameters_3.AD_Filter_Pole_pf;
                  subsys_1_in.Reset_Estimator = System_Parameters_3.Reset_Estimator;
                  subsys_1_in.Wheel_Disable_Mode5 = System_Parameters_3.Wheel_Disable_Mode5;
                  subsys_1_in.q1_M_LV = System_Parameters_3.q1_M_LV;
                  subsys_1_in.q2_M_LV = System_Parameters_3.q2_M_LV;
                  subsys_1_in.q3_M_LV = System_Parameters_3.q3_M_LV;
                  subsys_1_in.q4_M_LV = System_Parameters_3.q4_M_LV;
                  subsys_1_in.Target_Longitude = System_Parameters_3.Target_Longitude;
                  subsys_1_in.Target_Latitude = System_Parameters_3.Target_Latitude;
                  subsys_1_in.Target_Altitude = System_Parameters_3.Target_Altitude;
                  subsys_1_in.Slew_Angle = System_Parameters_3.Slew_Angle;
                  subsys_1_in.Sun_Sensor_MisAlmt_x = System_Parameters_3.Sun_Sensor_MisAlmt_x;
                  subsys_1_in.Sun_Sensor_MisAlmt_y = System_Parameters_3.Sun_Sensor_MisAlmt_y;
                  subsys_1_in.Sun_Sensor_MisAlmt_z = System_Parameters_3.Sun_Sensor_MisAlmt_z;
                  subsys_1_in.Gyro_MisAlmt_x = System_Parameters_3.Gyro_MisAlmt_x;
                  subsys_1_in.Gyro_MisAlmt_y = System_Parameters_3.Gyro_MisAlmt_y;
                  subsys_1_in.Gyro_MisAlmt_z = System_Parameters_3.Gyro_MisAlmt_z;
                  subsys_1_in.Gyro_Drift_x = System_Parameters_3.Gyro_Drift_x;
                  subsys_1_in.Gyro_Drift_y = System_Parameters_3.Gyro_Drift_y;
                  subsys_1_in.Gyro_Drift_z = System_Parameters_3.Gyro_Drift_z;
                  subsys_1_in.Gyro_AutoCalibrate = System_Parameters_3.Gyro_AutoCalibrate;
                  subsys_1_in.gyro_offset_x = gyro.gyro_offset_x;
                  subsys_1_in.gyro_offset_y = gyro.gyro_offset_y;
                  subsys_1_in.gyro_offset_z = gyro.gyro_offset_z;
                  subsys_1_in.n = gyro.n;
                  subsys_1_in.overwritepulse = gyro.overwritepulse;
                  subsys_1_in.Saved_Sun_Vector_q1 = Internal.Saved_Sun_Vector_q1;
                  subsys_1_in.Saved_Sun_Vector_q2 = Internal.Saved_Sun_Vector_q2;
                  subsys_1_in.Saved_Sun_Vector_q3 = Internal.Saved_Sun_Vector_q3;
                  subsys_1_in.Saved_Sun_Vector_q4 = Internal.Saved_Sun_Vector_q4;
                  break;

    default:
    break;
  }
}

/*
** Scheduler
** reads data from ExtIn vector
** executes subsystems for this cycle
** writes data to ExtOut vector
*/
void SCHEDULER() 
{

   int    NTSK;
//   int    J;
   struct TCB_TYPE *TCBptr;

   TIME_COUNT = TIME_COUNT + 1;

   /*** System Output ***/
   if(TIME_COUNT>0.5) Update_DS_With_Externals();
   ExtOut[0] = subsys_1_out.Meas_Sun_Vector_B_x;
      ExtOut[1] = subsys_1_out.Meas_Sun_Vector_B_y;
      ExtOut[2] = subsys_1_out.Meas_Sun_Vector_B_z;
      ExtOut[3] = subsys_1_out.Sun_Sensor_Processing_7_1;
      ExtOut[4] = subsys_1_out.Filt_Omega_B_N_B_x;
      ExtOut[5] = subsys_1_out.Filt_Omega_B_N_B_y;
      ExtOut[6] = subsys_1_out.Filt_Omega_B_N_B_z;
      ExtOut[7] = subsys_1_out.meas_mag_body_x;
      ExtOut[8] = subsys_1_out.meas_mag_body_y;
      ExtOut[9] = subsys_1_out.meas_mag_body_z;
      ExtOut[10] = subsys_1_out.Meas_Wheel_omega_rad_s_x;
      ExtOut[11] = subsys_1_out.Meas_Wheel_omega_rad_s_y;
      ExtOut[12] = subsys_1_out.Meas_Wheel_omega_rad_s_z;
      ExtOut[13] = subsys_1_out.SP_check_failed;
      ExtOut[14] = subsys_1_out.Rate_too_high;
      ExtOut[15] = subsys_1_out.True_Last_3_Samples_2;
      ExtOut[16] = subsys_1_out.True_Last_3_Samples_1;
      ExtOut[17] = subsys_1_out.Wheel_Hx_Body_Frame;
      ExtOut[18] = subsys_1_out.Wheel_Hy_Body_Frame;
      ExtOut[19] = subsys_1_out.Wheel_Hz_Body_Frame;
      ExtOut[20] = subsys_1_out.Mode_1_Idle;
      ExtOut[21] = subsys_1_out.Mode_2_Inertial_Capture;
      ExtOut[22] = subsys_1_out.Mode_3_Sun_Search;
      ExtOut[23] = subsys_1_out.Mode_4_Sun_Pointing;
      ExtOut[24] = subsys_1_out.Mode_5_Eclipse_Hold;
      ExtOut[25] = subsys_1_out.Mode_6_Quat_Hold;
      ExtOut[26] = subsys_1_out.Mode_7_Surface_Track;
      ExtOut[27] = subsys_1_out.Mode_8_Test;
      ExtOut[28] = subsys_1_out.Mode_9_Momentum_Dump;
      ExtOut[29] = subsys_1_out.True_Last_3_Samples;
      ExtOut[30] = subsys_1_out.q1_err;
      ExtOut[31] = subsys_1_out.q2_err;
      ExtOut[32] = subsys_1_out.q3_err;
      ExtOut[33] = subsys_1_out.q4_err;
      ExtOut[34] = subsys_1_out.wx_err;
      ExtOut[35] = subsys_1_out.wy_err;
      ExtOut[36] = subsys_1_out.wz_err;
      ExtOut[37] = subsys_1_out.Torque_Rod_Enable;
      ExtOut[38] = subsys_1_out.Reset_Estimator;
      ExtOut[39] = subsys_1_out.Latch_2_1;
      ExtOut[40] = subsys_1_out.Save_Sun_Quaternion;
      ExtOut[41] = subsys_1_out.Approx_Angular_Err_frm_Sun_Lin;
      ExtOut[42] = subsys_1_out.q1;
      ExtOut[43] = subsys_1_out.q2;
      ExtOut[44] = subsys_1_out.q3;
      ExtOut[45] = subsys_1_out.q4;
      ExtOut[46] = subsys_1_out.nadir_vect_x;
      ExtOut[47] = subsys_1_out.nadir_vect_y;
      ExtOut[48] = subsys_1_out.nadir_vect_z;
      ExtOut[49] = subsys_1_out.Body_Hx_ECI_Frame;
      ExtOut[50] = subsys_1_out.Body_Hy_ECI_Frame;
      ExtOut[51] = subsys_1_out.Body_Hz_ECI_Frame;
      ExtOut[52] = subsys_1_out.Mode;
      ExtOut[53] = dczero.dzero;
      ExtOut[54] = dczero.dzero;
      ExtOut[55] = dczero.dzero;
      ExtOut[56] = gyro.gyro_offset_x;
      ExtOut[57] = gyro.gyro_offset_y;
      ExtOut[58] = gyro.gyro_offset_z;
      ExtOut[59] = gyro.overwritepulse;
      ExtOut[60] = subsys_1_out.Requested_Torque_B_x;
      ExtOut[61] = subsys_1_out.Requested_Torque_B_y;
      ExtOut[62] = subsys_1_out.Requested_Torque_B_z;
      ExtOut[63] = subsys_1_out.PD_Control;
      ExtOut[64] = subsys_1_out.Mode_5_Wheel_Enable;
      ExtOut[65] = dczero.dzero;
      ExtOut[66] = dczero.dzero;
      ExtOut[67] = dczero.dzero;
      ExtOut[68] = dczero.dzero;
      ExtOut[69] = subsys_1_out.Cmded_Whl_Torque_x;
      ExtOut[70] = subsys_1_out.Cmded_Whl_Torque_y;
      ExtOut[71] = subsys_1_out.Cmded_Whl_Torque_z;
      ExtOut[72] = subsys_1_out.Cmd_Dipole_X;
      ExtOut[73] = subsys_1_out.Cmd_Dipole_Y;
      ExtOut[74] = subsys_1_out.Cmd_Dipole_Z;
      ExtOut[75] = subsys_1_out.Total_Hx;
      ExtOut[76] = subsys_1_out.Total_Hy;
      ExtOut[77] = subsys_1_out.Total_Hz;
      ExtOut[78] = subsys_1_out.Cmd_MTR_counts_X;
      ExtOut[79] = subsys_1_out.Cmd_MTR_counts_Y;
      ExtOut[80] = subsys_1_out.Cmd_MTR_counts_Z;


   /*** System Input ***/
   sys_extin.Mode_Override = ExtIn[0];
      sys_extin.Mode_Limit = ExtIn[1];
      sys_extin.Min_Omega = ExtIn[2];
      sys_extin.Max_Omega = ExtIn[3];
      sys_extin.Sun_Search_Slew_Transition = ExtIn[4];
      sys_extin.Min_Sun_Line_Trans = ExtIn[5];
      sys_extin.Min_Pitch_Trans = ExtIn[6];
      sys_extin.SP_Time_Limit = ExtIn[7];
      sys_extin.Enable_Momemtum_Dump = ExtIn[8];
      sys_extin.kpx = ExtIn[9];
      sys_extin.kpy = ExtIn[10];
      sys_extin.kpz = ExtIn[11];
      sys_extin.krx = ExtIn[12];
      sys_extin.kry = ExtIn[13];
      sys_extin.krz = ExtIn[14];
      sys_extin.Ixx = ExtIn[15];
      sys_extin.Ixy = ExtIn[16];
      sys_extin.Ixz = ExtIn[17];
      sys_extin.Iyy = ExtIn[18];
      sys_extin.Iyz = ExtIn[19];
      sys_extin.Izz = ExtIn[20];
      sys_extin.Slew_to_PD_Trans = ExtIn[21];
      sys_extin.Commanded_Aqsn_Slew_Rate = ExtIn[22];
      sys_extin.Rate_Limit = ExtIn[23];
      sys_extin.Q_Limit = ExtIn[24];
      sys_extin.Wheel_Sat_Limit = ExtIn[25];
      sys_extin.kMOMx = ExtIn[26];
      sys_extin.kMOMy = ExtIn[27];
      sys_extin.kMOMz = ExtIn[28];
      sys_extin.Ixx_Wheel = ExtIn[29];
      sys_extin.Ixy_Wheel = ExtIn[30];
      sys_extin.Ixz_Wheel = ExtIn[31];
      sys_extin.Iyy_Wheel = ExtIn[32];
      sys_extin.Iyz_Wheel = ExtIn[33];
      sys_extin.Izz_Wheel = ExtIn[34];
      sys_extin.Wheel_Enable = ExtIn[35];
      sys_extin.AD_Filter_Gain_Kf = ExtIn[36];
      sys_extin.AD_Filter_Pole = ExtIn[37];
      sys_extin.Whl_Filter_Gain = ExtIn[38];
      sys_extin.Whl_Filter_Pole = ExtIn[39];
      sys_extin.Reset_Estimator = ExtIn[40];
      sys_extin.q1_M_LV = ExtIn[41];
      sys_extin.q2_M_LV = ExtIn[42];
      sys_extin.q3_M_LV = ExtIn[43];
      sys_extin.q4_M_LV = ExtIn[44];
      sys_extin.Target_Lat = ExtIn[45];
      sys_extin.Target_Long = ExtIn[46];
      sys_extin.Target_Alt = ExtIn[47];
      sys_extin.Slew_Angle = ExtIn[48];
      sys_extin.SS_MisAln_x = ExtIn[49];
      sys_extin.SS_MisAln_y = ExtIn[50];
      sys_extin.SS_MisAln_z = ExtIn[51];
      sys_extin.Gyro_MisAln_x = ExtIn[52];
      sys_extin.Gyro_MisAln_y = ExtIn[53];
      sys_extin.Gyro_MisAln_z = ExtIn[54];
      sys_extin.Gyro_Drift_x = ExtIn[55];
      sys_extin.Gyro_Drift_y = ExtIn[56];
      sys_extin.Gyro_Drift_z = ExtIn[57];
      sys_extin.Gyro_AutoCal = ExtIn[58];
      sys_extin.MTR_Enable_Mode4 = ExtIn[59];
      sys_extin.Drag_Coeff = ExtIn[60];
      sys_extin.SUNVEC0 = ExtIn[61];
      sys_extin.SUNVEC1 = ExtIn[62];
      sys_extin.SUNVEC2 = ExtIn[63];
      sys_extin.ECI_POS_X = ExtIn[64];
      sys_extin.ECI_POS_Y = ExtIn[65];
      sys_extin.ECI_POS_Z = ExtIn[66];
      sys_extin.ECI_VEL_X = ExtIn[67];
      sys_extin.ECI_VEL_Y = ExtIn[68];
      sys_extin.ECI_VEL_Z = ExtIn[69];
      sys_extin.GHA = ExtIn[70];
      sys_extin.nT_ECI_x = ExtIn[71];
      sys_extin.nT_ECI_y = ExtIn[72];
      sys_extin.nT_ECI_z = ExtIn[73];
      sys_extin.Measured_Gyro_X = ExtIn[74];
      sys_extin.Measured_Gyro_Y = ExtIn[75];
      sys_extin.Measured_Gyro_Z = ExtIn[76];
      sys_extin.Measured_Mag_X = ExtIn[77];
      sys_extin.Measured_Mag_Y = ExtIn[78];
      sys_extin.Measured_Mag_Z = ExtIn[79];
      sys_extin.Measured_SS_x = ExtIn[80];
      sys_extin.Measured_SS_y = ExtIn[81];
      sys_extin.Measured_SS_z = ExtIn[82];
      sys_extin.Sun_Present_Flag = ExtIn[83];
      sys_extin.Measured_RW_omega_x = ExtIn[84];
      sys_extin.Measured_RW_omega_y = ExtIn[85];
      sys_extin.Measured_RW_omega_z = ExtIn[86];
      sys_extin.Wheel_Disable_Mode5 = ExtIn[87];
      sys_extin.Spare2 = ExtIn[88];
      sys_extin.Spare3 = ExtIn[89];


   

   


   /*** Update elapsed time ***/
   ELAPSED_TIME = ((RT_DURATION)TIME_COUNT)*SCHEDULER_INTERVAL;

   /*** Task Scheduling ***/

  for( NTSK=NTASKS; NTSK>=1; NTSK--  )
  {
    TCBptr = &TCB[NTSK];

    switch(TCBptr->TASK_TYPE)
    {
      case CONTINUOUS :
      case PERIODIC :
        if( TCBptr->START == 0 )
        { 
          queueTask(NTSK);
          TCBptr->START  = TCBptr->SCHEDULING_COUNT;
        } else { 
          (TCBptr->START)--;
        } 
      break;

      case ENABLED_PERIODIC :
        if( !TCBptr->ENABLED )
        {
          /*
          ** reset the start count to 0 so that we will run
          ** once, as soon as we are enabled
          */
          TCBptr->START  = 0;
        } else if( TCBptr->START == 0 ) { 
          queueTask(NTSK);
          TCBptr->START  = TCBptr->SCHEDULING_COUNT;
        } else { 
          (TCBptr->START)--;
        } 
      break;

      case TRIGGERED_ANT :
        if( TCBptr->START == 0 )
        {
          queueTask(NTSK);
          TCBptr->START  = 1;
        } 
      break;

      case TRIGGERED_ATR :
        if (TASK_STATE[NTSK] == BLOCKED)
        {
          if( TCBptr->START == 0 )
          {
            queueTask(NTSK);
            TCBptr->OUTPUT = TCB[NTSK].OUTPUT_COUNT;
            TCBptr->START  = 1;
          } 
        } else {
          if( TCBptr->OUTPUT == 0 )
          {
            TASK_STATE[NTSK] = BLOCKED;
            if( TCBptr->START == 0 )
            {
              queueTask(NTSK);
              TCBptr->OUTPUT = TCBptr->OUTPUT_COUNT;
              TCBptr->START  = 1;
            } 
          } else { 
            (TCBptr->OUTPUT)--;
          } 
        }
      break;

      case TRIGGERED_SAF :
        if (TASK_STATE[NTSK] == BLOCKED)
        {
          if( TCBptr->START == 0 )
          {
            queueTask(NTSK);
            TCBptr->OUTPUT = 0;
            TCBptr->START  = 1;
          } 
        } else {
          if( TCBptr->OUTPUT == 0 )
          {
            TASK_STATE[NTSK] = BLOCKED;
            if( TCBptr->START == 0 )
            {
              queueTask(NTSK);
              TCBptr->OUTPUT = 0;
              TCBptr->START  = 1;
            } 
          } 
        }
      break;

      default:
      break;
    } 
 }   /* end for loop */

 /*** Task Dispatching ***/
 for (cnt=1;cnt<=NTASKS; cnt++)
 {
   if( DISPATCH[cnt] )
   {
     DISPATCH[cnt] = FALSE;
     /*
     ** Call as a function for run-to-completion type
     ** simulation
     */
     switch(cnt)
     {
       case 1 : 
                  subsys_1(&subsys_1_in, &subsys_1_out);
                  break;

       default:
       break;
     }

     if( ERROR_FLAG[cnt] == 0 )
     {
       TASK_STATE[cnt] =  IDLE;
     } else {
       /* Signal and Error */
     }
   }
 }

}


/*-----------*
 *-- MAIN? --*
 *-----------*/
void initModel(void)
/* void initModel(int *NumInPtr, float** ExtInPtr, int *NumOutPtr, float** ExtOutPtr) */

{
  /* Initialize the External Inputs array */
  //memset(ExtIn, 0, sizeof(ExtIn));

  /* Set the values of the parameterized data that the main program needs
  *NumInPtr = NUMIN;
  *NumOutPtr = NUMOUT;
  *ExtInPtr = ExtIn;
  *ExtOutPtr = ExtOut; */

  /*** Initialize Scheduler ***/
  Init_Scheduler();

  /* Initialize Application Data */
  Init_Application_Data();

}


