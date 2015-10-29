/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: _coder_classifyNotes_api.c
 *
 * MATLAB Coder version            : 2.8
 * C/C++ source code generated on  : 26-Jun-2015 17:43:32
 */

/* Include Files */
#include "tmwtypes.h"
#include "_coder_classifyNotes_api.h"

/* Variable Definitions */
emlrtCTX emlrtRootTLSGlobal = NULL;
emlrtContext emlrtContextGlobal = { true, false, 131418U, NULL, "classifyNotes",
  NULL, false, { 2045744189U, 2170104910U, 2743257031U, 4284093946U }, NULL };

/* Function Declarations */
static void b_emlrt_marshallIn(const emlrtStack *sp, const mxArray *u, const
  emlrtMsgIdentifier *parentId, emxArray_real_T *y);
static const mxArray *b_emlrt_marshallOut(const emxArray_real_T *u);
static real_T c_emlrt_marshallIn(const emlrtStack *sp, const mxArray *onset_lim,
  const char_T *identifier);
static real_T d_emlrt_marshallIn(const emlrtStack *sp, const mxArray *u, const
  emlrtMsgIdentifier *parentId);
static void e_emlrt_marshallIn(const emlrtStack *sp, const mxArray *src, const
  emlrtMsgIdentifier *msgId, emxArray_real_T *ret);
static void emlrt_marshallIn(const emlrtStack *sp, const mxArray *notes_gt,
  const char_T *identifier, emxArray_real_T *y);
static const mxArray *emlrt_marshallOut(const struct0_T *u);
static void emxFreeStruct_struct0_T(struct0_T *pStruct);
static void emxFree_real_T(emxArray_real_T **pEmxArray);
static void emxInitStruct_struct0_T(const emlrtStack *sp, struct0_T *pStruct,
  boolean_T doPush);
static void emxInit_real_T(const emlrtStack *sp, emxArray_real_T **pEmxArray,
  int32_T numDimensions, boolean_T doPush);
static real_T f_emlrt_marshallIn(const emlrtStack *sp, const mxArray *src, const
  emlrtMsgIdentifier *msgId);

/* Function Definitions */

/*
 * Arguments    : const emlrtStack *sp
 *                const mxArray *u
 *                const emlrtMsgIdentifier *parentId
 *                emxArray_real_T *y
 * Return Type  : void
 */
static void b_emlrt_marshallIn(const emlrtStack *sp, const mxArray *u, const
  emlrtMsgIdentifier *parentId, emxArray_real_T *y)
{
  e_emlrt_marshallIn(sp, emlrtAlias(u), parentId, y);
  emlrtDestroyArray(&u);
}

/*
 * Arguments    : const emxArray_real_T *u
 * Return Type  : const mxArray *
 */
static const mxArray *b_emlrt_marshallOut(const emxArray_real_T *u)
{
  const mxArray *y;
  const mxArray *m1;
  real_T *pData;
  int32_T i0;
  int32_T i;
  y = NULL;
  m1 = emlrtCreateNumericArray(2, *(int32_T (*)[2])u->size, mxDOUBLE_CLASS,
    mxREAL);
  pData = (real_T *)mxGetPr(m1);
  i0 = 0;
  for (i = 0; i < u->size[1]; i++) {
    pData[i0] = u->data[u->size[0] * i];
    i0++;
  }

  emlrtAssign(&y, m1);
  return y;
}

/*
 * Arguments    : const emlrtStack *sp
 *                const mxArray *onset_lim
 *                const char_T *identifier
 * Return Type  : real_T
 */
static real_T c_emlrt_marshallIn(const emlrtStack *sp, const mxArray *onset_lim,
  const char_T *identifier)
{
  real_T y;
  emlrtMsgIdentifier thisId;
  thisId.fIdentifier = identifier;
  thisId.fParent = NULL;
  y = d_emlrt_marshallIn(sp, emlrtAlias(onset_lim), &thisId);
  emlrtDestroyArray(&onset_lim);
  return y;
}

/*
 * Arguments    : const emlrtStack *sp
 *                const mxArray *u
 *                const emlrtMsgIdentifier *parentId
 * Return Type  : real_T
 */
static real_T d_emlrt_marshallIn(const emlrtStack *sp, const mxArray *u, const
  emlrtMsgIdentifier *parentId)
{
  real_T y;
  y = f_emlrt_marshallIn(sp, emlrtAlias(u), parentId);
  emlrtDestroyArray(&u);
  return y;
}

