/*
 * GNC_Planner_Core_types.h
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
  BasicModeType_Retumble,
  BasicModeType_DETUMBLE
} BasicModeType;

#endif

#ifndef DEFINED_TYPEDEF_FOR_TLE_Bus_Type_
#define DEFINED_TYPEDEF_FOR_TLE_Bus_Type_

typedef struct {
  real_T epoch_days_1950;
  real_T bstar;
  real_T ndot;
  real_T nddot;
  real_T ecco;
  real_T argpo;
  real_T inclo;
  real_T mo;
  real_T no_kozai;
  real_T nodeo;
  real_T epoch_JD;
} TLE_Bus_Type;

#endif

/* Custom Type definition for MATLAB Function: '<S4>/fsw_spg4_step' */
#ifndef struct_tag_siqtL0olslAz0Re7SMjm43B
#define struct_tag_siqtL0olslAz0Re7SMjm43B

struct tag_siqtL0olslAz0Re7SMjm43B
{
  real_T error;
  real_T satnum;
  real_T epochyr;
  real_T epochdays;
  real_T ndot;
  real_T nddot;
  real_T bstar;
  real_T ecco;
  real_T argpo;
  real_T inclo;
  real_T mo;
  real_T no_kozai;
  real_T nodeo;
  real_T no;
  real_T no_unkozai;
  real_T a;
  real_T alta;
  real_T altp;
  real_T ephtype;
  char_T classification;
  char_T operationmode;
  char_T init;
  char_T method;
  real_T isimp;
  real_T aycof;
  real_T con41;
  real_T cc1;
  real_T cc4;
  real_T cc5;
  real_T d2;
  real_T d3;
  real_T d4;
  real_T delmo;
  real_T eta;
  real_T argpdot;
  real_T omgcof;
  real_T sinmao;
  real_T t;
  real_T t2;
  real_T t3;
  real_T t4;
  real_T t5;
  real_T t2cof;
  real_T t3cof;
  real_T t4cof;
  real_T t5cof;
  real_T x1mth2;
  real_T x7thm1;
  real_T mdot;
  real_T nodedot;
  real_T xlcof;
  real_T xmcof;
  real_T nodecf;
  real_T irez;
  real_T d2201;
  real_T d2211;
  real_T d3210;
  real_T d3222;
  real_T d4410;
  real_T d4422;
  real_T d5220;
  real_T d5232;
  real_T d5421;
  real_T d5433;
  real_T dedt;
  real_T del1;
  real_T del2;
  real_T del3;
  real_T didt;
  real_T dmdt;
  real_T dnodt;
  real_T domdt;
  real_T e3;
  real_T ee2;
  real_T peo;
  real_T pgho;
  real_T pho;
  real_T pinco;
  real_T plo;
  real_T se2;
  real_T se3;
  real_T sgh2;
  real_T sgh3;
  real_T sgh4;
  real_T sh2;
  real_T sh3;
  real_T si2;
  real_T si3;
  real_T sl2;
  real_T sl3;
  real_T sl4;
  real_T gsto;
  real_T xfact;
  real_T xgh2;
  real_T xgh3;
  real_T xgh4;
  real_T xh2;
  real_T xh3;
  real_T xi2;
  real_T xi3;
  real_T xl2;
  real_T xl3;
  real_T xl4;
  real_T xlamo;
  real_T zmol;
  real_T zmos;
  real_T atime;
  real_T xli;
  real_T xni;
  real_T am;
  real_T em;
  real_T im;
  real_T Om;
  real_T om;
  real_T mm;
  real_T nm;
  real_T t0;
  real_T tumin;
  real_T mu;
  real_T radiusearthkm;
  real_T xke;
  real_T j2;
  real_T j3;
  real_T j4;
  real_T j3oj2;
};

#endif                                 /* struct_tag_siqtL0olslAz0Re7SMjm43B */

#ifndef typedef_siqtL0olslAz0Re7SMjm43B_GNC_P_T
#define typedef_siqtL0olslAz0Re7SMjm43B_GNC_P_T

