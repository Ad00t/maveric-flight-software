/*
 * GNC_Planner_Core.h
 *
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * Code generation for model "GNC_Planner_Core".
 *
 * Model version              : 2.96
 * Simulink Coder version : 25.2 (R2025b) 28-Jul-2025
 * C source code generated on : Tue Mar 17 12:33:12 2026
 *
 * Target selection: grt.tlc
 * Note: GRT includes extra infrastructure and instrumentation for prototyping
 * Embedded hardware selection: Custom Processor->Custom Processor
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#ifndef GNC_Planner_Core_h_
#define GNC_Planner_Core_h_
#ifndef GNC_Planner_Core_COMMON_INCLUDES_
#define GNC_Planner_Core_COMMON_INCLUDES_
#include <stdio.h>
#include "rtwtypes.h"
#include "rtw_continuous.h"
#include "rtw_solver.h"
#include "rt_logging.h"
#endif                                 /* GNC_Planner_Core_COMMON_INCLUDES_ */

#include "GNC_Planner_Core_types.h"
#include "rt_nonfinite.h"
#include "rtGetInf.h"
#include "rtGetNaN.h"
#include <float.h>
#include <string.h>
#include <stddef.h>

/* Macros for accessing real-time model data structure */
#ifndef rtmGetFinalTime
#define rtmGetFinalTime(rtm)           ((rtm)->Timing.tFinal)
#endif

#ifndef rtmGetRTWLogInfo
#define rtmGetRTWLogInfo(rtm)          ((rtm)->rtwLogInfo)
#endif

#ifndef rtmGetErrorStatus
#define rtmGetErrorStatus(rtm)         ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
#define rtmSetErrorStatus(rtm, val)    ((rtm)->errorStatus = (val))
#endif

#ifndef rtmGetStopRequested
#define rtmGetStopRequested(rtm)       ((rtm)->Timing.stopRequestedFlag)
#endif

#ifndef rtmSetStopRequested
#define rtmSetStopRequested(rtm, val)  ((rtm)->Timing.stopRequestedFlag = (val))
#endif

#ifndef rtmGetStopRequestedPtr
#define rtmGetStopRequestedPtr(rtm)    (&((rtm)->Timing.stopRequestedFlag))
#endif

#ifndef rtmGetT
#define rtmGetT(rtm)                   ((rtm)->Timing.taskTime0)
#endif

#ifndef rtmGetTFinal
#define rtmGetTFinal(rtm)              ((rtm)->Timing.tFinal)
#endif

#ifndef rtmGetTPtr
#define rtmGetTPtr(rtm)                (&(rtm)->Timing.taskTime0)
#endif

/* Block signals (default storage) */
typedef struct {
  boolean_T Eclipse_Flag;              /* '<S4>/MATLAB Function' */
  boolean_T TUMB;                      /* '<S1>/MATLAB Function' */
  boolean_T SUN;                       /* '<S1>/MATLAB Function' */
} B_GNC_Planner_Core_T;

/* Block states (default storage) for system '<Root>' */
typedef struct {
  satrec_t_GNC_Planner_Core_T satrec;  /* '<S4>/fsw_spg4_step' */
  TLE_Bus_Type cached_bus;             /* '<S4>/parse_tle_string' */
  ADCSMode expected_mode;              /* '<S1>/GNC State Machine' */
  uint8_T cached_str[138];             /* '<S4>/parse_tle_string' */
  uint8_T is_active_c3_GNC_Planner_Core;/* '<S1>/GNC State Machine' */
  uint8_T is_c3_GNC_Planner_Core;      /* '<S1>/GNC State Machine' */
  boolean_T is_initialized;            /* '<S4>/fsw_spg4_step' */
  boolean_T unexpected_reset;          /* '<S1>/GNC State Machine' */
} DW_GNC_Planner_Core_T;

