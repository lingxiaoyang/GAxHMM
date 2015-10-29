//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
// File: foverlap_pitch.cpp
//
// MATLAB Coder version            : 2.8
// C/C++ source code generated on  : 26-Jun-2015 17:43:32
//

// Include Files
#include "rt_nonfinite.h"
#include "classifyNotes.h"
#include "foverlap_pitch.h"
#include "classifyNotes_emxutil.h"

// Function Definitions

//
// Arguments    : const emxArray_real_T *Mgamma
//                const emxArray_real_T *Mphi
//                double f0range_in_cents
//                emxArray_real_T *Moverlapped
// Return Type  : void
//
void foverlap_pitch(const emxArray_real_T *Mgamma, const emxArray_real_T *Mphi,
                    double f0range_in_cents, emxArray_real_T *Moverlapped)
{
  double f0range;
  int Nframes;
  int Mgamma_idx_0;
  int j;
  int k;
  f0range = f0range_in_cents / 100.0;
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
        if ((Mgamma->data[Mgamma_idx_0 + Mgamma->size[0] * k] > 0.0) &&
            (Mphi->data[j + Mphi->size[0] * k] > 0.0) && (Mgamma->
             data[Mgamma_idx_0 + Mgamma->size[0] * k] >= Mphi->data[j +
             Mphi->size[0] * k] - f0range) && (Mgamma->data[Mgamma_idx_0 +
             Mgamma->size[0] * k] <= Mphi->data[j + Mphi->size[0] * k] + f0range))
        {
          Moverlapped->data[Mgamma_idx_0 + Moverlapped->size[0] * j]++;
        }
      }
    }
  }
}

//
// File trailer for foverlap_pitch.cpp
//
// [EOF]
//
