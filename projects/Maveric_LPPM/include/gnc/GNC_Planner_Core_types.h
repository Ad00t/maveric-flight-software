/*
 * GNC_Planner_Core_types.h
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

#ifndef GNC_Planner_Core_types_h_
#define GNC_Planner_Core_types_h_
#include "rtwtypes.h"
#ifndef DEFINED_TYPEDEF_FOR_ADCSMode_
#define DEFINED_TYPEDEF_FOR_ADCSMode_

typedef enum {
  SAFE = 0,                            /* Default value */
  DETUMBLE,
  SUNPOINT,
  FINEPOINTING,
  LVLH,
  TARGETTRACKING,
  SUNSPIN,
  MANUAL
} ADCSMode;

#endif

#ifndef DEFINED_TYPEDEF_FOR_BasicModeType_
#define DEFINED_TYPEDEF_FOR_BasicModeType_

typedef enum {
  BasicModeType_None = 0,              /* Default value */
  BasicModeType_SAFE,
  BasicModeType_SUNSPIN,
  BasicModeType_DETUMBLE
} BasicModeType;

#endif

/* Forward declaration for rtModel */
typedef struct tag_RTM_GNC_Planner_Core_T RT_MODEL_GNC_Planner_Core_T;

#endif                                 /* GNC_Planner_Core_types_h_ */
