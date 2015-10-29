//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
// File: classifyNotes.h
//
// MATLAB Coder version            : 2.8
// C/C++ source code generated on  : 26-Jun-2015 17:43:32
//
#ifndef __CLASSIFYNOTES_H__
#define __CLASSIFYNOTES_H__

// Include Files
#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "rt_nonfinite.h"
#include "rtwtypes.h"
#include "classifyNotes_types.h"

// Function Declarations
extern void classifyNotes(const emxArray_real_T *notes_gt, const emxArray_real_T
  *notes_tr, double onset_lim, double dur_percent_range, double min_dur_dist,
  double f0_range_in_cents, double hopsize, struct0_T *Results);

#endif

//
// File trailer for classifyNotes.h
//
// [EOF]
//