/*
 * Arguments    : const emlrtStack *sp
 *                const mxArray *src
 *                const emlrtMsgIdentifier *msgId
 *                emxArray_real_T *ret
 * Return Type  : void
 */
static void e_emlrt_marshallIn(const emlrtStack *sp, const mxArray *src, const
  emlrtMsgIdentifier *msgId, emxArray_real_T *ret)
{
  int32_T iv0[2];
  int32_T i1;
  int32_T iv1[2];
  boolean_T bv0[2] = { true, false };

  for (i1 = 0; i1 < 2; i1++) {
    iv0[i1] = (i1 << 2) - 1;
  }

  emlrtCheckVsBuiltInR2012b(sp, msgId, src, "double", false, 2U, iv0, &bv0[0],
    iv1);
  ret->size[0] = iv1[0];
  ret->size[1] = iv1[1];
  ret->allocatedSize = ret->size[0] * ret->size[1];
  ret->data = (real_T *)mxGetData(src);
  ret->canFreeData = false;
  emlrtDestroyArray(&src);
}

/*
 * Arguments    : const emlrtStack *sp
 *                const mxArray *notes_gt
 *                const char_T *identifier
 *                emxArray_real_T *y
 * Return Type  : void
 */
static void emlrt_marshallIn(const emlrtStack *sp, const mxArray *notes_gt,
  const char_T *identifier, emxArray_real_T *y)
{
  emlrtMsgIdentifier thisId;
  thisId.fIdentifier = identifier;
  thisId.fParent = NULL;
  b_emlrt_marshallIn(sp, emlrtAlias(notes_gt), &thisId, y);
  emlrtDestroyArray(&notes_gt);
}

/*
 * Arguments    : const struct0_T *u
 * Return Type  : const mxArray *
 */
