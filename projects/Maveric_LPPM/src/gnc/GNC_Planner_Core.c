/*
 * GNC_Planner_Core.c
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

#include "GNC_Planner_Core.h"
#include "rtwtypes.h"
#include "GNC_Planner_Core_types.h"
#include <math.h>
#include <string.h>

/* Named constants for Chart: '<S1>/GNC State Machine' */
#define GNC_Planner_Core_IN_Basic      ((uint8_T)1U)
#define GNC_Planner_Core_IN_Pointing   ((uint8_T)2U)
#define GNC_Planner__IN_NO_ACTIVE_CHILD ((uint8_T)0U)

/* Block states (default storage) */
DW_GNC_Planner_Core_T GNC_Planner_Core_DW;

/* External inputs (root inport signals with default storage) */
ExtU_GNC_Planner_Core_T GNC_Planner_Core_U;

/* External outputs (root outports fed by signals with default storage) */
ExtY_GNC_Planner_Core_T GNC_Planner_Core_Y;

/* Real-time model */
static RT_MODEL_GNC_Planner_Core_T GNC_Planner_Core_M_;
RT_MODEL_GNC_Planner_Core_T *const GNC_Planner_Core_M = &GNC_Planner_Core_M_;

/* Model step function */
void GNC_Planner_Core_step(void)
{
  real_T y[3];
  int16_T b_k;
  boolean_T c_out;
  boolean_T exitg1;
  boolean_T rtb_SUN;
  boolean_T rtb_TUMB;
  ADCSMode rtb_mode;

  /* Outputs for Atomic SubSystem: '<Root>/GNC_Planner_Core' */
  /* MATLAB Function: '<S1>/MATLAB Function' incorporates:
   *  Inport: '<Root>/STAT'
   */
  switch ((uint32_T)((uint16_T)GNC_Planner_Core_U.STAT_out & 7U)) {
   case 0UL:
    rtb_mode = SAFE;
    break;

   case 1UL:
    rtb_mode = DETUMBLE;
    break;

   case 2UL:
    rtb_mode = SUNPOINT;
    break;

   case 3UL:
    rtb_mode = FINEPOINTING;
    break;

   case 4UL:
    rtb_mode = LVLH;
    break;

   case 5UL:
    rtb_mode = TARGETTRACKING;
    break;

   case 6UL:
    rtb_mode = SUNSPIN;
    break;

   default:
    rtb_mode = MANUAL;
    break;
  }

  rtb_TUMB = (((uint16_T)GNC_Planner_Core_U.STAT_out & 2048U) != 0U);
  rtb_SUN = (((uint16_T)GNC_Planner_Core_U.STAT_out & 8192U) != 0U);

  /* End of MATLAB Function: '<S1>/MATLAB Function' */

  /* Chart: '<S1>/GNC State Machine' incorporates:
   *  Inport: '<Root>/GNC_Planner_Custom_Pointing'
   *  Inport: '<Root>/RATE'
   *  Outport: '<Root>/GNC_Planner_Mode'
   */
  if (GNC_Planner_Core_DW.is_active_c3_GNC_Planner_Core == 0) {
    GNC_Planner_Core_DW.is_active_c3_GNC_Planner_Core = 1U;
    GNC_Planner_Core_DW.is_c3_GNC_Planner_Core = GNC_Planner_Core_IN_Basic;

    /* Outport: '<Root>/GNC_Planner_Mode' */
    GNC_Planner_Core_Y.GNC_Planner_Mode = BasicModeType_SAFE;
    GNC_Planner_Core_DW.expected_mode = SAFE;

    /* Outport: '<Root>/cmd_reset' */
    GNC_Planner_Core_Y.cmd_reset = false;
    GNC_Planner_Core_DW.unexpected_reset = false;
  } else if (GNC_Planner_Core_DW.is_c3_GNC_Planner_Core ==
             GNC_Planner_Core_IN_Basic) {
    if (GNC_Planner_Core_U.GNC_Planner_Custom_Pointing) {
      /* Outport: '<Root>/commanded_mode' */
      GNC_Planner_Core_Y.commanded_mode = MANUAL;

      /* Outport: '<Root>/GNC_Planner_Mode' */
      GNC_Planner_Core_Y.GNC_Planner_Mode = BasicModeType_None;
      GNC_Planner_Core_DW.is_c3_GNC_Planner_Core = GNC_Planner_Core_IN_Pointing;
      GNC_Planner_Core_DW.expected_mode = MANUAL;
    } else if (rtb_mode != GNC_Planner_Core_DW.expected_mode) {
      if (rtb_mode == DETUMBLE) {
        /* Outport: '<Root>/detumble_count' */
        GNC_Planner_Core_Y.detumble_count++;

        /* Outport: '<Root>/GNC_Planner_Mode' */
        GNC_Planner_Core_Y.GNC_Planner_Mode = BasicModeType_DETUMBLE;

        /* Outport: '<Root>/commanded_mode' */
        GNC_Planner_Core_Y.commanded_mode = DETUMBLE;
        GNC_Planner_Core_DW.expected_mode = DETUMBLE;
      } else if (rtb_mode == SUNSPIN) {
        /* Outport: '<Root>/GNC_Planner_Mode' */
        GNC_Planner_Core_Y.GNC_Planner_Mode = BasicModeType_SUNSPIN;

        /* Outport: '<Root>/commanded_mode' */
        GNC_Planner_Core_Y.commanded_mode = SUNSPIN;
        GNC_Planner_Core_DW.expected_mode = SUNSPIN;
      } else {
        /* Outport: '<Root>/reset_count' */
        GNC_Planner_Core_Y.reset_count++;

        /* Outport: '<Root>/GNC_Planner_Mode' */
        GNC_Planner_Core_Y.GNC_Planner_Mode = BasicModeType_SAFE;
        GNC_Planner_Core_DW.expected_mode = SAFE;

        /* Outport: '<Root>/cmd_reset' */
        GNC_Planner_Core_Y.cmd_reset = false;
        GNC_Planner_Core_DW.unexpected_reset = false;
      }
    } else {
      switch (GNC_Planner_Core_Y.GNC_Planner_Mode) {
       case BasicModeType_DETUMBLE:
        if (rtb_SUN && (!rtb_TUMB)) {
          GNC_Planner_Core_Y.GNC_Planner_Mode = BasicModeType_SUNSPIN;

          /* Outport: '<Root>/commanded_mode' */
          GNC_Planner_Core_Y.commanded_mode = SUNSPIN;
          GNC_Planner_Core_DW.expected_mode = SUNSPIN;
        }
        break;

       case BasicModeType_SAFE:
        y[0] = fabs(GNC_Planner_Core_U.w[0]);
        y[1] = fabs(GNC_Planner_Core_U.w[1]);
        y[2] = fabs(GNC_Planner_Core_U.w[2]);
        c_out = true;
        b_k = 0;
        exitg1 = false;
        while ((!exitg1) && (b_k < 3)) {
          if (y[b_k] > 360.0) {
            c_out = false;
            exitg1 = true;
          } else {
            b_k++;
          }
        }

        if (c_out) {
          if (rtb_SUN && (!rtb_TUMB)) {
            GNC_Planner_Core_Y.GNC_Planner_Mode = BasicModeType_SUNSPIN;

            /* Outport: '<Root>/commanded_mode' */
            GNC_Planner_Core_Y.commanded_mode = SUNSPIN;
            GNC_Planner_Core_DW.expected_mode = SUNSPIN;
          } else {
            GNC_Planner_Core_Y.GNC_Planner_Mode = BasicModeType_DETUMBLE;

            /* Outport: '<Root>/commanded_mode' */
            GNC_Planner_Core_Y.commanded_mode = DETUMBLE;
            GNC_Planner_Core_DW.expected_mode = DETUMBLE;
          }
        }
        break;

       default:
        /* State SUNSPIN */
        break;
      }
    }

    /* case IN_Pointing: */
  } else if ((!GNC_Planner_Core_U.GNC_Planner_Custom_Pointing) || (rtb_mode !=
              GNC_Planner_Core_DW.expected_mode)) {
    /* Outport: '<Root>/commanded_mode' */
    GNC_Planner_Core_Y.commanded_mode = SAFE;
    GNC_Planner_Core_DW.is_c3_GNC_Planner_Core = GNC_Planner_Core_IN_Basic;

    /* Outport: '<Root>/GNC_Planner_Mode' */
    GNC_Planner_Core_Y.GNC_Planner_Mode = BasicModeType_SAFE;
    GNC_Planner_Core_DW.expected_mode = SAFE;

    /* Outport: '<Root>/cmd_reset' */
    GNC_Planner_Core_Y.cmd_reset = false;
    GNC_Planner_Core_DW.unexpected_reset = false;
  }

  /* End of Chart: '<S1>/GNC State Machine' */
  /* End of Outputs for SubSystem: '<Root>/GNC_Planner_Core' */
}

