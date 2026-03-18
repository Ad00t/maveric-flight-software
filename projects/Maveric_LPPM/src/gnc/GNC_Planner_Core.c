/*
 * GNC_Planner_Core.c
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

#include "GNC_Planner_Core.h"
#include "rtwtypes.h"
#include "GNC_Planner_Core_types.h"
#include <string.h>
#include <math.h>
#include "GNC_Planner_Core_private.h"
#include "rt_nonfinite.h"
#include <float.h>
#include "rt_defines.h"

/* Named constants for Chart: '<S1>/GNC State Machine' */
#define GNC_Planner_Core_IN_Basic      ((uint8_T)1U)
#define GNC_Planner_Core_IN_Pointing   ((uint8_T)2U)
#define GNC_Planner__IN_NO_ACTIVE_CHILD ((uint8_T)0U)

/* Block signals (default storage) */
B_GNC_Planner_Core_T GNC_Planner_Core_B;

/* Block states (default storage) */
DW_GNC_Planner_Core_T GNC_Planner_Core_DW;

/* External inputs (root inport signals with default storage) */
ExtU_GNC_Planner_Core_T GNC_Planner_Core_U;

/* External outputs (root outports fed by signals with default storage) */
ExtY_GNC_Planner_Core_T GNC_Planner_Core_Y;

/* Real-time model */
static RT_MODEL_GNC_Planner_Core_T GNC_Planner_Core_M_;
RT_MODEL_GNC_Planner_Core_T *const GNC_Planner_Core_M = &GNC_Planner_Core_M_;

/* Forward declaration for local functions */
static boolean_T GNC_Planner_Core_isUnitImag(const char_T s[2], int16_T k,
  int16_T n);
static void GNC_Planner_Core_readNonFinite(const char_T s[2], int16_T *k,
  int16_T n, boolean_T *b_finite, real_T *fv);
static boolean_T GNC_Planner_Core_copydigits(char_T s1[4], int16_T *idx, const
  char_T s[2], int16_T *k, int16_T n, boolean_T allowpoint);
static boolean_T GNC_Planner_Core_copyexponent(char_T s1[4], int16_T *idx, const
  char_T s[2], int16_T *k, int16_T n);
static void GNC_Planner_Core_copysign(char_T s1[4], int16_T *idx, const char_T
  s[2], int16_T *k, int16_T n, boolean_T *foundsign, boolean_T *success);
static void GNC_Planner_Core_readfloat_j(char_T s1[4], int16_T *idx, const
  char_T s[2], int16_T *k, boolean_T *isimag, boolean_T *b_finite, real_T *nfv,
  boolean_T *foundsign, boolean_T *success);
static void GNC_Planner_Core_readfloat(const char_T s[2], int16_T *k, char_T s1
  [4], int16_T *idx, boolean_T *isimag, boolean_T *b_finite, real_T *nfv,
  boolean_T *foundsign, boolean_T *success);
static creal_T GNC_Planner_Core_str2double(const char_T s[2]);
static boolean_T GNC_Planner_Core_isUnitImag_e(const char_T s[12], int16_T k,
  int16_T n);
static void GNC_Planner_Cor_readNonFinite_o(const char_T s[12], int16_T *k,
  int16_T n, boolean_T *b_finite, real_T *fv);
static boolean_T GNC_Planner_Core_copydigits_k(char_T s1[14], int16_T *idx,
  const char_T s[12], int16_T *k, int16_T n, boolean_T allowpoint);
static boolean_T GNC_Planner_Core_copyexponent_e(char_T s1[14], int16_T *idx,
  const char_T s[12], int16_T *k, int16_T n);
static void GNC_Planner_Core_copysign_l(char_T s1[14], int16_T *idx, const
  char_T s[12], int16_T *k, int16_T n, boolean_T *foundsign, boolean_T *success);
static void GNC_Planner_Core_readfloat_jrw(char_T s1[14], int16_T *idx, const
  char_T s[12], int16_T *k, boolean_T *isimag, boolean_T *b_finite, real_T *nfv,
  boolean_T *foundsign, boolean_T *success);
static void GNC_Planner_Core_readfloat_jr(const char_T s[12], int16_T *k, char_T
  s1[14], int16_T *idx, boolean_T *isimag, boolean_T *b_finite, real_T *nfv,
  boolean_T *foundsign, boolean_T *success);
static creal_T GNC_Planner_Core_str2double_e(const char_T s[12]);
static boolean_T GNC_Planner_Core_isUnitImag_es(const char_T s[10], int16_T k,
  int16_T n);
static void GNC_Planner_Co_readNonFinite_o3(const char_T s[10], int16_T *k,
  int16_T n, boolean_T *b_finite, real_T *fv);
static boolean_T GNC_Planner_Core_copydigits_kg(char_T s1[12], int16_T *idx,
  const char_T s[10], int16_T *k, int16_T n, boolean_T allowpoint);
static boolean_T GNC_Planner_Cor_copyexponent_ea(char_T s1[12], int16_T *idx,
  const char_T s[10], int16_T *k, int16_T n);
static void GNC_Planner_Core_copysign_l1(char_T s1[12], int16_T *idx, const
  char_T s[10], int16_T *k, int16_T n, boolean_T *foundsign, boolean_T *success);
static void GNC_Planner_Cor_readfloat_jrw1f(char_T s1[12], int16_T *idx, const
  char_T s[10], int16_T *k, boolean_T *isimag, boolean_T *b_finite, real_T *nfv,
  boolean_T *foundsign, boolean_T *success);
static void GNC_Planner_Core_readfloat_jrw1(const char_T s[10], int16_T *k,
  char_T s1[12], int16_T *idx, boolean_T *isimag, boolean_T *b_finite, real_T
  *nfv, boolean_T *foundsign, boolean_T *success);
static creal_T GNC_Planner_Core_str2double_ek(const char_T s[10]);
static boolean_T GNC_Planner_Core_isUnitImag_esl(const char_T s[6], int16_T k,
  int16_T n);
static void GNC_Planner_C_readNonFinite_o30(const char_T s[6], int16_T *k,
  int16_T n, boolean_T *b_finite, real_T *fv);
static boolean_T GNC_Planner_Core_copydigits_kgo(char_T s1[8], int16_T *idx,
  const char_T s[6], int16_T *k, int16_T n, boolean_T allowpoint);
static boolean_T GNC_Planner_Co_copyexponent_eat(char_T s1[8], int16_T *idx,
  const char_T s[6], int16_T *k, int16_T n);
static void GNC_Planner_Core_copysign_l1u(char_T s1[8], int16_T *idx, const
  char_T s[6], int16_T *k, int16_T n, boolean_T *foundsign, boolean_T *success);
static void GNC_Planner_C_readfloat_jrw1fqe(char_T s1[8], int16_T *idx, const
  char_T s[6], int16_T *k, boolean_T *isimag, boolean_T *b_finite, real_T *nfv,
  boolean_T *foundsign, boolean_T *success);
static void GNC_Planner_Co_readfloat_jrw1fq(const char_T s[6], int16_T *k,
  char_T s1[8], int16_T *idx, boolean_T *isimag, boolean_T *b_finite, real_T
  *nfv, boolean_T *foundsign, boolean_T *success);
static creal_T GNC_Planner_Core_str2double_ekr(const char_T s[6]);
static boolean_T GNC_Planner_Cor_isUnitImag_esli(const char_T s[8], int16_T k,
  int16_T n);
static void GNC_Planner__readNonFinite_o30p(const char_T s[8], int16_T *k,
  int16_T n, boolean_T *b_finite, real_T *fv);
static boolean_T GNC_Planner_Cor_copydigits_kgor(char_T s1[10], int16_T *idx,
  const char_T s[8], int16_T *k, int16_T n, boolean_T allowpoint);
static boolean_T GNC_Planner_C_copyexponent_eato(char_T s1[10], int16_T *idx,
  const char_T s[8], int16_T *k, int16_T n);
static void GNC_Planner_Core_copysign_l1ub(char_T s1[10], int16_T *idx, const
  char_T s[8], int16_T *k, int16_T n, boolean_T *foundsign, boolean_T *success);
static void GNC_Planner_readfloat_jrw1fqedp(char_T s1[10], int16_T *idx, const
  char_T s[8], int16_T *k, boolean_T *isimag, boolean_T *b_finite, real_T *nfv,
  boolean_T *foundsign, boolean_T *success);
static void GNC_Planner__readfloat_jrw1fqed(const char_T s[8], int16_T *k,
  char_T s1[10], int16_T *idx, boolean_T *isimag, boolean_T *b_finite, real_T
  *nfv, boolean_T *foundsign, boolean_T *success);
static creal_T GNC_Planner_Cor_str2double_ekr3(const char_T s[8]);
static boolean_T GNC_Planner_Co_isUnitImag_eslih(const char_T s[7], int16_T k,
  int16_T n);
static void GNC_Planner_readNonFinite_o30pi(const char_T s[7], int16_T *k,
  int16_T n, boolean_T *b_finite, real_T *fv);
static boolean_T GNC_Planner_Co_copydigits_kgorr(char_T s1[9], int16_T *idx,
  const char_T s[7], int16_T *k, int16_T n, boolean_T allowpoint);
static boolean_T GNC_Planner__copyexponent_eaton(char_T s1[9], int16_T *idx,
  const char_T s[7], int16_T *k, int16_T n);
static void GNC_Planner_Core_copysign_l1uba(char_T s1[9], int16_T *idx, const
  char_T s[7], int16_T *k, int16_T n, boolean_T *foundsign, boolean_T *success);
static void GNC_Plann_readfloat_jrw1fqedprt(char_T s1[9], int16_T *idx, const
  char_T s[7], int16_T *k, boolean_T *isimag, boolean_T *b_finite, real_T *nfv,
  boolean_T *foundsign, boolean_T *success);
static void GNC_Planne_readfloat_jrw1fqedpr(const char_T s[7], int16_T *k,
  char_T s1[9], int16_T *idx, boolean_T *isimag, boolean_T *b_finite, real_T
  *nfv, boolean_T *foundsign, boolean_T *success);
static creal_T GNC_Planner_Co_str2double_ekr3v(const char_T s[7]);
static boolean_T GNC_Planner_C_isUnitImag_eslihp(const char_T s[17], int16_T k,
  int16_T n);
static void GNC_Planne_readNonFinite_o30pia(const char_T s[17], int16_T *k,
  int16_T n, boolean_T *b_finite, real_T *fv);
static boolean_T GNC_Planner_C_copydigits_kgorrr(char_T s1[19], int16_T *idx,
  const char_T s[17], int16_T *k, int16_T n, boolean_T allowpoint);
static boolean_T GNC_Planner_copyexponent_eatona(char_T s1[19], int16_T *idx,
  const char_T s[17], int16_T *k, int16_T n);
static void GNC_Planner_Cor_copysign_l1ubag(char_T s1[19], int16_T *idx, const
  char_T s[17], int16_T *k, int16_T n, boolean_T *foundsign, boolean_T *success);
static void GNC_Pla_readfloat_jrw1fqedprtkv(char_T s1[19], int16_T *idx, const
  char_T s[17], int16_T *k, boolean_T *isimag, boolean_T *b_finite, real_T *nfv,
  boolean_T *foundsign, boolean_T *success);
static void GNC_Plan_readfloat_jrw1fqedprtk(const char_T s[17], int16_T *k,
  char_T s1[19], int16_T *idx, boolean_T *isimag, boolean_T *b_finite, real_T
  *nfv, boolean_T *foundsign, boolean_T *success);
static creal_T GNC_Planner_C_str2double_ekr3v5(const char_T s[17]);
static void GNC_Planner_Core_initl(real_T ecco, real_T epoch, real_T inclo,
  real_T no_kozai, real_T *ainv, real_T *ao, real_T *con41, real_T *con42,
  real_T *cosio, real_T *cosio2, real_T *eccsq, real_T *omeosq, real_T *posq,
  real_T *rp, real_T *rteosq, real_T *sinio, real_T *gsto, real_T *no_unkozai);
static void GNC_Planner_Core_dscom(real_T epoch, real_T ep, real_T argpp, real_T
  inclp, real_T nodep, real_T np, real_T *sinim, real_T *cosim, real_T *sinomm,
  real_T *cosomm, real_T *snodm, real_T *cnodm, real_T *day, real_T *e3, real_T *
  ee2, real_T *em, real_T *emsq, real_T *gam, real_T *rtemsq, real_T *se2,
  real_T *se3, real_T *sgh2, real_T *sgh3, real_T *sgh4, real_T *sh2, real_T
  *sh3, real_T *si2, real_T *si3, real_T *sl2, real_T *sl3, real_T *sl4, real_T *
  s1, real_T *s2, real_T *s3, real_T *s4, real_T *s5, real_T *s6, real_T *s7,
  real_T *ss1, real_T *ss2, real_T *ss3, real_T *ss4, real_T *ss5, real_T *ss6,
  real_T *ss7, real_T *sz1, real_T *sz2, real_T *sz3, real_T *sz11, real_T *sz12,
  real_T *sz13, real_T *sz21, real_T *sz22, real_T *sz23, real_T *sz31, real_T
  *sz32, real_T *sz33, real_T *xgh2, real_T *xgh3, real_T *xgh4, real_T *xh2,
  real_T *xh3, real_T *xi2, real_T *xi3, real_T *xl2, real_T *xl3, real_T *xl4,
  real_T *nm, real_T *z1, real_T *z2, real_T *z3, real_T *z11, real_T *z12,
  real_T *z13, real_T *z21, real_T *z22, real_T *z23, real_T *z31, real_T *z32,
  real_T *z33, real_T *zmol, real_T *zmos);
static void GNC_Planner_Core_dpper(real_T e3, real_T ee2, real_T se2, real_T se3,
  real_T sgh2, real_T sgh3, real_T sgh4, real_T sh2, real_T sh3, real_T si2,
  real_T si3, real_T sl2, real_T sl3, real_T sl4, real_T t, real_T xgh2, real_T
  xgh3, real_T xgh4, real_T xh2, real_T xh3, real_T xi2, real_T xi3, real_T xl2,
  real_T xl3, real_T xl4, real_T zmol, real_T zmos, char_T init, real_T *ep,
  real_T *inclp, real_T *nodep, real_T *argpp, real_T *mp);
static void GNC_Planner_Core_dsinit(real_T cosim, real_T emsq, real_T argpo,
  real_T s1, real_T s2, real_T s3, real_T s4, real_T s5, real_T sinim, real_T
  ss1, real_T ss2, real_T ss3, real_T ss4, real_T ss5, real_T sz1, real_T sz3,
  real_T sz11, real_T sz13, real_T sz21, real_T sz23, real_T sz31, real_T sz33,
  real_T gsto, real_T mo, real_T mdot, real_T no, real_T nodeo, real_T nodedot,
  real_T xpidot, real_T z1, real_T z3, real_T z11, real_T z13, real_T z21,
  real_T z23, real_T z31, real_T z33, real_T *em, real_T *argpm, real_T *inclm,
  real_T *mm, real_T *nm, real_T *nodem, real_T ecco, real_T eccsq, real_T *irez,
  real_T *atime, real_T *d2201, real_T *d2211, real_T *d3210, real_T *d3222,
  real_T *d4410, real_T *d4422, real_T *d5220, real_T *d5232, real_T *d5421,
  real_T *d5433, real_T *dedt, real_T *didt, real_T *dmdt, real_T *dnodt, real_T
  *domdt, real_T *del1, real_T *del2, real_T *del3, real_T *xfact, real_T *xlamo,
  real_T *xli, real_T *xni);
static real_T GNC_Planner_Core_dspace(real_T d2201, real_T d2211, real_T d3210,
  real_T d3222, real_T d4410, real_T d4422, real_T d5220, real_T d5232, real_T
  d5421, real_T d5433, real_T dedt, real_T del1, real_T del2, real_T del3,
  real_T didt, real_T dmdt, real_T dnodt, real_T domdt, real_T irez, real_T
  argpo, real_T argpdot, real_T t, real_T tc, real_T gsto, real_T xfact, real_T
  xlamo, real_T no, real_T *atime, real_T *em, real_T *argpm, real_T *inclm,
  real_T *xli, real_T *mm, real_T *xni, real_T *nodem, real_T *nm);
static void GNC_Planner_Core_sgp4(siqtL0olslAz0Re7SMjm43B_GNC_P_T *satrec,
  real_T r_data[], int16_T r_size[2], real_T v_data[], int16_T v_size[2]);
static void GNC_Planner_Core_sgp4init(real_T epoch, real_T xbstar, real_T xndot,
  real_T xnddot, real_T xecco, real_T xargpo, real_T xinclo, real_T xmo, real_T
  xno_kozai, real_T xnodeo, siqtL0olslAz0Re7SMjm43B_GNC_P_T *satrec);
static void GNC_Planner_Core_sgp4_e(satrec_t_GNC_Planner_Core_T *satrec, real_T
  tsince, real_T r_data[], int16_T r_size[2], real_T v_data[], int16_T v_size[2]);