/* External inputs (root inport signals with default storage) */
typedef struct {
  uint32_T STAT_out;                   /* '<Root>/STAT' */
  real_T w[3];                         /* '<Root>/RATE' */
  real_T T_JD;                         /* '<Root>/T_JD' */
  real_T desired_tumble_rate[3];       /* '<Root>/desired_tumble_rate' */
  boolean_T retumble_enabled;          /* '<Root>/retumble_enabled' */
  boolean_T alt_bdot_enabled;          /* '<Root>/alt_bdot_enabled' */
  boolean_T alt_sunspin_enabled;       /* '<Root>/alt_sunspin_enabled' */
  boolean_T GNC_Planner_Custom_Pointing;
                                      /* '<Root>/GNC_Planner_Custom_Pointing' */
  uint8_T TLE_array[138];              /* '<Root>/TLE_array' */
} ExtU_GNC_Planner_Core_T;

/* External outputs (root outports fed by signals with default storage) */
typedef struct {
  ADCSMode commanded_mode;             /* '<Root>/commanded_mode' */
  boolean_T cmd_reset;                 /* '<Root>/cmd_reset' */
  real_T cmd_dipoles[3];               /* '<Root>/cmd_dipoles' */
  real_T detumble_count;               /* '<Root>/detumble_count' */
  real_T reset_count;                  /* '<Root>/reset_count' */
  BasicModeType GNC_Planner_Mode;      /* '<Root>/GNC_Planner_Mode' */
} ExtY_GNC_Planner_Core_T;

/* Real-time Model Data Structure */
struct tag_RTM_GNC_Planner_Core_T {
  const char_T *errorStatus;
  RTWLogInfo *rtwLogInfo;

  /*
   * Timing:
   * The following substructure contains information regarding
   * the timing information for the model.
   */
  struct {
    time_T taskTime0;
    uint32_T clockTick0;
    uint32_T clockTickH0;
    time_T stepSize0;
    time_T tFinal;
    boolean_T stopRequestedFlag;
  } Timing;
};

/* Block signals (default storage) */
extern B_GNC_Planner_Core_T GNC_Planner_Core_B;

/* Block states (default storage) */
extern DW_GNC_Planner_Core_T GNC_Planner_Core_DW;

/* External inputs (root inport signals with default storage) */
extern ExtU_GNC_Planner_Core_T GNC_Planner_Core_U;

/* External outputs (root outports fed by signals with default storage) */
extern ExtY_GNC_Planner_Core_T GNC_Planner_Core_Y;

/* Model entry point functions */
extern void GNC_Planner_Core_initialize(void);
extern void GNC_Planner_Core_step(void);
extern void GNC_Planner_Core_terminate(void);

/* Real-time Model object */
extern RT_MODEL_GNC_Planner_Core_T *const GNC_Planner_Core_M;

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Note that this particular code originates from a subsystem build,
 * and has its own system numbers different from the parent model.
 * Refer to the system hierarchy for this subsystem below, and use the
 * MATLAB hilite_system command to trace the generated code back
 * to the parent model.  For example,
 *
 * hilite_system('GNC_Planner/GNC_Planner_Core')    - opens subsystem GNC_Planner/GNC_Planner_Core
 * hilite_system('GNC_Planner/GNC_Planner_Core/Kp') - opens and selects block Kp
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'GNC_Planner'
 * '<S1>'   : 'GNC_Planner/GNC_Planner_Core'
 * '<S2>'   : 'GNC_Planner/GNC_Planner_Core/GNC State Machine'
 * '<S3>'   : 'GNC_Planner/GNC_Planner_Core/MATLAB Function'
 * '<S4>'   : 'GNC_Planner/GNC_Planner_Core/Subsystem'
 * '<S5>'   : 'GNC_Planner/GNC_Planner_Core/Subsystem/MATLAB Function'
 * '<S6>'   : 'GNC_Planner/GNC_Planner_Core/Subsystem/fsw_spg4_step'
 * '<S7>'   : 'GNC_Planner/GNC_Planner_Core/Subsystem/parse_tle_string'
 */
#endif                                 /* GNC_Planner_Core_h_ */
