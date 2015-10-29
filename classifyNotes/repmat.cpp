//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
// File: repmat.cpp
//
// MATLAB Coder version            : 2.8
// C/C++ source code generated on  : 26-Jun-2015 17:43:32
//

// Include Files
#include "rt_nonfinite.h"
#include "classifyNotes.h"
#include "repmat.h"
#include "classifyNotes_emxutil.h"

// Function Definitions

//
// Arguments    : double varargin_1
//                emxArray_d_struct_T *b
// Return Type  : void
//
void b_repmat(double varargin_1, emxArray_d_struct_T *b)
{
  int i8;
  int loop_ub;
  static const c_struct_T r9 = { { 0.0, 0.0, 0.0 } };

  i8 = b->size[0];
  b->size[0] = (int)varargin_1;
  emxEnsureCapacity((emxArray__common *)b, i8, (int)sizeof(c_struct_T));
  loop_ub = (int)varargin_1;
  for (i8 = 0; i8 < loop_ub; i8++) {
    b->data[i8] = r9;
  }
}

//
// Arguments    : double varargin_1
//                emxArray_c_struct_T *b
// Return Type  : void
//
void repmat(double varargin_1, emxArray_c_struct_T *b)
{
  int i7;
  int loop_ub;
  static const c_struct_T r8 = { { 0.0, 0.0, 0.0 } };

  i7 = b->size[0];
  b->size[0] = (int)varargin_1;
  emxEnsureCapacity((emxArray__common *)b, i7, (int)sizeof(c_struct_T));
  loop_ub = (int)varargin_1;
  for (i7 = 0; i7 < loop_ub; i7++) {
    b->data[i7] = r8;
  }
}

//
// File trailer for repmat.cpp
//
// [EOF]
//