static const mxArray *emlrt_marshallOut(const struct0_T *u)
{
  const mxArray *y;
  const mxArray *b_y;
  const mxArray *m0;
  const mxArray *c_y;
  const mxArray *d_y;
  const mxArray *e_y;
  const mxArray *f_y;
  const mxArray *g_y;
  const mxArray *h_y;
  const mxArray *i_y;
  const mxArray *j_y;
  const mxArray *k_y;
  const mxArray *l_y;
  const mxArray *m_y;
  const mxArray *n_y;
  const mxArray *o_y;
  const mxArray *p_y;
  const mxArray *q_y;
  const mxArray *r_y;
  const mxArray *s_y;
  const mxArray *t_y;
  const mxArray *u_y;
  const mxArray *v_y;
  const mxArray *w_y;
  const mxArray *x_y;
  const mxArray *y_y;
  const mxArray *ab_y;
  y = NULL;
  emlrtAssign(&y, emlrtCreateStructMatrix(1, 1, 0, NULL));
  b_y = NULL;
  m0 = emlrtCreateDoubleScalar(u->Dur_GT);
  emlrtAssign(&b_y, m0);
  emlrtAddField(y, b_y, "Dur_GT", 0);
  c_y = NULL;
  m0 = emlrtCreateDoubleScalar(u->Dur_TR);
  emlrtAssign(&c_y, m0);
  emlrtAddField(y, c_y, "Dur_TR", 0);
  d_y = NULL;
  m0 = emlrtCreateDoubleScalar(u->N_GT);
  emlrtAssign(&d_y, m0);
  emlrtAddField(y, d_y, "N_GT", 0);
  e_y = NULL;
  m0 = emlrtCreateDoubleScalar(u->N_TR);
  emlrtAssign(&e_y, m0);
  emlrtAddField(y, e_y, "N_TR", 0);
  emlrtAddField(y, b_emlrt_marshallOut(u->COnPOff_listgt), "COnPOff_listgt", 0);
  f_y = NULL;
  m0 = emlrtCreateDoubleScalar(u->COnPOff_Precision);
  emlrtAssign(&f_y, m0);
  emlrtAddField(y, f_y, "COnPOff_Precision", 0);
  g_y = NULL;
  m0 = emlrtCreateDoubleScalar(u->COnPOff_Recall);
  emlrtAssign(&g_y, m0);
  emlrtAddField(y, g_y, "COnPOff_Recall", 0);
  h_y = NULL;
  m0 = emlrtCreateDoubleScalar(u->COnPOff_Fmeasure);
  emlrtAssign(&h_y, m0);
  emlrtAddField(y, h_y, "COnPOff_Fmeasure", 0);
  emlrtAddField(y, b_emlrt_marshallOut(u->COnOff_listgt), "COnOff_listgt", 0);
  i_y = NULL;
  m0 = emlrtCreateDoubleScalar(u->COnOff_Precision);
  emlrtAssign(&i_y, m0);
  emlrtAddField(y, i_y, "COnOff_Precision", 0);
  j_y = NULL;
  m0 = emlrtCreateDoubleScalar(u->COnOff_Recall);
  emlrtAssign(&j_y, m0);
  emlrtAddField(y, j_y, "COnOff_Recall", 0);
  k_y = NULL;
  m0 = emlrtCreateDoubleScalar(u->COnOff_Fmeasure);
  emlrtAssign(&k_y, m0);
  emlrtAddField(y, k_y, "COnOff_Fmeasure", 0);
  emlrtAddField(y, b_emlrt_marshallOut(u->COnP_listgt), "COnP_listgt", 0);
  l_y = NULL;
  m0 = emlrtCreateDoubleScalar(u->COnP_Precision);
  emlrtAssign(&l_y, m0);
  emlrtAddField(y, l_y, "COnP_Precision", 0);
  m_y = NULL;
  m0 = emlrtCreateDoubleScalar(u->COnP_Recall);
  emlrtAssign(&m_y, m0);
  emlrtAddField(y, m_y, "COnP_Recall", 0);
  n_y = NULL;
  m0 = emlrtCreateDoubleScalar(u->COnP_Fmeasure);
  emlrtAssign(&n_y, m0);
  emlrtAddField(y, n_y, "COnP_Fmeasure", 0);
  emlrtAddField(y, b_emlrt_marshallOut(u->COn_listgt), "COn_listgt", 0);
  o_y = NULL;
  m0 = emlrtCreateDoubleScalar(u->COn_Precision);
  emlrtAssign(&o_y, m0);
  emlrtAddField(y, o_y, "COn_Precision", 0);
  p_y = NULL;
  m0 = emlrtCreateDoubleScalar(u->COn_Recall);
  emlrtAssign(&p_y, m0);
  emlrtAddField(y, p_y, "COn_Recall", 0);
  q_y = NULL;
  m0 = emlrtCreateDoubleScalar(u->COn_Fmeasure);
  emlrtAssign(&q_y, m0);
  emlrtAddField(y, q_y, "COn_Fmeasure", 0);
  emlrtAddField(y, b_emlrt_marshallOut(u->OBOn_listgt), "OBOn_listgt", 0);
  r_y = NULL;
  m0 = emlrtCreateDoubleScalar(u->OBOn_rategt);
  emlrtAssign(&r_y, m0);
  emlrtAddField(y, r_y, "OBOn_rategt", 0);
  emlrtAddField(y, b_emlrt_marshallOut(u->OBP_listgt), "OBP_listgt", 0);
  s_y = NULL;
  m0 = emlrtCreateDoubleScalar(u->OBP_rategt);
  emlrtAssign(&s_y, m0);
  emlrtAddField(y, s_y, "OBP_rategt", 0);
  emlrtAddField(y, b_emlrt_marshallOut(u->OBOff_listgt), "OBOff_listgt", 0);
  t_y = NULL;
  m0 = emlrtCreateDoubleScalar(u->OBOff_rategt);
  emlrtAssign(&t_y, m0);
  emlrtAddField(y, t_y, "OBOff_rategt", 0);
  emlrtAddField(y, b_emlrt_marshallOut(u->S_listgt), "S_listgt", 0);
  u_y = NULL;
  m0 = emlrtCreateDoubleScalar(u->S_rategt);
  emlrtAssign(&u_y, m0);
  emlrtAddField(y, u_y, "S_rategt", 0);
  v_y = NULL;
  m0 = emlrtCreateDoubleScalar(u->S_ratio);
  emlrtAssign(&v_y, m0);
  emlrtAddField(y, v_y, "S_ratio", 0);
  emlrtAddField(y, b_emlrt_marshallOut(u->M_listgt), "M_listgt", 0);
  w_y = NULL;
  m0 = emlrtCreateDoubleScalar(u->M_rategt);
  emlrtAssign(&w_y, m0);
  emlrtAddField(y, w_y, "M_rategt", 0);
  x_y = NULL;
  m0 = emlrtCreateDoubleScalar(u->M_ratio);
  emlrtAssign(&x_y, m0);
  emlrtAddField(y, x_y, "M_ratio", 0);
  emlrtAddField(y, b_emlrt_marshallOut(u->PU_listtr), "PU_listtr", 0);
  y_y = NULL;
  m0 = emlrtCreateDoubleScalar(u->PU_ratetr);
  emlrtAssign(&y_y, m0);
  emlrtAddField(y, y_y, "PU_ratetr", 0);
  emlrtAddField(y, b_emlrt_marshallOut(u->ND_listgt), "ND_listgt", 0);
  ab_y = NULL;
  m0 = emlrtCreateDoubleScalar(u->ND_rategt);
  emlrtAssign(&ab_y, m0);
  emlrtAddField(y, ab_y, "ND_rategt", 0);
  return y;
}