typedef struct tag_siqtL0olslAz0Re7SMjm43B siqtL0olslAz0Re7SMjm43B_GNC_P_T;

#endif                             /* typedef_siqtL0olslAz0Re7SMjm43B_GNC_P_T */

#ifndef struct_tag_0g50pZ1hIOjl63AO3oltHE
#define struct_tag_0g50pZ1hIOjl63AO3oltHE

struct tag_0g50pZ1hIOjl63AO3oltHE
{
  real_T error;
  real_T satnum;
  real_T epochyr;
  real_T epochdays;
  real_T ndot;
  real_T nddot;
  real_T bstar;
  real_T ecco;
  real_T argpo;
  real_T inclo;
  real_T mo;
  real_T no_kozai;
  real_T nodeo;
  real_T no;
  real_T no_unkozai;
  real_T a;
  real_T alta;
  real_T altp;
  real_T ephtype;
  char_T classification;
  char_T operationmode;
  char_T init;
  char_T method;
  real_T isimp;
  real_T aycof;
  real_T con41;
  real_T cc1;
  real_T cc4;
  real_T cc5;
  real_T d2;
  real_T d3;
  real_T d4;
  real_T delmo;
  real_T eta;
  real_T argpdot;
  real_T omgcof;
  real_T sinmao;
  real_T t;
  real_T t2;
  real_T t3;
  real_T t4;
  real_T t5;
  real_T t2cof;
  real_T t3cof;
  real_T t4cof;
  real_T t5cof;
  real_T x1mth2;
  real_T x7thm1;
  real_T mdot;
  real_T nodedot;
  real_T xlcof;
  real_T xmcof;
  real_T nodecf;
  real_T irez;
  real_T d2201;
  real_T d2211;
  real_T d3210;
  real_T d3222;
  real_T d4410;
  real_T d4422;
  real_T d5220;
  real_T d5232;
  real_T d5421;
  real_T d5433;
  real_T dedt;
  real_T del1;
  real_T del2;
  real_T del3;
  real_T didt;
  real_T dmdt;
  real_T dnodt;
  real_T domdt;
  real_T e3;
  real_T ee2;
  real_T peo;
  real_T pgho;
  real_T pho;
  real_T pinco;
  real_T plo;
  real_T se2;
  real_T se3;
  real_T sgh2;
  real_T sgh3;
  real_T sgh4;
  real_T sh2;
  real_T sh3;
  real_T si2;
  real_T si3;
  real_T sl2;
  real_T sl3;
  real_T sl4;
  real_T gsto;
  real_T xfact;
  real_T xgh2;
  real_T xgh3;
  real_T xgh4;
  real_T xh2;
  real_T xh3;
  real_T xi2;
  real_T xi3;
  real_T xl2;
  real_T xl3;
  real_T xl4;
  real_T xlamo;
  real_T zmol;
  real_T zmos;
  real_T atime;
  real_T xli;
  real_T xni;
  real_T am;
  real_T em;
  real_T im;
  real_T Om;
  real_T om;
  real_T mm;
  real_T nm;
  real_T t0;
  real_T tumin;
  real_T mu;
  real_T radiusearthkm;
  real_T xke;
  real_T j2;
  real_T j3;
  real_T j4;
  real_T j3oj2;
};

#endif                                 /* struct_tag_0g50pZ1hIOjl63AO3oltHE */

#ifndef typedef_satrec_t_GNC_Planner_Core_T
#define typedef_satrec_t_GNC_Planner_Core_T

typedef struct tag_0g50pZ1hIOjl63AO3oltHE satrec_t_GNC_Planner_Core_T;

#endif                                 /* typedef_satrec_t_GNC_Planner_Core_T */

/* Forward declaration for rtModel */
typedef struct tag_RTM_GNC_Planner_Core_T RT_MODEL_GNC_Planner_Core_T;

#endif                                 /* GNC_Planner_Core_types_h_ */
