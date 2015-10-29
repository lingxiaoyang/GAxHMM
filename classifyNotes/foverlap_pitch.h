//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
// File: foverlap_pitch.h
//
// MATLAB Coder version            : 2.8
// C/C++ source code generated on  : 26-Jun-2015 17:43:32
//
#ifndef __FOVERLAP_PITCH_H__
#define __FOVERLAP_PITCH_H__

// Include Files
#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "rt_nonfinite.h"
#include "rtwtypes.h"
#include "classifyNotes_types.h"

// Function Declarations
extern void foverlap_pitch(const emxArray_real_T *Mgamma, const emxArray_real_T *
  Mphi, double f0range_in_cents, emxArray_real_T *Moverlapped);

#endif

//
// File trailer for foverlap_pitch.h
//
// [EOF]
//