/*
 * Arguments    : struct0_T *pStruct
 * Return Type  : void
 */
static void emxFreeStruct_struct0_T(struct0_T *pStruct)
{
  emxFree_real_T(&pStruct->COnPOff_listgt);
  emxFree_real_T(&pStruct->COnOff_listgt);
  emxFree_real_T(&pStruct->COnP_listgt);
  emxFree_real_T(&pStruct->COn_listgt);
  emxFree_real_T(&pStruct->OBOn_listgt);
  emxFree_real_T(&pStruct->OBP_listgt);
  emxFree_real_T(&pStruct->OBOff_listgt);
  emxFree_real_T(&pStruct->S_listgt);
  emxFree_real_T(&pStruct->M_listgt);
  emxFree_real_T(&pStruct->PU_listtr);
  emxFree_real_T(&pStruct->ND_listgt);
}

/*
 * Arguments    : emxArray_real_T **pEmxArray
 * Return Type  : void
 */
static void emxFree_real_T(emxArray_real_T **pEmxArray)
{
  if (*pEmxArray != (emxArray_real_T *)NULL) {
    if (((*pEmxArray)->data != (real_T *)NULL) && (*pEmxArray)->canFreeData) {
      emlrtFreeMex((void *)(*pEmxArray)->data);
    }

    emlrtFreeMex((void *)(*pEmxArray)->size);
    emlrtFreeMex((void *)*pEmxArray);
    *pEmxArray = (emxArray_real_T *)NULL;
  }
}

/*
 * Arguments    : const emlrtStack *sp
 *                struct0_T *pStruct
 *                boolean_T doPush
 * Return Type  : void
 */
static void emxInitStruct_struct0_T(const emlrtStack *sp, struct0_T *pStruct,
  boolean_T doPush)
{
  emxInit_real_T(sp, &pStruct->COnPOff_listgt, 2, doPush);
  emxInit_real_T(sp, &pStruct->COnOff_listgt, 2, doPush);
  emxInit_real_T(sp, &pStruct->COnP_listgt, 2, doPush);
  emxInit_real_T(sp, &pStruct->COn_listgt, 2, doPush);
  emxInit_real_T(sp, &pStruct->OBOn_listgt, 2, doPush);
  emxInit_real_T(sp, &pStruct->OBP_listgt, 2, doPush);
  emxInit_real_T(sp, &pStruct->OBOff_listgt, 2, doPush);
  emxInit_real_T(sp, &pStruct->S_listgt, 2, doPush);
  emxInit_real_T(sp, &pStruct->M_listgt, 2, doPush);
  emxInit_real_T(sp, &pStruct->PU_listtr, 2, doPush);
  emxInit_real_T(sp, &pStruct->ND_listgt, 2, doPush);
}

/*
 * Arguments    : const emlrtStack *sp
 *                emxArray_real_T **pEmxArray
 *                int32_T numDimensions
 *                boolean_T doPush
 * Return Type  : void
 */
static void emxInit_real_T(const emlrtStack *sp, emxArray_real_T **pEmxArray,
  int32_T numDimensions, boolean_T doPush)
{
  emxArray_real_T *emxArray;
  int32_T i;
  *pEmxArray = (emxArray_real_T *)emlrtMallocMex(sizeof(emxArray_real_T));
  if (doPush) {
    emlrtPushHeapReferenceStackR2012b(sp, (void *)pEmxArray, (void (*)(void *))
      emxFree_real_T);
  }

  emxArray = *pEmxArray;
  emxArray->data = (real_T *)NULL;
  emxArray->numDimensions = numDimensions;
  emxArray->size = (int32_T *)emlrtMallocMex((uint32_T)(sizeof(int32_T)
    * numDimensions));
  emxArray->allocatedSize = 0;
  emxArray->canFreeData = true;
  for (i = 0; i < numDimensions; i++) {
    emxArray->size[i] = 0;
  }
}

/*
 * Arguments    : const emlrtStack *sp
 *                const mxArray *src
 *                const emlrtMsgIdentifier *msgId
 * Return Type  : real_T
 */
