/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: _coder_classifyNotes_api.h
 *
 * MATLAB Coder version            : 2.8
 * C/C++ source code generated on  : 26-Jun-2015 17:43:32
 */

#ifndef ___CODER_CLASSIFYNOTES_API_H__
#define ___CODER_CLASSIFYNOTES_API_H__

/* Include Files */
#include "tmwtypes.h"
#include "mex.h"
#include "emlrt.h"
#include <stddef.h>
#include <stdlib.h>
#include "_coder_classifyNotes_api.h"

/* Type Definitions */
#ifndef struct_emxArray_real_T
#define struct_emxArray_real_T

struct emxArray_real_T
{
  real_T *data;
  int32_T *size;
  int32_T allocatedSize;
  int32_T numDimensions;
  boolean_T canFreeData;
};

#endif                                 /*struct_emxArray_real_T*/

#ifndef typedef_emxArray_real_T
#define typedef_emxArray_real_T

typedef struct emxArray_real_T emxArray_real_T;

#endif                                 /*typedef_emxArray_real_T*/

#ifndef typedef_struct0_T
#define typedef_struct0_T

typedef struct {
  real_T Dur_GT;
  real_T Dur_TR;
  real_T N_GT;
  real_T N_TR;
  emxArray_real_T *COnPOff_listgt;
  real_T COnPOff_Precision;
  real_T COnPOff_Recall;
  real_T COnPOff_Fmeasure;
  emxArray_real_T *COnOff_listgt;
  real_T COnOff_Precision;
  real_T COnOff_Recall;
  real_T COnOff_Fmeasure;
  emxArray_real_T *COnP_listgt;
  real_T COnP_Precision;
  real_T COnP_Recall;
  real_T COnP_Fmeasure;
  emxArray_real_T *COn_listgt;
  real_T COn_Precision;
  real_T COn_Recall;
  real_T COn_Fmeasure;
  emxArray_real_T *OBOn_listgt;
  real_T OBOn_rategt;
  emxArray_real_T *OBP_listgt;
  real_T OBP_rategt;
  emxArray_real_T *OBOff_listgt;
  real_T OBOff_rategt;
  emxArray_real_T *S_listgt;
  real_T S_rategt;
  real_T S_ratio;
  emxArray_real_T *M_listgt;
  real_T M_rategt;
  real_T M_ratio;
  emxArray_real_T *PU_listtr;
  real_T PU_ratetr;
  emxArray_real_T *ND_listgt;
  real_T ND_rategt;
} struct0_T;

#endif                                 /*typedef_struct0_T*/

/* Variable Declarations */
extern emlrtCTX emlrtRootTLSGlobal;
extern emlrtContext emlrtContextGlobal;

/* Function Declarations */
extern void classifyNotes(emxArray_real_T *notes_gt, emxArray_real_T *notes_tr,
  real_T onset_lim, real_T dur_percent_range, real_T min_dur_dist, real_T
  f0_range_in_cents, real_T hopsize, struct0_T *Results);
extern void classifyNotes_api(const mxArray *prhs[7], const mxArray *plhs[1]);
extern void classifyNotes_atexit(void);
extern void classifyNotes_initialize(void);
extern void classifyNotes_terminate(void);
extern void classifyNotes_xil_terminate(void);

#endif

/*
 * File trailer for _coder_classifyNotes_api.h
 *
 * [EOF]
 */
