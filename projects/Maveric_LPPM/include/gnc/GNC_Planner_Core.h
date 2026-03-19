/*
 * GNC_Planner_Core.h
 *
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * Code generation for model "GNC_Planner_Core".
 *
 * Model version              : 2.114
 * Simulink Coder version : 25.2 (R2025b) 28-Jul-2025
 * C source code generated on : Wed Mar 18 13:23:25 2026
 *
 * Target selection: grt.tlc
 * Note: GRT includes extra infrastructure and instrumentation for prototyping
 * Embedded hardware selection: Custom Processor->Custom Processor
 * Code generation objective: Execution efficiency
 * Validation result: Not run
 */

#ifndef GNC_Planner_Core_h_
#define GNC_Planner_Core_h_
#ifndef GNC_Planner_Core_COMMON_INCLUDES_
#define GNC_Planner_Core_COMMON_INCLUDES_
#include "rtwtypes.h"
#include "rtw_continuous.h"
#include "rtw_solver.h"
#endif                                 /* GNC_Planner_Core_COMMON_INCLUDES_ */

#include "GNC_Planner_Core_types.h"
#include <stddef.h>
#include <string.h>

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
#define rtmGetErrorStatus(rtm)         ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
#define rtmSetErrorStatus(rtm, val)    ((rtm)->errorStatus = (val))
#endif

/* Block states (default storage) for system '<Root>' */
typedef struct {
  ADCSMode expected_mode;              /* '<S1>/GNC State Machine' */
  uint8_T is_active_c3_GNC_Planner_Core;/* '<S1>/GNC State Machine' */
  uint8_T is_c3_GNC_Planner_Core;      /* '<S1>/GNC State Machine' */
  boolean_T unexpected_reset;          /* '<S1>/GNC State Machine' */
} DW_GNC_Planner_Core_T;

/* External inputs (root inport signals with default storage) */
typedef struct {
  uint32_T STAT_out;                   /* '<Root>/STAT' */
  real_T w[3];                         /* '<Root>/RATE' */
  boolean_T GNC_Planner_Custom_Pointing;
                                      /* '<Root>/GNC_Planner_Custom_Pointing' */
} ExtU_GNC_Planner_Core_T;

/* External outputs (root outports fed by signals with default storage) */
typedef struct {
  ADCSMode commanded_mode;             /* '<Root>/commanded_mode' */
  boolean_T cmd_reset;                 /* '<Root>/cmd_reset' */
  real_T detumble_count;               /* '<Root>/detumble_count' */
  real_T reset_count;                  /* '<Root>/reset_count' */
  BasicModeType GNC_Planner_Mode;      /* '<Root>/GNC_Planner_Mode' */
} ExtY_GNC_Planner_Core_T;

/* Real-time Model Data Structure */
struct tag_RTM_GNC_Planner_Core_T {
  const char_T *errorStatus;
};

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
 * hilite_system('GNC_Planner_Simplified/GNC_Planner_Core')    - opens subsystem GNC_Planner_Simplified/GNC_Planner_Core
 * hilite_system('GNC_Planner_Simplified/GNC_Planner_Core/Kp') - opens and selects block Kp
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'GNC_Planner_Simplified'
 * '<S1>'   : 'GNC_Planner_Simplified/GNC_Planner_Core'
 * '<S2>'   : 'GNC_Planner_Simplified/GNC_Planner_Core/GNC State Machine'
 * '<S3>'   : 'GNC_Planner_Simplified/GNC_Planner_Core/MATLAB Function'
 */
#endif                                 /* GNC_Planner_Core_h_ */
