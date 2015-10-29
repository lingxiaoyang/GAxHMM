//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
// File: intersect_simple.cpp
//
// MATLAB Coder version            : 2.8
// C/C++ source code generated on  : 26-Jun-2015 17:43:32
//

// Include Files
#include "rt_nonfinite.h"
#include "classifyNotes.h"
#include "intersect_simple.h"
#include "classifyNotes_emxutil.h"

// Function Definitions

//
// ONLY SUPPORT TWO IN ARGS and ONE OUT ARG and ONLY COLUMN VEC
// INTERSECT Set intersection.
//    C = INTERSECT(A,B) for vectors A and B, returns the values common to
//    the two vectors with no repetitions. C will be sorted.
// Arguments    : const emxArray_real_T *a
//                const emxArray_real_T *b
//                emxArray_real_T *c
// Return Type  : void
//
void intersect_simple(const emxArray_real_T *a, const emxArray_real_T *b,
                      emxArray_real_T *c)
{
  unsigned int uv1[2];
  int i9;
  emxArray_boolean_T *r10;
  int i2;
  int k;
  double x;
  int n;
  int p;
  boolean_T exitg3;
  int q;
  double absxk;
  int pEnd;
  boolean_T b_p;
  boolean_T exitg2;
  int qEnd;
  emxArray_int32_T *loc;
  int i;
  emxArray_int32_T *idx;
  int na;
  emxArray_int32_T *iwork;
  int j;
  int kEnd;
  int32_T exitg1;
  int exponent;
  int i10;

  //  reverse a and b for better performance
  for (i9 = 0; i9 < 2; i9++) {
    uv1[i9] = (unsigned int)b->size[i9];
  }

  b_emxInit_boolean_T(&r10, 2);
  i9 = r10->size[0] * r10->size[1];
  r10->size[0] = 1;
  emxEnsureCapacity((emxArray__common *)r10, i9, (int)sizeof(boolean_T));
  i9 = r10->size[0] * r10->size[1];
  r10->size[1] = (int)uv1[1];
  emxEnsureCapacity((emxArray__common *)r10, i9, (int)sizeof(boolean_T));
  i2 = (int)uv1[1];
  for (i9 = 0; i9 < i2; i9++) {
    r10->data[i9] = false;
  }

  for (k = 0; k + 1 <= b->size[1]; k++) {
    x = b->data[k];
    n = -2;
    i2 = 1;
    p = a->size[1];
    exitg3 = false;
    while ((!exitg3) && (p >= i2)) {
      q = ((i2 >> 1) + (p >> 1)) - 1;
      if (((i2 & 1) == 1) && ((p & 1) == 1)) {
        q++;
      }

      absxk = fabs(a->data[q] / 2.0);
      if ((!rtIsInf(absxk)) && (!rtIsNaN(absxk))) {
        if (absxk <= 2.2250738585072014E-308) {
          absxk = 4.94065645841247E-324;
        } else {
          frexp(absxk, &pEnd);
          absxk = ldexp(1.0, pEnd - 53);
        }
      } else {
        absxk = rtNaN;
      }

      if ((fabs(a->data[q] - x) < absxk) || (rtIsInf(x) && rtIsInf(a->data[q]) &&
           ((x > 0.0) == (a->data[q] > 0.0)))) {
        b_p = true;
      } else {
        b_p = false;
      }

      if (b_p) {
        n = q - 1;
        exitg3 = true;
      } else {
        if ((x < a->data[q]) || rtIsNaN(a->data[q])) {
          b_p = true;
        } else {
          b_p = false;
        }

        if (b_p) {
          p = q;
        } else {
          i2 = q + 2;
        }
      }
    }

    if (n + 2 > 0) {
      exitg2 = false;
      while ((!exitg2) && (n + 1 > 0)) {
        absxk = fabs(a->data[n] / 2.0);
        if ((!rtIsInf(absxk)) && (!rtIsNaN(absxk))) {
          if (absxk <= 2.2250738585072014E-308) {
            absxk = 4.94065645841247E-324;
          } else {
            frexp(absxk, &qEnd);
            absxk = ldexp(1.0, qEnd - 53);
          }
        } else {
          absxk = rtNaN;
        }

        if ((fabs(a->data[n] - x) < absxk) || (rtIsInf(x) && rtIsInf(a->data[n])
             && ((x > 0.0) == (a->data[n] > 0.0)))) {
          b_p = true;
        } else {
          b_p = false;
        }

        if (b_p) {
          n--;
        } else {
          exitg2 = true;
        }
      }
    }

    if (n + 2 > 0) {
      r10->data[k] = true;
    }
  }

  emxInit_int32_T(&loc, 2);
  p = r10->size[1] - 1;
  i2 = 0;
  for (i = 0; i <= p; i++) {
    if (r10->data[i]) {
      i2++;
    }
  }

  i9 = loc->size[0] * loc->size[1];
  loc->size[0] = 1;
  loc->size[1] = i2;
  emxEnsureCapacity((emxArray__common *)loc, i9, (int)sizeof(int));
  i2 = 0;
  for (i = 0; i <= p; i++) {
    if (r10->data[i]) {
      loc->data[i2] = i + 1;
      i2++;
    }
  }

  emxFree_boolean_T(&r10);
  emxInit_int32_T(&idx, 2);
  na = loc->size[1];
  i9 = idx->size[0] * idx->size[1];
  idx->size[0] = 1;
  emxEnsureCapacity((emxArray__common *)idx, i9, (int)sizeof(int));
  i2 = loc->size[1];
  i9 = idx->size[0] * idx->size[1];
  idx->size[1] = i2;
  emxEnsureCapacity((emxArray__common *)idx, i9, (int)sizeof(int));
  i2 = loc->size[1];
  for (i9 = 0; i9 < i2; i9++) {
    idx->data[i9] = 0;
  }

  if (loc->size[1] == 0) {
  } else {
    b_emxInit_int32_T(&iwork, 1);
    n = loc->size[1] + 1;
    i2 = loc->size[1];
    i9 = iwork->size[0];
    iwork->size[0] = i2;
    emxEnsureCapacity((emxArray__common *)iwork, i9, (int)sizeof(int));
    for (k = 1; k <= n - 2; k += 2) {
      absxk = b->data[loc->data[loc->size[0] * k] - 1];
      if ((b->data[loc->data[loc->size[0] * (k - 1)] - 1] <= absxk) || rtIsNaN
          (absxk)) {
        b_p = true;
      } else {
        b_p = false;
      }

      if (b_p) {
        idx->data[k - 1] = k;
        idx->data[k] = k + 1;
      } else {
        idx->data[k - 1] = k + 1;
        idx->data[k] = k;
      }
    }

    if ((loc->size[1] & 1) != 0) {
      idx->data[loc->size[1] - 1] = loc->size[1];
    }

    i = 2;
    while (i < n - 1) {
      i2 = i << 1;
      j = 1;
      for (pEnd = 1 + i; pEnd < n; pEnd = qEnd + i) {
        p = j;
        q = pEnd;
        qEnd = j + i2;
        if (qEnd > n) {
          qEnd = n;
        }

        k = 0;
        kEnd = qEnd - j;
        while (k + 1 <= kEnd) {
          i9 = idx->data[q - 1] - 1;
          absxk = b->data[loc->data[loc->size[0] * i9] - 1];
          i9 = idx->data[p - 1] - 1;
          if ((b->data[loc->data[loc->size[0] * i9] - 1] <= absxk) || rtIsNaN
              (absxk)) {
            b_p = true;
          } else {
            b_p = false;
          }

          if (b_p) {
            iwork->data[k] = idx->data[p - 1];
            p++;
            if (p == pEnd) {
              while (q < qEnd) {
                k++;
                iwork->data[k] = idx->data[q - 1];
                q++;
              }
            }
          } else {
            iwork->data[k] = idx->data[q - 1];
            q++;
            if (q == qEnd) {
              while (p < pEnd) {
                k++;
                iwork->data[k] = idx->data[p - 1];
                p++;
              }
            }
          }

          k++;
        }

        for (k = 0; k + 1 <= kEnd; k++) {
          idx->data[(j + k) - 1] = iwork->data[k];
        }

        j = qEnd;
      }

      i = i2;
    }

    emxFree_int32_T(&iwork);
  }

  for (i9 = 0; i9 < 2; i9++) {
    uv1[i9] = (unsigned int)loc->size[i9];
  }

  i9 = c->size[0] * c->size[1];
  c->size[0] = 1;
  c->size[1] = (int)uv1[1];
  emxEnsureCapacity((emxArray__common *)c, i9, (int)sizeof(double));
  for (k = 0; k + 1 <= na; k++) {
    i9 = idx->data[k] - 1;
    c->data[k] = b->data[loc->data[loc->size[0] * i9] - 1];
  }

  emxFree_int32_T(&idx);
  k = 0;
  while ((k + 1 <= na) && rtIsInf(c->data[k]) && (c->data[k] < 0.0)) {
    k++;
  }

  pEnd = k;
  k = loc->size[1];
  while ((k >= 1) && rtIsNaN(c->data[k - 1])) {
    k--;
  }

  q = loc->size[1] - k;
  while ((k >= 1) && rtIsInf(c->data[k - 1]) && (c->data[k - 1] > 0.0)) {
    k--;
  }

  p = (loc->size[1] - k) - q;
  qEnd = -1;
  emxFree_int32_T(&loc);
  if (pEnd > 0) {
    qEnd = 0;
  }

  i2 = (pEnd + k) - pEnd;
  while (pEnd + 1 <= i2) {
    x = c->data[pEnd];
    do {
      exitg1 = 0;
      pEnd++;
      if (pEnd + 1 > i2) {
        exitg1 = 1;
      } else {
        absxk = fabs(x / 2.0);
        if ((!rtIsInf(absxk)) && (!rtIsNaN(absxk))) {
          if (absxk <= 2.2250738585072014E-308) {
            absxk = 4.94065645841247E-324;
          } else {
            frexp(absxk, &exponent);
            absxk = ldexp(1.0, exponent - 53);
          }
        } else {
          absxk = rtNaN;
        }

        if ((fabs(x - c->data[pEnd]) < absxk) || (rtIsInf(c->data[pEnd]) &&
             rtIsInf(x) && ((c->data[pEnd] > 0.0) == (x > 0.0)))) {
          b_p = true;
        } else {
          b_p = false;
        }

        if (!b_p) {
          exitg1 = 1;
        }
      }
    } while (exitg1 == 0);

    qEnd++;
    c->data[qEnd] = x;
  }

  if (p > 0) {
    qEnd++;
    c->data[qEnd] = c->data[i2];
  }

  pEnd = i2 + p;
  for (j = 1; j <= q; j++) {
    qEnd++;
    c->data[qEnd] = c->data[(pEnd + j) - 1];
  }

  i9 = c->size[0] * c->size[1];
  if (1 > qEnd + 1) {
    i10 = -1;
  } else {
    i10 = qEnd;
  }

  c->size[1] = i10 + 1;
  emxEnsureCapacity((emxArray__common *)c, i9, (int)sizeof(double));

  //  make sure output has correct type
}

//
// File trailer for intersect_simple.cpp
//
// [EOF]
//