static void GNC_Planner_Core_SAFE(void);
static void GNC_Planner_exit_internal_Basic(void);

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static boolean_T GNC_Planner_Core_isUnitImag(const char_T s[2], int16_T k,
  int16_T n)
{
  int16_T b_k;
  int16_T j;
  char_T c[3];
  boolean_T p;
  p = false;
  if (k <= n) {
    if (s[k - 1] == 'j') {
      p = true;
    } else if (s[k - 1] == 'i') {
      if (k >= n - 1) {
        p = true;
      } else {
        b_k = k;
        for (j = 0; j < 3; j++) {
          c[j] = '\x00';
          while ((b_k <= n) && (s[b_k - 1] == ',')) {
            b_k++;
          }

          if (b_k <= n) {
            c[j] = s[b_k - 1];
          }

          b_k++;
        }

        if ((((c[0] == 'I') || (c[0] == 'i')) && ((c[1] == 'N') || (c[1] == 'n'))
             && ((c[2] == 'F') || (c[2] == 'f'))) || (((c[0] == 'N') || (c[0] ==
               'n')) && ((c[1] == 'A') || (c[1] == 'a')) && ((c[2] == 'N') ||
              (c[2] == 'n')))) {
        } else {
          p = true;
        }
      }
    }
  }

  return p;
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static void GNC_Planner_Core_readNonFinite(const char_T s[2], int16_T *k,
  int16_T n, boolean_T *b_finite, real_T *fv)
{
  int16_T j;
  int16_T ksaved;
  char_T c[3];
  ksaved = *k;
  for (j = 0; j < 3; j++) {
    c[j] = '\x00';
    while ((*k <= n) && (s[*k - 1] == ',')) {
      (*k)++;
    }

    if (*k <= n) {
      c[j] = s[*k - 1];
    }

    (*k)++;
  }

  if (((c[0] == 'I') || (c[0] == 'i')) && ((c[1] == 'N') || (c[1] == 'n')) &&
      ((c[2] == 'F') || (c[2] == 'f'))) {
    *b_finite = false;
    *fv = (rtInf);
  } else if (((c[0] == 'N') || (c[0] == 'n')) && ((c[1] == 'A') || (c[1] == 'a'))
             && ((c[2] == 'N') || (c[2] == 'n'))) {
    *b_finite = false;
    *fv = (rtNaN);
  } else {
    *b_finite = true;
    *fv = 0.0;
    *k = ksaved;
  }
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static boolean_T GNC_Planner_Core_copydigits(char_T s1[4], int16_T *idx, const
  char_T s[2], int16_T *k, int16_T n, boolean_T allowpoint)
{
  char_T tmp;
  boolean_T exitg1;
  boolean_T haspoint;
  boolean_T success;
  success = (*k <= n);
  haspoint = false;
  exitg1 = false;
  while ((!exitg1) && (success && (*k <= n))) {
    tmp = s[*k - 1];
    if ((tmp >= '0') && (tmp <= '9')) {
      s1[*idx - 1] = tmp;
      (*idx)++;
      (*k)++;
    } else if (tmp == '.') {
      success = (allowpoint && (!haspoint));
      if (success) {
        s1[*idx - 1] = '.';
        (*idx)++;
        haspoint = true;
      }

      (*k)++;
    } else if (tmp == ',') {
      (*k)++;
    } else {
      exitg1 = true;
    }
  }

  return success;
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static boolean_T GNC_Planner_Core_copyexponent(char_T s1[4], int16_T *idx, const
  char_T s[2], int16_T *k, int16_T n)
{
  int16_T b_k;
  int16_T kexp;
  char_T tmp;
  boolean_T b_success;
  boolean_T success;
  success = true;
  if (*k <= n) {
    tmp = s[*k - 1];
    if ((tmp == 'E') || (tmp == 'e')) {
      s1[*idx - 1] = 'e';
      (*idx)++;
      (*k)++;
      while ((*k <= n) && (s[*k - 1] == ',')) {
        (*k)++;
      }

      if (*k <= n) {
        if (s[*k - 1] == '-') {
          s1[*idx - 1] = '-';
          (*idx)++;
          (*k)++;
        } else if (s[*k - 1] == '+') {
          (*k)++;
        }
      }

      kexp = *k;
      b_k = *k;
      b_success = GNC_Planner_Core_copydigits(s1, idx, s, &b_k, n, false);
      *k = b_k;
      if ((!b_success) || (b_k <= kexp)) {
        success = false;
      }
    }
  }

  return success;
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static void GNC_Planner_Core_copysign(char_T s1[4], int16_T *idx, const char_T
  s[2], int16_T *k, int16_T n, boolean_T *foundsign, boolean_T *success)
{
  char_T tmp;
  boolean_T isneg;
  static const boolean_T b[128] = { false, false, false, false, false, false,
    false, false, false, true, true, true, true, true, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    true, true, true, true, true, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false };

  boolean_T exitg1;
  isneg = false;
  *foundsign = false;
  exitg1 = false;
  while ((!exitg1) && (*k <= n)) {
    tmp = s[*k - 1];
    if (tmp == '-') {
      isneg = !isneg;
      *foundsign = true;
      (*k)++;
    } else if (tmp == ',') {
      (*k)++;
    } else if (tmp == '+') {
      *foundsign = true;
      (*k)++;
    } else if (!b[(int16_T)((uint8_T)tmp & 127U)]) {
      exitg1 = true;
    } else {
      (*k)++;
    }
  }

  *success = (*k <= n);
  if ((*success) && isneg) {
    if ((*idx >= 2) && (s1[*idx - 2] == '-')) {
      s1[*idx - 2] = ' ';
    } else {
      s1[*idx - 1] = '-';
      (*idx)++;
    }
  }
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static void GNC_Planner_Core_readfloat_j(char_T s1[4], int16_T *idx, const
  char_T s[2], int16_T *k, boolean_T *isimag, boolean_T *b_finite, real_T *nfv,
  boolean_T *foundsign, boolean_T *success)
{
  int16_T b_idx;
  int16_T b_k;
  char_T tmp;
  static const boolean_T b[128] = { false, false, false, false, false, false,
    false, false, false, true, true, true, true, true, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    true, true, true, true, true, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false };

  boolean_T exitg1;
  *isimag = false;
  *b_finite = true;
  *nfv = 0.0;
  b_idx = *idx;
  b_k = *k;
  GNC_Planner_Core_copysign(s1, &b_idx, s, &b_k, 2, foundsign, success);
  *idx = b_idx;
  *k = b_k;
  if (*success) {
    if (GNC_Planner_Core_isUnitImag(s, b_k, 2)) {
      *success = false;
    } else {
      GNC_Planner_Core_readNonFinite(s, k, 2, b_finite, nfv);
      if (*b_finite) {
        *success = GNC_Planner_Core_copydigits(s1, idx, s, k, 2, true);
        if (*success) {
          *success = GNC_Planner_Core_copyexponent(s1, idx, s, k, 2);
        }
      } else if ((b_idx >= 2) && (s1[b_idx - 2] == '-')) {
        *idx = b_idx - 1;
        s1[b_idx - 2] = ' ';
        *nfv = -*nfv;
      }

      exitg1 = false;
      while ((!exitg1) && (*k <= 2)) {
        tmp = s[*k - 1];
        if (b[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp == ','))
        {
          (*k)++;
        } else {
          exitg1 = true;
        }
      }

      if ((*k <= 2) && (s[*k - 1] == '*')) {
        (*k)++;
        exitg1 = false;
        while ((!exitg1) && (*k <= 2)) {
          tmp = s[*k - 1];
          if (b[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp ==
               ',')) {
            (*k)++;
          } else {
            exitg1 = true;
          }
        }
      }

      if (*k <= 2) {
        tmp = s[*k - 1];
        if ((tmp == 'i') || (tmp == 'j')) {
          (*k)++;
          *isimag = true;
        }
      }
    }

    exitg1 = false;
    while ((!exitg1) && (*k <= 2)) {
      tmp = s[*k - 1];
      if (b[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp == ','))
      {
        (*k)++;
      } else {
        exitg1 = true;
      }
    }
  }
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static void GNC_Planner_Core_readfloat(const char_T s[2], int16_T *k, char_T s1
  [4], int16_T *idx, boolean_T *isimag, boolean_T *b_finite, real_T *nfv,
  boolean_T *foundsign, boolean_T *success)
{
  int16_T b_k;
  char_T tmp;
  boolean_T a__3;
  boolean_T isneg;
  static const boolean_T b[128] = { false, false, false, false, false, false,
    false, false, false, true, true, true, true, true, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    true, true, true, true, true, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false };

  boolean_T exitg1;
  *isimag = false;
  *b_finite = true;
  *nfv = 0.0;
  b_k = *k;
  s1[0] = '\x00';
  s1[1] = '\x00';
  s1[2] = '\x00';
  s1[3] = '\x00';
  *idx = 1;
  isneg = false;
  *foundsign = false;
  exitg1 = false;
  while ((!exitg1) && (b_k <= 2)) {
    tmp = s[b_k - 1];
    if (tmp == '-') {
      isneg = !isneg;
      *foundsign = true;
      b_k++;
    } else if (tmp == ',') {
      b_k++;
    } else if (tmp == '+') {
      *foundsign = true;
      b_k++;
    } else if (!b[(int16_T)((uint8_T)tmp & 127U)]) {
      exitg1 = true;
    } else {
      b_k++;
    }
  }

  *success = (b_k <= 2);
  if ((*success) && isneg) {
    s1[0] = '-';
    *idx = 2;
  }

  *k = b_k;
  if (*success) {
    if (GNC_Planner_Core_isUnitImag(s, b_k, 2)) {
      *isimag = true;
      *k = b_k + 1;
      exitg1 = false;
      while ((!exitg1) && (*k <= 2)) {
        tmp = s[*k - 1];
        if (b[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp == ','))
        {
          (*k)++;
        } else {
          exitg1 = true;
        }
      }

      if ((*k <= 2) && (s[*k - 1] == '*')) {
        (*k)++;
        GNC_Planner_Core_readfloat_j(s1, idx, s, k, &isneg, b_finite, nfv, &a__3,
          success);
      } else {
        s1[*idx - 1] = '1';
        (*idx)++;
      }
    } else {
      GNC_Planner_Core_readNonFinite(s, k, 2, b_finite, nfv);
      if (*b_finite) {
        *success = GNC_Planner_Core_copydigits(s1, idx, s, k, 2, true);
        if (*success) {
          *success = GNC_Planner_Core_copyexponent(s1, idx, s, k, 2);
        }
      } else if ((*idx >= 2) && (s1[0] == '-')) {
        *idx = 1;
        s1[0] = ' ';
        *nfv = -*nfv;
      }

      exitg1 = false;
      while ((!exitg1) && (*k <= 2)) {
        tmp = s[*k - 1];
        if (b[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp == ','))
        {
          (*k)++;
        } else {
          exitg1 = true;
        }
      }

      if ((*k <= 2) && (s[*k - 1] == '*')) {
        (*k)++;
        exitg1 = false;
        while ((!exitg1) && (*k <= 2)) {
          tmp = s[*k - 1];
          if (b[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp ==
               ',')) {
            (*k)++;
          } else {
            exitg1 = true;
          }
        }
      }

      if (*k <= 2) {
        tmp = s[*k - 1];
        if ((tmp == 'i') || (tmp == 'j')) {
          (*k)++;
          *isimag = true;
        }
      }
    }

    exitg1 = false;
    while ((!exitg1) && (*k <= 2)) {
      tmp = s[*k - 1];
      if (b[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp == ','))
      {
        (*k)++;
      } else {
        exitg1 = true;
      }
    }
  }
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static creal_T GNC_Planner_Core_str2double(const char_T s[2])
{
  creal_T x;
  real_T b_scanned1;
  real_T scanned1;
  real_T scanned2;
  int16_T d_idx;
  int16_T idx;
  int16_T k;
  int16_T ntoread;
  char_T s1[4];
  char_T tmp;
  boolean_T a__1;
  boolean_T a__2;
  boolean_T a__3;
  boolean_T b_finite;
  boolean_T e_success;
  boolean_T isfinite1;
  boolean_T isimag1;
  boolean_T success;
  static const boolean_T c[128] = { false, false, false, false, false, false,
    false, false, false, true, true, true, true, true, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    true, true, true, true, true, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false };

  boolean_T exitg1;
  x.re = (rtNaN);
  x.im = 0.0;
  ntoread = 0;
  k = 1;
  exitg1 = false;
  while ((!exitg1) && (k <= 2)) {
    tmp = s[k - 1];
    if (c[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00')) {
      k++;
    } else {
      exitg1 = true;
    }
  }

  GNC_Planner_Core_readfloat(s, &k, s1, &idx, &isimag1, &isfinite1, &scanned1,
    &a__1, &success);
  if (isfinite1) {
    ntoread = 1;
  }

  if (success && (k <= 2)) {
    s1[idx - 1] = ' ';
    a__1 = false;
    b_finite = true;
    scanned2 = 0.0;
    d_idx = idx + 1;
    GNC_Planner_Core_copysign(s1, &d_idx, s, &k, 2, &success, &e_success);
    idx = d_idx;
    if (e_success) {
      if (GNC_Planner_Core_isUnitImag(s, k, 2)) {
        a__1 = true;
        k++;
        exitg1 = false;
        while ((!exitg1) && (k <= 2)) {
          tmp = s[k - 1];
          if (c[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp ==
               ',')) {
            k++;
          } else {
            exitg1 = true;
          }
        }

        if ((k <= 2) && (s[k - 1] == '*')) {
          k++;
          GNC_Planner_Core_readfloat_j(s1, &idx, s, &k, &a__2, &b_finite,
            &scanned2, &a__3, &e_success);
        } else {
          s1[d_idx - 1] = '1';
          idx = d_idx + 1;
        }
      } else {
        GNC_Planner_Core_readNonFinite(s, &k, 2, &b_finite, &scanned2);
        if (b_finite) {
          e_success = GNC_Planner_Core_copydigits(s1, &idx, s, &k, 2, true);
          if (e_success) {
            e_success = GNC_Planner_Core_copyexponent(s1, &idx, s, &k, 2);
          }
        } else if ((d_idx >= 2) && (s1[d_idx - 2] == '-')) {
          idx = d_idx - 1;
          s1[d_idx - 2] = ' ';
          scanned2 = -scanned2;
        }

        exitg1 = false;
        while ((!exitg1) && (k <= 2)) {
          tmp = s[k - 1];
          if (c[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp ==
               ',')) {
            k++;
          } else {
            exitg1 = true;
          }
        }

        if ((k <= 2) && (s[k - 1] == '*')) {
          k++;
          exitg1 = false;
          while ((!exitg1) && (k <= 2)) {
            tmp = s[k - 1];
            if (c[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp ==
                 ',')) {
              k++;
            } else {
              exitg1 = true;
            }
          }
        }

        if (k <= 2) {
          tmp = s[k - 1];
          if ((tmp == 'i') || (tmp == 'j')) {
            k++;
            a__1 = true;
          }
        }
      }

      exitg1 = false;
      while ((!exitg1) && (k <= 2)) {
        tmp = s[k - 1];
        if (c[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp == ','))
        {
          k++;
        } else {
          exitg1 = true;
        }
      }
    }

    if (b_finite) {
      ntoread++;
    }

    success = (e_success && (k > 2) && ((boolean_T)(isimag1 ^ a__1)) && success);
  } else {
    scanned2 = 0.0;
  }

  if (success) {
    s1[idx - 1] = '\x00';
    if (ntoread == 2) {
      ntoread = sscanf(&s1[0], "%lf %lf", &scanned1, &scanned2);
      if (ntoread != 2) {
        scanned1 = (rtNaN);
        scanned2 = (rtNaN);
      }
    } else if (ntoread == 1) {
      ntoread = sscanf(&s1[0], "%lf", &b_scanned1);
      if (isfinite1) {
        if (ntoread == 1) {
          scanned1 = b_scanned1;
        } else {
          scanned1 = (rtNaN);
        }
      } else if (ntoread == 1) {
        scanned2 = b_scanned1;
      } else {
        scanned2 = (rtNaN);
      }
    }

    if (isimag1) {
      x.re = scanned2;
      x.im = scanned1;
    } else {
      x.re = scanned1;
      x.im = scanned2;
    }
  }

  return x;
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static boolean_T GNC_Planner_Core_isUnitImag_e(const char_T s[12], int16_T k,
  int16_T n)
{
  int16_T b_k;
  int16_T j;
  char_T c[3];
  boolean_T p;
  p = false;
  if (k <= n) {
    if (s[k - 1] == 'j') {
      p = true;
    } else if (s[k - 1] == 'i') {
      if (k >= n - 1) {
        p = true;
      } else {
        b_k = k;
        for (j = 0; j < 3; j++) {
          c[j] = '\x00';
          while ((b_k <= n) && (s[b_k - 1] == ',')) {
            b_k++;
          }

          if (b_k <= n) {
            c[j] = s[b_k - 1];
          }

          b_k++;
        }

        if ((((c[0] == 'I') || (c[0] == 'i')) && ((c[1] == 'N') || (c[1] == 'n'))
             && ((c[2] == 'F') || (c[2] == 'f'))) || (((c[0] == 'N') || (c[0] ==
               'n')) && ((c[1] == 'A') || (c[1] == 'a')) && ((c[2] == 'N') ||
              (c[2] == 'n')))) {
        } else {
          p = true;
        }
      }
    }
  }

  return p;
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static void GNC_Planner_Cor_readNonFinite_o(const char_T s[12], int16_T *k,
  int16_T n, boolean_T *b_finite, real_T *fv)
{
  int16_T j;
  int16_T ksaved;
  char_T c[3];
  ksaved = *k;
  for (j = 0; j < 3; j++) {
    c[j] = '\x00';
    while ((*k <= n) && (s[*k - 1] == ',')) {
      (*k)++;
    }

    if (*k <= n) {
      c[j] = s[*k - 1];
    }

    (*k)++;
  }

  if (((c[0] == 'I') || (c[0] == 'i')) && ((c[1] == 'N') || (c[1] == 'n')) &&
      ((c[2] == 'F') || (c[2] == 'f'))) {
    *b_finite = false;
    *fv = (rtInf);
  } else if (((c[0] == 'N') || (c[0] == 'n')) && ((c[1] == 'A') || (c[1] == 'a'))
             && ((c[2] == 'N') || (c[2] == 'n'))) {
    *b_finite = false;
    *fv = (rtNaN);
  } else {
    *b_finite = true;
    *fv = 0.0;
    *k = ksaved;
  }
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static boolean_T GNC_Planner_Core_copydigits_k(char_T s1[14], int16_T *idx,
  const char_T s[12], int16_T *k, int16_T n, boolean_T allowpoint)
{
  char_T tmp;
  boolean_T exitg1;
  boolean_T haspoint;
  boolean_T success;
  success = (*k <= n);
  haspoint = false;
  exitg1 = false;
  while ((!exitg1) && (success && (*k <= n))) {
    tmp = s[*k - 1];
    if ((tmp >= '0') && (tmp <= '9')) {
      s1[*idx - 1] = tmp;
      (*idx)++;
      (*k)++;
    } else if (tmp == '.') {
      success = (allowpoint && (!haspoint));
      if (success) {
        s1[*idx - 1] = '.';
        (*idx)++;
        haspoint = true;
      }

      (*k)++;
    } else if (tmp == ',') {
      (*k)++;
    } else {
      exitg1 = true;
    }
  }

  return success;
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static boolean_T GNC_Planner_Core_copyexponent_e(char_T s1[14], int16_T *idx,
  const char_T s[12], int16_T *k, int16_T n)
{
  int16_T b_k;
  int16_T kexp;
  char_T tmp;
  boolean_T b_success;
  boolean_T success;
  success = true;
  if (*k <= n) {
    tmp = s[*k - 1];
    if ((tmp == 'E') || (tmp == 'e')) {
      s1[*idx - 1] = 'e';
      (*idx)++;
      (*k)++;
      while ((*k <= n) && (s[*k - 1] == ',')) {
        (*k)++;
      }

      if (*k <= n) {
        if (s[*k - 1] == '-') {
          s1[*idx - 1] = '-';
          (*idx)++;
          (*k)++;
        } else if (s[*k - 1] == '+') {
          (*k)++;
        }
      }

      kexp = *k;
      b_k = *k;
      b_success = GNC_Planner_Core_copydigits_k(s1, idx, s, &b_k, n, false);
      *k = b_k;
      if ((!b_success) || (b_k <= kexp)) {
        success = false;
      }
    }
  }

  return success;
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static void GNC_Planner_Core_copysign_l(char_T s1[14], int16_T *idx, const
  char_T s[12], int16_T *k, int16_T n, boolean_T *foundsign, boolean_T *success)
{
  char_T tmp;
  boolean_T isneg;
  static const boolean_T b[128] = { false, false, false, false, false, false,
    false, false, false, true, true, true, true, true, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    true, true, true, true, true, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false };

  boolean_T exitg1;
  isneg = false;
  *foundsign = false;
  exitg1 = false;
  while ((!exitg1) && (*k <= n)) {
    tmp = s[*k - 1];
    if (tmp == '-') {
      isneg = !isneg;
      *foundsign = true;
      (*k)++;
    } else if (tmp == ',') {
      (*k)++;
    } else if (tmp == '+') {
      *foundsign = true;
      (*k)++;
    } else if (!b[(int16_T)((uint8_T)tmp & 127U)]) {
      exitg1 = true;
    } else {
      (*k)++;
    }
  }

  *success = (*k <= n);
  if ((*success) && isneg) {
    if ((*idx >= 2) && (s1[*idx - 2] == '-')) {
      s1[*idx - 2] = ' ';
    } else {
      s1[*idx - 1] = '-';
      (*idx)++;
    }
  }
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static void GNC_Planner_Core_readfloat_jrw(char_T s1[14], int16_T *idx, const
  char_T s[12], int16_T *k, boolean_T *isimag, boolean_T *b_finite, real_T *nfv,
  boolean_T *foundsign, boolean_T *success)
{
  int16_T b_idx;
  int16_T b_k;
  char_T tmp;
  static const boolean_T b[128] = { false, false, false, false, false, false,
    false, false, false, true, true, true, true, true, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    true, true, true, true, true, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false };

  boolean_T exitg1;
  *isimag = false;
  *b_finite = true;
  *nfv = 0.0;
  b_idx = *idx;
  b_k = *k;
  GNC_Planner_Core_copysign_l(s1, &b_idx, s, &b_k, 12, foundsign, success);
  *idx = b_idx;
  *k = b_k;
  if (*success) {
    if (GNC_Planner_Core_isUnitImag_e(s, b_k, 12)) {
      *success = false;
    } else {
      GNC_Planner_Cor_readNonFinite_o(s, k, 12, b_finite, nfv);
      if (*b_finite) {
        *success = GNC_Planner_Core_copydigits_k(s1, idx, s, k, 12, true);
        if (*success) {
          *success = GNC_Planner_Core_copyexponent_e(s1, idx, s, k, 12);
        }
      } else if ((b_idx >= 2) && (s1[b_idx - 2] == '-')) {
        *idx = b_idx - 1;
        s1[b_idx - 2] = ' ';
        *nfv = -*nfv;
      }

      exitg1 = false;
      while ((!exitg1) && (*k <= 12)) {
        tmp = s[*k - 1];
        if (b[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp == ','))
        {
          (*k)++;
        } else {
          exitg1 = true;
        }
      }

      if ((*k <= 12) && (s[*k - 1] == '*')) {
        (*k)++;
        exitg1 = false;
        while ((!exitg1) && (*k <= 12)) {
          tmp = s[*k - 1];
          if (b[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp ==
               ',')) {
            (*k)++;
          } else {
            exitg1 = true;
          }
        }
      }

      if (*k <= 12) {
        tmp = s[*k - 1];
        if ((tmp == 'i') || (tmp == 'j')) {
          (*k)++;
          *isimag = true;
        }
      }
    }

    exitg1 = false;
    while ((!exitg1) && (*k <= 12)) {
      tmp = s[*k - 1];
      if (b[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp == ','))
      {
        (*k)++;
      } else {
        exitg1 = true;
      }
    }
  }
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static void GNC_Planner_Core_readfloat_jr(const char_T s[12], int16_T *k, char_T
  s1[14], int16_T *idx, boolean_T *isimag, boolean_T *b_finite, real_T *nfv,
  boolean_T *foundsign, boolean_T *success)
{
  int16_T b_k;
  int16_T i;
  char_T tmp;
  boolean_T a__3;
  boolean_T isneg;
  static const boolean_T b[128] = { false, false, false, false, false, false,
    false, false, false, true, true, true, true, true, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    true, true, true, true, true, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false };

  boolean_T exitg1;
  *isimag = false;
  *b_finite = true;
  *nfv = 0.0;
  b_k = *k;
  for (i = 0; i < 14; i++) {
    s1[i] = '\x00';
  }

  *idx = 1;
  isneg = false;
  *foundsign = false;
  exitg1 = false;
  while ((!exitg1) && (b_k <= 12)) {
    tmp = s[b_k - 1];
    if (tmp == '-') {
      isneg = !isneg;
      *foundsign = true;
      b_k++;
    } else if (tmp == ',') {
      b_k++;
    } else if (tmp == '+') {
      *foundsign = true;
      b_k++;
    } else if (!b[(int16_T)((uint8_T)tmp & 127U)]) {
      exitg1 = true;
    } else {
      b_k++;
    }
  }

  *success = (b_k <= 12);
  if ((*success) && isneg) {
    s1[0] = '-';
    *idx = 2;
  }

  *k = b_k;
  if (*success) {
    if (GNC_Planner_Core_isUnitImag_e(s, b_k, 12)) {
      *isimag = true;
      *k = b_k + 1;
      exitg1 = false;
      while ((!exitg1) && (*k <= 12)) {
        tmp = s[*k - 1];
        if (b[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp == ','))
        {
          (*k)++;
        } else {
          exitg1 = true;
        }
      }

      if ((*k <= 12) && (s[*k - 1] == '*')) {
        (*k)++;
        GNC_Planner_Core_readfloat_jrw(s1, idx, s, k, &isneg, b_finite, nfv,
          &a__3, success);
      } else {
        s1[*idx - 1] = '1';
        (*idx)++;
      }
    } else {
      GNC_Planner_Cor_readNonFinite_o(s, k, 12, b_finite, nfv);
      if (*b_finite) {
        *success = GNC_Planner_Core_copydigits_k(s1, idx, s, k, 12, true);
        if (*success) {
          *success = GNC_Planner_Core_copyexponent_e(s1, idx, s, k, 12);
        }
      } else if ((*idx >= 2) && (s1[0] == '-')) {
        *idx = 1;
        s1[0] = ' ';
        *nfv = -*nfv;
      }

      exitg1 = false;
      while ((!exitg1) && (*k <= 12)) {
        tmp = s[*k - 1];
        if (b[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp == ','))
        {
          (*k)++;
        } else {
          exitg1 = true;
        }
      }

      if ((*k <= 12) && (s[*k - 1] == '*')) {
        (*k)++;
        exitg1 = false;
        while ((!exitg1) && (*k <= 12)) {
          tmp = s[*k - 1];
          if (b[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp ==
               ',')) {
            (*k)++;
          } else {
            exitg1 = true;
          }
        }
      }

      if (*k <= 12) {
        tmp = s[*k - 1];
        if ((tmp == 'i') || (tmp == 'j')) {
          (*k)++;
          *isimag = true;
        }
      }
    }

    exitg1 = false;
    while ((!exitg1) && (*k <= 12)) {
      tmp = s[*k - 1];
      if (b[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp == ','))
      {
        (*k)++;
      } else {
        exitg1 = true;
      }
    }
  }
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static creal_T GNC_Planner_Core_str2double_e(const char_T s[12])
{
  creal_T x;
  real_T b_scanned1;
  real_T scanned1;
  real_T scanned2;
  int16_T d_idx;
  int16_T idx;
  int16_T k;
  int16_T ntoread;
  char_T s1[14];
  char_T tmp;
  boolean_T a__1;
  boolean_T a__2;
  boolean_T a__3;
  boolean_T b_finite;
  boolean_T e_success;
  boolean_T isfinite1;
  boolean_T isimag1;
  boolean_T success;
  static const boolean_T c[128] = { false, false, false, false, false, false,
    false, false, false, true, true, true, true, true, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    true, true, true, true, true, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false };

  boolean_T exitg1;
  x.re = (rtNaN);
  x.im = 0.0;
  ntoread = 0;
  k = 1;
  exitg1 = false;
  while ((!exitg1) && (k <= 12)) {
    tmp = s[k - 1];
    if (c[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00')) {
      k++;
    } else {
      exitg1 = true;
    }
  }

  GNC_Planner_Core_readfloat_jr(s, &k, s1, &idx, &isimag1, &isfinite1, &scanned1,
    &a__1, &success);
  if (isfinite1) {
    ntoread = 1;
  }

  if (success && (k <= 12)) {
    s1[idx - 1] = ' ';
    a__1 = false;
    b_finite = true;
    scanned2 = 0.0;
    d_idx = idx + 1;
    GNC_Planner_Core_copysign_l(s1, &d_idx, s, &k, 12, &success, &e_success);
    idx = d_idx;
    if (e_success) {
      if (GNC_Planner_Core_isUnitImag_e(s, k, 12)) {
        a__1 = true;
        k++;
        exitg1 = false;
        while ((!exitg1) && (k <= 12)) {
          tmp = s[k - 1];
          if (c[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp ==
               ',')) {
            k++;
          } else {
            exitg1 = true;
          }
        }

        if ((k <= 12) && (s[k - 1] == '*')) {
          k++;
          GNC_Planner_Core_readfloat_jrw(s1, &idx, s, &k, &a__2, &b_finite,
            &scanned2, &a__3, &e_success);
        } else {
          s1[d_idx - 1] = '1';
          idx = d_idx + 1;
        }
      } else {
        GNC_Planner_Cor_readNonFinite_o(s, &k, 12, &b_finite, &scanned2);
        if (b_finite) {
          e_success = GNC_Planner_Core_copydigits_k(s1, &idx, s, &k, 12, true);
          if (e_success) {
            e_success = GNC_Planner_Core_copyexponent_e(s1, &idx, s, &k, 12);
          }
        } else if ((d_idx >= 2) && (s1[d_idx - 2] == '-')) {
          idx = d_idx - 1;
          s1[d_idx - 2] = ' ';
          scanned2 = -scanned2;
        }

        exitg1 = false;
        while ((!exitg1) && (k <= 12)) {
          tmp = s[k - 1];
          if (c[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp ==
               ',')) {
            k++;
          } else {
            exitg1 = true;
          }
        }

        if ((k <= 12) && (s[k - 1] == '*')) {
          k++;
          exitg1 = false;
          while ((!exitg1) && (k <= 12)) {
            tmp = s[k - 1];
            if (c[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp ==
                 ',')) {
              k++;
            } else {
              exitg1 = true;
            }
          }
        }

        if (k <= 12) {
          tmp = s[k - 1];
          if ((tmp == 'i') || (tmp == 'j')) {
            k++;
            a__1 = true;
          }
        }
      }

      exitg1 = false;
      while ((!exitg1) && (k <= 12)) {
        tmp = s[k - 1];
        if (c[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp == ','))
        {
          k++;
        } else {
          exitg1 = true;
        }
      }
    }

    if (b_finite) {
      ntoread++;
    }

    success = (e_success && (k > 12) && ((boolean_T)(isimag1 ^ a__1)) && success);
  } else {
    scanned2 = 0.0;
  }

  if (success) {
    s1[idx - 1] = '\x00';
    if (ntoread == 2) {
      ntoread = sscanf(&s1[0], "%lf %lf", &scanned1, &scanned2);
      if (ntoread != 2) {
        scanned1 = (rtNaN);
        scanned2 = (rtNaN);
      }
    } else if (ntoread == 1) {
      ntoread = sscanf(&s1[0], "%lf", &b_scanned1);
      if (isfinite1) {
        if (ntoread == 1) {
          scanned1 = b_scanned1;
        } else {
          scanned1 = (rtNaN);
        }
      } else if (ntoread == 1) {
        scanned2 = b_scanned1;
      } else {
        scanned2 = (rtNaN);
      }
    }

    if (isimag1) {
      x.re = scanned2;
      x.im = scanned1;
    } else {
      x.re = scanned1;
      x.im = scanned2;
    }
  }

  return x;
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static boolean_T GNC_Planner_Core_isUnitImag_es(const char_T s[10], int16_T k,
  int16_T n)
{
  int16_T b_k;
  int16_T j;
  char_T c[3];
  boolean_T p;
  p = false;
  if (k <= n) {
    if (s[k - 1] == 'j') {
      p = true;
    } else if (s[k - 1] == 'i') {
      if (k >= n - 1) {
        p = true;
      } else {
        b_k = k;
        for (j = 0; j < 3; j++) {
          c[j] = '\x00';
          while ((b_k <= n) && (s[b_k - 1] == ',')) {
            b_k++;
          }

          if (b_k <= n) {
            c[j] = s[b_k - 1];
          }

          b_k++;
        }

        if ((((c[0] == 'I') || (c[0] == 'i')) && ((c[1] == 'N') || (c[1] == 'n'))
             && ((c[2] == 'F') || (c[2] == 'f'))) || (((c[0] == 'N') || (c[0] ==
               'n')) && ((c[1] == 'A') || (c[1] == 'a')) && ((c[2] == 'N') ||
              (c[2] == 'n')))) {
        } else {
          p = true;
        }
      }
    }
  }

  return p;
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static void GNC_Planner_Co_readNonFinite_o3(const char_T s[10], int16_T *k,
  int16_T n, boolean_T *b_finite, real_T *fv)
{
  int16_T j;
  int16_T ksaved;
  char_T c[3];
  ksaved = *k;
  for (j = 0; j < 3; j++) {
    c[j] = '\x00';
    while ((*k <= n) && (s[*k - 1] == ',')) {
      (*k)++;
    }

    if (*k <= n) {
      c[j] = s[*k - 1];
    }

    (*k)++;
  }

  if (((c[0] == 'I') || (c[0] == 'i')) && ((c[1] == 'N') || (c[1] == 'n')) &&
      ((c[2] == 'F') || (c[2] == 'f'))) {
    *b_finite = false;
    *fv = (rtInf);
  } else if (((c[0] == 'N') || (c[0] == 'n')) && ((c[1] == 'A') || (c[1] == 'a'))
             && ((c[2] == 'N') || (c[2] == 'n'))) {
    *b_finite = false;
    *fv = (rtNaN);
  } else {
    *b_finite = true;
    *fv = 0.0;
    *k = ksaved;
  }
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static boolean_T GNC_Planner_Core_copydigits_kg(char_T s1[12], int16_T *idx,
  const char_T s[10], int16_T *k, int16_T n, boolean_T allowpoint)
{
  char_T tmp;
  boolean_T exitg1;
  boolean_T haspoint;
  boolean_T success;
  success = (*k <= n);
  haspoint = false;
  exitg1 = false;
  while ((!exitg1) && (success && (*k <= n))) {
    tmp = s[*k - 1];
    if ((tmp >= '0') && (tmp <= '9')) {
      s1[*idx - 1] = tmp;
      (*idx)++;
      (*k)++;
    } else if (tmp == '.') {
      success = (allowpoint && (!haspoint));
      if (success) {
        s1[*idx - 1] = '.';
        (*idx)++;
        haspoint = true;
      }

      (*k)++;
    } else if (tmp == ',') {
      (*k)++;
    } else {
      exitg1 = true;
    }
  }

  return success;
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static boolean_T GNC_Planner_Cor_copyexponent_ea(char_T s1[12], int16_T *idx,
  const char_T s[10], int16_T *k, int16_T n)
{
  int16_T b_k;
  int16_T kexp;
  char_T tmp;
  boolean_T b_success;
  boolean_T success;
  success = true;
  if (*k <= n) {
    tmp = s[*k - 1];
    if ((tmp == 'E') || (tmp == 'e')) {
      s1[*idx - 1] = 'e';
      (*idx)++;
      (*k)++;
      while ((*k <= n) && (s[*k - 1] == ',')) {
        (*k)++;
      }

      if (*k <= n) {
        if (s[*k - 1] == '-') {
          s1[*idx - 1] = '-';
          (*idx)++;
          (*k)++;
        } else if (s[*k - 1] == '+') {
          (*k)++;
        }
      }

      kexp = *k;
      b_k = *k;
      b_success = GNC_Planner_Core_copydigits_kg(s1, idx, s, &b_k, n, false);
      *k = b_k;
      if ((!b_success) || (b_k <= kexp)) {
        success = false;
      }
    }
  }

  return success;
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static void GNC_Planner_Core_copysign_l1(char_T s1[12], int16_T *idx, const
  char_T s[10], int16_T *k, int16_T n, boolean_T *foundsign, boolean_T *success)
{
  char_T tmp;
  boolean_T isneg;
  static const boolean_T b[128] = { false, false, false, false, false, false,
    false, false, false, true, true, true, true, true, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    true, true, true, true, true, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false };

  boolean_T exitg1;
  isneg = false;
  *foundsign = false;
  exitg1 = false;
  while ((!exitg1) && (*k <= n)) {
    tmp = s[*k - 1];
    if (tmp == '-') {
      isneg = !isneg;
      *foundsign = true;
      (*k)++;
    } else if (tmp == ',') {
      (*k)++;
    } else if (tmp == '+') {
      *foundsign = true;
      (*k)++;
    } else if (!b[(int16_T)((uint8_T)tmp & 127U)]) {
      exitg1 = true;
    } else {
      (*k)++;
    }
  }

  *success = (*k <= n);
  if ((*success) && isneg) {
    if ((*idx >= 2) && (s1[*idx - 2] == '-')) {
      s1[*idx - 2] = ' ';
    } else {
      s1[*idx - 1] = '-';
      (*idx)++;
    }
  }
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static void GNC_Planner_Cor_readfloat_jrw1f(char_T s1[12], int16_T *idx, const
  char_T s[10], int16_T *k, boolean_T *isimag, boolean_T *b_finite, real_T *nfv,
  boolean_T *foundsign, boolean_T *success)
{
  int16_T b_idx;
  int16_T b_k;
  char_T tmp;
  static const boolean_T b[128] = { false, false, false, false, false, false,
    false, false, false, true, true, true, true, true, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    true, true, true, true, true, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false };

  boolean_T exitg1;
  *isimag = false;
  *b_finite = true;
  *nfv = 0.0;
  b_idx = *idx;
  b_k = *k;
  GNC_Planner_Core_copysign_l1(s1, &b_idx, s, &b_k, 10, foundsign, success);
  *idx = b_idx;
  *k = b_k;
  if (*success) {
    if (GNC_Planner_Core_isUnitImag_es(s, b_k, 10)) {
      *success = false;
    } else {
      GNC_Planner_Co_readNonFinite_o3(s, k, 10, b_finite, nfv);
      if (*b_finite) {
        *success = GNC_Planner_Core_copydigits_kg(s1, idx, s, k, 10, true);
        if (*success) {
          *success = GNC_Planner_Cor_copyexponent_ea(s1, idx, s, k, 10);
        }
      } else if ((b_idx >= 2) && (s1[b_idx - 2] == '-')) {
        *idx = b_idx - 1;
        s1[b_idx - 2] = ' ';
        *nfv = -*nfv;
      }

      exitg1 = false;
      while ((!exitg1) && (*k <= 10)) {
        tmp = s[*k - 1];
        if (b[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp == ','))
        {
          (*k)++;
        } else {
          exitg1 = true;
        }
      }

      if ((*k <= 10) && (s[*k - 1] == '*')) {
        (*k)++;
        exitg1 = false;
        while ((!exitg1) && (*k <= 10)) {
          tmp = s[*k - 1];
          if (b[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp ==
               ',')) {
            (*k)++;
          } else {
            exitg1 = true;
          }
        }
      }

      if (*k <= 10) {
        tmp = s[*k - 1];
        if ((tmp == 'i') || (tmp == 'j')) {
          (*k)++;
          *isimag = true;
        }
      }
    }

    exitg1 = false;
    while ((!exitg1) && (*k <= 10)) {
      tmp = s[*k - 1];
      if (b[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp == ','))
      {
        (*k)++;
      } else {
        exitg1 = true;
      }
    }
  }
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static void GNC_Planner_Core_readfloat_jrw1(const char_T s[10], int16_T *k,
  char_T s1[12], int16_T *idx, boolean_T *isimag, boolean_T *b_finite, real_T
  *nfv, boolean_T *foundsign, boolean_T *success)
{
  int16_T b_k;
  int16_T i;
  char_T tmp;
  boolean_T a__3;
  boolean_T isneg;
  static const boolean_T b[128] = { false, false, false, false, false, false,
    false, false, false, true, true, true, true, true, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    true, true, true, true, true, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false };

  boolean_T exitg1;
  *isimag = false;
  *b_finite = true;
  *nfv = 0.0;
  b_k = *k;
  for (i = 0; i < 12; i++) {
    s1[i] = '\x00';
  }

  *idx = 1;
  isneg = false;
  *foundsign = false;
  exitg1 = false;
  while ((!exitg1) && (b_k <= 10)) {
    tmp = s[b_k - 1];
    if (tmp == '-') {
      isneg = !isneg;
      *foundsign = true;
      b_k++;
    } else if (tmp == ',') {
      b_k++;
    } else if (tmp == '+') {
      *foundsign = true;
      b_k++;
    } else if (!b[(int16_T)((uint8_T)tmp & 127U)]) {
      exitg1 = true;
    } else {
      b_k++;
    }
  }

  *success = (b_k <= 10);
  if ((*success) && isneg) {
    s1[0] = '-';
    *idx = 2;
  }

  *k = b_k;
  if (*success) {
    if (GNC_Planner_Core_isUnitImag_es(s, b_k, 10)) {
      *isimag = true;
      *k = b_k + 1;
      exitg1 = false;
      while ((!exitg1) && (*k <= 10)) {
        tmp = s[*k - 1];
        if (b[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp == ','))
        {
          (*k)++;
        } else {
          exitg1 = true;
        }
      }

      if ((*k <= 10) && (s[*k - 1] == '*')) {
        (*k)++;
        GNC_Planner_Cor_readfloat_jrw1f(s1, idx, s, k, &isneg, b_finite, nfv,
          &a__3, success);
      } else {
        s1[*idx - 1] = '1';
        (*idx)++;
      }
    } else {
      GNC_Planner_Co_readNonFinite_o3(s, k, 10, b_finite, nfv);
      if (*b_finite) {
        *success = GNC_Planner_Core_copydigits_kg(s1, idx, s, k, 10, true);
        if (*success) {
          *success = GNC_Planner_Cor_copyexponent_ea(s1, idx, s, k, 10);
        }
      } else if ((*idx >= 2) && (s1[0] == '-')) {
        *idx = 1;
        s1[0] = ' ';
        *nfv = -*nfv;
      }

      exitg1 = false;
      while ((!exitg1) && (*k <= 10)) {
        tmp = s[*k - 1];
        if (b[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp == ','))
        {
          (*k)++;
        } else {
          exitg1 = true;
        }
      }

      if ((*k <= 10) && (s[*k - 1] == '*')) {
        (*k)++;
        exitg1 = false;
        while ((!exitg1) && (*k <= 10)) {
          tmp = s[*k - 1];
          if (b[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp ==
               ',')) {
            (*k)++;
          } else {
            exitg1 = true;
          }
        }
      }

      if (*k <= 10) {
        tmp = s[*k - 1];
        if ((tmp == 'i') || (tmp == 'j')) {
          (*k)++;
          *isimag = true;
        }
      }
    }

    exitg1 = false;
    while ((!exitg1) && (*k <= 10)) {
      tmp = s[*k - 1];
      if (b[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp == ','))
      {
        (*k)++;
      } else {
        exitg1 = true;
      }
    }
  }
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static creal_T GNC_Planner_Core_str2double_ek(const char_T s[10])
{
  creal_T x;
  real_T b_scanned1;
  real_T scanned1;
  real_T scanned2;
  int16_T d_idx;
  int16_T idx;
  int16_T k;
  int16_T ntoread;
  char_T s1[12];
  char_T tmp;
  boolean_T a__1;
  boolean_T a__2;
  boolean_T a__3;
  boolean_T b_finite;
  boolean_T e_success;
  boolean_T isfinite1;
  boolean_T isimag1;
  boolean_T success;
  static const boolean_T c[128] = { false, false, false, false, false, false,
    false, false, false, true, true, true, true, true, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    true, true, true, true, true, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false };

  boolean_T exitg1;
  x.re = (rtNaN);
  x.im = 0.0;
  ntoread = 0;
  k = 1;
  exitg1 = false;
  while ((!exitg1) && (k <= 10)) {
    tmp = s[k - 1];
    if (c[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00')) {
      k++;
    } else {
      exitg1 = true;
    }
  }

  GNC_Planner_Core_readfloat_jrw1(s, &k, s1, &idx, &isimag1, &isfinite1,
    &scanned1, &a__1, &success);
  if (isfinite1) {
    ntoread = 1;
  }

  if (success && (k <= 10)) {
    s1[idx - 1] = ' ';
    a__1 = false;
    b_finite = true;
    scanned2 = 0.0;
    d_idx = idx + 1;
    GNC_Planner_Core_copysign_l1(s1, &d_idx, s, &k, 10, &success, &e_success);
    idx = d_idx;
    if (e_success) {
      if (GNC_Planner_Core_isUnitImag_es(s, k, 10)) {
        a__1 = true;
        k++;
        exitg1 = false;
        while ((!exitg1) && (k <= 10)) {
          tmp = s[k - 1];
          if (c[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp ==
               ',')) {
            k++;
          } else {
            exitg1 = true;
          }
        }

        if ((k <= 10) && (s[k - 1] == '*')) {
          k++;
          GNC_Planner_Cor_readfloat_jrw1f(s1, &idx, s, &k, &a__2, &b_finite,
            &scanned2, &a__3, &e_success);
        } else {
          s1[d_idx - 1] = '1';
          idx = d_idx + 1;
        }
      } else {
        GNC_Planner_Co_readNonFinite_o3(s, &k, 10, &b_finite, &scanned2);
        if (b_finite) {
          e_success = GNC_Planner_Core_copydigits_kg(s1, &idx, s, &k, 10, true);
          if (e_success) {
            e_success = GNC_Planner_Cor_copyexponent_ea(s1, &idx, s, &k, 10);
          }
        } else if ((d_idx >= 2) && (s1[d_idx - 2] == '-')) {
          idx = d_idx - 1;
          s1[d_idx - 2] = ' ';
          scanned2 = -scanned2;
        }

        exitg1 = false;
        while ((!exitg1) && (k <= 10)) {
          tmp = s[k - 1];
          if (c[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp ==
               ',')) {
            k++;
          } else {
            exitg1 = true;
          }
        }

        if ((k <= 10) && (s[k - 1] == '*')) {
          k++;
          exitg1 = false;
          while ((!exitg1) && (k <= 10)) {
            tmp = s[k - 1];
            if (c[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp ==
                 ',')) {
              k++;
            } else {
              exitg1 = true;
            }
          }
        }

        if (k <= 10) {
          tmp = s[k - 1];
          if ((tmp == 'i') || (tmp == 'j')) {
            k++;
            a__1 = true;
          }
        }
      }

      exitg1 = false;
      while ((!exitg1) && (k <= 10)) {
        tmp = s[k - 1];
        if (c[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp == ','))
        {
          k++;
        } else {
          exitg1 = true;
        }
      }
    }

    if (b_finite) {
      ntoread++;
    }

    success = (e_success && (k > 10) && ((boolean_T)(isimag1 ^ a__1)) && success);
  } else {
    scanned2 = 0.0;
  }

  if (success) {
    s1[idx - 1] = '\x00';
    if (ntoread == 2) {
      ntoread = sscanf(&s1[0], "%lf %lf", &scanned1, &scanned2);
      if (ntoread != 2) {
        scanned1 = (rtNaN);
        scanned2 = (rtNaN);
      }
    } else if (ntoread == 1) {
      ntoread = sscanf(&s1[0], "%lf", &b_scanned1);
      if (isfinite1) {
        if (ntoread == 1) {
          scanned1 = b_scanned1;
        } else {
          scanned1 = (rtNaN);
        }
      } else if (ntoread == 1) {
        scanned2 = b_scanned1;
      } else {
        scanned2 = (rtNaN);
      }
    }

    if (isimag1) {
      x.re = scanned2;
      x.im = scanned1;
    } else {
      x.re = scanned1;
      x.im = scanned2;
    }
  }

  return x;
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static boolean_T GNC_Planner_Core_isUnitImag_esl(const char_T s[6], int16_T k,
  int16_T n)
{
  int16_T b_k;
  int16_T j;
  char_T c[3];
  boolean_T p;
  p = false;
  if (k <= n) {
    if (s[k - 1] == 'j') {
      p = true;
    } else if (s[k - 1] == 'i') {
      if (k >= n - 1) {
        p = true;
      } else {
        b_k = k;
        for (j = 0; j < 3; j++) {
          c[j] = '\x00';
          while ((b_k <= n) && (s[b_k - 1] == ',')) {
            b_k++;
          }

          if (b_k <= n) {
            c[j] = s[b_k - 1];
          }

          b_k++;
        }

        if ((((c[0] == 'I') || (c[0] == 'i')) && ((c[1] == 'N') || (c[1] == 'n'))
             && ((c[2] == 'F') || (c[2] == 'f'))) || (((c[0] == 'N') || (c[0] ==
               'n')) && ((c[1] == 'A') || (c[1] == 'a')) && ((c[2] == 'N') ||
              (c[2] == 'n')))) {
        } else {
          p = true;
        }
      }
    }
  }

  return p;
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static void GNC_Planner_C_readNonFinite_o30(const char_T s[6], int16_T *k,
  int16_T n, boolean_T *b_finite, real_T *fv)
{
  int16_T j;
  int16_T ksaved;
  char_T c[3];
  ksaved = *k;
  for (j = 0; j < 3; j++) {
    c[j] = '\x00';
    while ((*k <= n) && (s[*k - 1] == ',')) {
      (*k)++;
    }

    if (*k <= n) {
      c[j] = s[*k - 1];
    }

    (*k)++;
  }

  if (((c[0] == 'I') || (c[0] == 'i')) && ((c[1] == 'N') || (c[1] == 'n')) &&
      ((c[2] == 'F') || (c[2] == 'f'))) {
    *b_finite = false;
    *fv = (rtInf);
  } else if (((c[0] == 'N') || (c[0] == 'n')) && ((c[1] == 'A') || (c[1] == 'a'))
             && ((c[2] == 'N') || (c[2] == 'n'))) {
    *b_finite = false;
    *fv = (rtNaN);
  } else {
    *b_finite = true;
    *fv = 0.0;
    *k = ksaved;
  }
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static boolean_T GNC_Planner_Core_copydigits_kgo(char_T s1[8], int16_T *idx,
  const char_T s[6], int16_T *k, int16_T n, boolean_T allowpoint)
{
  char_T tmp;
  boolean_T exitg1;
  boolean_T haspoint;
  boolean_T success;
  success = (*k <= n);
  haspoint = false;
  exitg1 = false;
  while ((!exitg1) && (success && (*k <= n))) {
    tmp = s[*k - 1];
    if ((tmp >= '0') && (tmp <= '9')) {
      s1[*idx - 1] = tmp;
      (*idx)++;
      (*k)++;
    } else if (tmp == '.') {
      success = (allowpoint && (!haspoint));
      if (success) {
        s1[*idx - 1] = '.';
        (*idx)++;
        haspoint = true;
      }

      (*k)++;
    } else if (tmp == ',') {
      (*k)++;
    } else {
      exitg1 = true;
    }
  }

  return success;
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static boolean_T GNC_Planner_Co_copyexponent_eat(char_T s1[8], int16_T *idx,
  const char_T s[6], int16_T *k, int16_T n)
{
  int16_T b_k;
  int16_T kexp;
  char_T tmp;
  boolean_T b_success;
  boolean_T success;
  success = true;
  if (*k <= n) {
    tmp = s[*k - 1];
    if ((tmp == 'E') || (tmp == 'e')) {
      s1[*idx - 1] = 'e';
      (*idx)++;
      (*k)++;
      while ((*k <= n) && (s[*k - 1] == ',')) {
        (*k)++;
      }

      if (*k <= n) {
        if (s[*k - 1] == '-') {
          s1[*idx - 1] = '-';
          (*idx)++;
          (*k)++;
        } else if (s[*k - 1] == '+') {
          (*k)++;
        }
      }

      kexp = *k;
      b_k = *k;
      b_success = GNC_Planner_Core_copydigits_kgo(s1, idx, s, &b_k, n, false);
      *k = b_k;
      if ((!b_success) || (b_k <= kexp)) {
        success = false;
      }
    }
  }

  return success;
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static void GNC_Planner_Core_copysign_l1u(char_T s1[8], int16_T *idx, const
  char_T s[6], int16_T *k, int16_T n, boolean_T *foundsign, boolean_T *success)
{
  char_T tmp;
  boolean_T isneg;
  static const boolean_T b[128] = { false, false, false, false, false, false,
    false, false, false, true, true, true, true, true, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    true, true, true, true, true, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false };

  boolean_T exitg1;
  isneg = false;
  *foundsign = false;
  exitg1 = false;
  while ((!exitg1) && (*k <= n)) {
    tmp = s[*k - 1];
    if (tmp == '-') {
      isneg = !isneg;
      *foundsign = true;
      (*k)++;
    } else if (tmp == ',') {
      (*k)++;
    } else if (tmp == '+') {
      *foundsign = true;
      (*k)++;
    } else if (!b[(int16_T)((uint8_T)tmp & 127U)]) {
      exitg1 = true;
    } else {
      (*k)++;
    }
  }

  *success = (*k <= n);
  if ((*success) && isneg) {
    if ((*idx >= 2) && (s1[*idx - 2] == '-')) {
      s1[*idx - 2] = ' ';
    } else {
      s1[*idx - 1] = '-';
      (*idx)++;
    }
  }
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static void GNC_Planner_C_readfloat_jrw1fqe(char_T s1[8], int16_T *idx, const
  char_T s[6], int16_T *k, boolean_T *isimag, boolean_T *b_finite, real_T *nfv,
  boolean_T *foundsign, boolean_T *success)
{
  int16_T b_idx;
  int16_T b_k;
  char_T tmp;
  static const boolean_T b[128] = { false, false, false, false, false, false,
    false, false, false, true, true, true, true, true, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    true, true, true, true, true, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false };

  boolean_T exitg1;
  *isimag = false;
  *b_finite = true;
  *nfv = 0.0;
  b_idx = *idx;
  b_k = *k;
  GNC_Planner_Core_copysign_l1u(s1, &b_idx, s, &b_k, 6, foundsign, success);
  *idx = b_idx;
  *k = b_k;
  if (*success) {
    if (GNC_Planner_Core_isUnitImag_esl(s, b_k, 6)) {
      *success = false;
    } else {
      GNC_Planner_C_readNonFinite_o30(s, k, 6, b_finite, nfv);
      if (*b_finite) {
        *success = GNC_Planner_Core_copydigits_kgo(s1, idx, s, k, 6, true);
        if (*success) {
          *success = GNC_Planner_Co_copyexponent_eat(s1, idx, s, k, 6);
        }
      } else if ((b_idx >= 2) && (s1[b_idx - 2] == '-')) {
        *idx = b_idx - 1;
        s1[b_idx - 2] = ' ';
        *nfv = -*nfv;
      }

      exitg1 = false;
      while ((!exitg1) && (*k <= 6)) {
        tmp = s[*k - 1];
        if (b[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp == ','))
        {
          (*k)++;
        } else {
          exitg1 = true;
        }
      }

      if ((*k <= 6) && (s[*k - 1] == '*')) {
        (*k)++;
        exitg1 = false;
        while ((!exitg1) && (*k <= 6)) {
          tmp = s[*k - 1];
          if (b[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp ==
               ',')) {
            (*k)++;
          } else {
            exitg1 = true;
          }
        }
      }

      if (*k <= 6) {
        tmp = s[*k - 1];
        if ((tmp == 'i') || (tmp == 'j')) {
          (*k)++;
          *isimag = true;
        }
      }
    }

    exitg1 = false;
    while ((!exitg1) && (*k <= 6)) {
      tmp = s[*k - 1];
      if (b[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp == ','))
      {
        (*k)++;
      } else {
        exitg1 = true;
      }
    }
  }
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static void GNC_Planner_Co_readfloat_jrw1fq(const char_T s[6], int16_T *k,
  char_T s1[8], int16_T *idx, boolean_T *isimag, boolean_T *b_finite, real_T
  *nfv, boolean_T *foundsign, boolean_T *success)
{
  int16_T b_k;
  int16_T i;
  char_T tmp;
  boolean_T a__3;
  boolean_T isneg;
  static const boolean_T b[128] = { false, false, false, false, false, false,
    false, false, false, true, true, true, true, true, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    true, true, true, true, true, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false };

  boolean_T exitg1;
  *isimag = false;
  *b_finite = true;
  *nfv = 0.0;
  b_k = *k;
  for (i = 0; i < 8; i++) {
    s1[i] = '\x00';
  }

  *idx = 1;
  isneg = false;
  *foundsign = false;
  exitg1 = false;
  while ((!exitg1) && (b_k <= 6)) {
    tmp = s[b_k - 1];
    if (tmp == '-') {
      isneg = !isneg;
      *foundsign = true;
      b_k++;
    } else if (tmp == ',') {
      b_k++;
    } else if (tmp == '+') {
      *foundsign = true;
      b_k++;
    } else if (!b[(int16_T)((uint8_T)tmp & 127U)]) {
      exitg1 = true;
    } else {
      b_k++;
    }
  }

  *success = (b_k <= 6);
  if ((*success) && isneg) {
    s1[0] = '-';
    *idx = 2;
  }

  *k = b_k;
  if (*success) {
    if (GNC_Planner_Core_isUnitImag_esl(s, b_k, 6)) {
      *isimag = true;
      *k = b_k + 1;
      exitg1 = false;
      while ((!exitg1) && (*k <= 6)) {
        tmp = s[*k - 1];
        if (b[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp == ','))
        {
          (*k)++;
        } else {
          exitg1 = true;
        }
      }

      if ((*k <= 6) && (s[*k - 1] == '*')) {
        (*k)++;
        GNC_Planner_C_readfloat_jrw1fqe(s1, idx, s, k, &isneg, b_finite, nfv,
          &a__3, success);
      } else {
        s1[*idx - 1] = '1';
        (*idx)++;
      }
    } else {
      GNC_Planner_C_readNonFinite_o30(s, k, 6, b_finite, nfv);
      if (*b_finite) {
        *success = GNC_Planner_Core_copydigits_kgo(s1, idx, s, k, 6, true);
        if (*success) {
          *success = GNC_Planner_Co_copyexponent_eat(s1, idx, s, k, 6);
        }
      } else if ((*idx >= 2) && (s1[0] == '-')) {
        *idx = 1;
        s1[0] = ' ';
        *nfv = -*nfv;
      }

      exitg1 = false;
      while ((!exitg1) && (*k <= 6)) {
        tmp = s[*k - 1];
        if (b[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp == ','))
        {
          (*k)++;
        } else {
          exitg1 = true;
        }
      }

      if ((*k <= 6) && (s[*k - 1] == '*')) {
        (*k)++;
        exitg1 = false;
        while ((!exitg1) && (*k <= 6)) {
          tmp = s[*k - 1];
          if (b[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp ==
               ',')) {
            (*k)++;
          } else {
            exitg1 = true;
          }
        }
      }

      if (*k <= 6) {
        tmp = s[*k - 1];
        if ((tmp == 'i') || (tmp == 'j')) {
          (*k)++;
          *isimag = true;
        }
      }
    }

    exitg1 = false;
    while ((!exitg1) && (*k <= 6)) {
      tmp = s[*k - 1];
      if (b[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp == ','))
      {
        (*k)++;
      } else {
        exitg1 = true;
      }
    }
  }
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static creal_T GNC_Planner_Core_str2double_ekr(const char_T s[6])
{
  creal_T x;
  real_T b_scanned1;
  real_T scanned1;
  real_T scanned2;
  int16_T d_idx;
  int16_T idx;
  int16_T k;
  int16_T ntoread;
  char_T s1[8];
  char_T tmp;
  boolean_T a__1;
  boolean_T a__2;
  boolean_T a__3;
  boolean_T b_finite;
  boolean_T e_success;
  boolean_T isfinite1;
  boolean_T isimag1;
  boolean_T success;
  static const boolean_T c[128] = { false, false, false, false, false, false,
    false, false, false, true, true, true, true, true, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    true, true, true, true, true, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false };

  boolean_T exitg1;
  x.re = (rtNaN);
  x.im = 0.0;
  ntoread = 0;
  k = 1;
  exitg1 = false;
  while ((!exitg1) && (k <= 6)) {
    tmp = s[k - 1];
    if (c[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00')) {
      k++;
    } else {
      exitg1 = true;
    }
  }

  GNC_Planner_Co_readfloat_jrw1fq(s, &k, s1, &idx, &isimag1, &isfinite1,
    &scanned1, &a__1, &success);
  if (isfinite1) {
    ntoread = 1;
  }

  if (success && (k <= 6)) {
    s1[idx - 1] = ' ';
    a__1 = false;
    b_finite = true;
    scanned2 = 0.0;
    d_idx = idx + 1;
    GNC_Planner_Core_copysign_l1u(s1, &d_idx, s, &k, 6, &success, &e_success);
    idx = d_idx;
    if (e_success) {
      if (GNC_Planner_Core_isUnitImag_esl(s, k, 6)) {
        a__1 = true;
        k++;
        exitg1 = false;
        while ((!exitg1) && (k <= 6)) {
          tmp = s[k - 1];
          if (c[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp ==
               ',')) {
            k++;
          } else {
            exitg1 = true;
          }
        }

        if ((k <= 6) && (s[k - 1] == '*')) {
          k++;
          GNC_Planner_C_readfloat_jrw1fqe(s1, &idx, s, &k, &a__2, &b_finite,
            &scanned2, &a__3, &e_success);
        } else {
          s1[d_idx - 1] = '1';
          idx = d_idx + 1;
        }
      } else {
        GNC_Planner_C_readNonFinite_o30(s, &k, 6, &b_finite, &scanned2);
        if (b_finite) {
          e_success = GNC_Planner_Core_copydigits_kgo(s1, &idx, s, &k, 6, true);
          if (e_success) {
            e_success = GNC_Planner_Co_copyexponent_eat(s1, &idx, s, &k, 6);
          }
        } else if ((d_idx >= 2) && (s1[d_idx - 2] == '-')) {
          idx = d_idx - 1;
          s1[d_idx - 2] = ' ';
          scanned2 = -scanned2;
        }

        exitg1 = false;
        while ((!exitg1) && (k <= 6)) {
          tmp = s[k - 1];
          if (c[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp ==
               ',')) {
            k++;
          } else {
            exitg1 = true;
          }
        }

        if ((k <= 6) && (s[k - 1] == '*')) {
          k++;
          exitg1 = false;
          while ((!exitg1) && (k <= 6)) {
            tmp = s[k - 1];
            if (c[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp ==
                 ',')) {
              k++;
            } else {
              exitg1 = true;
            }
          }
        }

        if (k <= 6) {
          tmp = s[k - 1];
          if ((tmp == 'i') || (tmp == 'j')) {
            k++;
            a__1 = true;
          }
        }
      }

      exitg1 = false;
      while ((!exitg1) && (k <= 6)) {
        tmp = s[k - 1];
        if (c[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp == ','))
        {
          k++;
        } else {
          exitg1 = true;
        }
      }
    }

    if (b_finite) {
      ntoread++;
    }

    success = (e_success && (k > 6) && ((boolean_T)(isimag1 ^ a__1)) && success);
  } else {
    scanned2 = 0.0;
  }

  if (success) {
    s1[idx - 1] = '\x00';
    if (ntoread == 2) {
      ntoread = sscanf(&s1[0], "%lf %lf", &scanned1, &scanned2);
      if (ntoread != 2) {
        scanned1 = (rtNaN);
        scanned2 = (rtNaN);
      }
    } else if (ntoread == 1) {
      ntoread = sscanf(&s1[0], "%lf", &b_scanned1);
      if (isfinite1) {
        if (ntoread == 1) {
          scanned1 = b_scanned1;
        } else {
          scanned1 = (rtNaN);
        }
      } else if (ntoread == 1) {
        scanned2 = b_scanned1;
      } else {
        scanned2 = (rtNaN);
      }
    }

    if (isimag1) {
      x.re = scanned2;
      x.im = scanned1;
    } else {
      x.re = scanned1;
      x.im = scanned2;
    }
  }

  return x;
}

real_T rt_powd_snf(real_T u0, real_T u1)
{
  real_T tmp;
  real_T tmp_0;
  real_T y;
  if (rtIsNaN(u0) || rtIsNaN(u1)) {
    y = (rtNaN);
  } else {
    tmp = fabs(u0);
    tmp_0 = fabs(u1);
    if (rtIsInf(u1)) {
      if (tmp == 1.0) {
        y = 1.0;
      } else if (tmp > 1.0) {
        if (u1 > 0.0) {
          y = (rtInf);
        } else {
          y = 0.0;
        }
      } else if (u1 > 0.0) {
        y = 0.0;
      } else {
        y = (rtInf);
      }
    } else if (tmp_0 == 0.0) {
      y = 1.0;
    } else if (tmp_0 == 1.0) {
      if (u1 > 0.0) {
        y = u0;
      } else {
        y = 1.0 / u0;
      }
    } else if (u1 == 2.0) {
      y = u0 * u0;
    } else if ((u1 == 0.5) && (u0 >= 0.0)) {
      y = sqrt(u0);
    } else if ((u0 < 0.0) && (u1 > floor(u1))) {
      y = (rtNaN);
    } else {
      y = pow(u0, u1);
    }
  }

  return y;
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static boolean_T GNC_Planner_Cor_isUnitImag_esli(const char_T s[8], int16_T k,
  int16_T n)
{
  int16_T b_k;
  int16_T j;
  char_T c[3];
  boolean_T p;
  p = false;
  if (k <= n) {
    if (s[k - 1] == 'j') {
      p = true;
    } else if (s[k - 1] == 'i') {
      if (k >= n - 1) {
        p = true;
      } else {
        b_k = k;
        for (j = 0; j < 3; j++) {
          c[j] = '\x00';
          while ((b_k <= n) && (s[b_k - 1] == ',')) {
            b_k++;
          }

          if (b_k <= n) {
            c[j] = s[b_k - 1];
          }

          b_k++;
        }

        if ((((c[0] == 'I') || (c[0] == 'i')) && ((c[1] == 'N') || (c[1] == 'n'))
             && ((c[2] == 'F') || (c[2] == 'f'))) || (((c[0] == 'N') || (c[0] ==
               'n')) && ((c[1] == 'A') || (c[1] == 'a')) && ((c[2] == 'N') ||
              (c[2] == 'n')))) {
        } else {
          p = true;
        }
      }
    }
  }

  return p;
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static void GNC_Planner__readNonFinite_o30p(const char_T s[8], int16_T *k,
  int16_T n, boolean_T *b_finite, real_T *fv)
{
  int16_T j;
  int16_T ksaved;
  char_T c[3];
  ksaved = *k;
  for (j = 0; j < 3; j++) {
    c[j] = '\x00';
    while ((*k <= n) && (s[*k - 1] == ',')) {
      (*k)++;
    }

    if (*k <= n) {
      c[j] = s[*k - 1];
    }

    (*k)++;
  }

  if (((c[0] == 'I') || (c[0] == 'i')) && ((c[1] == 'N') || (c[1] == 'n')) &&
      ((c[2] == 'F') || (c[2] == 'f'))) {
    *b_finite = false;
    *fv = (rtInf);
  } else if (((c[0] == 'N') || (c[0] == 'n')) && ((c[1] == 'A') || (c[1] == 'a'))
             && ((c[2] == 'N') || (c[2] == 'n'))) {
    *b_finite = false;
    *fv = (rtNaN);
  } else {
    *b_finite = true;
    *fv = 0.0;
    *k = ksaved;
  }
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static boolean_T GNC_Planner_Cor_copydigits_kgor(char_T s1[10], int16_T *idx,
  const char_T s[8], int16_T *k, int16_T n, boolean_T allowpoint)
{
  char_T tmp;
  boolean_T exitg1;
  boolean_T haspoint;
  boolean_T success;
  success = (*k <= n);
  haspoint = false;
  exitg1 = false;
  while ((!exitg1) && (success && (*k <= n))) {
    tmp = s[*k - 1];
    if ((tmp >= '0') && (tmp <= '9')) {
      s1[*idx - 1] = tmp;
      (*idx)++;
      (*k)++;
    } else if (tmp == '.') {
      success = (allowpoint && (!haspoint));
      if (success) {
        s1[*idx - 1] = '.';
        (*idx)++;
        haspoint = true;
      }

      (*k)++;
    } else if (tmp == ',') {
      (*k)++;
    } else {
      exitg1 = true;
    }
  }

  return success;
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static boolean_T GNC_Planner_C_copyexponent_eato(char_T s1[10], int16_T *idx,
  const char_T s[8], int16_T *k, int16_T n)
{
  int16_T b_k;
  int16_T kexp;
  char_T tmp;
  boolean_T b_success;
  boolean_T success;
  success = true;
  if (*k <= n) {
    tmp = s[*k - 1];
    if ((tmp == 'E') || (tmp == 'e')) {
      s1[*idx - 1] = 'e';
      (*idx)++;
      (*k)++;
      while ((*k <= n) && (s[*k - 1] == ',')) {
        (*k)++;
      }

      if (*k <= n) {
        if (s[*k - 1] == '-') {
          s1[*idx - 1] = '-';
          (*idx)++;
          (*k)++;
        } else if (s[*k - 1] == '+') {
          (*k)++;
        }
      }

      kexp = *k;
      b_k = *k;
      b_success = GNC_Planner_Cor_copydigits_kgor(s1, idx, s, &b_k, n, false);
      *k = b_k;
      if ((!b_success) || (b_k <= kexp)) {
        success = false;
      }
    }
  }

  return success;
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static void GNC_Planner_Core_copysign_l1ub(char_T s1[10], int16_T *idx, const
  char_T s[8], int16_T *k, int16_T n, boolean_T *foundsign, boolean_T *success)
{
  char_T tmp;
  boolean_T isneg;
  static const boolean_T b[128] = { false, false, false, false, false, false,
    false, false, false, true, true, true, true, true, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    true, true, true, true, true, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false };

  boolean_T exitg1;
  isneg = false;
  *foundsign = false;
  exitg1 = false;
  while ((!exitg1) && (*k <= n)) {
    tmp = s[*k - 1];
    if (tmp == '-') {
      isneg = !isneg;
      *foundsign = true;
      (*k)++;
    } else if (tmp == ',') {
      (*k)++;
    } else if (tmp == '+') {
      *foundsign = true;
      (*k)++;
    } else if (!b[(int16_T)((uint8_T)tmp & 127U)]) {
      exitg1 = true;
    } else {
      (*k)++;
    }
  }

  *success = (*k <= n);
  if ((*success) && isneg) {
    if ((*idx >= 2) && (s1[*idx - 2] == '-')) {
      s1[*idx - 2] = ' ';
    } else {
      s1[*idx - 1] = '-';
      (*idx)++;
    }
  }
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static void GNC_Planner_readfloat_jrw1fqedp(char_T s1[10], int16_T *idx, const
  char_T s[8], int16_T *k, boolean_T *isimag, boolean_T *b_finite, real_T *nfv,
  boolean_T *foundsign, boolean_T *success)
{
  int16_T b_idx;
  int16_T b_k;
  char_T tmp;
  static const boolean_T b[128] = { false, false, false, false, false, false,
    false, false, false, true, true, true, true, true, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    true, true, true, true, true, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false };

  boolean_T exitg1;
  *isimag = false;
  *b_finite = true;
  *nfv = 0.0;
  b_idx = *idx;
  b_k = *k;
  GNC_Planner_Core_copysign_l1ub(s1, &b_idx, s, &b_k, 8, foundsign, success);
  *idx = b_idx;
  *k = b_k;
  if (*success) {
    if (GNC_Planner_Cor_isUnitImag_esli(s, b_k, 8)) {
      *success = false;
    } else {
      GNC_Planner__readNonFinite_o30p(s, k, 8, b_finite, nfv);
      if (*b_finite) {
        *success = GNC_Planner_Cor_copydigits_kgor(s1, idx, s, k, 8, true);
        if (*success) {
          *success = GNC_Planner_C_copyexponent_eato(s1, idx, s, k, 8);
        }
      } else if ((b_idx >= 2) && (s1[b_idx - 2] == '-')) {
        *idx = b_idx - 1;
        s1[b_idx - 2] = ' ';
        *nfv = -*nfv;
      }

      exitg1 = false;
      while ((!exitg1) && (*k <= 8)) {
        tmp = s[*k - 1];
        if (b[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp == ','))
        {
          (*k)++;
        } else {
          exitg1 = true;
        }
      }

      if ((*k <= 8) && (s[*k - 1] == '*')) {
        (*k)++;
        exitg1 = false;
        while ((!exitg1) && (*k <= 8)) {
          tmp = s[*k - 1];
          if (b[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp ==
               ',')) {
            (*k)++;
          } else {
            exitg1 = true;
          }
        }
      }

      if (*k <= 8) {
        tmp = s[*k - 1];
        if ((tmp == 'i') || (tmp == 'j')) {
          (*k)++;
          *isimag = true;
        }
      }
    }

    exitg1 = false;
    while ((!exitg1) && (*k <= 8)) {
      tmp = s[*k - 1];
      if (b[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp == ','))
      {
        (*k)++;
      } else {
        exitg1 = true;
      }
    }
  }
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static void GNC_Planner__readfloat_jrw1fqed(const char_T s[8], int16_T *k,
  char_T s1[10], int16_T *idx, boolean_T *isimag, boolean_T *b_finite, real_T
  *nfv, boolean_T *foundsign, boolean_T *success)
{
  int16_T b_k;
  int16_T i;
  char_T tmp;
  boolean_T a__3;
  boolean_T isneg;
  static const boolean_T b[128] = { false, false, false, false, false, false,
    false, false, false, true, true, true, true, true, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    true, true, true, true, true, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false };

  boolean_T exitg1;
  *isimag = false;
  *b_finite = true;
  *nfv = 0.0;
  b_k = *k;
  for (i = 0; i < 10; i++) {
    s1[i] = '\x00';
  }

  *idx = 1;
  isneg = false;
  *foundsign = false;
  exitg1 = false;
  while ((!exitg1) && (b_k <= 8)) {
    tmp = s[b_k - 1];
    if (tmp == '-') {
      isneg = !isneg;
      *foundsign = true;
      b_k++;
    } else if (tmp == ',') {
      b_k++;
    } else if (tmp == '+') {
      *foundsign = true;
      b_k++;
    } else if (!b[(int16_T)((uint8_T)tmp & 127U)]) {
      exitg1 = true;
    } else {
      b_k++;
    }
  }

  *success = (b_k <= 8);
  if ((*success) && isneg) {
    s1[0] = '-';
    *idx = 2;
  }

  *k = b_k;
  if (*success) {
    if (GNC_Planner_Cor_isUnitImag_esli(s, b_k, 8)) {
      *isimag = true;
      *k = b_k + 1;
      exitg1 = false;
      while ((!exitg1) && (*k <= 8)) {
        tmp = s[*k - 1];
        if (b[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp == ','))
        {
          (*k)++;
        } else {
          exitg1 = true;
        }
      }

      if ((*k <= 8) && (s[*k - 1] == '*')) {
        (*k)++;
        GNC_Planner_readfloat_jrw1fqedp(s1, idx, s, k, &isneg, b_finite, nfv,
          &a__3, success);
      } else {
        s1[*idx - 1] = '1';
        (*idx)++;
      }
    } else {
      GNC_Planner__readNonFinite_o30p(s, k, 8, b_finite, nfv);
      if (*b_finite) {
        *success = GNC_Planner_Cor_copydigits_kgor(s1, idx, s, k, 8, true);
        if (*success) {
          *success = GNC_Planner_C_copyexponent_eato(s1, idx, s, k, 8);
        }
      } else if ((*idx >= 2) && (s1[0] == '-')) {
        *idx = 1;
        s1[0] = ' ';
        *nfv = -*nfv;
      }

      exitg1 = false;
      while ((!exitg1) && (*k <= 8)) {
        tmp = s[*k - 1];
        if (b[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp == ','))
        {
          (*k)++;
        } else {
          exitg1 = true;
        }
      }

      if ((*k <= 8) && (s[*k - 1] == '*')) {
        (*k)++;
        exitg1 = false;
        while ((!exitg1) && (*k <= 8)) {
          tmp = s[*k - 1];
          if (b[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp ==
               ',')) {
            (*k)++;
          } else {
            exitg1 = true;
          }
        }
      }

      if (*k <= 8) {
        tmp = s[*k - 1];
        if ((tmp == 'i') || (tmp == 'j')) {
          (*k)++;
          *isimag = true;
        }
      }
    }

    exitg1 = false;
    while ((!exitg1) && (*k <= 8)) {
      tmp = s[*k - 1];
      if (b[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp == ','))
      {
        (*k)++;
      } else {
        exitg1 = true;
      }
    }
  }
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static creal_T GNC_Planner_Cor_str2double_ekr3(const char_T s[8])
{
  creal_T x;
  real_T b_scanned1;
  real_T scanned1;
  real_T scanned2;
  int16_T d_idx;
  int16_T idx;
  int16_T k;
  int16_T ntoread;
  char_T s1[10];
  char_T tmp;
  boolean_T a__1;
  boolean_T a__2;
  boolean_T a__3;
  boolean_T b_finite;
  boolean_T e_success;
  boolean_T isfinite1;
  boolean_T isimag1;
  boolean_T success;
  static const boolean_T c[128] = { false, false, false, false, false, false,
    false, false, false, true, true, true, true, true, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    true, true, true, true, true, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false };

  boolean_T exitg1;
  x.re = (rtNaN);
  x.im = 0.0;
  ntoread = 0;
  k = 1;
  exitg1 = false;
  while ((!exitg1) && (k <= 8)) {
    tmp = s[k - 1];
    if (c[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00')) {
      k++;
    } else {
      exitg1 = true;
    }
  }

  GNC_Planner__readfloat_jrw1fqed(s, &k, s1, &idx, &isimag1, &isfinite1,
    &scanned1, &a__1, &success);
  if (isfinite1) {
    ntoread = 1;
  }

  if (success && (k <= 8)) {
    s1[idx - 1] = ' ';
    a__1 = false;
    b_finite = true;
    scanned2 = 0.0;
    d_idx = idx + 1;
    GNC_Planner_Core_copysign_l1ub(s1, &d_idx, s, &k, 8, &success, &e_success);
    idx = d_idx;
    if (e_success) {
      if (GNC_Planner_Cor_isUnitImag_esli(s, k, 8)) {
        a__1 = true;
        k++;
        exitg1 = false;
        while ((!exitg1) && (k <= 8)) {
          tmp = s[k - 1];
          if (c[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp ==
               ',')) {
            k++;
          } else {
            exitg1 = true;
          }
        }

        if ((k <= 8) && (s[k - 1] == '*')) {
          k++;
          GNC_Planner_readfloat_jrw1fqedp(s1, &idx, s, &k, &a__2, &b_finite,
            &scanned2, &a__3, &e_success);
        } else {
          s1[d_idx - 1] = '1';
          idx = d_idx + 1;
        }
      } else {
        GNC_Planner__readNonFinite_o30p(s, &k, 8, &b_finite, &scanned2);
        if (b_finite) {
          e_success = GNC_Planner_Cor_copydigits_kgor(s1, &idx, s, &k, 8, true);
          if (e_success) {
            e_success = GNC_Planner_C_copyexponent_eato(s1, &idx, s, &k, 8);
          }
        } else if ((d_idx >= 2) && (s1[d_idx - 2] == '-')) {
          idx = d_idx - 1;
          s1[d_idx - 2] = ' ';
          scanned2 = -scanned2;
        }

        exitg1 = false;
        while ((!exitg1) && (k <= 8)) {
          tmp = s[k - 1];
          if (c[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp ==
               ',')) {
            k++;
          } else {
            exitg1 = true;
          }
        }

        if ((k <= 8) && (s[k - 1] == '*')) {
          k++;
          exitg1 = false;
          while ((!exitg1) && (k <= 8)) {
            tmp = s[k - 1];
            if (c[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp ==
                 ',')) {
              k++;
            } else {
              exitg1 = true;
            }
          }
        }

        if (k <= 8) {
          tmp = s[k - 1];
          if ((tmp == 'i') || (tmp == 'j')) {
            k++;
            a__1 = true;
          }
        }
      }

      exitg1 = false;
      while ((!exitg1) && (k <= 8)) {
        tmp = s[k - 1];
        if (c[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp == ','))
        {
          k++;
        } else {
          exitg1 = true;
        }
      }
    }

    if (b_finite) {
      ntoread++;
    }

    success = (e_success && (k > 8) && ((boolean_T)(isimag1 ^ a__1)) && success);
  } else {
    scanned2 = 0.0;
  }

  if (success) {
    s1[idx - 1] = '\x00';
    if (ntoread == 2) {
      ntoread = sscanf(&s1[0], "%lf %lf", &scanned1, &scanned2);
      if (ntoread != 2) {
        scanned1 = (rtNaN);
        scanned2 = (rtNaN);
      }
    } else if (ntoread == 1) {
      ntoread = sscanf(&s1[0], "%lf", &b_scanned1);
      if (isfinite1) {
        if (ntoread == 1) {
          scanned1 = b_scanned1;
        } else {
          scanned1 = (rtNaN);
        }
      } else if (ntoread == 1) {
        scanned2 = b_scanned1;
      } else {
        scanned2 = (rtNaN);
      }
    }

    if (isimag1) {
      x.re = scanned2;
      x.im = scanned1;
    } else {
      x.re = scanned1;
      x.im = scanned2;
    }
  }

  return x;
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static boolean_T GNC_Planner_Co_isUnitImag_eslih(const char_T s[7], int16_T k,
  int16_T n)
{
  int16_T b_k;
  int16_T j;
  char_T c[3];
  boolean_T p;
  p = false;
  if (k <= n) {
    if (s[k - 1] == 'j') {
      p = true;
    } else if (s[k - 1] == 'i') {
      if (k >= n - 1) {
        p = true;
      } else {
        b_k = k;
        for (j = 0; j < 3; j++) {
          c[j] = '\x00';
          while ((b_k <= n) && (s[b_k - 1] == ',')) {
            b_k++;
          }

          if (b_k <= n) {
            c[j] = s[b_k - 1];
          }

          b_k++;
        }

        if ((((c[0] == 'I') || (c[0] == 'i')) && ((c[1] == 'N') || (c[1] == 'n'))
             && ((c[2] == 'F') || (c[2] == 'f'))) || (((c[0] == 'N') || (c[0] ==
               'n')) && ((c[1] == 'A') || (c[1] == 'a')) && ((c[2] == 'N') ||
              (c[2] == 'n')))) {
        } else {
          p = true;
        }
      }
    }
  }

  return p;
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static void GNC_Planner_readNonFinite_o30pi(const char_T s[7], int16_T *k,
  int16_T n, boolean_T *b_finite, real_T *fv)
{
  int16_T j;
  int16_T ksaved;
  char_T c[3];
  ksaved = *k;
  for (j = 0; j < 3; j++) {
    c[j] = '\x00';
    while ((*k <= n) && (s[*k - 1] == ',')) {
      (*k)++;
    }

    if (*k <= n) {
      c[j] = s[*k - 1];
    }

    (*k)++;
  }

  if (((c[0] == 'I') || (c[0] == 'i')) && ((c[1] == 'N') || (c[1] == 'n')) &&
      ((c[2] == 'F') || (c[2] == 'f'))) {
    *b_finite = false;
    *fv = (rtInf);
  } else if (((c[0] == 'N') || (c[0] == 'n')) && ((c[1] == 'A') || (c[1] == 'a'))
             && ((c[2] == 'N') || (c[2] == 'n'))) {
    *b_finite = false;
    *fv = (rtNaN);
  } else {
    *b_finite = true;
    *fv = 0.0;
    *k = ksaved;
  }
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static boolean_T GNC_Planner_Co_copydigits_kgorr(char_T s1[9], int16_T *idx,
  const char_T s[7], int16_T *k, int16_T n, boolean_T allowpoint)
{
  char_T tmp;
  boolean_T exitg1;
  boolean_T haspoint;
  boolean_T success;
  success = (*k <= n);
  haspoint = false;
  exitg1 = false;
  while ((!exitg1) && (success && (*k <= n))) {
    tmp = s[*k - 1];
    if ((tmp >= '0') && (tmp <= '9')) {
      s1[*idx - 1] = tmp;
      (*idx)++;
      (*k)++;
    } else if (tmp == '.') {
      success = (allowpoint && (!haspoint));
      if (success) {
        s1[*idx - 1] = '.';
        (*idx)++;
        haspoint = true;
      }

      (*k)++;
    } else if (tmp == ',') {
      (*k)++;
    } else {
      exitg1 = true;
    }
  }

  return success;
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static boolean_T GNC_Planner__copyexponent_eaton(char_T s1[9], int16_T *idx,
  const char_T s[7], int16_T *k, int16_T n)
{
  int16_T b_k;
  int16_T kexp;
  char_T tmp;
  boolean_T b_success;
  boolean_T success;
  success = true;
  if (*k <= n) {
    tmp = s[*k - 1];
    if ((tmp == 'E') || (tmp == 'e')) {
      s1[*idx - 1] = 'e';
      (*idx)++;
      (*k)++;
      while ((*k <= n) && (s[*k - 1] == ',')) {
        (*k)++;
      }

      if (*k <= n) {
        if (s[*k - 1] == '-') {
          s1[*idx - 1] = '-';
          (*idx)++;
          (*k)++;
        } else if (s[*k - 1] == '+') {
          (*k)++;
        }
      }

      kexp = *k;
      b_k = *k;
      b_success = GNC_Planner_Co_copydigits_kgorr(s1, idx, s, &b_k, n, false);
      *k = b_k;
      if ((!b_success) || (b_k <= kexp)) {
        success = false;
      }
    }
  }

  return success;
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static void GNC_Planner_Core_copysign_l1uba(char_T s1[9], int16_T *idx, const
  char_T s[7], int16_T *k, int16_T n, boolean_T *foundsign, boolean_T *success)
{
  char_T tmp;
  boolean_T isneg;
  static const boolean_T b[128] = { false, false, false, false, false, false,
    false, false, false, true, true, true, true, true, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    true, true, true, true, true, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false };

  boolean_T exitg1;
  isneg = false;
  *foundsign = false;
  exitg1 = false;
  while ((!exitg1) && (*k <= n)) {
    tmp = s[*k - 1];
    if (tmp == '-') {
      isneg = !isneg;
      *foundsign = true;
      (*k)++;
    } else if (tmp == ',') {
      (*k)++;
    } else if (tmp == '+') {
      *foundsign = true;
      (*k)++;
    } else if (!b[(int16_T)((uint8_T)tmp & 127U)]) {
      exitg1 = true;
    } else {
      (*k)++;
    }
  }

  *success = (*k <= n);
  if ((*success) && isneg) {
    if ((*idx >= 2) && (s1[*idx - 2] == '-')) {
      s1[*idx - 2] = ' ';
    } else {
      s1[*idx - 1] = '-';
      (*idx)++;
    }
  }
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static void GNC_Plann_readfloat_jrw1fqedprt(char_T s1[9], int16_T *idx, const
  char_T s[7], int16_T *k, boolean_T *isimag, boolean_T *b_finite, real_T *nfv,
  boolean_T *foundsign, boolean_T *success)
{
  int16_T b_idx;
  int16_T b_k;
  char_T tmp;
  static const boolean_T b[128] = { false, false, false, false, false, false,
    false, false, false, true, true, true, true, true, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    true, true, true, true, true, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false };

  boolean_T exitg1;
  *isimag = false;
  *b_finite = true;
  *nfv = 0.0;
  b_idx = *idx;
  b_k = *k;
  GNC_Planner_Core_copysign_l1uba(s1, &b_idx, s, &b_k, 7, foundsign, success);
  *idx = b_idx;
  *k = b_k;
  if (*success) {
    if (GNC_Planner_Co_isUnitImag_eslih(s, b_k, 7)) {
      *success = false;
    } else {
      GNC_Planner_readNonFinite_o30pi(s, k, 7, b_finite, nfv);
      if (*b_finite) {
        *success = GNC_Planner_Co_copydigits_kgorr(s1, idx, s, k, 7, true);
        if (*success) {
          *success = GNC_Planner__copyexponent_eaton(s1, idx, s, k, 7);
        }
      } else if ((b_idx >= 2) && (s1[b_idx - 2] == '-')) {
        *idx = b_idx - 1;
        s1[b_idx - 2] = ' ';
        *nfv = -*nfv;
      }

      exitg1 = false;
      while ((!exitg1) && (*k <= 7)) {
        tmp = s[*k - 1];
        if (b[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp == ','))
        {
          (*k)++;
        } else {
          exitg1 = true;
        }
      }

      if ((*k <= 7) && (s[*k - 1] == '*')) {
        (*k)++;
        exitg1 = false;
        while ((!exitg1) && (*k <= 7)) {
          tmp = s[*k - 1];
          if (b[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp ==
               ',')) {
            (*k)++;
          } else {
            exitg1 = true;
          }
        }
      }

      if (*k <= 7) {
        tmp = s[*k - 1];
        if ((tmp == 'i') || (tmp == 'j')) {
          (*k)++;
          *isimag = true;
        }
      }
    }

    exitg1 = false;
    while ((!exitg1) && (*k <= 7)) {
      tmp = s[*k - 1];
      if (b[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp == ','))
      {
        (*k)++;
      } else {
        exitg1 = true;
      }
    }
  }
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static void GNC_Planne_readfloat_jrw1fqedpr(const char_T s[7], int16_T *k,
  char_T s1[9], int16_T *idx, boolean_T *isimag, boolean_T *b_finite, real_T
  *nfv, boolean_T *foundsign, boolean_T *success)
{
  int16_T b_k;
  int16_T i;
  char_T tmp;
  boolean_T a__3;
  boolean_T isneg;
  static const boolean_T b[128] = { false, false, false, false, false, false,
    false, false, false, true, true, true, true, true, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    true, true, true, true, true, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false };

  boolean_T exitg1;
  *isimag = false;
  *b_finite = true;
  *nfv = 0.0;
  b_k = *k;
  for (i = 0; i < 9; i++) {
    s1[i] = '\x00';
  }

  *idx = 1;
  isneg = false;
  *foundsign = false;
  exitg1 = false;
  while ((!exitg1) && (b_k <= 7)) {
    tmp = s[b_k - 1];
    if (tmp == '-') {
      isneg = !isneg;
      *foundsign = true;
      b_k++;
    } else if (tmp == ',') {
      b_k++;
    } else if (tmp == '+') {
      *foundsign = true;
      b_k++;
    } else if (!b[(int16_T)((uint8_T)tmp & 127U)]) {
      exitg1 = true;
    } else {
      b_k++;
    }
  }

  *success = (b_k <= 7);
  if ((*success) && isneg) {
    s1[0] = '-';
    *idx = 2;
  }

  *k = b_k;
  if (*success) {
    if (GNC_Planner_Co_isUnitImag_eslih(s, b_k, 7)) {
      *isimag = true;
      *k = b_k + 1;
      exitg1 = false;
      while ((!exitg1) && (*k <= 7)) {
        tmp = s[*k - 1];
        if (b[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp == ','))
        {
          (*k)++;
        } else {
          exitg1 = true;
        }
      }

      if ((*k <= 7) && (s[*k - 1] == '*')) {
        (*k)++;
        GNC_Plann_readfloat_jrw1fqedprt(s1, idx, s, k, &isneg, b_finite, nfv,
          &a__3, success);
      } else {
        s1[*idx - 1] = '1';
        (*idx)++;
      }
    } else {
      GNC_Planner_readNonFinite_o30pi(s, k, 7, b_finite, nfv);
      if (*b_finite) {
        *success = GNC_Planner_Co_copydigits_kgorr(s1, idx, s, k, 7, true);
        if (*success) {
          *success = GNC_Planner__copyexponent_eaton(s1, idx, s, k, 7);
        }
      } else if ((*idx >= 2) && (s1[0] == '-')) {
        *idx = 1;
        s1[0] = ' ';
        *nfv = -*nfv;
      }

      exitg1 = false;
      while ((!exitg1) && (*k <= 7)) {
        tmp = s[*k - 1];
        if (b[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp == ','))
        {
          (*k)++;
        } else {
          exitg1 = true;
        }
      }

      if ((*k <= 7) && (s[*k - 1] == '*')) {
        (*k)++;
        exitg1 = false;
        while ((!exitg1) && (*k <= 7)) {
          tmp = s[*k - 1];
          if (b[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp ==
               ',')) {
            (*k)++;
          } else {
            exitg1 = true;
          }
        }
      }

      if (*k <= 7) {
        tmp = s[*k - 1];
        if ((tmp == 'i') || (tmp == 'j')) {
          (*k)++;
          *isimag = true;
        }
      }
    }

    exitg1 = false;
    while ((!exitg1) && (*k <= 7)) {
      tmp = s[*k - 1];
      if (b[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp == ','))
      {
        (*k)++;
      } else {
        exitg1 = true;
      }
    }
  }
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static creal_T GNC_Planner_Co_str2double_ekr3v(const char_T s[7])
{
  creal_T x;
  real_T b_scanned1;
  real_T scanned1;
  real_T scanned2;
  int16_T d_idx;
  int16_T idx;
  int16_T k;
  int16_T ntoread;
  char_T s1[9];
  char_T tmp;
  boolean_T a__1;
  boolean_T a__2;
  boolean_T a__3;
  boolean_T b_finite;
  boolean_T e_success;
  boolean_T isfinite1;
  boolean_T isimag1;
  boolean_T success;
  static const boolean_T c[128] = { false, false, false, false, false, false,
    false, false, false, true, true, true, true, true, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    true, true, true, true, true, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false };

  boolean_T exitg1;
  x.re = (rtNaN);
  x.im = 0.0;
  ntoread = 0;
  k = 1;
  exitg1 = false;
  while ((!exitg1) && (k <= 7)) {
    tmp = s[k - 1];
    if (c[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00')) {
      k++;
    } else {
      exitg1 = true;
    }
  }

  GNC_Planne_readfloat_jrw1fqedpr(s, &k, s1, &idx, &isimag1, &isfinite1,
    &scanned1, &a__1, &success);
  if (isfinite1) {
    ntoread = 1;
  }

  if (success && (k <= 7)) {
    s1[idx - 1] = ' ';
    a__1 = false;
    b_finite = true;
    scanned2 = 0.0;
    d_idx = idx + 1;
    GNC_Planner_Core_copysign_l1uba(s1, &d_idx, s, &k, 7, &success, &e_success);
    idx = d_idx;
    if (e_success) {
      if (GNC_Planner_Co_isUnitImag_eslih(s, k, 7)) {
        a__1 = true;
        k++;
        exitg1 = false;
        while ((!exitg1) && (k <= 7)) {
          tmp = s[k - 1];
          if (c[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp ==
               ',')) {
            k++;
          } else {
            exitg1 = true;
          }
        }

        if ((k <= 7) && (s[k - 1] == '*')) {
          k++;
          GNC_Plann_readfloat_jrw1fqedprt(s1, &idx, s, &k, &a__2, &b_finite,
            &scanned2, &a__3, &e_success);
        } else {
          s1[d_idx - 1] = '1';
          idx = d_idx + 1;
        }
      } else {
        GNC_Planner_readNonFinite_o30pi(s, &k, 7, &b_finite, &scanned2);
        if (b_finite) {
          e_success = GNC_Planner_Co_copydigits_kgorr(s1, &idx, s, &k, 7, true);
          if (e_success) {
            e_success = GNC_Planner__copyexponent_eaton(s1, &idx, s, &k, 7);
          }
        } else if ((d_idx >= 2) && (s1[d_idx - 2] == '-')) {
          idx = d_idx - 1;
          s1[d_idx - 2] = ' ';
          scanned2 = -scanned2;
        }

        exitg1 = false;
        while ((!exitg1) && (k <= 7)) {
          tmp = s[k - 1];
          if (c[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp ==
               ',')) {
            k++;
          } else {
            exitg1 = true;
          }
        }

        if ((k <= 7) && (s[k - 1] == '*')) {
          k++;
          exitg1 = false;
          while ((!exitg1) && (k <= 7)) {
            tmp = s[k - 1];
            if (c[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp ==
                 ',')) {
              k++;
            } else {
              exitg1 = true;
            }
          }
        }

        if (k <= 7) {
          tmp = s[k - 1];
          if ((tmp == 'i') || (tmp == 'j')) {
            k++;
            a__1 = true;
          }
        }
      }

      exitg1 = false;
      while ((!exitg1) && (k <= 7)) {
        tmp = s[k - 1];
        if (c[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp == ','))
        {
          k++;
        } else {
          exitg1 = true;
        }
      }
    }

    if (b_finite) {
      ntoread++;
    }

    success = (e_success && (k > 7) && ((boolean_T)(isimag1 ^ a__1)) && success);
  } else {
    scanned2 = 0.0;
  }

  if (success) {
    s1[idx - 1] = '\x00';
    if (ntoread == 2) {
      ntoread = sscanf(&s1[0], "%lf %lf", &scanned1, &scanned2);
      if (ntoread != 2) {
        scanned1 = (rtNaN);
        scanned2 = (rtNaN);
      }
    } else if (ntoread == 1) {
      ntoread = sscanf(&s1[0], "%lf", &b_scanned1);
      if (isfinite1) {
        if (ntoread == 1) {
          scanned1 = b_scanned1;
        } else {
          scanned1 = (rtNaN);
        }
      } else if (ntoread == 1) {
        scanned2 = b_scanned1;
      } else {
        scanned2 = (rtNaN);
      }
    }

    if (isimag1) {
      x.re = scanned2;
      x.im = scanned1;
    } else {
      x.re = scanned1;
      x.im = scanned2;
    }
  }

  return x;
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static boolean_T GNC_Planner_C_isUnitImag_eslihp(const char_T s[17], int16_T k,
  int16_T n)
{
  int16_T b_k;
  int16_T j;
  char_T c[3];
  boolean_T p;
  p = false;
  if (k <= n) {
    if (s[k - 1] == 'j') {
      p = true;
    } else if (s[k - 1] == 'i') {
      if (k >= n - 1) {
        p = true;
      } else {
        b_k = k;
        for (j = 0; j < 3; j++) {
          c[j] = '\x00';
          while ((b_k <= n) && (s[b_k - 1] == ',')) {
            b_k++;
          }

          if (b_k <= n) {
            c[j] = s[b_k - 1];
          }

          b_k++;
        }

        if ((((c[0] == 'I') || (c[0] == 'i')) && ((c[1] == 'N') || (c[1] == 'n'))
             && ((c[2] == 'F') || (c[2] == 'f'))) || (((c[0] == 'N') || (c[0] ==
               'n')) && ((c[1] == 'A') || (c[1] == 'a')) && ((c[2] == 'N') ||
              (c[2] == 'n')))) {
        } else {
          p = true;
        }
      }
    }
  }

  return p;
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static void GNC_Planne_readNonFinite_o30pia(const char_T s[17], int16_T *k,
  int16_T n, boolean_T *b_finite, real_T *fv)
{
  int16_T j;
  int16_T ksaved;
  char_T c[3];
  ksaved = *k;
  for (j = 0; j < 3; j++) {
    c[j] = '\x00';
    while ((*k <= n) && (s[*k - 1] == ',')) {
      (*k)++;
    }

    if (*k <= n) {
      c[j] = s[*k - 1];
    }

    (*k)++;
  }

  if (((c[0] == 'I') || (c[0] == 'i')) && ((c[1] == 'N') || (c[1] == 'n')) &&
      ((c[2] == 'F') || (c[2] == 'f'))) {
    *b_finite = false;
    *fv = (rtInf);
  } else if (((c[0] == 'N') || (c[0] == 'n')) && ((c[1] == 'A') || (c[1] == 'a'))
             && ((c[2] == 'N') || (c[2] == 'n'))) {
    *b_finite = false;
    *fv = (rtNaN);
  } else {
    *b_finite = true;
    *fv = 0.0;
    *k = ksaved;
  }
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static boolean_T GNC_Planner_C_copydigits_kgorrr(char_T s1[19], int16_T *idx,
  const char_T s[17], int16_T *k, int16_T n, boolean_T allowpoint)
{
  char_T tmp;
  boolean_T exitg1;
  boolean_T haspoint;
  boolean_T success;
  success = (*k <= n);
  haspoint = false;
  exitg1 = false;
  while ((!exitg1) && (success && (*k <= n))) {
    tmp = s[*k - 1];
    if ((tmp >= '0') && (tmp <= '9')) {
      s1[*idx - 1] = tmp;
      (*idx)++;
      (*k)++;
    } else if (tmp == '.') {
      success = (allowpoint && (!haspoint));
      if (success) {
        s1[*idx - 1] = '.';
        (*idx)++;
        haspoint = true;
      }

      (*k)++;
    } else if (tmp == ',') {
      (*k)++;
    } else {
      exitg1 = true;
    }
  }

  return success;
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static boolean_T GNC_Planner_copyexponent_eatona(char_T s1[19], int16_T *idx,
  const char_T s[17], int16_T *k, int16_T n)
{
  int16_T b_k;
  int16_T kexp;
  char_T tmp;
  boolean_T b_success;
  boolean_T success;
  success = true;
  if (*k <= n) {
    tmp = s[*k - 1];
    if ((tmp == 'E') || (tmp == 'e')) {
      s1[*idx - 1] = 'e';
      (*idx)++;
      (*k)++;
      while ((*k <= n) && (s[*k - 1] == ',')) {
        (*k)++;
      }

      if (*k <= n) {
        if (s[*k - 1] == '-') {
          s1[*idx - 1] = '-';
          (*idx)++;
          (*k)++;
        } else if (s[*k - 1] == '+') {
          (*k)++;
        }
      }

      kexp = *k;
      b_k = *k;
      b_success = GNC_Planner_C_copydigits_kgorrr(s1, idx, s, &b_k, n, false);
      *k = b_k;
      if ((!b_success) || (b_k <= kexp)) {
        success = false;
      }
    }
  }

  return success;
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static void GNC_Planner_Cor_copysign_l1ubag(char_T s1[19], int16_T *idx, const
  char_T s[17], int16_T *k, int16_T n, boolean_T *foundsign, boolean_T *success)
{
  char_T tmp;
  boolean_T isneg;
  static const boolean_T b[128] = { false, false, false, false, false, false,
    false, false, false, true, true, true, true, true, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    true, true, true, true, true, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false };

  boolean_T exitg1;
  isneg = false;
  *foundsign = false;
  exitg1 = false;
  while ((!exitg1) && (*k <= n)) {
    tmp = s[*k - 1];
    if (tmp == '-') {
      isneg = !isneg;
      *foundsign = true;
      (*k)++;
    } else if (tmp == ',') {
      (*k)++;
    } else if (tmp == '+') {
      *foundsign = true;
      (*k)++;
    } else if (!b[(int16_T)((uint8_T)tmp & 127U)]) {
      exitg1 = true;
    } else {
      (*k)++;
    }
  }

  *success = (*k <= n);
  if ((*success) && isneg) {
    if ((*idx >= 2) && (s1[*idx - 2] == '-')) {
      s1[*idx - 2] = ' ';
    } else {
      s1[*idx - 1] = '-';
      (*idx)++;
    }
  }
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static void GNC_Pla_readfloat_jrw1fqedprtkv(char_T s1[19], int16_T *idx, const
  char_T s[17], int16_T *k, boolean_T *isimag, boolean_T *b_finite, real_T *nfv,
  boolean_T *foundsign, boolean_T *success)
{
  int16_T b_idx;
  int16_T b_k;
  char_T tmp;
  static const boolean_T b[128] = { false, false, false, false, false, false,
    false, false, false, true, true, true, true, true, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    true, true, true, true, true, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false };

  boolean_T exitg1;
  *isimag = false;
  *b_finite = true;
  *nfv = 0.0;
  b_idx = *idx;
  b_k = *k;
  GNC_Planner_Cor_copysign_l1ubag(s1, &b_idx, s, &b_k, 17, foundsign, success);
  *idx = b_idx;
  *k = b_k;
  if (*success) {
    if (GNC_Planner_C_isUnitImag_eslihp(s, b_k, 17)) {
      *success = false;
    } else {
      GNC_Planne_readNonFinite_o30pia(s, k, 17, b_finite, nfv);
      if (*b_finite) {
        *success = GNC_Planner_C_copydigits_kgorrr(s1, idx, s, k, 17, true);
        if (*success) {
          *success = GNC_Planner_copyexponent_eatona(s1, idx, s, k, 17);
        }
      } else if ((b_idx >= 2) && (s1[b_idx - 2] == '-')) {
        *idx = b_idx - 1;
        s1[b_idx - 2] = ' ';
        *nfv = -*nfv;
      }

      exitg1 = false;
      while ((!exitg1) && (*k <= 17)) {
        tmp = s[*k - 1];
        if (b[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp == ','))
        {
          (*k)++;
        } else {
          exitg1 = true;
        }
      }

      if ((*k <= 17) && (s[*k - 1] == '*')) {
        (*k)++;
        exitg1 = false;
        while ((!exitg1) && (*k <= 17)) {
          tmp = s[*k - 1];
          if (b[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp ==
               ',')) {
            (*k)++;
          } else {
            exitg1 = true;
          }
        }
      }

      if (*k <= 17) {
        tmp = s[*k - 1];
        if ((tmp == 'i') || (tmp == 'j')) {
          (*k)++;
          *isimag = true;
        }
      }
    }

    exitg1 = false;
    while ((!exitg1) && (*k <= 17)) {
      tmp = s[*k - 1];
      if (b[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp == ','))
      {
        (*k)++;
      } else {
        exitg1 = true;
      }
    }
  }
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static void GNC_Plan_readfloat_jrw1fqedprtk(const char_T s[17], int16_T *k,
  char_T s1[19], int16_T *idx, boolean_T *isimag, boolean_T *b_finite, real_T
  *nfv, boolean_T *foundsign, boolean_T *success)
{
  int16_T b_k;
  int16_T i;
  char_T tmp;
  boolean_T a__3;
  boolean_T isneg;
  static const boolean_T b[128] = { false, false, false, false, false, false,
    false, false, false, true, true, true, true, true, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    true, true, true, true, true, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false };

  boolean_T exitg1;
  *isimag = false;
  *b_finite = true;
  *nfv = 0.0;
  b_k = *k;
  for (i = 0; i < 19; i++) {
    s1[i] = '\x00';
  }

  *idx = 1;
  isneg = false;
  *foundsign = false;
  exitg1 = false;
  while ((!exitg1) && (b_k <= 17)) {
    tmp = s[b_k - 1];
    if (tmp == '-') {
      isneg = !isneg;
      *foundsign = true;
      b_k++;
    } else if (tmp == ',') {
      b_k++;
    } else if (tmp == '+') {
      *foundsign = true;
      b_k++;
    } else if (!b[(int16_T)((uint8_T)tmp & 127U)]) {
      exitg1 = true;
    } else {
      b_k++;
    }
  }

  *success = (b_k <= 17);
  if ((*success) && isneg) {
    s1[0] = '-';
    *idx = 2;
  }

  *k = b_k;
  if (*success) {
    if (GNC_Planner_C_isUnitImag_eslihp(s, b_k, 17)) {
      *isimag = true;
      *k = b_k + 1;
      exitg1 = false;
      while ((!exitg1) && (*k <= 17)) {
        tmp = s[*k - 1];
        if (b[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp == ','))
        {
          (*k)++;
        } else {
          exitg1 = true;
        }
      }

      if ((*k <= 17) && (s[*k - 1] == '*')) {
        (*k)++;
        GNC_Pla_readfloat_jrw1fqedprtkv(s1, idx, s, k, &isneg, b_finite, nfv,
          &a__3, success);
      } else {
        s1[*idx - 1] = '1';
        (*idx)++;
      }
    } else {
      GNC_Planne_readNonFinite_o30pia(s, k, 17, b_finite, nfv);
      if (*b_finite) {
        *success = GNC_Planner_C_copydigits_kgorrr(s1, idx, s, k, 17, true);
        if (*success) {
          *success = GNC_Planner_copyexponent_eatona(s1, idx, s, k, 17);
        }
      } else if ((*idx >= 2) && (s1[0] == '-')) {
        *idx = 1;
        s1[0] = ' ';
        *nfv = -*nfv;
      }

      exitg1 = false;
      while ((!exitg1) && (*k <= 17)) {
        tmp = s[*k - 1];
        if (b[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp == ','))
        {
          (*k)++;
        } else {
          exitg1 = true;
        }
      }

      if ((*k <= 17) && (s[*k - 1] == '*')) {
        (*k)++;
        exitg1 = false;
        while ((!exitg1) && (*k <= 17)) {
          tmp = s[*k - 1];
          if (b[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp ==
               ',')) {
            (*k)++;
          } else {
            exitg1 = true;
          }
        }
      }

      if (*k <= 17) {
        tmp = s[*k - 1];
        if ((tmp == 'i') || (tmp == 'j')) {
          (*k)++;
          *isimag = true;
        }
      }
    }

    exitg1 = false;
    while ((!exitg1) && (*k <= 17)) {
      tmp = s[*k - 1];
      if (b[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp == ','))
      {
        (*k)++;
      } else {
        exitg1 = true;
      }
    }
  }
}

/* Function for MATLAB Function: '<S4>/parse_tle_string' */
static creal_T GNC_Planner_C_str2double_ekr3v5(const char_T s[17])
{
  creal_T x;
  real_T b_scanned1;
  real_T scanned1;
  real_T scanned2;
  int16_T d_idx;
  int16_T idx;
  int16_T k;
  int16_T ntoread;
  char_T s1[19];
  char_T tmp;
  boolean_T a__1;
  boolean_T a__2;
  boolean_T a__3;
  boolean_T b_finite;
  boolean_T e_success;
  boolean_T isfinite1;
  boolean_T isimag1;
  boolean_T success;
  static const boolean_T c[128] = { false, false, false, false, false, false,
    false, false, false, true, true, true, true, true, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    true, true, true, true, true, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false };

  boolean_T exitg1;
  x.re = (rtNaN);
  x.im = 0.0;
  ntoread = 0;
  k = 1;
  exitg1 = false;
  while ((!exitg1) && (k <= 17)) {
    tmp = s[k - 1];
    if (c[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00')) {
      k++;
    } else {
      exitg1 = true;
    }
  }

  GNC_Plan_readfloat_jrw1fqedprtk(s, &k, s1, &idx, &isimag1, &isfinite1,
    &scanned1, &a__1, &success);
  if (isfinite1) {
    ntoread = 1;
  }

  if (success && (k <= 17)) {
    s1[idx - 1] = ' ';
    a__1 = false;
    b_finite = true;
    scanned2 = 0.0;
    d_idx = idx + 1;
    GNC_Planner_Cor_copysign_l1ubag(s1, &d_idx, s, &k, 17, &success, &e_success);
    idx = d_idx;
    if (e_success) {
      if (GNC_Planner_C_isUnitImag_eslihp(s, k, 17)) {
        a__1 = true;
        k++;
        exitg1 = false;
        while ((!exitg1) && (k <= 17)) {
          tmp = s[k - 1];
          if (c[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp ==
               ',')) {
            k++;
          } else {
            exitg1 = true;
          }
        }

        if ((k <= 17) && (s[k - 1] == '*')) {
          k++;
          GNC_Pla_readfloat_jrw1fqedprtkv(s1, &idx, s, &k, &a__2, &b_finite,
            &scanned2, &a__3, &e_success);
        } else {
          s1[d_idx - 1] = '1';
          idx = d_idx + 1;
        }
      } else {
        GNC_Planne_readNonFinite_o30pia(s, &k, 17, &b_finite, &scanned2);
        if (b_finite) {
          e_success = GNC_Planner_C_copydigits_kgorrr(s1, &idx, s, &k, 17, true);
          if (e_success) {
            e_success = GNC_Planner_copyexponent_eatona(s1, &idx, s, &k, 17);
          }
        } else if ((d_idx >= 2) && (s1[d_idx - 2] == '-')) {
          idx = d_idx - 1;
          s1[d_idx - 2] = ' ';
          scanned2 = -scanned2;
        }

        exitg1 = false;
        while ((!exitg1) && (k <= 17)) {
          tmp = s[k - 1];
          if (c[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp ==
               ',')) {
            k++;
          } else {
            exitg1 = true;
          }
        }

        if ((k <= 17) && (s[k - 1] == '*')) {
          k++;
          exitg1 = false;
          while ((!exitg1) && (k <= 17)) {
            tmp = s[k - 1];
            if (c[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp ==
                 ',')) {
              k++;
            } else {
              exitg1 = true;
            }
          }
        }

        if (k <= 17) {
          tmp = s[k - 1];
          if ((tmp == 'i') || (tmp == 'j')) {
            k++;
            a__1 = true;
          }
        }
      }

      exitg1 = false;
      while ((!exitg1) && (k <= 17)) {
        tmp = s[k - 1];
        if (c[(int16_T)((uint8_T)tmp & 127U)] || (tmp == '\x00') || (tmp == ','))
        {
          k++;
        } else {
          exitg1 = true;
        }
      }
    }

    if (b_finite) {
      ntoread++;
    }

    success = (e_success && (k > 17) && ((boolean_T)(isimag1 ^ a__1)) && success);
  } else {
    scanned2 = 0.0;
  }

  if (success) {
    s1[idx - 1] = '\x00';
    if (ntoread == 2) {
      ntoread = sscanf(&s1[0], "%lf %lf", &scanned1, &scanned2);
      if (ntoread != 2) {
        scanned1 = (rtNaN);
        scanned2 = (rtNaN);
      }
    } else if (ntoread == 1) {
      ntoread = sscanf(&s1[0], "%lf", &b_scanned1);
      if (isfinite1) {
        if (ntoread == 1) {
          scanned1 = b_scanned1;
        } else {
          scanned1 = (rtNaN);
        }
      } else if (ntoread == 1) {
        scanned2 = b_scanned1;
      } else {
        scanned2 = (rtNaN);
      }
    }

    if (isimag1) {
      x.re = scanned2;
      x.im = scanned1;
    } else {
      x.re = scanned1;
      x.im = scanned2;
    }
  }

  return x;
}

real_T rt_remd_snf(real_T u0, real_T u1)
{
  real_T q;
  real_T y;
  if (rtIsNaN(u0) || rtIsNaN(u1) || rtIsInf(u0)) {
    y = (rtNaN);
  } else if (rtIsInf(u1)) {
    y = u0;
  } else {
    if (u1 < 0.0) {
      q = ceil(u1);
    } else {
      q = floor(u1);
    }

    if ((u1 != 0.0) && (u1 != q)) {
      q = fabs(u0 / u1);
      if (!(fabs(q - floor(q + 0.5)) > DBL_EPSILON * q)) {
        y = 0.0 * u0;
      } else {
        y = fmod(u0, u1);
      }
    } else {
      y = fmod(u0, u1);
    }
  }

  return y;
}

/* Function for MATLAB Function: '<S4>/fsw_spg4_step' */
static void GNC_Planner_Core_initl(real_T ecco, real_T epoch, real_T inclo,
  real_T no_kozai, real_T *ainv, real_T *ao, real_T *con41, real_T *con42,
  real_T *cosio, real_T *cosio2, real_T *eccsq, real_T *omeosq, real_T *posq,
  real_T *rp, real_T *rteosq, real_T *sinio, real_T *gsto, real_T *no_unkozai)
{
  real_T ak;
  real_T d1;
  real_T del;
  *eccsq = ecco * ecco;
  *omeosq = 1.0 - *eccsq;
  *rteosq = sqrt(1.0 - *eccsq);
  *cosio = cos(inclo);
  *cosio2 = *cosio * *cosio;
  ak = rt_powd_snf(0.074366916133173422 / no_kozai, 0.66666666666666663);
  d1 = (3.0 * *cosio2 - 1.0) * 0.00081196200000000006 / ((1.0 - *eccsq) *
    *rteosq);
  del = d1 / (ak * ak);
  ak *= (1.0 - del * del) - (134.0 * del * del / 81.0 + 0.33333333333333331) *
    del;
  *no_unkozai = no_kozai / (d1 / (ak * ak) + 1.0);
  *ao = rt_powd_snf(0.074366916133173422 / *no_unkozai, 0.66666666666666663);
  *sinio = sin(inclo);
  d1 = (1.0 - *eccsq) * *ao;
  *con42 = 1.0 - 5.0 * *cosio2;
  *con41 = (-*con42 - *cosio2) - *cosio2;
  *ainv = 1.0 / *ao;
  *posq = d1 * d1;
  *rp = (1.0 - ecco) * *ao;
  d1 = floor((epoch - 7305.0) + 1.0E-8);
  *gsto = rt_remd_snf((((epoch - 7305.0) - d1) * 6.3003880988736567 +
                       (0.017202791694070362 * d1 + 1.7321343856509375)) +
                      (epoch - 7305.0) * (epoch - 7305.0) *
                      5.0755141943226946E-15, 6.2831853071795862);
  if (*gsto < 0.0) {
    *gsto += 6.2831853071795862;
  }
}

real_T rt_atan2d_snf(real_T u0, real_T u1)
{
  real_T y;
  int16_T tmp;
  int16_T tmp_0;
  if (rtIsNaN(u0) || rtIsNaN(u1)) {
    y = (rtNaN);
  } else if (rtIsInf(u0) && rtIsInf(u1)) {
    if (u0 > 0.0) {
      tmp = 1;
    } else {
      tmp = -1;
    }

    if (u1 > 0.0) {
      tmp_0 = 1;
    } else {
      tmp_0 = -1;
    }

    y = atan2(tmp, tmp_0);
  } else if (u1 == 0.0) {
    if (u0 > 0.0) {
      y = RT_PI / 2.0;
    } else if (u0 < 0.0) {
      y = -(RT_PI / 2.0);
    } else {
      y = 0.0;
    }
  } else {
    y = atan2(u0, u1);
  }

  return y;
}

/* Function for MATLAB Function: '<S4>/fsw_spg4_step' */
static void GNC_Planner_Core_dscom(real_T epoch, real_T ep, real_T argpp, real_T
  inclp, real_T nodep, real_T np, real_T *sinim, real_T *cosim, real_T *sinomm,
  real_T *cosomm, real_T *snodm, real_T *cnodm, real_T *day, real_T *e3, real_T *
  ee2, real_T *em, real_T *emsq, real_T *gam, real_T *rtemsq, real_T *se2,
  real_T *se3, real_T *sgh2, real_T *sgh3, real_T *sgh4, real_T *sh2, real_T
  *sh3, real_T *si2, real_T *si3, real_T *sl2, real_T *sl3, real_T *sl4, real_T *
  s1, real_T *s2, real_T *s3, real_T *s4, real_T *s5, real_T *s6, real_T *s7,
  real_T *ss1, real_T *ss2, real_T *ss3, real_T *ss4, real_T *ss5, real_T *ss6,
  real_T *ss7, real_T *sz1, real_T *sz2, real_T *sz3, real_T *sz11, real_T *sz12,
  real_T *sz13, real_T *sz21, real_T *sz22, real_T *sz23, real_T *sz31, real_T
  *sz32, real_T *sz33, real_T *xgh2, real_T *xgh3, real_T *xgh4, real_T *xh2,
  real_T *xh3, real_T *xi2, real_T *xi3, real_T *xl2, real_T *xl3, real_T *xl4,
  real_T *nm, real_T *z1, real_T *z2, real_T *z3, real_T *z11, real_T *z12,
  real_T *z13, real_T *z21, real_T *z22, real_T *z23, real_T *z31, real_T *z32,
  real_T *z33, real_T *zmol, real_T *zmos)
{
  real_T a1;
  real_T a2;
  real_T a3;
  real_T a4;
  real_T a5;
  real_T a6;
  real_T a9;
  real_T ctem;
  real_T ss5_tmp;
  real_T ss5_tmp_0;
  real_T stem;
  real_T x3;
  real_T x4;
  real_T x5;
  real_T x6;
  real_T x7;
  real_T x8;
  real_T xnodce;
  real_T z32_tmp;
  real_T z32_tmp_0;
  real_T zcosgl;
  real_T zcoshl;
  real_T zcosil;
  real_T zsingl;
  real_T zsinhl;
  real_T zsinil;
  *nm = np;
  *em = ep;
  *snodm = sin(nodep);
  *cnodm = cos(nodep);
  *sinomm = sin(argpp);
  *cosomm = cos(argpp);
  *sinim = sin(inclp);
  *cosim = cos(inclp);
  *emsq = ep * ep;
  *rtemsq = sqrt(1.0 - *emsq);
  *day = epoch + 18261.5;
  xnodce = rt_remd_snf(4.523602 - (epoch + 18261.5) * 0.00092422029,
                       6.2831853071795862);
  stem = sin(xnodce);
  ctem = cos(xnodce);
  zcosil = 0.91375164 - 0.03568096 * ctem;
  zsinil = sqrt(1.0 - zcosil * zcosil);
  zsinhl = 0.089683511 * stem / zsinil;
  zcoshl = sqrt(1.0 - zsinhl * zsinhl);
  *gam = (epoch + 18261.5) * 0.001944368 + 5.8351514;
  xnodce = (rt_atan2d_snf(0.39785416 * stem / zsinil, 0.91744867 * zsinhl * stem
             + zcoshl * ctem) + *gam) - xnodce;
  zcosgl = cos(xnodce);
  zsingl = sin(xnodce);
  stem = 1.0 / np;
  a1 = 0.1945905 * *cnodm + -0.89991125334450861 * *snodm;
  a3 = 0.98088458 * *cnodm + 0.17852679541963498 * *snodm;
  ctem = -0.1945905 * *snodm + -0.89991125334450861 * *cnodm;
  a9 = -0.98088458 * *snodm + 0.17852679541963498 * *cnodm;
  a2 = *cosim * ctem + *sinim * -0.39024901063285283;
  a4 = *cosim * a9 + *sinim * 0.07741863992148;
  a5 = -*sinim * ctem + *cosim * -0.39024901063285283;
  a6 = -*sinim * a9 + *cosim * 0.07741863992148;
  ctem = a1 * *cosomm + a2 * *sinomm;
  a9 = a3 * *cosomm + a4 * *sinomm;
  x3 = -a1 * *sinomm + a2 * *cosomm;
  x4 = -a3 * *sinomm + a4 * *cosomm;
  x5 = a5 * *sinomm;
  x6 = a6 * *sinomm;
  x7 = a5 * *cosomm;
  x8 = a6 * *cosomm;
  *z31 = 12.0 * ctem * ctem - 3.0 * x3 * x3;
  z32_tmp = 6.0 * x3;
  z32_tmp_0 = 24.0 * ctem;
  *z32 = z32_tmp_0 * a9 - z32_tmp * x4;
  *z33 = 12.0 * a9 * a9 - 3.0 * x4 * x4;
  *z1 = (a1 * a1 + a2 * a2) * 3.0 + *z31 * *emsq;
  *z2 = (a1 * a3 + a2 * a4) * 6.0 + *z32 * *emsq;
  *z3 = (a3 * a3 + a4 * a4) * 3.0 + *z33 * *emsq;
  *sz1 = (1.0 - *emsq) * *z31 + (*z1 + *z1);
  *sz2 = (1.0 - *emsq) * *z32 + (*z2 + *z2);
  *sz3 = (1.0 - *emsq) * *z33 + (*z3 + *z3);
  *s3 = 2.9864797E-6 * stem;
  *s4 = *s3 * *rtemsq;
  xnodce = -15.0 * ep;
  *ss1 = xnodce * *s4;
  *ss2 = -0.5 * *s3 / *rtemsq;
  *ss3 = *s3;
  *ss4 = *s4;
  ss5_tmp = a9 * x4;
  ss5_tmp_0 = ctem * x3;
  *ss5 = ss5_tmp_0 + ss5_tmp;
  *ss6 = a9 * x3 + ctem * x4;
  *ss7 = ss5_tmp - ss5_tmp_0;
  *sz11 = (-24.0 * ctem * x7 - z32_tmp * x5) * *emsq + -6.0 * a1 * a5;
  *sz12 = ((a9 * x7 + ctem * x8) * -24.0 - (x3 * x6 + x4 * x5) * 6.0) * *emsq +
    (a1 * a6 + a3 * a5) * -6.0;
  a1 = 6.0 * x4;
  *sz13 = (-24.0 * a9 * x8 - a1 * x6) * *emsq + -6.0 * a3 * a6;
  *sz21 = (z32_tmp_0 * x5 - z32_tmp * x7) * *emsq + 6.0 * a2 * a5;
  *sz22 = ((a9 * x5 + ctem * x6) * 24.0 - (x4 * x7 + x3 * x8) * 6.0) * *emsq +
    (a4 * a5 + a2 * a6) * 6.0;
  *sz23 = (24.0 * a9 * x6 - a1 * x8) * *emsq + 6.0 * a4 * a6;
  *sz31 = *z31;
  *sz32 = *z32;
  *sz33 = *z33;
  a9 = zcoshl * *cnodm + zsinhl * *snodm;
  x4 = *snodm * zcoshl - *cnodm * zsinhl;
  ctem = zsingl * zcosil;
  a1 = ctem * x4 + zcosgl * a9;
  a4 = zcosgl * zcosil;
  a3 = a4 * x4 + -zsingl * a9;
  ctem = ctem * a9 + -zcosgl * x4;
  x3 = zsingl * zsinil;
  a9 = a4 * a9 + zsingl * x4;
  x4 = zcosgl * zsinil;
  a2 = *cosim * ctem + *sinim * x3;
  a4 = *cosim * a9 + *sinim * x4;
  a5 = -*sinim * ctem + *cosim * x3;
  a6 = -*sinim * a9 + *cosim * x4;
  ctem = a1 * *cosomm + a2 * *sinomm;
  a9 = a3 * *cosomm + a4 * *sinomm;
  x3 = -a1 * *sinomm + a2 * *cosomm;
  x4 = -a3 * *sinomm + a4 * *cosomm;
  x5 = a5 * *sinomm;
  x6 = a6 * *sinomm;
  x7 = a5 * *cosomm;
  x8 = a6 * *cosomm;
  *z31 = 12.0 * ctem * ctem - 3.0 * x3 * x3;
  z32_tmp = 6.0 * x3;
  z32_tmp_0 = 24.0 * ctem;
  *z32 = z32_tmp_0 * a9 - z32_tmp * x4;
  *z33 = 12.0 * a9 * a9 - 3.0 * x4 * x4;
  *z1 = (a1 * a1 + a2 * a2) * 3.0 + *z31 * *emsq;
  *z2 = (a1 * a3 + a2 * a4) * 6.0 + *z32 * *emsq;
  *z3 = (a3 * a3 + a4 * a4) * 3.0 + *z33 * *emsq;
  *z11 = (-24.0 * ctem * x7 - z32_tmp * x5) * *emsq + -6.0 * a1 * a5;
  *z12 = ((a9 * x7 + ctem * x8) * -24.0 - (x3 * x6 + x4 * x5) * 6.0) * *emsq +
    (a1 * a6 + a3 * a5) * -6.0;
  zsinil = 6.0 * x4;
  *z13 = (-24.0 * a9 * x8 - zsinil * x6) * *emsq + -6.0 * a3 * a6;
  *z21 = (z32_tmp_0 * x5 - z32_tmp * x7) * *emsq + 6.0 * a2 * a5;
  *z22 = ((a9 * x5 + ctem * x6) * 24.0 - (x4 * x7 + x3 * x8) * 6.0) * *emsq +
    (a4 * a5 + a2 * a6) * 6.0;
  *z23 = (24.0 * a9 * x6 - zsinil * x8) * *emsq + 6.0 * a4 * a6;
  *z1 = (1.0 - *emsq) * *z31 + (*z1 + *z1);
  *z2 = (1.0 - *emsq) * *z32 + (*z2 + *z2);
  *z3 = (1.0 - *emsq) * *z33 + (*z3 + *z3);
  *s3 = 4.7968065E-7 * stem;
  *s2 = -0.5 * *s3 / *rtemsq;
  *s4 = *s3 * *rtemsq;
  *s1 = xnodce * *s4;
  xnodce = a9 * x4;
  stem = ctem * x3;
  *s5 = stem + xnodce;
  *s6 = a9 * x3 + ctem * x4;
  *s7 = xnodce - stem;
  *zmol = rt_remd_snf(((epoch + 18261.5) * 0.2299715 + 4.7199672) - *gam,
                      6.2831853071795862);
  *zmos = rt_remd_snf((epoch + 18261.5) * 0.017201977 + 6.2565837,
                      6.2831853071795862);
  xnodce = 2.0 * *ss1;
  *se2 = xnodce * *ss6;
  *se3 = xnodce * *ss7;
  xnodce = 2.0 * *ss2;
  *si2 = xnodce * *sz12;
  *si3 = (*sz13 - *sz11) * xnodce;
  stem = -2.0 * *ss3;
  *sl2 = stem * *sz2;
  *sl3 = (*sz3 - *sz1) * stem;
  xnodce = -21.0 - 9.0 * *emsq;
  *sl4 = xnodce * stem * 0.01675;
  stem = 2.0 * *ss4;
  *sgh2 = stem * *sz32;
  *sgh3 = (*sz33 - *sz31) * stem;
  *sgh4 = -18.0 * *ss4 * 0.01675;
  stem = -2.0 * *ss2;
  *sh2 = stem * *sz22;
  *sh3 = (*sz23 - *sz21) * stem;
  stem = 2.0 * *s1;
  *ee2 = stem * *s6;
  *e3 = stem * *s7;
  stem = 2.0 * *s2;
  *xi2 = stem * *z12;
  *xi3 = (*z13 - *z11) * stem;
  stem = -2.0 * *s3;
  *xl2 = stem * *z2;
  *xl3 = (*z3 - *z1) * stem;
  *xl4 = xnodce * stem * 0.0549;
  xnodce = 2.0 * *s4;
  *xgh2 = xnodce * *z32;
  *xgh3 = (*z33 - *z31) * xnodce;
  *xgh4 = -18.0 * *s4 * 0.0549;
  xnodce = -2.0 * *s2;
  *xh2 = xnodce * *z22;
  *xh3 = (*z23 - *z21) * xnodce;
}

/* Function for MATLAB Function: '<S4>/fsw_spg4_step' */
static void GNC_Planner_Core_dpper(real_T e3, real_T ee2, real_T se2, real_T se3,
  real_T sgh2, real_T sgh3, real_T sgh4, real_T sh2, real_T sh3, real_T si2,
  real_T si3, real_T sl2, real_T sl3, real_T sl4, real_T t, real_T xgh2, real_T
  xgh3, real_T xgh4, real_T xh2, real_T xh3, real_T xi2, real_T xi3, real_T xl2,
  real_T xl3, real_T xl4, real_T zmol, real_T zmos, char_T init, real_T *ep,
  real_T *inclp, real_T *nodep, real_T *argpp, real_T *mp)
{
  real_T ses;
  real_T sghs;
  real_T shs;
  real_T sinzf;
  real_T sis;
  real_T sls;
  real_T xnoh;
  real_T zf;
  real_T zm;
  zm = 1.19459E-5 * t + zmos;
  if (init == 'y') {
    zm = zmos;
  }

  zf = 0.0335 * sin(zm) + zm;
  sinzf = sin(zf);
  zm = 0.5 * sinzf * sinzf - 0.25;
  zf = -0.5 * sinzf * cos(zf);
  ses = se2 * zm + se3 * zf;
  sis = si2 * zm + si3 * zf;
  sls = (sl2 * zm + sl3 * zf) + sl4 * sinzf;
  sghs = (sgh2 * zm + sgh3 * zf) + sgh4 * sinzf;
  shs = sh2 * zm + sh3 * zf;
  zm = 0.00015835218 * t + zmol;
  if (init == 'y') {
    zm = zmol;
  }

  zf = 0.1098 * sin(zm) + zm;
  sinzf = sin(zf);
  zm = 0.5 * sinzf * sinzf - 0.25;
  zf = -0.5 * sinzf * cos(zf);
  if (init == 'n') {
    sis += xi2 * zm + xi3 * zf;
    sls += (xl2 * zm + xl3 * zf) + xl4 * sinzf;
    sghs += (xgh2 * zm + xgh3 * zf) + xgh4 * sinzf;
    shs += xh2 * zm + xh3 * zf;
    *inclp += sis;
    *ep += (ee2 * zm + e3 * zf) + ses;
    ses = sin(*inclp);
    zm = cos(*inclp);
    if (*inclp >= 0.2) {
      shs /= ses;
      *argpp += sghs - zm * shs;
      *nodep += shs;
      *mp += sls;
    } else {
      zf = sin(*nodep);
      sinzf = cos(*nodep);
      *nodep = rt_remd_snf(*nodep, 6.2831853071795862);
      if (*nodep < 0.0) {
        *nodep += 6.2831853071795862;
      }

      sghs = ((sls + sghs) - sis * *nodep * ses) + ((*mp + *argpp) + zm * *nodep);
      xnoh = *nodep;
      sis *= zm;
      *nodep = rt_atan2d_snf((sis * zf + shs * sinzf) + ses * zf, (sis * sinzf +
        -shs * zf) + ses * sinzf);
      if (*nodep < 0.0) {
        *nodep += 6.2831853071795862;
      }

      if (fabs(xnoh - *nodep) > 3.1415926535897931) {
        if (*nodep < xnoh) {
          *nodep += 6.2831853071795862;
        } else {
          *nodep -= 6.2831853071795862;
        }
      }

      *mp += sls;
      *argpp = (sghs - *mp) - zm * *nodep;
    }
  }
}

/* Function for MATLAB Function: '<S4>/fsw_spg4_step' */
static void GNC_Planner_Core_dsinit(real_T cosim, real_T emsq, real_T argpo,
  real_T s1, real_T s2, real_T s3, real_T s4, real_T s5, real_T sinim, real_T
  ss1, real_T ss2, real_T ss3, real_T ss4, real_T ss5, real_T sz1, real_T sz3,
  real_T sz11, real_T sz13, real_T sz21, real_T sz23, real_T sz31, real_T sz33,
  real_T gsto, real_T mo, real_T mdot, real_T no, real_T nodeo, real_T nodedot,
  real_T xpidot, real_T z1, real_T z3, real_T z11, real_T z13, real_T z21,
  real_T z23, real_T z31, real_T z33, real_T *em, real_T *argpm, real_T *inclm,
  real_T *mm, real_T *nm, real_T *nodem, real_T ecco, real_T eccsq, real_T *irez,
  real_T *atime, real_T *d2201, real_T *d2211, real_T *d3210, real_T *d3222,
  real_T *d4410, real_T *d4422, real_T *d5220, real_T *d5232, real_T *d5421,
  real_T *d5433, real_T *dedt, real_T *didt, real_T *dmdt, real_T *dnodt, real_T
  *domdt, real_T *del1, real_T *del2, real_T *del3, real_T *xfact, real_T *xlamo,
  real_T *xli, real_T *xni)
{
  real_T aonv;
  real_T cosisq;
  real_T eoc;
  real_T f220;
  real_T f220_tmp;
  real_T g211;
  real_T g310;
  real_T g322;
  real_T g410;
  real_T g422;
  real_T g520;
  real_T g521;
  real_T g532;
  real_T g533;
  real_T shll;
  real_T sini2;
  real_T temp1;
  boolean_T tmp;
  *irez = 0.0;
  if ((*nm < 0.0052359877) && (*nm > 0.0034906585)) {
    *irez = 1.0;
  }

  if ((*nm >= 0.00826) && (*nm <= 0.00924) && (*em >= 0.5)) {
    *irez = 2.0;
  }

  *d2201 = 0.0;
  *d2211 = 0.0;
  *d3210 = 0.0;
  *d3222 = 0.0;
  *d4410 = 0.0;
  *d4422 = 0.0;
  *d5220 = 0.0;
  *d5232 = 0.0;
  *d5421 = 0.0;
  *d5433 = 0.0;
  *del1 = 0.0;
  *del2 = 0.0;
  *del3 = 0.0;
  *atime = 0.0;
  *xfact = 0.0;
  *xlamo = 0.0;
  *xli = 0.0;
  *xni = 0.0;
  *dnodt = -1.19459E-5 * ss2 * (sz21 + sz23);
  tmp = ((*inclm < 0.052359877) || (*inclm > 3.0892327765897933));
  if (tmp) {
    *dnodt = 0.0;
  }

  if (sinim != 0.0) {
    *dnodt /= sinim;
  }

  *dedt = ss1 * 1.19459E-5 * ss5 + s1 * 0.00015835218 * s5;
  *didt = ss2 * 1.19459E-5 * (sz11 + sz13) + s2 * 0.00015835218 * (z11 + z13);
  shll = 6.0 * emsq;
  *dmdt = (((sz1 + sz3) - 14.0) - shll) * (-1.19459E-5 * ss3) - (((z1 + z3) -
    14.0) - shll) * (0.00015835218 * s3);
  shll = -0.00015835218 * s2 * (z21 + z23);
  if (tmp) {
    shll = 0.0;
  }

  *domdt = (((sz31 + sz33) - 6.0) * (ss4 * 1.19459E-5) - cosim * *dnodt) + ((z31
    + z33) - 6.0) * (s4 * 0.00015835218);
  if (sinim != 0.0) {
    *domdt -= cosim / sinim * shll;
    *dnodt += shll / sinim;
  }

  shll = rt_remd_snf(gsto, 6.2831853071795862);
  *em += *dedt * 0.0;
  *inclm += *didt * 0.0;
  *argpm = *domdt * 0.0;
  *nodem = *dnodt * 0.0;
  *mm = *dmdt * 0.0;
  if (*irez != 0.0) {
    aonv = rt_powd_snf(*nm / 0.074366916133173422, 0.66666666666666663);
    if (*irez == 2.0) {
      cosisq = cosim * cosim;
      eoc = ecco * eccsq;
      if (ecco <= 0.65) {
        g211 = (3.616 - 13.247 * ecco) + 16.29 * eccsq;
        g310 = ((117.39 * ecco - 19.302) - 228.419 * eccsq) + 156.591 * eoc;
        g322 = ((109.7927 * ecco - 18.9068) - 214.6334 * eccsq) + 146.5816 * eoc;
        g410 = ((242.694 * ecco - 41.122) - 471.094 * eccsq) + 313.953 * eoc;
        g422 = ((841.88 * ecco - 146.407) - 1629.014 * eccsq) + 1083.435 * eoc;
        g520 = ((3017.977 * ecco - 532.114) - 5740.032 * eccsq) + 3708.276 * eoc;
      } else {
        g211 = ((331.819 * ecco - 72.099) - 508.738 * eccsq) + 266.724 * eoc;
        g310 = ((1582.851 * ecco - 346.844) - 2415.925 * eccsq) + 1246.113 * eoc;
        g322 = ((1554.908 * ecco - 342.585) - 2366.899 * eccsq) + 1215.972 * eoc;
        g410 = ((4758.686 * ecco - 1052.797) - 7193.992 * eccsq) + 3651.957 *
          eoc;
        g422 = ((16178.11 * ecco - 3581.69) - 24462.77 * eccsq) + 12422.52 * eoc;
        if (ecco > 0.715) {
          g520 = ((29936.92 * ecco - 5149.66) - 54087.36 * eccsq) + 31324.56 *
            eoc;
        } else {
          g520 = (1464.74 - 4664.75 * ecco) + 3763.64 * eccsq;
        }
      }

      if (ecco < 0.7) {
        g533 = ((4988.61 * ecco - 919.2277) - 9064.77 * eccsq) + 5542.21 * eoc;
        g521 = ((4568.6173 * ecco - 822.71072) - 8491.4146 * eccsq) + 5337.524 *
          eoc;
        g532 = ((4690.25 * ecco - 853.666) - 8624.77 * eccsq) + 5341.4 * eoc;
      } else {
        g533 = ((161616.52 * ecco - 37995.78) - 229838.2 * eccsq) + 109377.94 *
          eoc;
        g521 = ((218913.95 * ecco - 51752.104) - 309468.16 * eccsq) + 146349.42 *
          eoc;
        g532 = ((170470.89 * ecco - 40023.88) - 242699.48 * eccsq) + 115605.82 *
          eoc;
      }

      sini2 = sinim * sinim;
      f220_tmp = 2.0 * cosim;
      f220 = ((f220_tmp + 1.0) + cosisq) * 0.75;
      temp1 = *nm * *nm * 3.0 * (aonv * aonv);
      eoc = temp1 * 1.7891679E-6;
      *d2201 = (-0.306 - (ecco - 0.64) * 0.44) * (eoc * f220);
      *d2211 = 1.5 * sini2 * eoc * g211;
      temp1 *= aonv;
      eoc = temp1 * 3.7393792E-7;
      g211 = 3.0 * cosisq;
      *d3210 = ((1.0 - f220_tmp) - g211) * (1.875 * sinim) * eoc * g310;
      g310 = (f220_tmp + 1.0) - g211;
      *d3222 = -1.875 * sinim * g310 * eoc * g322;
      temp1 *= aonv;
      eoc = 2.0 * temp1 * 7.3636953E-9;
      *d4410 = 35.0 * sini2 * f220 * eoc * g410;
      *d4422 = 39.375 * sini2 * sini2 * eoc * g422;
      temp1 *= aonv;
      eoc = temp1 * 1.1428639E-7;
      g422 = 4.0 * cosim;
      *d5220 = (((1.0 - f220_tmp) - 5.0 * cosisq) * sini2 + ((g422 - 2.0) + 6.0 *
                 cosisq) * 0.33333333) * (9.84375 * sinim) * eoc * g520;
      aonv = 10.0 * cosisq;
      *d5232 = (((-2.0 - g422) + aonv) * (4.92187512 * sini2) + g310 *
                6.56250012) * sinim * eoc * g532;
      eoc = 2.0 * temp1 * 2.1765803E-9;
      g532 = 8.0 * cosim;
      sini2 = 29.53125 * sinim;
      *d5421 = (((g532 - 12.0) + aonv) * cosisq + (2.0 - g532)) * sini2 * eoc *
        g521;
      *d5433 = (((g532 + 12.0) - aonv) * cosisq + (-2.0 - g532)) * sini2 * eoc *
        g533;
      *xlamo = rt_remd_snf((((mo + nodeo) + nodeo) - shll) - shll,
                           6.2831853071795862);
      *xfact = (((nodedot + *dnodt) - 0.0043752690880113) * 2.0 + (mdot + *dmdt))
        - no;
    } else {
      *del1 = 3.0 * *nm * *nm * aonv * aonv;
      cosisq = (cosim + 1.0) * 0.75;
      *del2 = ((0.8125 * emsq - 2.5) * emsq + 1.0) * ((cosim + 1.0) * cosisq *
        (2.0 * *del1)) * 1.7891679E-6;
      *del3 = (cosim + 1.0) * 1.875 * (cosim + 1.0) * (cosim + 1.0) * (3.0 *
        *del1) * ((6.60937 * emsq - 6.0) * emsq + 1.0) * 2.2123015E-7 * aonv;
      *del1 = (0.9375 * sinim * sinim * (3.0 * cosim + 1.0) - cosisq) * *del1 *
        (2.0 * emsq + 1.0) * 2.1460748E-6 * aonv;
      *xlamo = rt_remd_snf(((mo + nodeo) + argpo) - shll, 6.2831853071795862);
      *xfact = (((((mdot + xpidot) - 0.0043752690880113) + *dmdt) + *domdt) +
                *dnodt) - no;
    }

    *xli = *xlamo;
    *xni = no;
    *nm = no;
  }
}

/* Function for MATLAB Function: '<S4>/fsw_spg4_step' */
static real_T GNC_Planner_Core_dspace(real_T d2201, real_T d2211, real_T d3210,
  real_T d3222, real_T d4410, real_T d4422, real_T d5220, real_T d5232, real_T
  d5421, real_T d5433, real_T dedt, real_T del1, real_T del2, real_T del3,
  real_T didt, real_T dmdt, real_T dnodt, real_T domdt, real_T irez, real_T
  argpo, real_T argpdot, real_T t, real_T tc, real_T gsto, real_T xfact, real_T
  xlamo, real_T no, real_T *atime, real_T *em, real_T *argpm, real_T *inclm,
  real_T *xli, real_T *mm, real_T *xni, real_T *nodem, real_T *nm)
{
  real_T dndt;
  real_T ft;
  real_T x2li;
  real_T x2omi;
  real_T xnddt;
  real_T xndt_tmp;
  real_T xndt_tmp_0;
  real_T xndt_tmp_1;
  real_T xndt_tmp_2;
  real_T xndt_tmp_3;
  real_T xomi;
  int32_T exitg1;
  int16_T delt;
  int16_T iretn;
  dndt = 0.0;
  *em += dedt * t;
  *inclm += didt * t;
  *argpm += domdt * t;
  *nodem += dnodt * t;
  *mm += dmdt * t;
  ft = 0.0;
  if (irez != 0.0) {
    if ((*atime == 0.0) || (t * *atime <= 0.0) || (fabs(t) < fabs(*atime))) {
      *atime = 0.0;
      *xni = no;
      *xli = xlamo;
    }

    if (t >= 0.0) {
      delt = 720;
    } else {
      delt = -720;
    }

    iretn = 381;
    do {
      exitg1 = 0L;
      if (irez != 2.0) {
        xnddt = (*xli - 2.8843198) * 2.0;
        xndt_tmp = (*xli - 0.37448087) * 3.0;
        x2omi = (sin(*xli - 0.13130908) * del1 + sin(xnddt) * del2) + sin
          (xndt_tmp) * del3;
        xomi = *xni + xfact;
        xnddt = ((2.0 * del2 * cos(xnddt) + cos(*xli - 0.13130908) * del1) + 3.0
                 * del3 * cos(xndt_tmp)) * xomi;
      } else {
        xomi = argpdot * *atime + argpo;
        x2omi = xomi + xomi;
        x2li = *xli + *xli;
        xnddt = xomi + *xli;
        xndt_tmp = -xomi + *xli;
        xndt_tmp_0 = (x2omi + *xli) - 5.7686396;
        xndt_tmp_1 = (x2omi + x2li) - 1.8014998;
        xndt_tmp_2 = (xomi + x2li) - 4.4108898;
        xndt_tmp_3 = (-xomi + x2li) - 4.4108898;
        x2omi = ((((((((sin(*xli - 5.7686396) * d2211 + sin(xndt_tmp_0) * d2201)
                       + sin(xnddt - 0.95240898) * d3210) + sin(xndt_tmp -
          0.95240898) * d3222) + sin(xndt_tmp_1) * d4410) + sin(x2li - 1.8014998)
                    * d4422) + sin(xnddt - 1.050833) * d5220) + sin(xndt_tmp -
                   1.050833) * d5232) + sin(xndt_tmp_2) * d5421) + sin
          (xndt_tmp_3) * d5433;
        xomi = *xni + xfact;
        xnddt = ((((((cos(*xli - 5.7686396) * d2211 + cos(xndt_tmp_0) * d2201) +
                     cos(xnddt - 0.95240898) * d3210) + cos(xndt_tmp -
          0.95240898) * d3222) + cos(xnddt - 1.050833) * d5220) + cos(xndt_tmp -
                   1.050833) * d5232) + (((cos(x2li - 1.8014998) * d4422 + cos
                    (xndt_tmp_1) * d4410) + cos(xndt_tmp_2) * d5421) + cos
                  (xndt_tmp_3) * d5433) * 2.0) * xomi;
      }

      x2li = t - *atime;
      if (!(fabs(x2li) >= 720.0)) {
        ft = x2li;
        iretn = 0;
      }

      if (iretn == 381) {
        *xli = (xomi * (real_T)delt + *xli) + x2omi * 259200.0;
        *xni = (x2omi * (real_T)delt + *xni) + xnddt * 259200.0;
        *atime += (real_T)delt;
      } else {
        exitg1 = 1L;
      }
    } while (exitg1 == 0L);

    x2li = x2omi * ft;
    *nm = xnddt * ft * ft * 0.5 + (x2li + *xni);
    if (irez != 1.0) {
      *mm = ((x2li * ft * 0.5 + (xomi * ft + *xli)) - 2.0 * *nodem) +
        rt_remd_snf(tc * 0.0043752690880113 + gsto, 6.2831853071795862) * 2.0;
      dndt = *nm - no;
    } else {
      *mm = (((x2li * ft * 0.5 + (xomi * ft + *xli)) - *nodem) - *argpm) +
        rt_remd_snf(tc * 0.0043752690880113 + gsto, 6.2831853071795862);
      dndt = *nm - no;
    }

    *nm = no + dndt;
  }

  return dndt;
}

/* Function for MATLAB Function: '<S4>/fsw_spg4_step' */
static void GNC_Planner_Core_sgp4(siqtL0olslAz0Re7SMjm43B_GNC_P_T *satrec,
  real_T r_data[], int16_T r_size[2], real_T v_data[], int16_T v_size[2])
{
  real_T argpdf;
  real_T axnl;
  real_T aynl;
  real_T coseo1;
  real_T cosisq;
  real_T em;
  real_T inclm;
  real_T mrt;
  real_T nodem;
  real_T sineo1;
  real_T sinu;
  real_T temp;
  real_T tempa;
  real_T tempe;
  real_T templ;
  real_T xmdf;
  int16_T ktr;
  coseo1 = 0.0;
  sineo1 = 0.0;
  r_size[0] = 1;
  r_size[1] = 3;
  v_size[0] = 1;
  v_size[1] = 3;
  satrec->t = 0.0;
  satrec->error = 0.0;
  mrt = 0.0;
  xmdf = satrec->mdot * 0.0 + satrec->mo;
  argpdf = satrec->argpdot * 0.0 + satrec->argpo;
  nodem = (satrec->nodedot * 0.0 + satrec->nodeo) + satrec->nodecf * 0.0;
  tempa = 1.0 - satrec->cc1 * 0.0;
  tempe = satrec->bstar * satrec->cc4 * 0.0;
  templ = satrec->t2cof * 0.0;
  if (satrec->isimp != 1.0) {
    temp = (rt_powd_snf(satrec->eta * cos(xmdf) + 1.0, 3.0) - satrec->delmo) *
      satrec->xmcof + satrec->omgcof * 0.0;
    xmdf += temp;
    argpdf -= temp;
    tempa = ((tempa - satrec->d2 * 0.0) - satrec->d3 * 0.0) - satrec->d4 * 0.0;
    tempe += satrec->bstar * satrec->cc5 * (sin(xmdf) - satrec->sinmao);
    templ = (0.0 * satrec->t5cof + satrec->t4cof) * 0.0 + (satrec->t3cof * 0.0 +
      templ);
  }

  temp = satrec->no;
  em = satrec->ecco;
  inclm = satrec->inclo;
  if (satrec->method == 'd') {
    satrec->atime = 0.0;
    GNC_Planner_Core_dspace(satrec->d2201, satrec->d2211, satrec->d3210,
      satrec->d3222, satrec->d4410, satrec->d4422, satrec->d5220, satrec->d5232,
      satrec->d5421, satrec->d5433, satrec->dedt, satrec->del1, satrec->del2,
      satrec->del3, satrec->didt, satrec->dmdt, satrec->dnodt, satrec->domdt,
      satrec->irez, satrec->argpo, satrec->argpdot, 0.0, 0.0, satrec->gsto,
      satrec->xfact, satrec->xlamo, satrec->no, &satrec->atime, &em, &argpdf,
      &inclm, &satrec->xli, &xmdf, &satrec->xni, &nodem, &temp);
  }

  if (temp <= 0.0) {
    satrec->error = 2.0;
  }

  tempa *= rt_powd_snf(0.074366916133173422 / temp, 0.66666666666666663) * tempa;
  em -= tempe;
  if ((em >= 1.0) || (em < -0.001) || (tempa < 0.95)) {
    satrec->error = 1.0;
  }

  if (em < 1.0E-6) {
    em = 1.0E-6;
  }

  templ = ((satrec->no * templ + xmdf) + argpdf) + nodem;
  nodem = rt_remd_snf(nodem, 6.2831853071795862);
  argpdf = rt_remd_snf(argpdf, 6.2831853071795862);
  xmdf = sin(inclm);
  tempe = cos(inclm);
  templ = rt_remd_snf((rt_remd_snf(templ, 6.2831853071795862) - argpdf) - nodem,
                      6.2831853071795862);
  if (satrec->method == 'd') {
    GNC_Planner_Core_dpper(satrec->e3, satrec->ee2, satrec->se2, satrec->se3,
      satrec->sgh2, satrec->sgh3, satrec->sgh4, satrec->sh2, satrec->sh3,
      satrec->si2, satrec->si3, satrec->sl2, satrec->sl3, satrec->sl4, 0.0,
      satrec->xgh2, satrec->xgh3, satrec->xgh4, satrec->xh2, satrec->xh3,
      satrec->xi2, satrec->xi3, satrec->xl2, satrec->xl3, satrec->xl4,
      satrec->zmol, satrec->zmos, '\x79', &em, &inclm, &nodem, &argpdf, &templ);
    if (inclm < 0.0) {
      inclm = -inclm;
      nodem += 3.1415926535897931;
      argpdf -= 3.1415926535897931;
    }

    if ((em < 0.0) || (em > 1.0)) {
      satrec->error = 3.0;
    }
  }

  if (satrec->method == 'd') {
    xmdf = sin(inclm);
    tempe = cos(inclm);
    satrec->aycof = 0.0011725348600057639 * xmdf;
    if (tempe + 1.0 > 1.5E-12) {
      satrec->xlcof = (5.0 * tempe + 3.0) * (0.000586267430002882 * xmdf) /
        (tempe + 1.0);
    } else {
      satrec->xlcof = (5.0 * tempe + 3.0) * (0.000586267430002882 * xmdf) /
        1.5E-12;
    }
  }

  axnl = em * cos(argpdf);
  temp = 1.0 / ((1.0 - em * em) * tempa);
  aynl = em * sin(argpdf) + temp * satrec->aycof;
  argpdf = rt_remd_snf((((templ + argpdf) + nodem) + temp * satrec->xlcof * axnl)
                       - nodem, 6.2831853071795862);
  em = argpdf;
  templ = 9999.9;
  ktr = 1;
  while ((fabs(templ) >= 1.0E-12) && (ktr <= 10)) {
    sineo1 = sin(em);
    coseo1 = cos(em);
    templ = (((argpdf - aynl * coseo1) + axnl * sineo1) - em) / ((1.0 - coseo1 *
      axnl) - sineo1 * aynl);
    if (fabs(templ) >= 0.95) {
      if (templ > 0.0) {
        templ = 0.95;
      } else {
        templ = -0.95;
      }
    }

    em += templ;
    ktr++;
  }

  templ = axnl * sineo1 - aynl * coseo1;
  temp = axnl * axnl + aynl * aynl;
  argpdf = (1.0 - temp) * tempa;
  if (argpdf < 0.0) {
    satrec->error = 4.0;
    r_size[0] = 3;
    r_size[1] = 1;
    v_size[0] = 3;
    v_size[1] = 1;
    r_data[0] = 0.0;
    v_data[0] = 0.0;
    r_data[1] = 0.0;
    v_data[1] = 0.0;
    r_data[2] = 0.0;
    v_data[2] = 0.0;
  } else {
    em = (1.0 - (axnl * coseo1 + aynl * sineo1)) * tempa;
    mrt = sqrt(1.0 - temp);
    temp = templ / (mrt + 1.0);
    cosisq = tempa / em;
    sinu = ((sineo1 - aynl) - axnl * temp) * cosisq;
    aynl = ((coseo1 - axnl) + aynl * temp) * cosisq;
    sineo1 = (aynl + aynl) * sinu;
    coseo1 = 1.0 - 2.0 * sinu * sinu;
    temp = 1.0 / argpdf;
    axnl = 0.000541308 * temp;
    temp *= axnl;
    if (satrec->method == 'd') {
      cosisq = tempe * tempe;
      satrec->con41 = 3.0 * cosisq - 1.0;
      satrec->x1mth2 = 1.0 - cosisq;
      satrec->x7thm1 = 7.0 * cosisq - 1.0;
    }

    cosisq = 1.5 * temp;
    mrt = (1.0 - cosisq * mrt * satrec->con41) * em + 0.5 * axnl *
      satrec->x1mth2 * coseo1;
    temp = rt_atan2d_snf(sinu, aynl) - 0.25 * temp * satrec->x7thm1 * sineo1;
    tempe *= cosisq;
    nodem += tempe * sineo1;
    xmdf = tempe * xmdf * coseo1 + inclm;
    tempe = 0.074366916133173422 / rt_powd_snf(tempa, 1.5) * axnl;
    inclm = sqrt(tempa) * templ / em - tempe * satrec->x1mth2 * sineo1 /
      0.074366916133173422;
    tempa = (satrec->x1mth2 * coseo1 + 1.5 * satrec->con41) * tempe /
      0.074366916133173422 + sqrt(argpdf) / em;
    tempe = sin(temp);
    argpdf = cos(temp);
    em = sin(nodem);
    nodem = cos(nodem);
    coseo1 = sin(xmdf);
    templ = cos(xmdf);
    xmdf = -em * templ;
    templ *= nodem;
    sineo1 = xmdf * tempe + nodem * argpdf;
    temp = templ * tempe + em * argpdf;
    axnl = coseo1 * tempe;
    r_data[0] = mrt * sineo1 * 6378.135;
    r_data[1] = mrt * temp * 6378.135;
    r_data[2] = mrt * axnl * 6378.135;
    v_data[0] = ((xmdf * argpdf - nodem * tempe) * tempa + inclm * sineo1) *
      7.905370510517634;
    v_data[1] = ((templ * argpdf - em * tempe) * tempa + inclm * temp) *
      7.905370510517634;
    v_data[2] = (coseo1 * argpdf * tempa + inclm * axnl) * 7.905370510517634;
  }

  if (mrt < 1.0) {
    satrec->error = 6.0;
  }
}

/* Function for MATLAB Function: '<S4>/fsw_spg4_step' */
static void GNC_Planner_Core_sgp4init(real_T epoch, real_T xbstar, real_T xndot,
  real_T xnddot, real_T xecco, real_T xargpo, real_T xinclo, real_T xmo, real_T
  xno_kozai, real_T xnodeo, siqtL0olslAz0Re7SMjm43B_GNC_P_T *satrec)
{
  real_T r_data[9];
  real_T v_data[9];
  real_T ao;
  real_T b_nm;
  real_T cc1sq;
  real_T cc3;
  real_T cnodm;
  real_T coef1;
  real_T cosio2;
  real_T cosomm;
  real_T day;
  real_T etasq;
  real_T gam;
  real_T omeosq;
  real_T perige;
  real_T pinvsq;
  real_T psisq;
  real_T qzms24;
  real_T rtemsq;
  real_T rteosq;
  real_T s6;
  real_T s7;
  real_T satrec_tmp;
  real_T sfour;
  real_T sinio;
  real_T sinomm;
  real_T snodm;
  real_T ss6;
  real_T ss7;
  real_T sz11;
  real_T sz12;
  real_T sz13;
  real_T sz2;
  real_T sz21;
  real_T sz22;
  real_T sz23;
  real_T sz3;
  real_T sz31;
  real_T sz32;
  real_T sz33;
  real_T temp;
  real_T temp2_tmp;
  real_T tsi;
  real_T xhdot1;
  real_T z1;
  real_T z11;
  real_T z12;
  real_T z13;
  real_T z2;
  real_T z21;
  real_T z22;
  real_T z23;
  real_T z3;
  real_T z31;
  real_T z32;
  real_T z33;
  int16_T r_size[2];
  int16_T v_size[2];
  satrec->satnum = 0.0;
  satrec->epochyr = 0.0;
  satrec->epochdays = 0.0;
  satrec->no_unkozai = 0.0;
  satrec->ephtype = 0.0;
  satrec->classification = 'U';
  satrec->t2 = 0.0;
  satrec->t3 = 0.0;
  satrec->t4 = 0.0;
  satrec->t5 = 0.0;
  satrec->om = 0.0;
  satrec->t0 = 0.0;
  satrec->isimp = 0.0;
  satrec->aycof = 0.0;
  satrec->cc1 = 0.0;
  satrec->cc4 = 0.0;
  satrec->cc5 = 0.0;
  satrec->d2 = 0.0;
  satrec->d3 = 0.0;
  satrec->d4 = 0.0;
  satrec->delmo = 0.0;
  satrec->eta = 0.0;
  satrec->argpdot = 0.0;
  satrec->omgcof = 0.0;
  satrec->sinmao = 0.0;
  satrec->t2cof = 0.0;
  satrec->t3cof = 0.0;
  satrec->t4cof = 0.0;
  satrec->t5cof = 0.0;
  satrec->x1mth2 = 0.0;
  satrec->x7thm1 = 0.0;
  satrec->mdot = 0.0;
  satrec->nodedot = 0.0;
  satrec->xlcof = 0.0;
  satrec->xmcof = 0.0;
  satrec->nodecf = 0.0;
  satrec->irez = 0.0;
  satrec->d2201 = 0.0;
  satrec->d2211 = 0.0;
  satrec->d3210 = 0.0;
  satrec->d3222 = 0.0;
  satrec->d4410 = 0.0;
  satrec->d4422 = 0.0;
  satrec->d5220 = 0.0;
  satrec->d5232 = 0.0;
  satrec->d5421 = 0.0;
  satrec->d5433 = 0.0;
  satrec->dedt = 0.0;
  satrec->del1 = 0.0;
  satrec->del2 = 0.0;
  satrec->del3 = 0.0;
  satrec->didt = 0.0;
  satrec->dmdt = 0.0;
  satrec->dnodt = 0.0;
  satrec->domdt = 0.0;
  satrec->e3 = 0.0;
  satrec->ee2 = 0.0;
  satrec->peo = 0.0;
  satrec->pgho = 0.0;
  satrec->pho = 0.0;
  satrec->pinco = 0.0;
  satrec->plo = 0.0;
  satrec->se2 = 0.0;
  satrec->se3 = 0.0;
  satrec->sgh2 = 0.0;
  satrec->sgh3 = 0.0;
  satrec->sgh4 = 0.0;
  satrec->sh2 = 0.0;
  satrec->sh3 = 0.0;
  satrec->si2 = 0.0;
  satrec->si3 = 0.0;
  satrec->sl2 = 0.0;
  satrec->sl3 = 0.0;
  satrec->sl4 = 0.0;
  satrec->xfact = 0.0;
  satrec->xgh2 = 0.0;
  satrec->xgh3 = 0.0;
  satrec->xgh4 = 0.0;
  satrec->xh2 = 0.0;
  satrec->xh3 = 0.0;
  satrec->xi2 = 0.0;
  satrec->xi3 = 0.0;
  satrec->xl2 = 0.0;
  satrec->xl3 = 0.0;
  satrec->xl4 = 0.0;
  satrec->xlamo = 0.0;
  satrec->zmol = 0.0;
  satrec->zmos = 0.0;
  satrec->atime = 0.0;
  satrec->xli = 0.0;
  satrec->xni = 0.0;
  satrec->operationmode = 'a';
  satrec->error = 0.0;
  satrec->bstar = xbstar;
  satrec->ndot = xndot;
  satrec->nddot = xnddot;
  satrec->ecco = xecco;
  satrec->argpo = xargpo;
  satrec->inclo = xinclo;
  satrec->mo = xmo;
  satrec->no_kozai = xno_kozai;
  satrec->nodeo = xnodeo;
  satrec->am = 0.0;
  satrec->em = 0.0;
  satrec->im = 0.0;
  satrec->Om = 0.0;
  satrec->mm = 0.0;
  satrec->nm = 0.0;
  satrec->tumin = 13.446839696959309;
  satrec->mu = 398600.8;
  satrec->radiusearthkm = 6378.135;
  satrec->xke = 0.074366916133173422;
  satrec->j2 = 0.001082616;
  satrec->j3 = -2.53881E-6;
  satrec->j4 = -1.65597E-6;
  satrec->j3oj2 = -0.0023450697200115278;
  satrec->t = 0.0;
  GNC_Planner_Core_initl(xecco, epoch, xinclo, xno_kozai, &sfour, &ao,
    &satrec->con41, &xhdot1, &temp, &cosio2, &cc1sq, &omeosq, &tsi, &perige,
    &rteosq, &sinio, &satrec->gsto, &satrec->no);
  satrec->method = 'n';
  satrec->a = rt_powd_snf(satrec->no * 13.446839696959309, -0.66666666666666663);
  satrec->alta = (xecco + 1.0) * satrec->a - 1.0;
  satrec->altp = (1.0 - xecco) * satrec->a - 1.0;
  if ((omeosq >= 0.0) || (satrec->no >= 0.0)) {
    satrec->isimp = 0.0;
    if (perige < 1.0344928415594841) {
      satrec->isimp = 1.0;
    }

    sfour = 1.0122292801892716;
    qzms24 = 1.8802791590152709E-9;
    perige = (perige - 1.0) * 6378.135;
    if (perige < 156.0) {
      sfour = perige - 78.0;
      if (perige < 98.0) {
        sfour = 20.0;
      }

      qzms24 = rt_powd_snf((120.0 - sfour) / 6378.135, 4.0);
      sfour = sfour / 6378.135 + 1.0;
    }

    pinvsq = 1.0 / tsi;
    tsi = 1.0 / (ao - sfour);
    satrec->eta = ao * xecco * tsi;
    etasq = satrec->eta * satrec->eta;
    perige = xecco * satrec->eta;
    psisq = fabs(1.0 - etasq);
    qzms24 *= rt_powd_snf(tsi, 4.0);
    coef1 = qzms24 / rt_powd_snf(psisq, 3.5);
    satrec->cc1 = (((1.5 * etasq + 1.0) + (etasq + 4.0) * perige) * ao +
                   0.00040598100000000003 * tsi / psisq * satrec->con41 * (3.0 *
      etasq * (etasq + 8.0) + 8.0)) * (coef1 * satrec->no) * xbstar;
    cc3 = 0.0;
    if (xecco > 0.0001) {
      cc3 = -2.0 * qzms24 * tsi * -0.0023450697200115278 * satrec->no * sinio /
        xecco;
    }

    satrec->x1mth2 = 1.0 - cosio2;
    satrec_tmp = 2.0 * etasq;
    satrec->cc4 = (((0.5 * etasq + 2.0) * satrec->eta + (satrec_tmp + 0.5) *
                    xecco) - (((1.5 - 0.5 * perige) * etasq + (1.0 - 2.0 *
      perige)) * (-3.0 * satrec->con41) + (satrec_tmp - (etasq + 1.0) * perige) *
      ((1.0 - cosio2) * 0.75) * cos(2.0 * xargpo)) * (0.001082616 * tsi / (ao *
      psisq))) * (2.0 * satrec->no * coef1 * ao * omeosq);
    satrec->cc5 = (((etasq + perige) * 2.75 + 1.0) + perige * etasq) * (2.0 *
      coef1 * ao * omeosq);
    psisq = cosio2 * cosio2;
    coef1 = 0.0016239240000000001 * pinvsq * satrec->no;
    temp2_tmp = 0.5 * coef1;
    etasq = temp2_tmp * 0.001082616 * pinvsq;
    pinvsq = 7.762359375E-7 * pinvsq * pinvsq * satrec->no;
    satrec_tmp = 0.0625 * etasq;
    satrec->mdot = ((13.0 - 78.0 * cosio2) + 137.0 * psisq) * (satrec_tmp *
      rteosq) + (temp2_tmp * rteosq * satrec->con41 + satrec->no);
    satrec->argpdot = (((7.0 - 114.0 * cosio2) + 395.0 * psisq) * satrec_tmp +
                       -0.5 * coef1 * xhdot1) + ((3.0 - 36.0 * cosio2) + 49.0 *
      psisq) * pinvsq;
    xhdot1 = -coef1 * temp;
    satrec_tmp = 7.0 * cosio2;
    satrec->nodedot = ((4.0 - 19.0 * cosio2) * (0.5 * etasq) + (3.0 - satrec_tmp)
                       * (2.0 * pinvsq)) * temp + xhdot1;
    satrec->omgcof = xbstar * cc3 * cos(xargpo);
    satrec->xmcof = 0.0;
    if (xecco > 0.0001) {
      satrec->xmcof = -0.66666666666666663 * qzms24 * xbstar / perige;
    }

    satrec->nodecf = 3.5 * omeosq * xhdot1 * satrec->cc1;
    satrec->t2cof = 1.5 * satrec->cc1;
    if (fabs(temp + 1.0) > 1.5E-12) {
      satrec->xlcof = (5.0 * temp + 3.0) * (0.000586267430002882 * sinio) /
        (temp + 1.0);
    } else {
      satrec->xlcof = (5.0 * temp + 3.0) * (0.000586267430002882 * sinio) /
        1.5E-12;
    }

    satrec->aycof = 0.0011725348600057639 * sinio;
    satrec->delmo = rt_powd_snf(satrec->eta * cos(xmo) + 1.0, 3.0);
    satrec->sinmao = sin(xmo);
    satrec->x7thm1 = satrec_tmp - 1.0;
    if (6.2831853071795862 / satrec->no >= 225.0) {
      satrec->method = 'd';
      satrec->isimp = 1.0;
      GNC_Planner_Core_dscom(epoch, xecco, xargpo, xinclo, xnodeo, satrec->no,
        &temp, &cosio2, &sinomm, &cosomm, &snodm, &cnodm, &day, &satrec->e3,
        &satrec->ee2, &omeosq, &sinio, &gam, &rtemsq, &satrec->se2, &satrec->se3,
        &satrec->sgh2, &satrec->sgh3, &satrec->sgh4, &satrec->sh2, &satrec->sh3,
        &satrec->si2, &satrec->si3, &satrec->sl2, &satrec->sl3, &satrec->sl4,
        &xhdot1, &rteosq, &perige, &qzms24, &cc3, &s6, &s7, &satrec_tmp, &pinvsq,
        &etasq, &psisq, &coef1, &ss6, &ss7, &temp2_tmp, &sz2, &sz3, &sz11, &sz12,
        &sz13, &sz21, &sz22, &sz23, &sz31, &sz32, &sz33, &satrec->xgh2,
        &satrec->xgh3, &satrec->xgh4, &satrec->xh2, &satrec->xh3, &satrec->xi2,
        &satrec->xi3, &satrec->xl2, &satrec->xl3, &satrec->xl4, &b_nm, &z1, &z2,
        &z3, &z11, &z12, &z13, &z21, &z22, &z23, &z31, &z32, &z33, &satrec->zmol,
        &satrec->zmos);
      satrec->peo = 0.0;
      satrec->pgho = 0.0;
      satrec->pho = 0.0;
      satrec->pinco = 0.0;
      satrec->plo = 0.0;
      GNC_Planner_Core_dpper(satrec->e3, satrec->ee2, satrec->se2, satrec->se3,
        satrec->sgh2, satrec->sgh3, satrec->sgh4, satrec->sh2, satrec->sh3,
        satrec->si2, satrec->si3, satrec->sl2, satrec->sl3, satrec->sl4, 0.0,
        satrec->xgh2, satrec->xgh3, satrec->xgh4, satrec->xh2, satrec->xh3,
        satrec->xi2, satrec->xi3, satrec->xl2, satrec->xl3, satrec->xl4,
        satrec->zmol, satrec->zmos, '\x79', &satrec->ecco, &satrec->inclo,
        &satrec->nodeo, &satrec->argpo, &satrec->mo);
      sinomm = xinclo;
      GNC_Planner_Core_dsinit(cosio2, sinio, satrec->argpo, xhdot1, rteosq,
        perige, qzms24, cc3, temp, satrec_tmp, pinvsq, etasq, psisq, coef1,
        temp2_tmp, sz3, sz11, sz13, sz21, sz23, sz31, sz33, satrec->gsto,
        satrec->mo, satrec->mdot, satrec->no, satrec->nodeo, satrec->nodedot,
        satrec->argpdot + satrec->nodedot, z1, z3, z11, z13, z21, z23, z31, z33,
        &omeosq, &cosomm, &sinomm, &snodm, &b_nm, &cnodm, satrec->ecco, cc1sq,
        &satrec->irez, &day, &satrec->d2201, &satrec->d2211, &satrec->d3210,
        &satrec->d3222, &satrec->d4410, &satrec->d4422, &satrec->d5220,
        &satrec->d5232, &satrec->d5421, &satrec->d5433, &satrec->dedt,
        &satrec->didt, &satrec->dmdt, &satrec->dnodt, &satrec->domdt,
        &satrec->del1, &satrec->del2, &satrec->del3, &satrec->xfact,
        &satrec->xlamo, &satrec->xli, &satrec->xni);
      satrec->atime = 0.0;
    }

    if (satrec->isimp != 1.0) {
      cc1sq = satrec->cc1 * satrec->cc1;
      satrec->d2 = 4.0 * ao * tsi * cc1sq;
      temp = satrec->d2 * tsi * satrec->cc1 / 3.0;
      satrec->d3 = (17.0 * ao + sfour) * temp;
      satrec->d4 = 0.5 * temp * ao * tsi * (221.0 * ao + 31.0 * sfour) *
        satrec->cc1;
      satrec->t3cof = 2.0 * cc1sq + satrec->d2;
      satrec->t4cof = ((12.0 * satrec->d2 + 10.0 * cc1sq) * satrec->cc1 + 3.0 *
                       satrec->d3) * 0.25;
      satrec->t5cof = (((12.0 * satrec->cc1 * satrec->d3 + 3.0 * satrec->d4) +
                        6.0 * satrec->d2 * satrec->d2) + (2.0 * satrec->d2 +
        cc1sq) * (15.0 * cc1sq)) * 0.2;
    }
  }

  GNC_Planner_Core_sgp4(satrec, r_data, r_size, v_data, v_size);
  satrec->init = 'n';
}

/* Function for MATLAB Function: '<S4>/fsw_spg4_step' */
static void GNC_Planner_Core_sgp4_e(satrec_t_GNC_Planner_Core_T *satrec, real_T
  tsince, real_T r_data[], int16_T r_size[2], real_T v_data[], int16_T v_size[2])
{
  real_T argpdf;
  real_T axnl;
  real_T aynl;
  real_T coseo1;
  real_T cosisq;
  real_T mrt;
  real_T nodem;
  real_T sineo1;
  real_T sinu;
  real_T t2;
  real_T t3;
  real_T temp;
  real_T tempa;
  real_T tempe;
  real_T templ;
  real_T vkmpersec;
  real_T xmdf;
  int16_T ktr;
  coseo1 = 0.0;
  sineo1 = 0.0;
  r_size[0] = 1;
  r_size[1] = 3;
  v_size[0] = 1;
  v_size[1] = 3;
  vkmpersec = satrec->radiusearthkm * satrec->xke / 60.0;
  satrec->t = tsince;
  satrec->error = 0.0;
  mrt = 0.0;
  xmdf = satrec->mdot * tsince + satrec->mo;
  argpdf = satrec->argpdot * tsince + satrec->argpo;
  t2 = tsince * tsince;
  nodem = (satrec->nodedot * tsince + satrec->nodeo) + satrec->nodecf * t2;
  tempa = 1.0 - satrec->cc1 * tsince;
  tempe = satrec->bstar * satrec->cc4 * tsince;
  templ = satrec->t2cof * t2;
  if (satrec->isimp != 1.0) {
    temp = (rt_powd_snf(satrec->eta * cos(xmdf) + 1.0, 3.0) - satrec->delmo) *
      satrec->xmcof + satrec->omgcof * tsince;
    xmdf += temp;
    argpdf -= temp;
    t3 = t2 * tsince;
    temp = t3 * tsince;
    tempa = ((tempa - satrec->d2 * t2) - satrec->d3 * t3) - satrec->d4 * temp;
    tempe += satrec->bstar * satrec->cc5 * (sin(xmdf) - satrec->sinmao);
    templ = (tsince * satrec->t5cof + satrec->t4cof) * temp + (satrec->t3cof *
      t3 + templ);
  }

  temp = satrec->no;
  t3 = satrec->ecco;
  t2 = satrec->inclo;
  if (satrec->method == 'd') {
    GNC_Planner_Core_dspace(satrec->d2201, satrec->d2211, satrec->d3210,
      satrec->d3222, satrec->d4410, satrec->d4422, satrec->d5220, satrec->d5232,
      satrec->d5421, satrec->d5433, satrec->dedt, satrec->del1, satrec->del2,
      satrec->del3, satrec->didt, satrec->dmdt, satrec->dnodt, satrec->domdt,
      satrec->irez, satrec->argpo, satrec->argpdot, tsince, tsince, satrec->gsto,
      satrec->xfact, satrec->xlamo, satrec->no, &satrec->atime, &t3, &argpdf,
      &t2, &satrec->xli, &xmdf, &satrec->xni, &nodem, &temp);
  }

  if (temp <= 0.0) {
    satrec->error = 2.0;
  }

  tempa *= rt_powd_snf(satrec->xke / temp, 0.66666666666666663) * tempa;
  t3 -= tempe;
  if ((t3 >= 1.0) || (t3 < -0.001) || (tempa < 0.95)) {
    satrec->error = 1.0;
  }

  if (t3 < 1.0E-6) {
    t3 = 1.0E-6;
  }

  templ = ((satrec->no * templ + xmdf) + argpdf) + nodem;
  nodem = rt_remd_snf(nodem, 6.2831853071795862);
  argpdf = rt_remd_snf(argpdf, 6.2831853071795862);
  xmdf = sin(t2);
  tempe = cos(t2);
  templ = rt_remd_snf((rt_remd_snf(templ, 6.2831853071795862) - argpdf) - nodem,
                      6.2831853071795862);
  if (satrec->method == 'd') {
    GNC_Planner_Core_dpper(satrec->e3, satrec->ee2, satrec->se2, satrec->se3,
      satrec->sgh2, satrec->sgh3, satrec->sgh4, satrec->sh2, satrec->sh3,
      satrec->si2, satrec->si3, satrec->sl2, satrec->sl3, satrec->sl4, tsince,
      satrec->xgh2, satrec->xgh3, satrec->xgh4, satrec->xh2, satrec->xh3,
      satrec->xi2, satrec->xi3, satrec->xl2, satrec->xl3, satrec->xl4,
      satrec->zmol, satrec->zmos, satrec->init, &t3, &t2, &nodem, &argpdf,
      &templ);
    if (t2 < 0.0) {
      t2 = -t2;
      nodem += 3.1415926535897931;
      argpdf -= 3.1415926535897931;
    }

    if ((t3 < 0.0) || (t3 > 1.0)) {
      satrec->error = 3.0;
    }
  }

  if (satrec->method == 'd') {
    xmdf = sin(t2);
    tempe = cos(t2);
    satrec->aycof = -0.5 * satrec->j3oj2 * xmdf;
    if (tempe + 1.0 > 1.5E-12) {
      satrec->xlcof = -0.25 * satrec->j3oj2 * xmdf * (5.0 * tempe + 3.0) /
        (tempe + 1.0);
    } else {
      satrec->xlcof = -0.25 * satrec->j3oj2 * xmdf * (5.0 * tempe + 3.0) /
        1.5E-12;
    }
  }

  axnl = t3 * cos(argpdf);
  temp = 1.0 / ((1.0 - t3 * t3) * tempa);
  aynl = t3 * sin(argpdf) + temp * satrec->aycof;
  argpdf = rt_remd_snf((((templ + argpdf) + nodem) + temp * satrec->xlcof * axnl)
                       - nodem, 6.2831853071795862);
  t3 = argpdf;
  templ = 9999.9;
  ktr = 1;
  while ((fabs(templ) >= 1.0E-12) && (ktr <= 10)) {
    sineo1 = sin(t3);
    coseo1 = cos(t3);
    templ = (((argpdf - aynl * coseo1) + axnl * sineo1) - t3) / ((1.0 - coseo1 *
      axnl) - sineo1 * aynl);
    if (fabs(templ) >= 0.95) {
      if (templ > 0.0) {
        templ = 0.95;
      } else {
        templ = -0.95;
      }
    }

    t3 += templ;
    ktr++;
  }

  templ = axnl * sineo1 - aynl * coseo1;
  temp = axnl * axnl + aynl * aynl;
  argpdf = (1.0 - temp) * tempa;
  if (argpdf < 0.0) {
    satrec->error = 4.0;
    r_size[0] = 3;
    r_size[1] = 1;
    v_size[0] = 3;
    v_size[1] = 1;
    r_data[0] = 0.0;
    v_data[0] = 0.0;
    r_data[1] = 0.0;
    v_data[1] = 0.0;
    r_data[2] = 0.0;
    v_data[2] = 0.0;
  } else {
    t3 = (1.0 - (axnl * coseo1 + aynl * sineo1)) * tempa;
    mrt = sqrt(1.0 - temp);
    temp = templ / (mrt + 1.0);
    cosisq = tempa / t3;
    sinu = ((sineo1 - aynl) - axnl * temp) * cosisq;
    aynl = ((coseo1 - axnl) + aynl * temp) * cosisq;
    sineo1 = (aynl + aynl) * sinu;
    coseo1 = 1.0 - 2.0 * sinu * sinu;
    temp = 1.0 / argpdf;
    axnl = 0.5 * satrec->j2 * temp;
    temp *= axnl;
    if (satrec->method == 'd') {
      cosisq = tempe * tempe;
      satrec->con41 = 3.0 * cosisq - 1.0;
      satrec->x1mth2 = 1.0 - cosisq;
      satrec->x7thm1 = 7.0 * cosisq - 1.0;
    }

    cosisq = 1.5 * temp;
    mrt = (1.0 - cosisq * mrt * satrec->con41) * t3 + 0.5 * axnl *
      satrec->x1mth2 * coseo1;
    temp = rt_atan2d_snf(sinu, aynl) - 0.25 * temp * satrec->x7thm1 * sineo1;
    aynl = cosisq * tempe;
    tempe = aynl * sineo1 + nodem;
    t2 += aynl * xmdf * coseo1;
    xmdf = satrec->xke / rt_powd_snf(tempa, 1.5) * axnl;
    nodem = sqrt(tempa) * templ / t3 - xmdf * satrec->x1mth2 * sineo1 /
      satrec->xke;
    tempa = (satrec->x1mth2 * coseo1 + 1.5 * satrec->con41) * xmdf / satrec->xke
      + sqrt(argpdf) / t3;
    xmdf = sin(temp);
    coseo1 = cos(temp);
    sineo1 = sin(tempe);
    tempe = cos(tempe);
    argpdf = sin(t2);
    t3 = cos(t2);
    t2 = -sineo1 * t3;
    t3 *= tempe;
    templ = t2 * xmdf + tempe * coseo1;
    temp = t3 * xmdf + sineo1 * coseo1;
    axnl = argpdf * xmdf;
    r_data[0] = mrt * templ * satrec->radiusearthkm;
    r_data[1] = mrt * temp * satrec->radiusearthkm;
    r_data[2] = mrt * axnl * satrec->radiusearthkm;
    v_data[0] = ((t2 * coseo1 - tempe * xmdf) * tempa + nodem * templ) *
      vkmpersec;
    v_data[1] = ((t3 * coseo1 - sineo1 * xmdf) * tempa + nodem * temp) *
      vkmpersec;
    v_data[2] = (argpdf * coseo1 * tempa + nodem * axnl) * vkmpersec;
  }

  if (mrt < 1.0) {
    satrec->error = 6.0;
  }
}

/* Function for Chart: '<S1>/GNC State Machine' */
static void GNC_Planner_Core_SAFE(void)
{
  real_T y[3];
  real_T y_tmp;
  real_T y_tmp_0;
  real_T y_tmp_1;
  int16_T e_k;
  boolean_T exitg1;
  boolean_T guard1;
  boolean_T guard2;
  boolean_T guard3;
  boolean_T out;

  /* Inport: '<Root>/RATE' */
  y_tmp = fabs(GNC_Planner_Core_U.w[0]);
  y[0] = y_tmp;

  /* Inport: '<Root>/RATE' */
  y_tmp_0 = fabs(GNC_Planner_Core_U.w[1]);
  y[1] = y_tmp_0;

  /* Inport: '<Root>/RATE' */
  y_tmp_1 = fabs(GNC_Planner_Core_U.w[2]);
  y[2] = y_tmp_1;
  out = true;
  e_k = 0;
  exitg1 = false;
  while ((!exitg1) && (e_k < 3)) {
    if (!(y[e_k] <= 360.0)) {
      out = false;
      exitg1 = true;
    } else {
      e_k++;
    }
  }

  if (out) {
    if (GNC_Planner_Core_B.SUN && (!GNC_Planner_Core_B.TUMB)) {
      /* Outport: '<Root>/GNC_Planner_Mode' */
      GNC_Planner_Core_Y.GNC_Planner_Mode = BasicModeType_SUNSPIN;

      /* Inport: '<Root>/alt_sunspin_enabled' */
      if (GNC_Planner_Core_U.alt_sunspin_enabled) {
        /* Outport: '<Root>/commanded_mode' */
        GNC_Planner_Core_Y.commanded_mode = MANUAL;
        GNC_Planner_Core_DW.expected_mode = MANUAL;
      } else {
        /* Outport: '<Root>/commanded_mode' */
        GNC_Planner_Core_Y.commanded_mode = SUNSPIN;
        GNC_Planner_Core_DW.expected_mode = SUNSPIN;
      }

      /* End of Inport: '<Root>/alt_sunspin_enabled' */
    } else {
      /* Inport: '<Root>/retumble_enabled' */
      guard1 = false;
      guard2 = false;
      guard3 = false;
      if (GNC_Planner_Core_U.retumble_enabled) {
        y[0] = y_tmp;
        y[1] = y_tmp_0;
        y[2] = y_tmp_1;
        out = true;
        e_k = 0;
        exitg1 = false;
        while ((!exitg1) && (e_k < 3)) {
          if (!(y[e_k] <= GNC_Planner_Core_U.desired_tumble_rate[e_k])) {
            out = false;
            exitg1 = true;
          } else {
            e_k++;
          }
        }

        if (out && (!GNC_Planner_Core_B.TUMB) &&
            (!GNC_Planner_Core_B.Eclipse_Flag)) {
          /* Outport: '<Root>/commanded_mode' */
          GNC_Planner_Core_Y.commanded_mode = MANUAL;

          /* Outport: '<Root>/GNC_Planner_Mode' */
          GNC_Planner_Core_Y.GNC_Planner_Mode = BasicModeType_Retumble;
          GNC_Planner_Core_DW.expected_mode = MANUAL;
        } else {
          guard3 = true;
        }
      } else {
        guard3 = true;
      }

      if (guard3) {
        y[0] = y_tmp;
        y[1] = y_tmp_0;
        y[2] = y_tmp_1;
        out = true;
        e_k = 0;
        exitg1 = false;
        while ((!exitg1) && (e_k < 3)) {
          if (!(y[e_k] >= GNC_Planner_Core_U.desired_tumble_rate[e_k])) {
            out = false;
            exitg1 = true;
          } else {
            e_k++;
          }
        }

        if (out && (!GNC_Planner_Core_B.TUMB)) {
          guard1 = true;
        } else if (GNC_Planner_Core_U.retumble_enabled) {
          y[0] = y_tmp;
          y[1] = y_tmp_0;
          y[2] = y_tmp_1;
          out = true;
          e_k = 0;
          exitg1 = false;
          while ((!exitg1) && (e_k < 3)) {
            if (!(y[e_k] <= GNC_Planner_Core_U.desired_tumble_rate[e_k])) {
              out = false;
              exitg1 = true;
            } else {
              e_k++;
            }
          }

          if (out && (!GNC_Planner_Core_B.TUMB) &&
              GNC_Planner_Core_B.Eclipse_Flag) {
            guard1 = true;
          } else {
            guard2 = true;
          }
        } else {
          guard2 = true;
        }
      }

      if (guard2) {
        /* Outport: '<Root>/GNC_Planner_Mode' */
        GNC_Planner_Core_Y.GNC_Planner_Mode = BasicModeType_DETUMBLE;

        /* Inport: '<Root>/alt_bdot_enabled' */
        if (GNC_Planner_Core_U.alt_bdot_enabled) {
          /* Outport: '<Root>/commanded_mode' */
          GNC_Planner_Core_Y.commanded_mode = MANUAL;
          GNC_Planner_Core_DW.expected_mode = MANUAL;
        } else {
          /* Outport: '<Root>/commanded_mode' */
          GNC_Planner_Core_Y.commanded_mode = DETUMBLE;
          GNC_Planner_Core_DW.expected_mode = DETUMBLE;
        }

        /* End of Inport: '<Root>/alt_bdot_enabled' */
      }

      if (guard1) {
        /* Outport: '<Root>/GNC_Planner_Mode' */
        GNC_Planner_Core_Y.GNC_Planner_Mode = BasicModeType_SAFE;
        GNC_Planner_Core_DW.expected_mode = SAFE;

        /* Outport: '<Root>/cmd_reset' */
        GNC_Planner_Core_Y.cmd_reset = false;
        GNC_Planner_Core_DW.unexpected_reset = false;
      }

      /* End of Inport: '<Root>/retumble_enabled' */
    }
  }
}

/* Function for Chart: '<S1>/GNC State Machine' */
static void GNC_Planner_exit_internal_Basic(void)
{
  /* Outport: '<Root>/GNC_Planner_Mode' incorporates:
   *  Outport: '<Root>/cmd_dipoles'
   */
  if (GNC_Planner_Core_Y.GNC_Planner_Mode == BasicModeType_Retumble) {
    GNC_Planner_Core_Y.cmd_dipoles[0] = 0.0;
    GNC_Planner_Core_Y.cmd_dipoles[1] = 0.0;
    GNC_Planner_Core_Y.cmd_dipoles[2] = 0.0;
    GNC_Planner_Core_Y.GNC_Planner_Mode = BasicModeType_None;
  } else {
    GNC_Planner_Core_Y.GNC_Planner_Mode = BasicModeType_None;
  }

  /* End of Outport: '<Root>/GNC_Planner_Mode' */
}

/* Model step function */
void GNC_Planner_Core_step(void)
{
  siqtL0olslAz0Re7SMjm43B_GNC_P_T expl_temp;
  creal_T b_x;
  creal_T c_x;
  creal_T x_0;
  real_T r_ECI_row_data[9];
  real_T v_ECI_row_data[9];
  real_T rsun_km[3];
  real_T rtb_r_ECI[3];
  real_T A;
  real_T b_t;
  real_T epoch_yr;
  real_T meananomaly;
  real_T t;
  int32_T exitg2;
  int16_T r_ECI_row_size[2];
  int16_T v_ECI_row_size[2];
  int16_T i;
  char_T tle_char[138];
  boolean_T x[3];
  boolean_T b_p;
  boolean_T exitg1;
  boolean_T rtb_update_flag;
  ADCSMode mode;

  /* Outputs for Atomic SubSystem: '<Root>/GNC_Planner_Core' */
  /* MATLAB Function: '<S1>/MATLAB Function' incorporates:
   *  Inport: '<Root>/STAT'
   */
  switch ((uint32_T)((uint16_T)GNC_Planner_Core_U.STAT_out & 7U)) {
   case 0UL:
    mode = SAFE;
    break;

   case 1UL:
    mode = DETUMBLE;
    break;

   case 2UL:
    mode = SUNPOINT;
    break;

   case 3UL:
    mode = FINEPOINTING;
    break;

   case 4UL:
    mode = LVLH;
    break;

   case 5UL:
    mode = TARGETTRACKING;
    break;

   case 6UL:
    mode = SUNSPIN;
    break;

   default:
    mode = MANUAL;
    break;
  }

  GNC_Planner_Core_B.TUMB = (((uint16_T)GNC_Planner_Core_U.STAT_out & 2048U) !=
    0U);
  GNC_Planner_Core_B.SUN = (((uint16_T)GNC_Planner_Core_U.STAT_out & 8192U) !=
    0U);

  /* End of MATLAB Function: '<S1>/MATLAB Function' */

  /* Outputs for Enabled SubSystem: '<S1>/Subsystem' incorporates:
   *  EnablePort: '<S4>/Enable'
   */
  /* Logic: '<S1>/Logical Operator' incorporates:
   *  Inport: '<Root>/GNC_Planner_Custom_Pointing'
   *  Inport: '<Root>/alt_bdot_enabled'
   *  Inport: '<Root>/alt_sunspin_enabled'
   *  Inport: '<Root>/retumble_enabled'
   */
  if (GNC_Planner_Core_U.GNC_Planner_Custom_Pointing ||
      GNC_Planner_Core_U.alt_sunspin_enabled ||
      GNC_Planner_Core_U.alt_bdot_enabled || GNC_Planner_Core_U.retumble_enabled)
  {
    /* MATLAB Function: '<S4>/parse_tle_string' incorporates:
     *  Inport: '<Root>/TLE_array'
     */
    rtb_update_flag = false;
    b_p = true;
    i = 0;
    exitg1 = false;
    while ((!exitg1) && (i < 138)) {
      if (GNC_Planner_Core_U.TLE_array[i] != GNC_Planner_Core_DW.cached_str[i])
      {
        b_p = false;
        exitg1 = true;
      } else {
        i++;
      }
    }

    if (b_p) {
      rtb_update_flag = true;
    }

    if (!rtb_update_flag) {
      for (i = 0; i < 138; i++) {
        tle_char[i] = (int8_T)GNC_Planner_Core_U.TLE_array[i];
      }

      x_0 = GNC_Planner_Core_str2double(&tle_char[18]);
      epoch_yr = x_0.re;
      x_0 = GNC_Planner_Core_str2double_e(&tle_char[20]);
      b_x = GNC_Planner_Core_str2double_ek(&tle_char[33]);
      GNC_Planner_Core_DW.cached_bus.ndot = b_x.re;
      b_x = GNC_Planner_Core_str2double_ekr(&tle_char[44]);
      c_x = GNC_Planner_Core_str2double(&tle_char[50]);
      GNC_Planner_Core_DW.cached_bus.nddot = b_x.re * 1.0E-5 * rt_powd_snf(10.0,
        c_x.re);
      b_x = GNC_Planner_Core_str2double_ekr(&tle_char[53]);
      c_x = GNC_Planner_Core_str2double(&tle_char[59]);
      GNC_Planner_Core_DW.cached_bus.bstar = b_x.re * 1.0E-5 * rt_powd_snf(10.0,
        c_x.re);
      b_x = GNC_Planner_Cor_str2double_ekr3(&tle_char[77]);
      GNC_Planner_Core_DW.cached_bus.inclo = b_x.re * 0.017453292519943295;
      b_x = GNC_Planner_Cor_str2double_ekr3(&tle_char[86]);
      GNC_Planner_Core_DW.cached_bus.nodeo = b_x.re * 0.017453292519943295;
      b_x = GNC_Planner_Co_str2double_ekr3v(&tle_char[95]);
      GNC_Planner_Core_DW.cached_bus.ecco = b_x.re * 1.0E-7;
      b_x = GNC_Planner_Cor_str2double_ekr3(&tle_char[103]);
      GNC_Planner_Core_DW.cached_bus.argpo = b_x.re * 0.017453292519943295;
      b_x = GNC_Planner_Cor_str2double_ekr3(&tle_char[112]);
      GNC_Planner_Core_DW.cached_bus.mo = b_x.re * 0.017453292519943295;
      b_x = GNC_Planner_C_str2double_ekr3v5(&tle_char[121]);
      GNC_Planner_Core_DW.cached_bus.no_kozai = b_x.re * 0.0043633231299858239;
      if (epoch_yr < 57.0) {
        epoch_yr += 2000.0;
      } else {
        epoch_yr += 1900.0;
      }

      A = floor((epoch_yr - 1.0) / 100.0);
      GNC_Planner_Core_DW.cached_bus.epoch_JD = ((floor((epoch_yr - 1.0) *
        365.25) + ((2.0 - A) + floor(A / 4.0))) + 1.7214225E+6) + x_0.re;
      GNC_Planner_Core_DW.cached_bus.epoch_days_1950 =
        GNC_Planner_Core_DW.cached_bus.epoch_JD - 2.4332815E+6;
      memcpy(&GNC_Planner_Core_DW.cached_str[0], &GNC_Planner_Core_U.TLE_array[0],
             138U * sizeof(uint8_T));
      rtb_update_flag = true;
    } else {
      rtb_update_flag = false;
    }

    /* MATLAB Function: '<S4>/fsw_spg4_step' incorporates:
     *  Inport: '<Root>/T_JD'
     *  MATLAB Function: '<S4>/parse_tle_string'
     */
    if (rtb_update_flag || (!GNC_Planner_Core_DW.is_initialized)) {
      GNC_Planner_Core_sgp4init(GNC_Planner_Core_DW.cached_bus.epoch_days_1950,
        GNC_Planner_Core_DW.cached_bus.bstar,
        GNC_Planner_Core_DW.cached_bus.ndot,
        GNC_Planner_Core_DW.cached_bus.nddot,
        GNC_Planner_Core_DW.cached_bus.ecco,
        GNC_Planner_Core_DW.cached_bus.argpo,
        GNC_Planner_Core_DW.cached_bus.inclo, GNC_Planner_Core_DW.cached_bus.mo,
        GNC_Planner_Core_DW.cached_bus.no_kozai,
        GNC_Planner_Core_DW.cached_bus.nodeo, &expl_temp);
      GNC_Planner_Core_DW.satrec.error = expl_temp.error;
      GNC_Planner_Core_DW.satrec.satnum = expl_temp.satnum;
      GNC_Planner_Core_DW.satrec.epochyr = expl_temp.epochyr;
      GNC_Planner_Core_DW.satrec.epochdays = expl_temp.epochdays;
      GNC_Planner_Core_DW.satrec.ndot = expl_temp.ndot;
      GNC_Planner_Core_DW.satrec.nddot = expl_temp.nddot;
      GNC_Planner_Core_DW.satrec.bstar = expl_temp.bstar;
      GNC_Planner_Core_DW.satrec.ecco = expl_temp.ecco;
      GNC_Planner_Core_DW.satrec.argpo = expl_temp.argpo;
      GNC_Planner_Core_DW.satrec.inclo = expl_temp.inclo;
      GNC_Planner_Core_DW.satrec.mo = expl_temp.mo;
      GNC_Planner_Core_DW.satrec.no_kozai = expl_temp.no_kozai;
      GNC_Planner_Core_DW.satrec.nodeo = expl_temp.nodeo;
      GNC_Planner_Core_DW.satrec.no = expl_temp.no;
      GNC_Planner_Core_DW.satrec.no_unkozai = expl_temp.no_unkozai;
      GNC_Planner_Core_DW.satrec.a = expl_temp.a;
      GNC_Planner_Core_DW.satrec.alta = expl_temp.alta;
      GNC_Planner_Core_DW.satrec.altp = expl_temp.altp;
      GNC_Planner_Core_DW.satrec.ephtype = expl_temp.ephtype;
      GNC_Planner_Core_DW.satrec.classification = expl_temp.classification;
      GNC_Planner_Core_DW.satrec.operationmode = expl_temp.operationmode;
      GNC_Planner_Core_DW.satrec.init = expl_temp.init;
      GNC_Planner_Core_DW.satrec.method = expl_temp.method;
      GNC_Planner_Core_DW.satrec.isimp = expl_temp.isimp;
      GNC_Planner_Core_DW.satrec.aycof = expl_temp.aycof;
      GNC_Planner_Core_DW.satrec.con41 = expl_temp.con41;
      GNC_Planner_Core_DW.satrec.cc1 = expl_temp.cc1;
      GNC_Planner_Core_DW.satrec.cc4 = expl_temp.cc4;
      GNC_Planner_Core_DW.satrec.cc5 = expl_temp.cc5;
      GNC_Planner_Core_DW.satrec.d2 = expl_temp.d2;
      GNC_Planner_Core_DW.satrec.d3 = expl_temp.d3;
      GNC_Planner_Core_DW.satrec.d4 = expl_temp.d4;
      GNC_Planner_Core_DW.satrec.delmo = expl_temp.delmo;
      GNC_Planner_Core_DW.satrec.eta = expl_temp.eta;
      GNC_Planner_Core_DW.satrec.argpdot = expl_temp.argpdot;
      GNC_Planner_Core_DW.satrec.omgcof = expl_temp.omgcof;
      GNC_Planner_Core_DW.satrec.sinmao = expl_temp.sinmao;
      GNC_Planner_Core_DW.satrec.t = expl_temp.t;
      GNC_Planner_Core_DW.satrec.t2 = expl_temp.t2;
      GNC_Planner_Core_DW.satrec.t3 = expl_temp.t3;
      GNC_Planner_Core_DW.satrec.t4 = expl_temp.t4;
      GNC_Planner_Core_DW.satrec.t5 = expl_temp.t5;
      GNC_Planner_Core_DW.satrec.t2cof = expl_temp.t2cof;
      GNC_Planner_Core_DW.satrec.t3cof = expl_temp.t3cof;
      GNC_Planner_Core_DW.satrec.t4cof = expl_temp.t4cof;
      GNC_Planner_Core_DW.satrec.t5cof = expl_temp.t5cof;
      GNC_Planner_Core_DW.satrec.x1mth2 = expl_temp.x1mth2;
      GNC_Planner_Core_DW.satrec.x7thm1 = expl_temp.x7thm1;
      GNC_Planner_Core_DW.satrec.mdot = expl_temp.mdot;
      GNC_Planner_Core_DW.satrec.nodedot = expl_temp.nodedot;
      GNC_Planner_Core_DW.satrec.xlcof = expl_temp.xlcof;
      GNC_Planner_Core_DW.satrec.xmcof = expl_temp.xmcof;
      GNC_Planner_Core_DW.satrec.nodecf = expl_temp.nodecf;
      GNC_Planner_Core_DW.satrec.irez = expl_temp.irez;
      GNC_Planner_Core_DW.satrec.d2201 = expl_temp.d2201;
      GNC_Planner_Core_DW.satrec.d2211 = expl_temp.d2211;
      GNC_Planner_Core_DW.satrec.d3210 = expl_temp.d3210;
      GNC_Planner_Core_DW.satrec.d3222 = expl_temp.d3222;
      GNC_Planner_Core_DW.satrec.d4410 = expl_temp.d4410;
      GNC_Planner_Core_DW.satrec.d4422 = expl_temp.d4422;
      GNC_Planner_Core_DW.satrec.d5220 = expl_temp.d5220;
      GNC_Planner_Core_DW.satrec.d5232 = expl_temp.d5232;
      GNC_Planner_Core_DW.satrec.d5421 = expl_temp.d5421;
      GNC_Planner_Core_DW.satrec.d5433 = expl_temp.d5433;
      GNC_Planner_Core_DW.satrec.dedt = expl_temp.dedt;
      GNC_Planner_Core_DW.satrec.del1 = expl_temp.del1;
      GNC_Planner_Core_DW.satrec.del2 = expl_temp.del2;
      GNC_Planner_Core_DW.satrec.del3 = expl_temp.del3;
      GNC_Planner_Core_DW.satrec.didt = expl_temp.didt;
      GNC_Planner_Core_DW.satrec.dmdt = expl_temp.dmdt;
      GNC_Planner_Core_DW.satrec.dnodt = expl_temp.dnodt;
      GNC_Planner_Core_DW.satrec.domdt = expl_temp.domdt;
      GNC_Planner_Core_DW.satrec.e3 = expl_temp.e3;
      GNC_Planner_Core_DW.satrec.ee2 = expl_temp.ee2;
      GNC_Planner_Core_DW.satrec.peo = expl_temp.peo;
      GNC_Planner_Core_DW.satrec.pgho = expl_temp.pgho;
      GNC_Planner_Core_DW.satrec.pho = expl_temp.pho;
      GNC_Planner_Core_DW.satrec.pinco = expl_temp.pinco;
      GNC_Planner_Core_DW.satrec.plo = expl_temp.plo;
      GNC_Planner_Core_DW.satrec.se2 = expl_temp.se2;
      GNC_Planner_Core_DW.satrec.se3 = expl_temp.se3;
      GNC_Planner_Core_DW.satrec.sgh2 = expl_temp.sgh2;
      GNC_Planner_Core_DW.satrec.sgh3 = expl_temp.sgh3;
      GNC_Planner_Core_DW.satrec.sgh4 = expl_temp.sgh4;
      GNC_Planner_Core_DW.satrec.sh2 = expl_temp.sh2;
      GNC_Planner_Core_DW.satrec.sh3 = expl_temp.sh3;
      GNC_Planner_Core_DW.satrec.si2 = expl_temp.si2;
      GNC_Planner_Core_DW.satrec.si3 = expl_temp.si3;
      GNC_Planner_Core_DW.satrec.sl2 = expl_temp.sl2;
      GNC_Planner_Core_DW.satrec.sl3 = expl_temp.sl3;
      GNC_Planner_Core_DW.satrec.sl4 = expl_temp.sl4;
      GNC_Planner_Core_DW.satrec.gsto = expl_temp.gsto;
      GNC_Planner_Core_DW.satrec.xfact = expl_temp.xfact;
      GNC_Planner_Core_DW.satrec.xgh2 = expl_temp.xgh2;
      GNC_Planner_Core_DW.satrec.xgh3 = expl_temp.xgh3;
      GNC_Planner_Core_DW.satrec.xgh4 = expl_temp.xgh4;
      GNC_Planner_Core_DW.satrec.xh2 = expl_temp.xh2;
      GNC_Planner_Core_DW.satrec.xh3 = expl_temp.xh3;
      GNC_Planner_Core_DW.satrec.xi2 = expl_temp.xi2;
      GNC_Planner_Core_DW.satrec.xi3 = expl_temp.xi3;
      GNC_Planner_Core_DW.satrec.xl2 = expl_temp.xl2;
      GNC_Planner_Core_DW.satrec.xl3 = expl_temp.xl3;
      GNC_Planner_Core_DW.satrec.xl4 = expl_temp.xl4;
      GNC_Planner_Core_DW.satrec.xlamo = expl_temp.xlamo;
      GNC_Planner_Core_DW.satrec.zmol = expl_temp.zmol;
      GNC_Planner_Core_DW.satrec.zmos = expl_temp.zmos;
      GNC_Planner_Core_DW.satrec.atime = expl_temp.atime;
      GNC_Planner_Core_DW.satrec.xli = expl_temp.xli;
      GNC_Planner_Core_DW.satrec.xni = expl_temp.xni;
      GNC_Planner_Core_DW.satrec.am = expl_temp.am;
      GNC_Planner_Core_DW.satrec.em = expl_temp.em;
      GNC_Planner_Core_DW.satrec.im = expl_temp.im;
      GNC_Planner_Core_DW.satrec.Om = expl_temp.Om;
      GNC_Planner_Core_DW.satrec.om = expl_temp.om;
      GNC_Planner_Core_DW.satrec.mm = expl_temp.mm;
      GNC_Planner_Core_DW.satrec.nm = expl_temp.nm;
      GNC_Planner_Core_DW.satrec.t0 = expl_temp.t0;
      GNC_Planner_Core_DW.satrec.tumin = expl_temp.tumin;
      GNC_Planner_Core_DW.satrec.mu = expl_temp.mu;
      GNC_Planner_Core_DW.satrec.radiusearthkm = expl_temp.radiusearthkm;
      GNC_Planner_Core_DW.satrec.xke = expl_temp.xke;
      GNC_Planner_Core_DW.satrec.j2 = expl_temp.j2;
      GNC_Planner_Core_DW.satrec.j3 = expl_temp.j3;
      GNC_Planner_Core_DW.satrec.j4 = expl_temp.j4;
      GNC_Planner_Core_DW.satrec.j3oj2 = expl_temp.j3oj2;
      GNC_Planner_Core_DW.is_initialized = true;
    }

    GNC_Planner_Core_sgp4_e(&GNC_Planner_Core_DW.satrec,
      (GNC_Planner_Core_U.T_JD - GNC_Planner_Core_DW.cached_bus.epoch_JD) *
      1440.0, r_ECI_row_data, r_ECI_row_size, v_ECI_row_data, v_ECI_row_size);
    rtb_r_ECI[0] = r_ECI_row_data[0] * 1000.0;
    rtb_r_ECI[1] = r_ECI_row_data[1] * 1000.0;
    rtb_r_ECI[2] = r_ECI_row_data[2] * 1000.0;

    /* End of MATLAB Function: '<S4>/fsw_spg4_step' */

    /* MATLAB Function: '<S4>/MATLAB Function' incorporates:
     *  Inport: '<Root>/T_JD'
     */
    A = (GNC_Planner_Core_U.T_JD - 2.451545E+6) / 36525.0;
    meananomaly = rt_remd_snf((35999.05034 * A + 357.5277233) *
      0.017453292519943295, 6.2831853071795862);
    if (meananomaly < 0.0) {
      meananomaly += 6.2831853071795862;
    }

    t = 2.0 * meananomaly;
    epoch_yr = rt_remd_snf((rt_remd_snf(36000.77 * A + 280.46, 360.0) +
      1.914666471 * sin(meananomaly)) + sin(t) * 0.019994643, 360.0) *
      0.017453292519943295;
    A = (23.439291 - 0.0130042 * A) * 0.017453292519943295;
    meananomaly = (1.000140612 - 0.016708617 * cos(meananomaly)) - cos(t) *
      0.000139589;
    rsun_km[0] = meananomaly * cos(epoch_yr);
    epoch_yr = sin(epoch_yr);
    rsun_km[1] = meananomaly * cos(A) * epoch_yr;
    rsun_km[2] = meananomaly * sin(A) * epoch_yr;
    rsun_km[0] *= 1.495978707E+8;
    rsun_km[1] *= 1.495978707E+8;
    rsun_km[2] *= 1.495978707E+8;
    rsun_km[2] *= 1.0033640923512714;
    rtb_r_ECI[2] *= 1.0033640923512714;
    A = 3.3121686421112381E-170;
    meananomaly = fabs(rsun_km[0]);
    if (meananomaly > 3.3121686421112381E-170) {
      epoch_yr = 1.0;
      A = meananomaly;
    } else {
      t = meananomaly / 3.3121686421112381E-170;
      epoch_yr = t * t;
    }

    meananomaly = fabs(rsun_km[1]);
    if (meananomaly > A) {
      t = A / meananomaly;
      epoch_yr = epoch_yr * t * t + 1.0;
      A = meananomaly;
    } else {
      t = meananomaly / A;
      epoch_yr += t * t;
    }

    meananomaly = fabs(rsun_km[2]);
    if (meananomaly > A) {
      t = A / meananomaly;
      epoch_yr = epoch_yr * t * t + 1.0;
      A = meananomaly;
    } else {
      t = meananomaly / A;
      epoch_yr += t * t;
    }

    epoch_yr = A * sqrt(epoch_yr);
    if (rtIsNaN(epoch_yr)) {
      i = 0;
      do {
        exitg2 = 0L;
        if (i < 3) {
          if (rtIsNaN(rsun_km[i])) {
            exitg2 = 1L;
          } else {
            i++;
          }
        } else {
          epoch_yr = (rtInf);
          exitg2 = 1L;
        }
      } while (exitg2 == 0L);
    }

    meananomaly = 3.3121686421112381E-170;
    t = fabs(rtb_r_ECI[0]);
    if (t > 3.3121686421112381E-170) {
      A = 1.0;
      meananomaly = t;
    } else {
      b_t = t / 3.3121686421112381E-170;
      A = b_t * b_t;
    }

    t = fabs(rtb_r_ECI[1]);
    if (t > meananomaly) {
      b_t = meananomaly / t;
      A = A * b_t * b_t + 1.0;
      meananomaly = t;
    } else {
      b_t = t / meananomaly;
      A += b_t * b_t;
    }

    t = fabs(rtb_r_ECI[2]);
    if (t > meananomaly) {
      b_t = meananomaly / t;
      A = A * b_t * b_t + 1.0;
      meananomaly = t;
    } else {
      b_t = t / meananomaly;
      A += b_t * b_t;
    }

    A = meananomaly * sqrt(A);
    if (rtIsNaN(A)) {
      i = 0;
      do {
        exitg2 = 0L;
        if (i < 3) {
          if (rtIsNaN(rtb_r_ECI[i])) {
            exitg2 = 1L;
          } else {
            i++;
          }
        } else {
          A = (rtInf);
          exitg2 = 1L;
        }
      } while (exitg2 == 0L);
    }

    epoch_yr *= epoch_yr;
    meananomaly = (rsun_km[0] * rtb_r_ECI[0] + rsun_km[1] * rtb_r_ECI[1]) +
      rsun_km[2] * rtb_r_ECI[2];
    A = (A * A + epoch_yr) - 2.0 * meananomaly;
    if (fabs(A) < 0.0001) {
      A = 0.0;
    } else {
      A = (epoch_yr - meananomaly) / A;
    }

    GNC_Planner_Core_B.Eclipse_Flag = ((A < 0.0) || (A > 1.0) || (((1.0 - A) *
      epoch_yr + meananomaly * A) / 4.0680631590768993E+7 > 1.0));

    /* End of MATLAB Function: '<S4>/MATLAB Function' */
  }

  /* End of Logic: '<S1>/Logical Operator' */
  /* End of Outputs for SubSystem: '<S1>/Subsystem' */

  /* Chart: '<S1>/GNC State Machine' incorporates:
   *  Inport: '<Root>/GNC_Planner_Custom_Pointing'
   *  Inport: '<Root>/RATE'
   *  Inport: '<Root>/alt_bdot_enabled'
   *  Inport: '<Root>/alt_sunspin_enabled'
   *  Inport: '<Root>/desired_tumble_rate'
   *  Inport: '<Root>/retumble_enabled'
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
      GNC_Planner_exit_internal_Basic();
      GNC_Planner_Core_DW.is_c3_GNC_Planner_Core = GNC_Planner_Core_IN_Pointing;
      GNC_Planner_Core_DW.expected_mode = MANUAL;
    } else if (mode != GNC_Planner_Core_DW.expected_mode) {
      if (mode == DETUMBLE) {
        /* Outport: '<Root>/detumble_count' */
        GNC_Planner_Core_Y.detumble_count++;
        GNC_Planner_exit_internal_Basic();

        /* Outport: '<Root>/GNC_Planner_Mode' */
        GNC_Planner_Core_Y.GNC_Planner_Mode = BasicModeType_DETUMBLE;
        if (GNC_Planner_Core_U.alt_bdot_enabled) {
          /* Outport: '<Root>/commanded_mode' */
          GNC_Planner_Core_Y.commanded_mode = MANUAL;
          GNC_Planner_Core_DW.expected_mode = MANUAL;
        } else {
          /* Outport: '<Root>/commanded_mode' */
          GNC_Planner_Core_Y.commanded_mode = DETUMBLE;
          GNC_Planner_Core_DW.expected_mode = DETUMBLE;
        }
      } else if (mode == SUNSPIN) {
        GNC_Planner_exit_internal_Basic();

        /* Outport: '<Root>/GNC_Planner_Mode' */
        GNC_Planner_Core_Y.GNC_Planner_Mode = BasicModeType_SUNSPIN;
        if (GNC_Planner_Core_U.alt_sunspin_enabled) {
          /* Outport: '<Root>/commanded_mode' */
          GNC_Planner_Core_Y.commanded_mode = MANUAL;
          GNC_Planner_Core_DW.expected_mode = MANUAL;
        } else {
          /* Outport: '<Root>/commanded_mode' */
          GNC_Planner_Core_Y.commanded_mode = SUNSPIN;
          GNC_Planner_Core_DW.expected_mode = SUNSPIN;
        }
      } else {
        /* Outport: '<Root>/reset_count' */
        GNC_Planner_Core_Y.reset_count++;
        GNC_Planner_exit_internal_Basic();

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
        rtb_update_flag = !GNC_Planner_Core_B.TUMB;
        if (GNC_Planner_Core_B.SUN && rtb_update_flag) {
          GNC_Planner_Core_Y.GNC_Planner_Mode = BasicModeType_SUNSPIN;
          if (GNC_Planner_Core_U.alt_sunspin_enabled) {
            /* Outport: '<Root>/commanded_mode' */
            GNC_Planner_Core_Y.commanded_mode = MANUAL;
            GNC_Planner_Core_DW.expected_mode = MANUAL;
          } else {
            /* Outport: '<Root>/commanded_mode' */
            GNC_Planner_Core_Y.commanded_mode = SUNSPIN;
            GNC_Planner_Core_DW.expected_mode = SUNSPIN;
          }
        } else if (GNC_Planner_Core_U.retumble_enabled) {
          rtb_r_ECI[0] = fabs(GNC_Planner_Core_U.w[0]);
          rtb_r_ECI[1] = fabs(GNC_Planner_Core_U.w[1]);
          rtb_r_ECI[2] = fabs(GNC_Planner_Core_U.w[2]);
          b_p = true;
          i = 0;
          exitg1 = false;
          while ((!exitg1) && (i < 3)) {
            if (!(rtb_r_ECI[i] <= GNC_Planner_Core_U.desired_tumble_rate[i])) {
              b_p = false;
              exitg1 = true;
            } else {
              i++;
            }
          }

          if (b_p && rtb_update_flag) {
            /* Outport: '<Root>/commanded_mode' */
            GNC_Planner_Core_Y.commanded_mode = SAFE;
            GNC_Planner_Core_Y.GNC_Planner_Mode = BasicModeType_SAFE;
            GNC_Planner_Core_DW.expected_mode = SAFE;

            /* Outport: '<Root>/cmd_reset' */
            GNC_Planner_Core_Y.cmd_reset = false;
            GNC_Planner_Core_DW.unexpected_reset = false;
          }
        }
        break;

       case BasicModeType_Retumble:
        x[0] = (fabs(GNC_Planner_Core_U.w[0]) >=
                GNC_Planner_Core_U.desired_tumble_rate[0] * 1.01);
        x[1] = (fabs(GNC_Planner_Core_U.w[1]) >=
                GNC_Planner_Core_U.desired_tumble_rate[1] * 1.01);
        x[2] = (fabs(GNC_Planner_Core_U.w[2]) >=
                GNC_Planner_Core_U.desired_tumble_rate[2] * 1.01);
        rtb_update_flag = false;
        i = 0;
        exitg1 = false;
        while ((!exitg1) && (i < 3)) {
          if (x[i]) {
            rtb_update_flag = true;
            exitg1 = true;
          } else {
            i++;
          }
        }

        if (rtb_update_flag || GNC_Planner_Core_B.Eclipse_Flag) {
          /* Outport: '<Root>/commanded_mode' */
          GNC_Planner_Core_Y.commanded_mode = SAFE;

          /* Outport: '<Root>/cmd_dipoles' */
          GNC_Planner_Core_Y.cmd_dipoles[0] = 0.0;
          GNC_Planner_Core_Y.cmd_dipoles[1] = 0.0;
          GNC_Planner_Core_Y.cmd_dipoles[2] = 0.0;
          GNC_Planner_Core_Y.GNC_Planner_Mode = BasicModeType_SAFE;
          GNC_Planner_Core_DW.expected_mode = SAFE;

          /* Outport: '<Root>/cmd_reset' */
          GNC_Planner_Core_Y.cmd_reset = false;
          GNC_Planner_Core_DW.unexpected_reset = false;
        } else if (GNC_Planner_Core_B.SUN && (!GNC_Planner_Core_B.TUMB)) {
          /* Outport: '<Root>/cmd_dipoles' */
          GNC_Planner_Core_Y.cmd_dipoles[0] = 0.0;
          GNC_Planner_Core_Y.cmd_dipoles[1] = 0.0;
          GNC_Planner_Core_Y.cmd_dipoles[2] = 0.0;
          GNC_Planner_Core_Y.GNC_Planner_Mode = BasicModeType_SUNSPIN;
          if (GNC_Planner_Core_U.alt_sunspin_enabled) {
            /* Outport: '<Root>/commanded_mode' */
            GNC_Planner_Core_Y.commanded_mode = MANUAL;
            GNC_Planner_Core_DW.expected_mode = MANUAL;
          } else {
            /* Outport: '<Root>/commanded_mode' */
            GNC_Planner_Core_Y.commanded_mode = SUNSPIN;
            GNC_Planner_Core_DW.expected_mode = SUNSPIN;
          }
        } else {
          /* Outport: '<Root>/cmd_dipoles' */
          GNC_Planner_Core_Y.cmd_dipoles[0] = 0.1;
          GNC_Planner_Core_Y.cmd_dipoles[1] = 0.1;
          GNC_Planner_Core_Y.cmd_dipoles[2] = 0.1;
        }
        break;

       case BasicModeType_SAFE:
        GNC_Planner_Core_SAFE();
        break;

       default:
        /* State SUNSPIN */
        break;
      }
    }

    /* case IN_Pointing: */
  } else if ((!GNC_Planner_Core_U.GNC_Planner_Custom_Pointing) || (mode !=
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

  /* Matfile logging */
  rt_UpdateTXYLogVars(GNC_Planner_Core_M->rtwLogInfo,
                      (&GNC_Planner_Core_M->Timing.taskTime0));

  /* signal main to stop simulation */
  {                                    /* Sample time: [1.0s, 0.0s] */
    if ((rtmGetTFinal(GNC_Planner_Core_M)!=-1) &&
        !((rtmGetTFinal(GNC_Planner_Core_M)-GNC_Planner_Core_M->Timing.taskTime0)
          > GNC_Planner_Core_M->Timing.taskTime0 * (DBL_EPSILON))) {
      rtmSetErrorStatus(GNC_Planner_Core_M, "Simulation finished");
    }
  }

  /* Update absolute time for base rate */
  /* The "clockTick0" counts the number of times the code of this task has
   * been executed. The absolute time is the multiplication of "clockTick0"
   * and "Timing.stepSize0". Size of "clockTick0" ensures timer will not
   * overflow during the application lifespan selected.
   * Timer of this task consists of two 32 bit unsigned integers.
   * The two integers represent the low bits Timing.clockTick0 and the high bits
   * Timing.clockTickH0. When the low bit overflows to 0, the high bits increment.
   */
  if (!(++GNC_Planner_Core_M->Timing.clockTick0)) {
    ++GNC_Planner_Core_M->Timing.clockTickH0;
  }

  GNC_Planner_Core_M->Timing.taskTime0 = GNC_Planner_Core_M->Timing.clockTick0 *
    GNC_Planner_Core_M->Timing.stepSize0 +
    GNC_Planner_Core_M->Timing.clockTickH0 *
    GNC_Planner_Core_M->Timing.stepSize0 * 4294967296.0;
}

/* Model initialize function */
void GNC_Planner_Core_initialize(void)
{
  /* Registration code */

  /* initialize non-finites */
  rt_InitInfAndNaN(sizeof(real_T));

  /* initialize real-time model */
  (void) memset((void *)GNC_Planner_Core_M, 0,
                sizeof(RT_MODEL_GNC_Planner_Core_T));
  rtmSetTFinal(GNC_Planner_Core_M, 10.0);
  GNC_Planner_Core_M->Timing.stepSize0 = 1.0;

  /* Setup for data logging */
  {
    static RTWLogInfo rt_DataLoggingInfo;
    rt_DataLoggingInfo.loggingInterval = (NULL);
    GNC_Planner_Core_M->rtwLogInfo = &rt_DataLoggingInfo;
  }

  /* Setup for data logging */
  {
    rtliSetLogXSignalInfo(GNC_Planner_Core_M->rtwLogInfo, (NULL));
    rtliSetLogXSignalPtrs(GNC_Planner_Core_M->rtwLogInfo, (NULL));
    rtliSetLogT(GNC_Planner_Core_M->rtwLogInfo, "tout");
    rtliSetLogX(GNC_Planner_Core_M->rtwLogInfo, "");
    rtliSetLogXFinal(GNC_Planner_Core_M->rtwLogInfo, "");
    rtliSetLogVarNameModifier(GNC_Planner_Core_M->rtwLogInfo, "rt_");
    rtliSetLogFormat(GNC_Planner_Core_M->rtwLogInfo, 4);
    rtliSetLogMaxRows(GNC_Planner_Core_M->rtwLogInfo, 0);
    rtliSetLogDecimation(GNC_Planner_Core_M->rtwLogInfo, 1);
    rtliSetLogY(GNC_Planner_Core_M->rtwLogInfo, "");
    rtliSetLogYSignalInfo(GNC_Planner_Core_M->rtwLogInfo, (NULL));
    rtliSetLogYSignalPtrs(GNC_Planner_Core_M->rtwLogInfo, (NULL));
  }

  /* block I/O */
  (void) memset(((void *) &GNC_Planner_Core_B), 0,
                sizeof(B_GNC_Planner_Core_T));

  /* states (dwork) */
  (void) memset((void *)&GNC_Planner_Core_DW, 0,
                sizeof(DW_GNC_Planner_Core_T));

  /* external inputs */
  (void)memset(&GNC_Planner_Core_U, 0, sizeof(ExtU_GNC_Planner_Core_T));

  /* external outputs */
  (void)memset(&GNC_Planner_Core_Y, 0, sizeof(ExtY_GNC_Planner_Core_T));

  /* Matfile logging */
  rt_StartDataLoggingWithStartTime(GNC_Planner_Core_M->rtwLogInfo, 0.0,
    rtmGetTFinal(GNC_Planner_Core_M), GNC_Planner_Core_M->Timing.stepSize0,
    (&rtmGetErrorStatus(GNC_Planner_Core_M)));

  /* SystemInitialize for Atomic SubSystem: '<Root>/GNC_Planner_Core' */
  /* SystemInitialize for Enabled SubSystem: '<S1>/Subsystem' */
  /* SystemInitialize for MATLAB Function: '<S4>/parse_tle_string' */
  GNC_Planner_Core_DW.cached_bus.epoch_days_1950 = 0.0;
  GNC_Planner_Core_DW.cached_bus.bstar = 0.0;
  GNC_Planner_Core_DW.cached_bus.ndot = 0.0;
  GNC_Planner_Core_DW.cached_bus.nddot = 0.0;
  GNC_Planner_Core_DW.cached_bus.ecco = 0.0;
  GNC_Planner_Core_DW.cached_bus.argpo = 0.0;
  GNC_Planner_Core_DW.cached_bus.inclo = 0.0;
  GNC_Planner_Core_DW.cached_bus.mo = 0.0;
  GNC_Planner_Core_DW.cached_bus.no_kozai = 0.0;
  GNC_Planner_Core_DW.cached_bus.nodeo = 0.0;
  GNC_Planner_Core_DW.cached_bus.epoch_JD = 0.0;
  memset(&GNC_Planner_Core_DW.cached_str[0], 0, 138U * sizeof(uint8_T));

  /* SystemInitialize for MATLAB Function: '<S4>/fsw_spg4_step' */
  GNC_Planner_Core_DW.is_initialized = false;
  GNC_Planner_Core_DW.satrec.error = 0.0;
  GNC_Planner_Core_DW.satrec.satnum = 0.0;
  GNC_Planner_Core_DW.satrec.epochyr = 0.0;
  GNC_Planner_Core_DW.satrec.epochdays = 0.0;
  GNC_Planner_Core_DW.satrec.ndot = 0.0;
  GNC_Planner_Core_DW.satrec.nddot = 0.0;
  GNC_Planner_Core_DW.satrec.bstar = 0.0;
  GNC_Planner_Core_DW.satrec.ecco = 0.0;
  GNC_Planner_Core_DW.satrec.argpo = 0.0;
  GNC_Planner_Core_DW.satrec.inclo = 0.0;
  GNC_Planner_Core_DW.satrec.mo = 0.0;
  GNC_Planner_Core_DW.satrec.no_kozai = 0.0;
  GNC_Planner_Core_DW.satrec.nodeo = 0.0;
  GNC_Planner_Core_DW.satrec.no = 0.0;
  GNC_Planner_Core_DW.satrec.no_unkozai = 0.0;
  GNC_Planner_Core_DW.satrec.a = 0.0;
  GNC_Planner_Core_DW.satrec.alta = 0.0;
  GNC_Planner_Core_DW.satrec.altp = 0.0;
  GNC_Planner_Core_DW.satrec.ephtype = 0.0;
  GNC_Planner_Core_DW.satrec.classification = 'U';
  GNC_Planner_Core_DW.satrec.operationmode = 'a';
  GNC_Planner_Core_DW.satrec.init = 'y';
  GNC_Planner_Core_DW.satrec.method = 'n';
  GNC_Planner_Core_DW.satrec.isimp = 0.0;
  GNC_Planner_Core_DW.satrec.aycof = 0.0;
  GNC_Planner_Core_DW.satrec.con41 = 0.0;
  GNC_Planner_Core_DW.satrec.cc1 = 0.0;
  GNC_Planner_Core_DW.satrec.cc4 = 0.0;
  GNC_Planner_Core_DW.satrec.cc5 = 0.0;
  GNC_Planner_Core_DW.satrec.d2 = 0.0;
  GNC_Planner_Core_DW.satrec.d3 = 0.0;
  GNC_Planner_Core_DW.satrec.d4 = 0.0;
  GNC_Planner_Core_DW.satrec.delmo = 0.0;
  GNC_Planner_Core_DW.satrec.eta = 0.0;
  GNC_Planner_Core_DW.satrec.argpdot = 0.0;
  GNC_Planner_Core_DW.satrec.omgcof = 0.0;
  GNC_Planner_Core_DW.satrec.sinmao = 0.0;
  GNC_Planner_Core_DW.satrec.t = 0.0;
  GNC_Planner_Core_DW.satrec.t2 = 0.0;
  GNC_Planner_Core_DW.satrec.t3 = 0.0;
  GNC_Planner_Core_DW.satrec.t4 = 0.0;
  GNC_Planner_Core_DW.satrec.t5 = 0.0;
  GNC_Planner_Core_DW.satrec.t2cof = 0.0;
  GNC_Planner_Core_DW.satrec.t3cof = 0.0;
  GNC_Planner_Core_DW.satrec.t4cof = 0.0;
  GNC_Planner_Core_DW.satrec.t5cof = 0.0;
  GNC_Planner_Core_DW.satrec.x1mth2 = 0.0;
  GNC_Planner_Core_DW.satrec.x7thm1 = 0.0;
  GNC_Planner_Core_DW.satrec.mdot = 0.0;
  GNC_Planner_Core_DW.satrec.nodedot = 0.0;
  GNC_Planner_Core_DW.satrec.xlcof = 0.0;
  GNC_Planner_Core_DW.satrec.xmcof = 0.0;
  GNC_Planner_Core_DW.satrec.nodecf = 0.0;
  GNC_Planner_Core_DW.satrec.irez = 0.0;
  GNC_Planner_Core_DW.satrec.d2201 = 0.0;
  GNC_Planner_Core_DW.satrec.d2211 = 0.0;
  GNC_Planner_Core_DW.satrec.d3210 = 0.0;
  GNC_Planner_Core_DW.satrec.d3222 = 0.0;
  GNC_Planner_Core_DW.satrec.d4410 = 0.0;
  GNC_Planner_Core_DW.satrec.d4422 = 0.0;
  GNC_Planner_Core_DW.satrec.d5220 = 0.0;
  GNC_Planner_Core_DW.satrec.d5232 = 0.0;
  GNC_Planner_Core_DW.satrec.d5421 = 0.0;
  GNC_Planner_Core_DW.satrec.d5433 = 0.0;
  GNC_Planner_Core_DW.satrec.dedt = 0.0;
  GNC_Planner_Core_DW.satrec.del1 = 0.0;
  GNC_Planner_Core_DW.satrec.del2 = 0.0;
  GNC_Planner_Core_DW.satrec.del3 = 0.0;
  GNC_Planner_Core_DW.satrec.didt = 0.0;
  GNC_Planner_Core_DW.satrec.dmdt = 0.0;
  GNC_Planner_Core_DW.satrec.dnodt = 0.0;
  GNC_Planner_Core_DW.satrec.domdt = 0.0;
  GNC_Planner_Core_DW.satrec.e3 = 0.0;
  GNC_Planner_Core_DW.satrec.ee2 = 0.0;
  GNC_Planner_Core_DW.satrec.peo = 0.0;
  GNC_Planner_Core_DW.satrec.pgho = 0.0;
  GNC_Planner_Core_DW.satrec.pho = 0.0;
  GNC_Planner_Core_DW.satrec.pinco = 0.0;
  GNC_Planner_Core_DW.satrec.plo = 0.0;
  GNC_Planner_Core_DW.satrec.se2 = 0.0;
  GNC_Planner_Core_DW.satrec.se3 = 0.0;
  GNC_Planner_Core_DW.satrec.sgh2 = 0.0;
  GNC_Planner_Core_DW.satrec.sgh3 = 0.0;
  GNC_Planner_Core_DW.satrec.sgh4 = 0.0;
  GNC_Planner_Core_DW.satrec.sh2 = 0.0;
  GNC_Planner_Core_DW.satrec.sh3 = 0.0;
  GNC_Planner_Core_DW.satrec.si2 = 0.0;
  GNC_Planner_Core_DW.satrec.si3 = 0.0;
  GNC_Planner_Core_DW.satrec.sl2 = 0.0;
  GNC_Planner_Core_DW.satrec.sl3 = 0.0;
  GNC_Planner_Core_DW.satrec.sl4 = 0.0;
  GNC_Planner_Core_DW.satrec.gsto = 0.0;
  GNC_Planner_Core_DW.satrec.xfact = 0.0;
  GNC_Planner_Core_DW.satrec.xgh2 = 0.0;
  GNC_Planner_Core_DW.satrec.xgh3 = 0.0;
  GNC_Planner_Core_DW.satrec.xgh4 = 0.0;
  GNC_Planner_Core_DW.satrec.xh2 = 0.0;
  GNC_Planner_Core_DW.satrec.xh3 = 0.0;
  GNC_Planner_Core_DW.satrec.xi2 = 0.0;
  GNC_Planner_Core_DW.satrec.xi3 = 0.0;
  GNC_Planner_Core_DW.satrec.xl2 = 0.0;
  GNC_Planner_Core_DW.satrec.xl3 = 0.0;
  GNC_Planner_Core_DW.satrec.xl4 = 0.0;
  GNC_Planner_Core_DW.satrec.xlamo = 0.0;
  GNC_Planner_Core_DW.satrec.zmol = 0.0;
  GNC_Planner_Core_DW.satrec.zmos = 0.0;
  GNC_Planner_Core_DW.satrec.atime = 0.0;
  GNC_Planner_Core_DW.satrec.xli = 0.0;
  GNC_Planner_Core_DW.satrec.xni = 0.0;
  GNC_Planner_Core_DW.satrec.am = 0.0;
  GNC_Planner_Core_DW.satrec.em = 0.0;
  GNC_Planner_Core_DW.satrec.im = 0.0;
  GNC_Planner_Core_DW.satrec.Om = 0.0;
  GNC_Planner_Core_DW.satrec.om = 0.0;
  GNC_Planner_Core_DW.satrec.mm = 0.0;
  GNC_Planner_Core_DW.satrec.nm = 0.0;
  GNC_Planner_Core_DW.satrec.t0 = 0.0;
  GNC_Planner_Core_DW.satrec.tumin = 0.0;
  GNC_Planner_Core_DW.satrec.mu = 0.0;
  GNC_Planner_Core_DW.satrec.radiusearthkm = 0.0;
  GNC_Planner_Core_DW.satrec.xke = 0.0;
  GNC_Planner_Core_DW.satrec.j2 = 0.0;
  GNC_Planner_Core_DW.satrec.j3 = 0.0;
  GNC_Planner_Core_DW.satrec.j4 = 0.0;
  GNC_Planner_Core_DW.satrec.j3oj2 = 0.0;

  /* SystemInitialize for Outport: '<S4>/Eclipse_Flag' */
  GNC_Planner_Core_B.Eclipse_Flag = false;

  /* End of SystemInitialize for SubSystem: '<S1>/Subsystem' */

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

  /* InitializeConditions for Outport: '<Root>/cmd_dipoles' incorporates:
   *  Chart: '<S1>/GNC State Machine'
   */
  GNC_Planner_Core_Y.cmd_dipoles[0] = 0.0;
  GNC_Planner_Core_Y.cmd_dipoles[1] = 0.0;
  GNC_Planner_Core_Y.cmd_dipoles[2] = 0.0;

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
