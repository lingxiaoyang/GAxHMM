//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
// File: setdiff_simple.cpp
//
// MATLAB Coder version            : 2.8
// C/C++ source code generated on  : 26-Jun-2015 17:43:32
//

// Include Files
#include "rt_nonfinite.h"
#include "classifyNotes.h"
#include "setdiff_simple.h"
#include "classifyNotes_emxutil.h"

// Function Definitions

//
// ONLY SUPPORT TWO IN ARGS and ONE OUT ARG only FOR ROW VEC
// SETDIFF Set difference.
//    C = SETDIFF(A,B) for vectors A and B, returns the values in A that
//    are not in B with no repetitions. C will be sorted.
// Arguments    : const emxArray_real_T *a
//                const emxArray_real_T *b
//                emxArray_real_T *c
// Return Type  : void
//
void setdiff_simple(const emxArray_real_T *a, const emxArray_real_T *b,
                    emxArray_real_T *c)
{
  unsigned int uv2[2];
  int p;
  emxArray_boolean_T *logUA;
  int pEnd;
  int k;
  double x;
  int n;
  int i2;
  boolean_T exitg3;
  double absxk;
  int q;
  boolean_T b_p;
  boolean_T exitg2;
  int qEnd;
  int i;
  emxArray_real_T *b_a;
  emxArray_int32_T *loc;
  int na;
  emxArray_int32_T *iwork;
  int j;
  int kEnd;
  int32_T exitg1;
  int exponent;
  int i11;

  //  Call ISMEMBER to determine list of non-matching elements of A.
  for (p = 0; p < 2; p++) {
    uv2[p] = (unsigned int)a->size[p];
  }

  b_emxInit_boolean_T(&logUA, 2);
  p = logUA->size[0] * logUA->size[1];
  logUA->size[0] = 1;
  emxEnsureCapacity((emxArray__common *)logUA, p, (int)sizeof(boolean_T));
  p = logUA->size[0] * logUA->size[1];
  logUA->size[1] = (int)uv2[1];
  emxEnsureCapacity((emxArray__common *)logUA, p, (int)sizeof(boolean_T));
  pEnd = (int)uv2[1];
  for (p = 0; p < pEnd; p++) {
    logUA->data[p] = false;
  }

  for (k = 0; k + 1 <= a->size[1]; k++) {
    x = a->data[k];
    n = -2;
    i2 = 1;
    pEnd = b->size[1];
    exitg3 = false;
    while ((!exitg3) && (pEnd >= i2)) {
      p = ((i2 >> 1) + (pEnd >> 1)) - 1;
      if (((i2 & 1) == 1) && ((pEnd & 1) == 1)) {
        p++;
      }

      absxk = fabs(b->data[p] / 2.0);
      if ((!rtIsInf(absxk)) && (!rtIsNaN(absxk))) {
        if (absxk <= 2.2250738585072014E-308) {
          absxk = 4.94065645841247E-324;
        } else {
          frexp(absxk, &q);
          absxk = ldexp(1.0, q - 53);
        }
      } else {
        absxk = rtNaN;
      }

      if ((fabs(b->data[p] - x) < absxk) || (rtIsInf(x) && rtIsInf(b->data[p]) &&
           ((x > 0.0) == (b->data[p] > 0.0)))) {
        b_p = true;
      } else {
        b_p = false;
      }

      if (b_p) {
        n = p - 1;
        exitg3 = true;
      } else {
        if ((x < b->data[p]) || rtIsNaN(b->data[p])) {
          b_p = true;
        } else {
          b_p = false;
        }

        if (b_p) {
          pEnd = p;
        } else {
          i2 = p + 2;
        }
      }
    }

    if (n + 2 > 0) {
      exitg2 = false;
      while ((!exitg2) && (n + 1 > 0)) {
        absxk = fabs(b->data[n] / 2.0);
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

        if ((fabs(b->data[n] - x) < absxk) || (rtIsInf(x) && rtIsInf(b->data[n])
             && ((x > 0.0) == (b->data[n] > 0.0)))) {
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
      logUA->data[k] = true;
    }
  }

  p = logUA->size[0] * logUA->size[1];
  logUA->size[0] = 1;
  emxEnsureCapacity((emxArray__common *)logUA, p, (int)sizeof(boolean_T));
  pEnd = logUA->size[0];
  i2 = logUA->size[1];
  pEnd *= i2;
  for (p = 0; p < pEnd; p++) {
    logUA->data[p] = !logUA->data[p];
  }

  i2 = logUA->size[1] - 1;
  pEnd = 0;
  for (i = 0; i <= i2; i++) {
    if (logUA->data[i]) {
      pEnd++;
    }
  }

  p = c->size[0] * c->size[1];
  c->size[0] = 1;
  c->size[1] = pEnd;
  emxEnsureCapacity((emxArray__common *)c, p, (int)sizeof(double));
  pEnd = 0;
  for (i = 0; i <= i2; i++) {
    if (logUA->data[i]) {
      c->data[pEnd] = a->data[i];
      pEnd++;
    }
  }

  emxFree_boolean_T(&logUA);
  emxInit_real_T(&b_a, 2);

  //  Call UNIQUE to remove duplicates from list of non-matches.
  p = b_a->size[0] * b_a->size[1];
  b_a->size[0] = 1;
  b_a->size[1] = c->size[1];
  emxEnsureCapacity((emxArray__common *)b_a, p, (int)sizeof(double));
  pEnd = c->size[0] * c->size[1];
  for (p = 0; p < pEnd; p++) {
    b_a->data[p] = c->data[p];
  }

  emxInit_int32_T(&loc, 2);
  na = c->size[1];
  p = loc->size[0] * loc->size[1];
  loc->size[0] = 1;
  emxEnsureCapacity((emxArray__common *)loc, p, (int)sizeof(int));
  pEnd = c->size[1];
  p = loc->size[0] * loc->size[1];
  loc->size[1] = pEnd;
  emxEnsureCapacity((emxArray__common *)loc, p, (int)sizeof(int));
  pEnd = c->size[1];
  for (p = 0; p < pEnd; p++) {
    loc->data[p] = 0;
  }

  if (c->size[1] == 0) {
  } else {
    b_emxInit_int32_T(&iwork, 1);
    n = c->size[1] + 1;
    pEnd = c->size[1];
    p = iwork->size[0];
    iwork->size[0] = pEnd;
    emxEnsureCapacity((emxArray__common *)iwork, p, (int)sizeof(int));
    for (k = 1; k <= n - 2; k += 2) {
      if ((c->data[k - 1] <= c->data[k]) || rtIsNaN(c->data[k])) {
        b_p = true;
      } else {
        b_p = false;
      }

      if (b_p) {
        loc->data[k - 1] = k;
        loc->data[k] = k + 1;
      } else {
        loc->data[k - 1] = k + 1;
        loc->data[k] = k;
      }
    }

    if ((c->size[1] & 1) != 0) {
      loc->data[c->size[1] - 1] = c->size[1];
    }

    i = 2;
    while (i < n - 1) {
      i2 = i << 1;
      j = 1;
      for (pEnd = 1 + i; pEnd < n; pEnd = qEnd + i) {
        p = j;
        q = pEnd - 1;
        qEnd = j + i2;
        if (qEnd > n) {
          qEnd = n;
        }

        k = 0;
        kEnd = qEnd - j;
        while (k + 1 <= kEnd) {
          if ((c->data[loc->data[p - 1] - 1] <= c->data[loc->data[q] - 1]) ||
              rtIsNaN(c->data[loc->data[q] - 1])) {
            b_p = true;
          } else {
            b_p = false;
          }

          if (b_p) {
            iwork->data[k] = loc->data[p - 1];
            p++;
            if (p == pEnd) {
              while (q + 1 < qEnd) {
                k++;
                iwork->data[k] = loc->data[q];
                q++;
              }
            }
          } else {
            iwork->data[k] = loc->data[q];
            q++;
            if (q + 1 == qEnd) {
              while (p < pEnd) {
                k++;
                iwork->data[k] = loc->data[p - 1];
                p++;
              }
            }
          }

          k++;
        }

        for (k = 0; k + 1 <= kEnd; k++) {
          loc->data[(j + k) - 1] = iwork->data[k];
        }

        j = qEnd;
      }

      i = i2;
    }

    emxFree_int32_T(&iwork);
  }

  for (p = 0; p < 2; p++) {
    uv2[p] = (unsigned int)c->size[p];
  }

  p = c->size[0] * c->size[1];
  c->size[0] = 1;
  c->size[1] = (int)uv2[1];
  emxEnsureCapacity((emxArray__common *)c, p, (int)sizeof(double));
  for (k = 0; k + 1 <= na; k++) {
    c->data[k] = b_a->data[loc->data[k] - 1];
  }

  emxFree_real_T(&b_a);
  emxFree_int32_T(&loc);
  k = 0;
  while ((k + 1 <= na) && rtIsInf(c->data[k]) && (c->data[k] < 0.0)) {
    k++;
  }

  qEnd = k;
  k = na;
  while ((k >= 1) && rtIsNaN(c->data[k - 1])) {
    k--;
  }

  p = na - k;
  while ((k >= 1) && rtIsInf(c->data[k - 1]) && (c->data[k - 1] > 0.0)) {
    k--;
  }

  pEnd = (na - k) - p;
  q = -1;
  if (qEnd > 0) {
    q = 0;
  }

  i2 = (qEnd + k) - qEnd;
  while (qEnd + 1 <= i2) {
    x = c->data[qEnd];
    do {
      exitg1 = 0;
      qEnd++;
      if (qEnd + 1 > i2) {
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

        if ((fabs(x - c->data[qEnd]) < absxk) || (rtIsInf(c->data[qEnd]) &&
             rtIsInf(x) && ((c->data[qEnd] > 0.0) == (x > 0.0)))) {
          b_p = true;
        } else {
          b_p = false;
        }

        if (!b_p) {
          exitg1 = 1;
        }
      }
    } while (exitg1 == 0);

    q++;
    c->data[q] = x;
  }

  if (pEnd > 0) {
    q++;
    c->data[q] = c->data[i2];
  }

  qEnd = i2 + pEnd;
  for (j = 1; j <= p; j++) {
    q++;
    c->data[q] = c->data[(qEnd + j) - 1];
  }

  p = c->size[0] * c->size[1];
  if (1 > q + 1) {
    i11 = -1;
  } else {
    i11 = q;
  }

  c->size[1] = i11 + 1;
  emxEnsureCapacity((emxArray__common *)c, p, (int)sizeof(double));
}

//
// File trailer for setdiff_simple.cpp
//
// [EOF]
//