/* Model initialize function */
void GNC_Planner_Core_initialize(void)
{
  /* Registration code */

  /* initialize error status */
  rtmSetErrorStatus(GNC_Planner_Core_M, (NULL));

  /* states (dwork) */
  (void) memset((void *)&GNC_Planner_Core_DW, 0,
                sizeof(DW_GNC_Planner_Core_T));

  /* external inputs */
  (void)memset(&GNC_Planner_Core_U, 0, sizeof(ExtU_GNC_Planner_Core_T));

  /* external outputs */
  (void)memset(&GNC_Planner_Core_Y, 0, sizeof(ExtY_GNC_Planner_Core_T));

  /* SystemInitialize for Atomic SubSystem: '<Root>/GNC_Planner_Core' */
  /* InitializeConditions for Outport: '<Root>/detumble_count' incorporates:
   *  Chart: '<S1>/GNC State Machine'
   */
  GNC_Planner_Core_Y.detumble_count = 0.0;

  /* SystemInitialize for Chart: '<S1>/GNC State Machine' */
  GNC_Planner_Core_DW.expected_mode = SAFE;

  /* InitializeConditions for Outport: '<Root>/reset_count' incorporates:
   *  Chart: '<S1>/GNC State Machine'
   */
  GNC_Planner_Core_Y.reset_count = 0.0;

  /* SystemInitialize for Chart: '<S1>/GNC State Machine' */
  GNC_Planner_Core_DW.unexpected_reset = false;

  /* InitializeConditions for Outport: '<Root>/commanded_mode' incorporates:
   *  Chart: '<S1>/GNC State Machine'
   */
  GNC_Planner_Core_Y.commanded_mode = SAFE;

  /* InitializeConditions for Outport: '<Root>/cmd_reset' incorporates:
   *  Chart: '<S1>/GNC State Machine'
   */
  GNC_Planner_Core_Y.cmd_reset = false;

  /* SystemInitialize for Chart: '<S1>/GNC State Machine' */
  GNC_Planner_Core_DW.is_active_c3_GNC_Planner_Core = 0U;
  GNC_Planner_Core_DW.is_c3_GNC_Planner_Core = GNC_Planner__IN_NO_ACTIVE_CHILD;

  /* InitializeConditions for Outport: '<Root>/GNC_Planner_Mode' incorporates:
   *  Chart: '<S1>/GNC State Machine'
   */
  GNC_Planner_Core_Y.GNC_Planner_Mode = BasicModeType_None;

  /* End of SystemInitialize for SubSystem: '<Root>/GNC_Planner_Core' */
}

/* Model terminate function */
void GNC_Planner_Core_terminate(void)
{
  /* (no terminate code required) */
}
