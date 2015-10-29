//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
// File: classifyNotes_types.h
//
// MATLAB Coder version            : 2.8
// C/C++ source code generated on  : 26-Jun-2015 17:43:32
//
#ifndef __CLASSIFYNOTES_TYPES_H__
#define __CLASSIFYNOTES_TYPES_H__

// Include Files
#include "rtwtypes.h"

// Type Definitions
#ifndef struct_emxArray_real_T
#define struct_emxArray_real_T

struct emxArray_real_T
{
  double *data;
  int *size;
  int allocatedSize;
  int numDimensions;
  boolean_T canFreeData;
};

#endif                                 //struct_emxArray_real_T

#ifndef struct_emxArray_real_T_1x1
#define struct_emxArray_real_T_1x1

struct emxArray_real_T_1x1
{
  double data[1];
  int size[2];
};

#endif                                 //struct_emxArray_real_T_1x1

#ifndef struct_sYDpH6oRw1KZ3X8wjOfHS0E
#define struct_sYDpH6oRw1KZ3X8wjOfHS0E

struct sYDpH6oRw1KZ3X8wjOfHS0E
{
  double data[3];
  emxArray_real_T *tr_onsetsok;
  emxArray_real_T *tr_offsetsok;
  emxArray_real_T *ovlaptime;
  emxArray_real_T *ovlaptimepitch;
  emxArray_real_T *tr_split;
  emxArray_real_T_1x1 tr_merged;
};

#endif                                 //struct_sYDpH6oRw1KZ3X8wjOfHS0E

typedef sYDpH6oRw1KZ3X8wjOfHS0E b_struct_T;
typedef struct {
  double data[3];
} c_struct_T;

#ifndef struct_emxArray__common
#define struct_emxArray__common

struct emxArray__common
{
  void *data;
  int *size;
  int allocatedSize;
  int numDimensions;
  boolean_T canFreeData;
};

#endif                                 //struct_emxArray__common

#ifndef struct_emxArray_b_struct_T
#define struct_emxArray_b_struct_T

struct emxArray_b_struct_T
{
  b_struct_T *data;
  int *size;
  int allocatedSize;
  int numDimensions;
  boolean_T canFreeData;
};

#endif                                 //struct_emxArray_b_struct_T

#ifndef struct_emxArray_boolean_T
#define struct_emxArray_boolean_T

struct emxArray_boolean_T
{
  boolean_T *data;
  int *size;
  int allocatedSize;
  int numDimensions;
  boolean_T canFreeData;
};

#endif                                 //struct_emxArray_boolean_T

#ifndef struct_emxArray_c_struct_T
#define struct_emxArray_c_struct_T

struct emxArray_c_struct_T
{
  c_struct_T *data;
  int *size;
  int allocatedSize;
  int numDimensions;
  boolean_T canFreeData;
};

#endif                                 //struct_emxArray_c_struct_T

#ifndef struct_emxArray_d_struct_T
#define struct_emxArray_d_struct_T

struct emxArray_d_struct_T
{
  c_struct_T *data;
  int *size;
  int allocatedSize;
  int numDimensions;
  boolean_T canFreeData;
};

#endif                                 //struct_emxArray_d_struct_T

#ifndef struct_emxArray_int32_T
#define struct_emxArray_int32_T

struct emxArray_int32_T
{
  int *data;
  int *size;
  int allocatedSize;
  int numDimensions;
  boolean_T canFreeData;
};

#endif                                 //struct_emxArray_int32_T

#ifndef struct_sJ3O4L116kkWkF27lNXKBwH
#define struct_sJ3O4L116kkWkF27lNXKBwH

struct sJ3O4L116kkWkF27lNXKBwH
{
  double data[3];
  emxArray_real_T *gt_onsetsok;
  emxArray_real_T *gt_offsetsok;
  emxArray_real_T *ovlaptime;
  emxArray_real_T *ovlaptimepitch;
  emxArray_real_T_1x1 gt_split;
  emxArray_real_T *gt_merged;
};

#endif                                 //struct_sJ3O4L116kkWkF27lNXKBwH

typedef sJ3O4L116kkWkF27lNXKBwH struct_T;

#ifndef struct_emxArray_struct_T
#define struct_emxArray_struct_T

struct emxArray_struct_T
{
  struct_T *data;
  int *size;
  int allocatedSize;
  int numDimensions;
  boolean_T canFreeData;
};

#endif                                 //struct_emxArray_struct_T

typedef struct {
  double Dur_GT;
  double Dur_TR;
  double N_GT;
  double N_TR;
  emxArray_real_T *COnPOff_listgt;
  double COnPOff_Precision;
  double COnPOff_Recall;
  double COnPOff_Fmeasure;
  emxArray_real_T *COnOff_listgt;
  double COnOff_Precision;
  double COnOff_Recall;
  double COnOff_Fmeasure;
  emxArray_real_T *COnP_listgt;
  double COnP_Precision;
  double COnP_Recall;
  double COnP_Fmeasure;
  emxArray_real_T *COn_listgt;
  double COn_Precision;
  double COn_Recall;
  double COn_Fmeasure;
  emxArray_real_T *OBOn_listgt;
  double OBOn_rategt;
  emxArray_real_T *OBP_listgt;
  double OBP_rategt;
  emxArray_real_T *OBOff_listgt;
  double OBOff_rategt;
  emxArray_real_T *S_listgt;
  double S_rategt;
  double S_ratio;
  emxArray_real_T *M_listgt;
  double M_rategt;
  double M_ratio;
  emxArray_real_T *PU_listtr;
  double PU_ratetr;
  emxArray_real_T *ND_listgt;
  double ND_rategt;
} struct0_T;

#endif

//
// File trailer for classifyNotes_types.h
//
// [EOF]
//
