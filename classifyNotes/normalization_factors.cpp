//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
// File: normalization_factors.cpp
//
// MATLAB Coder version            : 2.8
// C/C++ source code generated on  : 26-Jun-2015 17:43:32
//

// Include Files
#include "rt_nonfinite.h"
#include "classifyNotes.h"
#include "normalization_factors.h"
#include "classifyNotes_emxutil.h"

// Function Definitions

//
// Arguments    : const emxArray_real_T *Mnotes
//                emxArray_real_T *Lnorm
// Return Type  : void
//
void normalization_factors(const emxArray_real_T *Mnotes, emxArray_real_T *Lnorm)
{
  int k;
  int i6;
  int i;
  emxArray_boolean_T *x;
  double nnz;
  k = Mnotes->size[0];
  i6 = Lnorm->size[0] * Lnorm->size[1];
  Lnorm->size[0] = k;
  emxEnsureCapacity((emxArray__common *)Lnorm, i6, (int)sizeof(double));
  k = Mnotes->size[0];
  i6 = Lnorm->size[0] * Lnorm->size[1];
  Lnorm->size[1] = k;
  emxEnsureCapacity((emxArray__common *)Lnorm, i6, (int)sizeof(double));
  k = Mnotes->size[0] * Mnotes->size[0];
  for (i6 = 0; i6 < k; i6++) {
    Lnorm->data[i6] = 0.0;
  }

  i = 0;
  b_emxInit_boolean_T(&x, 2);
  while (i <= Mnotes->size[0] - 1) {
    k = Mnotes->size[1];
    i6 = x->size[0] * x->size[1];
    x->size[0] = 1;
    x->size[1] = k;
    emxEnsureCapacity((emxArray__common *)x, i6, (int)sizeof(boolean_T));
    for (i6 = 0; i6 < k; i6++) {
      x->data[x->size[0] * i6] = (Mnotes->data[i + Mnotes->size[0] * i6] != 0.0);
    }

    if (x->size[1] == 0) {
      nnz = 0.0;
    } else {
      nnz = x->data[0];
      for (k = 2; k <= x->size[1]; k++) {
        nnz += (double)x->data[k - 1];
      }
    }

    if (nnz == 0.0) {
      Lnorm->data[i + Lnorm->size[0] * i] = 0.0;
    } else {
      Lnorm->data[i + Lnorm->size[0] * i] = 1.0 / nnz;
    }

    i++;
  }

  emxFree_boolean_T(&x);
}

//
// File trailer for normalization_factors.cpp
//
// [EOF]
//
