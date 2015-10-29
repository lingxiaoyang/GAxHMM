//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
// File: foverlap.cpp
//
// MATLAB Coder version            : 2.8
// C/C++ source code generated on  : 26-Jun-2015 17:43:32
//

// Include Files
#include "rt_nonfinite.h"
#include "classifyNotes.h"
#include "foverlap.h"
#include "classifyNotes_emxutil.h"

// Function Definitions

//
// Arguments    : const emxArray_real_T *Mgamma
//                const emxArray_real_T *Mphi
//                emxArray_real_T *Moverlapped
// Return Type  : void
//
void foverlap(const emxArray_real_T *Mgamma, const emxArray_real_T *Mphi,
              emxArray_real_T *Moverlapped)
{
  int Nframes;
  int Mgamma_idx_0;
  int j;
  int k;
  if (Mgamma->size[1] <= Mphi->size[1]) {
    Nframes = Mgamma->size[1];
  } else {
    Nframes = Mphi->size[1];
  }

  Mgamma_idx_0 = Mgamma->size[0];
  j = Moverlapped->size[0] * Moverlapped->size[1];
  Moverlapped->size[0] = Mgamma_idx_0;
  emxEnsureCapacity((emxArray__common *)Moverlapped, j, (int)sizeof(double));
  Mgamma_idx_0 = Mphi->size[0];
  j = Moverlapped->size[0] * Moverlapped->size[1];
  Moverlapped->size[1] = Mgamma_idx_0;
  emxEnsureCapacity((emxArray__common *)Moverlapped, j, (int)sizeof(double));
  Mgamma_idx_0 = Mgamma->size[0] * Mphi->size[0];
  for (j = 0; j < Mgamma_idx_0; j++) {
    Moverlapped->data[j] = 0.0;
  }

  for (Mgamma_idx_0 = 0; Mgamma_idx_0 < Mgamma->size[0]; Mgamma_idx_0++) {
    for (j = 0; j < Mphi->size[0]; j++) {
      for (k = 0; k < Nframes; k++) {
        if ((Mgamma->data[Mgamma_idx_0 + Mgamma->size[0] * k] != 0.0) &&
            (Mphi->data[j + Mphi->size[0] * k] != 0.0)) {
          Moverlapped->data[Mgamma_idx_0 + Moverlapped->size[0] * j]++;
        }
      }
    }
  }
}

//
// File trailer for foverlap.cpp
//
// [EOF]
//
