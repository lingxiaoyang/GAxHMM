//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
// File: notes2matrixnotes.cpp
//
// MATLAB Coder version            : 2.8
// C/C++ source code generated on  : 26-Jun-2015 17:43:32
//

// Include Files
#include "rt_nonfinite.h"
#include "classifyNotes.h"
#include "notes2matrixnotes.h"
#include "classifyNotes_emxutil.h"

// Function Declarations
static double rt_roundd_snf(double u);

// Function Definitions

//
// Arguments    : double u
// Return Type  : double
//
static double rt_roundd_snf(double u)
{
  double y;
  if (fabs(u) < 4.503599627370496E+15) {
    if (u >= 0.5) {
      y = floor(u + 0.5);
    } else if (u > -0.5) {
      y = u * 0.0;
    } else {
      y = ceil(u - 0.5);
    }
  } else {
    y = u;
  }

  return y;
}

//
// Arguments    : emxArray_real_T *notes
//                double hopsize_secs
//                emxArray_real_T *Mnotes
// Return Type  : void
//
void notes2matrixnotes(emxArray_real_T *notes, double hopsize_secs,
  emxArray_real_T *Mnotes)
{
  emxArray_boolean_T *r11;
  int loop_ub;
  int i14;
  emxArray_int32_T *r12;
  int end;
  int i;
  emxArray_real_T *b_notes;
  double maxval;
  double Hend;
  double b_i;
  unsigned int j;
  double t;
  emxArray_real_T *c_notes;
  emxInit_boolean_T(&r11, 1);
  loop_ub = notes->size[0];
  i14 = r11->size[0];
  r11->size[0] = loop_ub;
  emxEnsureCapacity((emxArray__common *)r11, i14, (int)sizeof(boolean_T));
  for (i14 = 0; i14 < loop_ub; i14++) {
    r11->data[i14] = (notes->data[i14 + (notes->size[0] << 1)] > 0.0);
  }

  b_emxInit_int32_T(&r12, 1);
  end = r11->size[0] - 1;
  loop_ub = 0;
  for (i = 0; i <= end; i++) {
    if (r11->data[i]) {
      loop_ub++;
    }
  }

  i14 = r12->size[0];
  r12->size[0] = loop_ub;
  emxEnsureCapacity((emxArray__common *)r12, i14, (int)sizeof(int));
  loop_ub = 0;
  for (i = 0; i <= end; i++) {
    if (r11->data[i]) {
      r12->data[loop_ub] = i + 1;
      loop_ub++;
    }
  }

  emxFree_boolean_T(&r11);
  emxInit_real_T(&b_notes, 2);
  i14 = b_notes->size[0] * b_notes->size[1];
  b_notes->size[0] = r12->size[0];
  b_notes->size[1] = 3;
  emxEnsureCapacity((emxArray__common *)b_notes, i14, (int)sizeof(double));
  for (i14 = 0; i14 < 3; i14++) {
    loop_ub = r12->size[0];
    for (i = 0; i < loop_ub; i++) {
      b_notes->data[i + b_notes->size[0] * i14] = notes->data[(r12->data[i] +
        notes->size[0] * i14) - 1];
    }
  }

  emxFree_int32_T(&r12);
  i14 = notes->size[0] * notes->size[1];
  notes->size[0] = b_notes->size[0];
  notes->size[1] = 3;
  emxEnsureCapacity((emxArray__common *)notes, i14, (int)sizeof(double));
  for (i14 = 0; i14 < 3; i14++) {
    loop_ub = b_notes->size[0];
    for (i = 0; i < loop_ub; i++) {
      notes->data[i + notes->size[0] * i14] = b_notes->data[i + b_notes->size[0]
        * i14];
    }
  }

  emxFree_real_T(&b_notes);

  // Remove 0-pitch notes.
  if ((notes->data[(notes->size[0] + notes->size[0]) - 1] >= notes->data
       [notes->size[0] - 1]) || rtIsNaN(notes->data[notes->size[0] - 1])) {
    maxval = notes->data[(notes->size[0] + notes->size[0]) - 1];
  } else {
    maxval = notes->data[notes->size[0] - 1];
  }

  Hend = rt_roundd_snf(maxval / hopsize_secs);
  b_i = 0.0;
  j = 1U;
  loop_ub = notes->size[0];
  i14 = Mnotes->size[0] * Mnotes->size[1];
  Mnotes->size[0] = loop_ub;
  emxEnsureCapacity((emxArray__common *)Mnotes, i14, (int)sizeof(double));
  i14 = Mnotes->size[0] * Mnotes->size[1];
  Mnotes->size[1] = (int)Hend;
  emxEnsureCapacity((emxArray__common *)Mnotes, i14, (int)sizeof(double));
  loop_ub = notes->size[0] * (int)Hend;
  for (i14 = 0; i14 < loop_ub; i14++) {
    Mnotes->data[i14] = 0.0;
  }

  while ((b_i < Hend) && ((double)j <= notes->size[0])) {
    b_i++;
    t = b_i * hopsize_secs;
    if ((t > notes->data[(int)j - 1]) && (t < notes->data[((int)j + notes->size
          [0]) - 1])) {
      Mnotes->data[((int)j + Mnotes->size[0] * ((int)b_i - 1)) - 1] =
        notes->data[((int)j + (notes->size[0] << 1)) - 1];
    } else {
      if (t > notes->data[((int)j + notes->size[0]) - 1]) {
        j++;
        b_i--;
      }
    }
  }

  b_emxInit_real_T(&c_notes, 1);
  loop_ub = notes->size[0];
  i14 = c_notes->size[0];
  c_notes->size[0] = loop_ub;
  emxEnsureCapacity((emxArray__common *)c_notes, i14, (int)sizeof(double));
  for (i14 = 0; i14 < loop_ub; i14++) {
    c_notes->data[i14] = notes->data[i14 + notes->size[0]] - notes->data[i14];
  }

  loop_ub = c_notes->size[0];
  for (i14 = 0; i14 < loop_ub; i14++) {
    notes->data[i14 + notes->size[0]] = c_notes->data[i14];
  }

  emxFree_real_T(&c_notes);
}

//
// File trailer for notes2matrixnotes.cpp
//
// [EOF]
//