static real_T f_emlrt_marshallIn(const emlrtStack *sp, const mxArray *src, const
  emlrtMsgIdentifier *msgId)
{
  real_T ret;
  emlrtCheckBuiltInR2012b(sp, msgId, src, "double", false, 0U, 0);
  ret = *(real_T *)mxGetData(src);
  emlrtDestroyArray(&src);
  return ret;
}

/*
 * Arguments    : const mxArray *prhs[7]
 *                const mxArray *plhs[1]
 * Return Type  : void
 */
void classifyNotes_api(const mxArray *prhs[7], const mxArray *plhs[1])
{
  emxArray_real_T *notes_gt;
  emxArray_real_T *notes_tr;
  struct0_T Results;
  real_T onset_lim;
  real_T dur_percent_range;
  real_T min_dur_dist;
  real_T f0_range_in_cents;
  real_T hopsize;
  emlrtStack st = { NULL, NULL, NULL };

  st.tls = emlrtRootTLSGlobal;
  emlrtHeapReferenceStackEnterFcnR2012b(&st);
  emxInit_real_T(&st, &notes_gt, 2, true);
  emxInit_real_T(&st, &notes_tr, 2, true);
  emxInitStruct_struct0_T(&st, &Results, true);
  prhs[0] = emlrtProtectR2012b(prhs[0], 0, false, -1);
  prhs[1] = emlrtProtectR2012b(prhs[1], 1, false, -1);

  /* Marshall function inputs */
  emlrt_marshallIn(&st, emlrtAlias(prhs[0]), "notes_gt", notes_gt);
  emlrt_marshallIn(&st, emlrtAlias(prhs[1]), "notes_tr", notes_tr);
  onset_lim = c_emlrt_marshallIn(&st, emlrtAliasP(prhs[2]), "onset_lim");
  dur_percent_range = c_emlrt_marshallIn(&st, emlrtAliasP(prhs[3]),
    "dur_percent_range");
  min_dur_dist = c_emlrt_marshallIn(&st, emlrtAliasP(prhs[4]), "min_dur_dist");
  f0_range_in_cents = c_emlrt_marshallIn(&st, emlrtAliasP(prhs[5]),
    "f0_range_in_cents");
  hopsize = c_emlrt_marshallIn(&st, emlrtAliasP(prhs[6]), "hopsize");

  /* Invoke the target function */
  classifyNotes(notes_gt, notes_tr, onset_lim, dur_percent_range, min_dur_dist,
                f0_range_in_cents, hopsize, &Results);

  /* Marshall function outputs */
  plhs[0] = emlrt_marshallOut(&Results);
  emxFreeStruct_struct0_T(&Results);
  notes_tr->canFreeData = false;
  emxFree_real_T(&notes_tr);
  notes_gt->canFreeData = false;
  emxFree_real_T(&notes_gt);
  emlrtHeapReferenceStackLeaveFcnR2012b(&st);
}

/*
 * Arguments    : void
 * Return Type  : void
 */
void classifyNotes_atexit(void)
{
  emlrtStack st = { NULL, NULL, NULL };

  emlrtCreateRootTLS(&emlrtRootTLSGlobal, &emlrtContextGlobal, NULL, 1);
  st.tls = emlrtRootTLSGlobal;
  emlrtEnterRtStackR2012b(&st);
  emlrtLeaveRtStackR2012b(&st);
  emlrtDestroyRootTLS(&emlrtRootTLSGlobal);
  classifyNotes_xil_terminate();
}

/*
 * Arguments    : void
 * Return Type  : void
 */
void classifyNotes_initialize(void)
{
  emlrtStack st = { NULL, NULL, NULL };

  emlrtCreateRootTLS(&emlrtRootTLSGlobal, &emlrtContextGlobal, NULL, 1);
  st.tls = emlrtRootTLSGlobal;
  emlrtClearAllocCountR2012b(&st, false, 0U, 0);
  emlrtEnterRtStackR2012b(&st);
  emlrtFirstTimeR2012b(emlrtRootTLSGlobal);
}

/*
 * Arguments    : void
 * Return Type  : void
 */
void classifyNotes_terminate(void)
{
  emlrtStack st = { NULL, NULL, NULL };

  st.tls = emlrtRootTLSGlobal;
  emlrtLeaveRtStackR2012b(&st);
  emlrtDestroyRootTLS(&emlrtRootTLSGlobal);
}

/*
 * File trailer for _coder_classifyNotes_api.c
 *
 * [EOF]
 */
