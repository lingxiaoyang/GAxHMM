//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
// File: classifyNotes_emxutil.cpp
//
// MATLAB Coder version            : 2.8
// C/C++ source code generated on  : 26-Jun-2015 17:43:32
//

// Include Files
#include "rt_nonfinite.h"
#include "classifyNotes.h"
#include "classifyNotes_emxutil.h"

// Function Declarations
static void b_emxExpand_struct_T(emxArray_struct_T *emxArray, int fromIndex, int
  toIndex);
static void b_emxTrim_struct_T(emxArray_struct_T *emxArray, int fromIndex, int
  toIndex);
static void emxCopyMatrix_real_T(double dst[3], const double src[3]);
static void emxCopy_real_T(emxArray_real_T **dst, emxArray_real_T * const *src);
static void emxCopy_real_T_1x1(emxArray_real_T_1x1 *dst, const
  emxArray_real_T_1x1 *src);
static void emxExpand_struct_T(emxArray_b_struct_T *emxArray, int fromIndex, int
  toIndex);
static void emxTrim_struct_T(emxArray_b_struct_T *emxArray, int fromIndex, int
  toIndex);

// Function Definitions

//
// Arguments    : emxArray_struct_T *emxArray
//                int fromIndex
//                int toIndex
// Return Type  : void
//
static void b_emxExpand_struct_T(emxArray_struct_T *emxArray, int fromIndex, int
  toIndex)
{
  int i;
  for (i = fromIndex; i < toIndex; i++) {
    emxInitStruct_struct_T(&emxArray->data[i]);
  }
}

//
// Arguments    : emxArray_struct_T *emxArray
//                int fromIndex
//                int toIndex
// Return Type  : void
//
static void b_emxTrim_struct_T(emxArray_struct_T *emxArray, int fromIndex, int
  toIndex)
{
  int i;
  for (i = fromIndex; i < toIndex; i++) {
    emxFreeStruct_struct_T(&emxArray->data[i]);
  }
}

//
// Arguments    : double dst[3]
//                const double src[3]
// Return Type  : void
//
static void emxCopyMatrix_real_T(double dst[3], const double src[3])
{
  int i;
  for (i = 0; i < 3; i++) {
    dst[i] = src[i];
  }
}

//
// Arguments    : emxArray_real_T **dst
//                emxArray_real_T * const *src
// Return Type  : void
//
static void emxCopy_real_T(emxArray_real_T **dst, emxArray_real_T * const *src)
{
  int numElDst;
  int numElSrc;
  int i;
  numElDst = 1;
  numElSrc = 1;
  for (i = 0; i < (*dst)->numDimensions; i++) {
    numElDst *= (*dst)->size[i];
    numElSrc *= (*src)->size[i];
  }

  for (i = 0; i < (*dst)->numDimensions; i++) {
    (*dst)->size[i] = (*src)->size[i];
  }

  emxEnsureCapacity((emxArray__common *)*dst, numElDst, (int)sizeof(double));
  for (i = 0; i < numElSrc; i++) {
    (*dst)->data[i] = (*src)->data[i];
  }
}

//
// Arguments    : emxArray_real_T_1x1 *dst
//                const emxArray_real_T_1x1 *src
// Return Type  : void
//
static void emxCopy_real_T_1x1(emxArray_real_T_1x1 *dst, const
  emxArray_real_T_1x1 *src)
{
  int numElSrc;
  int i;
  numElSrc = 1;
  for (i = 0; i < 2; i++) {
    numElSrc *= src->size[i];
  }

  for (i = 0; i < 2; i++) {
    dst->size[i] = src->size[i];
  }

  for (i = 0; i < numElSrc; i++) {
    dst->data[i] = src->data[i];
  }
}

//
// Arguments    : emxArray_b_struct_T *emxArray
//                int fromIndex
//                int toIndex
// Return Type  : void
//
static void emxExpand_struct_T(emxArray_b_struct_T *emxArray, int fromIndex, int
  toIndex)
{
  int i;
  for (i = fromIndex; i < toIndex; i++) {
    b_emxInitStruct_struct_T(&emxArray->data[i]);
  }
}

//
// Arguments    : emxArray_b_struct_T *emxArray
//                int fromIndex
//                int toIndex
// Return Type  : void
//
static void emxTrim_struct_T(emxArray_b_struct_T *emxArray, int fromIndex, int
  toIndex)
{
  int i;
  for (i = fromIndex; i < toIndex; i++) {
    b_emxFreeStruct_struct_T(&emxArray->data[i]);
  }
}

//
// Arguments    : b_struct_T *dst
//                const b_struct_T *src
// Return Type  : void
//
void b_emxCopyStruct_struct_T(b_struct_T *dst, const b_struct_T *src)
{
  emxCopyMatrix_real_T(dst->data, src->data);
  emxCopy_real_T(&dst->tr_onsetsok, &src->tr_onsetsok);
  emxCopy_real_T(&dst->tr_offsetsok, &src->tr_offsetsok);
  emxCopy_real_T(&dst->ovlaptime, &src->ovlaptime);
  emxCopy_real_T(&dst->ovlaptimepitch, &src->ovlaptimepitch);
  emxCopy_real_T(&dst->tr_split, &src->tr_split);
  emxCopy_real_T_1x1(&dst->tr_merged, &src->tr_merged);
}

//
// Arguments    : emxArray_struct_T *emxArray
//                int oldNumel
// Return Type  : void
//
void b_emxEnsureCapacity_struct_T(emxArray_struct_T *emxArray, int oldNumel)
{
  int elementSize;
  int newNumel;
  int i;
  void *newData;
  elementSize = (int)sizeof(struct_T);
  newNumel = 1;
  for (i = 0; i < emxArray->numDimensions; i++) {
    newNumel *= emxArray->size[i];
  }

  if (newNumel > emxArray->allocatedSize) {
    i = emxArray->allocatedSize;
    if (i < 16) {
      i = 16;
    }

    while (i < newNumel) {
      i <<= 1;
    }

    newData = calloc((unsigned int)i, (unsigned int)elementSize);
    if (emxArray->data != NULL) {
      memcpy(newData, (void *)emxArray->data, (unsigned int)(elementSize *
              oldNumel));
      if (emxArray->canFreeData) {
        free((void *)emxArray->data);
      }
    }

    emxArray->data = (struct_T *)newData;
    emxArray->allocatedSize = i;
    emxArray->canFreeData = true;
  }

  if (oldNumel > newNumel) {
    b_emxTrim_struct_T(emxArray, newNumel, oldNumel);
  } else {
    if (oldNumel < newNumel) {
      b_emxExpand_struct_T(emxArray, oldNumel, newNumel);
    }
  }
}

//
// Arguments    : b_struct_T *pStruct
// Return Type  : void
//
void b_emxFreeStruct_struct_T(b_struct_T *pStruct)
{
  emxFree_real_T(&pStruct->tr_onsetsok);
  emxFree_real_T(&pStruct->tr_offsetsok);
  emxFree_real_T(&pStruct->ovlaptime);
  emxFree_real_T(&pStruct->ovlaptimepitch);
  emxFree_real_T(&pStruct->tr_split);
}

//
// Arguments    : emxArray_b_struct_T **pEmxArray
// Return Type  : void
//
void b_emxFree_struct_T(emxArray_b_struct_T **pEmxArray)
{
  int numEl;
  int i;
  if (*pEmxArray != (emxArray_b_struct_T *)NULL) {
    if ((*pEmxArray)->data != (b_struct_T *)NULL) {
      numEl = 1;
      for (i = 0; i < (*pEmxArray)->numDimensions; i++) {
        numEl *= (*pEmxArray)->size[i];
      }

      for (i = 0; i < numEl; i++) {
        b_emxFreeStruct_struct_T(&(*pEmxArray)->data[i]);
      }

      if ((*pEmxArray)->canFreeData) {
        free((void *)(*pEmxArray)->data);
      }
    }

    free((void *)(*pEmxArray)->size);
    free((void *)*pEmxArray);
    *pEmxArray = (emxArray_b_struct_T *)NULL;
  }
}

//
// Arguments    : b_struct_T *pStruct
// Return Type  : void
//
void b_emxInitStruct_struct_T(b_struct_T *pStruct)
{
  emxInit_real_T(&pStruct->tr_onsetsok, 2);
  emxInit_real_T(&pStruct->tr_offsetsok, 2);
  emxInit_real_T(&pStruct->ovlaptime, 2);
  emxInit_real_T(&pStruct->ovlaptimepitch, 2);
  emxInit_real_T(&pStruct->tr_split, 2);
  pStruct->tr_merged.size[0] = 0;
  pStruct->tr_merged.size[1] = 0;
}

//
// Arguments    : emxArray_boolean_T **pEmxArray
//                int numDimensions
// Return Type  : void
//
void b_emxInit_boolean_T(emxArray_boolean_T **pEmxArray, int numDimensions)
{
  emxArray_boolean_T *emxArray;
  int i;
  *pEmxArray = (emxArray_boolean_T *)malloc(sizeof(emxArray_boolean_T));
  emxArray = *pEmxArray;
  emxArray->data = (boolean_T *)NULL;
  emxArray->numDimensions = numDimensions;
  emxArray->size = (int *)malloc((unsigned int)(sizeof(int) * numDimensions));
  emxArray->allocatedSize = 0;
  emxArray->canFreeData = true;
  for (i = 0; i < numDimensions; i++) {
    emxArray->size[i] = 0;
  }
}

//
// Arguments    : emxArray_int32_T **pEmxArray
//                int numDimensions
// Return Type  : void
//
void b_emxInit_int32_T(emxArray_int32_T **pEmxArray, int numDimensions)
{
  emxArray_int32_T *emxArray;
  int i;
  *pEmxArray = (emxArray_int32_T *)malloc(sizeof(emxArray_int32_T));
  emxArray = *pEmxArray;
  emxArray->data = (int *)NULL;
  emxArray->numDimensions = numDimensions;
  emxArray->size = (int *)malloc((unsigned int)(sizeof(int) * numDimensions));
  emxArray->allocatedSize = 0;
  emxArray->canFreeData = true;
  for (i = 0; i < numDimensions; i++) {
    emxArray->size[i] = 0;
  }
}

//
// Arguments    : emxArray_real_T **pEmxArray
//                int numDimensions
// Return Type  : void
//
void b_emxInit_real_T(emxArray_real_T **pEmxArray, int numDimensions)
{
  emxArray_real_T *emxArray;
  int i;
  *pEmxArray = (emxArray_real_T *)malloc(sizeof(emxArray_real_T));
  emxArray = *pEmxArray;
  emxArray->data = (double *)NULL;
  emxArray->numDimensions = numDimensions;
  emxArray->size = (int *)malloc((unsigned int)(sizeof(int) * numDimensions));
  emxArray->allocatedSize = 0;
  emxArray->canFreeData = true;
  for (i = 0; i < numDimensions; i++) {
    emxArray->size[i] = 0;
  }
}

//
// Arguments    : emxArray_b_struct_T **pEmxArray
//                int numDimensions
// Return Type  : void
//
void b_emxInit_struct_T(emxArray_b_struct_T **pEmxArray, int numDimensions)
{
  emxArray_b_struct_T *emxArray;
  int i;
  *pEmxArray = (emxArray_b_struct_T *)malloc(sizeof(emxArray_b_struct_T));
  emxArray = *pEmxArray;
  emxArray->data = (b_struct_T *)NULL;
  emxArray->numDimensions = numDimensions;
  emxArray->size = (int *)malloc((unsigned int)(sizeof(int) * numDimensions));
  emxArray->allocatedSize = 0;
  emxArray->canFreeData = true;
  for (i = 0; i < numDimensions; i++) {
    emxArray->size[i] = 0;
  }
}

//
// Arguments    : emxArray_c_struct_T **pEmxArray
// Return Type  : void
//
void c_emxFree_struct_T(emxArray_c_struct_T **pEmxArray)
{
  if (*pEmxArray != (emxArray_c_struct_T *)NULL) {
    if (((*pEmxArray)->data != (c_struct_T *)NULL) && (*pEmxArray)->canFreeData)
    {
      free((void *)(*pEmxArray)->data);
    }

    free((void *)(*pEmxArray)->size);
    free((void *)*pEmxArray);
    *pEmxArray = (emxArray_c_struct_T *)NULL;
  }
}

//
// Arguments    : emxArray_c_struct_T **pEmxArray
//                int numDimensions
// Return Type  : void
//
void c_emxInit_struct_T(emxArray_c_struct_T **pEmxArray, int numDimensions)
{
  emxArray_c_struct_T *emxArray;
  int i;
  *pEmxArray = (emxArray_c_struct_T *)malloc(sizeof(emxArray_c_struct_T));
  emxArray = *pEmxArray;
  emxArray->data = (c_struct_T *)NULL;
  emxArray->numDimensions = numDimensions;
  emxArray->size = (int *)malloc((unsigned int)(sizeof(int) * numDimensions));
  emxArray->allocatedSize = 0;
  emxArray->canFreeData = true;
  for (i = 0; i < numDimensions; i++) {
    emxArray->size[i] = 0;
  }
}

//
// Arguments    : emxArray_d_struct_T **pEmxArray
// Return Type  : void
//
void d_emxFree_struct_T(emxArray_d_struct_T **pEmxArray)
{
  if (*pEmxArray != (emxArray_d_struct_T *)NULL) {
    if (((*pEmxArray)->data != (c_struct_T *)NULL) && (*pEmxArray)->canFreeData)
    {
      free((void *)(*pEmxArray)->data);
    }

    free((void *)(*pEmxArray)->size);
    free((void *)*pEmxArray);
    *pEmxArray = (emxArray_d_struct_T *)NULL;
  }
}

//
// Arguments    : emxArray_d_struct_T **pEmxArray
//                int numDimensions
// Return Type  : void
//
void d_emxInit_struct_T(emxArray_d_struct_T **pEmxArray, int numDimensions)
{
  emxArray_d_struct_T *emxArray;
  int i;
  *pEmxArray = (emxArray_d_struct_T *)malloc(sizeof(emxArray_d_struct_T));
  emxArray = *pEmxArray;
  emxArray->data = (c_struct_T *)NULL;
  emxArray->numDimensions = numDimensions;
  emxArray->size = (int *)malloc((unsigned int)(sizeof(int) * numDimensions));
  emxArray->allocatedSize = 0;
  emxArray->canFreeData = true;
  for (i = 0; i < numDimensions; i++) {
    emxArray->size[i] = 0;
  }
}

//
// Arguments    : struct_T *dst
//                const struct_T *src
// Return Type  : void
//
void emxCopyStruct_struct_T(struct_T *dst, const struct_T *src)
{
  emxCopyMatrix_real_T(dst->data, src->data);
  emxCopy_real_T(&dst->gt_onsetsok, &src->gt_onsetsok);
  emxCopy_real_T(&dst->gt_offsetsok, &src->gt_offsetsok);
  emxCopy_real_T(&dst->ovlaptime, &src->ovlaptime);
  emxCopy_real_T(&dst->ovlaptimepitch, &src->ovlaptimepitch);
  emxCopy_real_T_1x1(&dst->gt_split, &src->gt_split);
  emxCopy_real_T(&dst->gt_merged, &src->gt_merged);
}

//
// Arguments    : emxArray__common *emxArray
//                int oldNumel
//                int elementSize
// Return Type  : void
//
void emxEnsureCapacity(emxArray__common *emxArray, int oldNumel, int elementSize)
{
  int newNumel;
  int i;
  void *newData;
  newNumel = 1;
  for (i = 0; i < emxArray->numDimensions; i++) {
    newNumel *= emxArray->size[i];
  }

  if (newNumel > emxArray->allocatedSize) {
    i = emxArray->allocatedSize;
    if (i < 16) {
      i = 16;
    }

    while (i < newNumel) {
      i <<= 1;
    }

    newData = calloc((unsigned int)i, (unsigned int)elementSize);
    if (emxArray->data != NULL) {
      memcpy(newData, emxArray->data, (unsigned int)(elementSize * oldNumel));
      if (emxArray->canFreeData) {
        free(emxArray->data);
      }
    }

    emxArray->data = newData;
    emxArray->allocatedSize = i;
    emxArray->canFreeData = true;
  }
}

//
// Arguments    : emxArray_b_struct_T *emxArray
//                int oldNumel
// Return Type  : void
//
void emxEnsureCapacity_struct_T(emxArray_b_struct_T *emxArray, int oldNumel)
{
  int elementSize;
  int newNumel;
  int i;
  void *newData;
  elementSize = (int)sizeof(b_struct_T);
  newNumel = 1;
  for (i = 0; i < emxArray->numDimensions; i++) {
    newNumel *= emxArray->size[i];
  }

  if (newNumel > emxArray->allocatedSize) {
    i = emxArray->allocatedSize;
    if (i < 16) {
      i = 16;
    }

    while (i < newNumel) {
      i <<= 1;
    }

    newData = calloc((unsigned int)i, (unsigned int)elementSize);
    if (emxArray->data != NULL) {
      memcpy(newData, (void *)emxArray->data, (unsigned int)(elementSize *
              oldNumel));
      if (emxArray->canFreeData) {
        free((void *)emxArray->data);
      }
    }

    emxArray->data = (b_struct_T *)newData;
    emxArray->allocatedSize = i;
    emxArray->canFreeData = true;
  }

  if (oldNumel > newNumel) {
    emxTrim_struct_T(emxArray, newNumel, oldNumel);
  } else {
    if (oldNumel < newNumel) {
      emxExpand_struct_T(emxArray, oldNumel, newNumel);
    }
  }
}

//
// Arguments    : struct0_T *pStruct
// Return Type  : void
//
void emxFreeStruct_struct0_T(struct0_T *pStruct)
{
  emxFree_real_T(&pStruct->COnPOff_listgt);
  emxFree_real_T(&pStruct->COnOff_listgt);
  emxFree_real_T(&pStruct->COnP_listgt);
  emxFree_real_T(&pStruct->COn_listgt);
  emxFree_real_T(&pStruct->OBOn_listgt);
  emxFree_real_T(&pStruct->OBP_listgt);
  emxFree_real_T(&pStruct->OBOff_listgt);
  emxFree_real_T(&pStruct->S_listgt);
  emxFree_real_T(&pStruct->M_listgt);
  emxFree_real_T(&pStruct->PU_listtr);
  emxFree_real_T(&pStruct->ND_listgt);
}

//
// Arguments    : struct_T *pStruct
// Return Type  : void
//
void emxFreeStruct_struct_T(struct_T *pStruct)
{
  emxFree_real_T(&pStruct->gt_onsetsok);
  emxFree_real_T(&pStruct->gt_offsetsok);
  emxFree_real_T(&pStruct->ovlaptime);
  emxFree_real_T(&pStruct->ovlaptimepitch);
  emxFree_real_T(&pStruct->gt_merged);
}

//
// Arguments    : emxArray_boolean_T **pEmxArray
// Return Type  : void
//
void emxFree_boolean_T(emxArray_boolean_T **pEmxArray)
{
  if (*pEmxArray != (emxArray_boolean_T *)NULL) {
    if (((*pEmxArray)->data != (boolean_T *)NULL) && (*pEmxArray)->canFreeData)
    {
      free((void *)(*pEmxArray)->data);
    }

    free((void *)(*pEmxArray)->size);
    free((void *)*pEmxArray);
    *pEmxArray = (emxArray_boolean_T *)NULL;
  }
}

//
// Arguments    : emxArray_int32_T **pEmxArray
// Return Type  : void
//
void emxFree_int32_T(emxArray_int32_T **pEmxArray)
{
  if (*pEmxArray != (emxArray_int32_T *)NULL) {
    if (((*pEmxArray)->data != (int *)NULL) && (*pEmxArray)->canFreeData) {
      free((void *)(*pEmxArray)->data);
    }

    free((void *)(*pEmxArray)->size);
    free((void *)*pEmxArray);
    *pEmxArray = (emxArray_int32_T *)NULL;
  }
}

//
// Arguments    : emxArray_real_T **pEmxArray
// Return Type  : void
//
void emxFree_real_T(emxArray_real_T **pEmxArray)
{
  if (*pEmxArray != (emxArray_real_T *)NULL) {
    if (((*pEmxArray)->data != (double *)NULL) && (*pEmxArray)->canFreeData) {
      free((void *)(*pEmxArray)->data);
    }

    free((void *)(*pEmxArray)->size);
    free((void *)*pEmxArray);
    *pEmxArray = (emxArray_real_T *)NULL;
  }
}

//
// Arguments    : emxArray_struct_T **pEmxArray
// Return Type  : void
//
void emxFree_struct_T(emxArray_struct_T **pEmxArray)
{
  int numEl;
  int i;
  if (*pEmxArray != (emxArray_struct_T *)NULL) {
    if ((*pEmxArray)->data != (struct_T *)NULL) {
      numEl = 1;
      for (i = 0; i < (*pEmxArray)->numDimensions; i++) {
        numEl *= (*pEmxArray)->size[i];
      }

      for (i = 0; i < numEl; i++) {
        emxFreeStruct_struct_T(&(*pEmxArray)->data[i]);
      }

      if ((*pEmxArray)->canFreeData) {
        free((void *)(*pEmxArray)->data);
      }
    }

    free((void *)(*pEmxArray)->size);
    free((void *)*pEmxArray);
    *pEmxArray = (emxArray_struct_T *)NULL;
  }
}

//
// Arguments    : struct0_T *pStruct
// Return Type  : void
//
void emxInitStruct_struct0_T(struct0_T *pStruct)
{
  emxInit_real_T(&pStruct->COnPOff_listgt, 2);
  emxInit_real_T(&pStruct->COnOff_listgt, 2);
  emxInit_real_T(&pStruct->COnP_listgt, 2);
  emxInit_real_T(&pStruct->COn_listgt, 2);
  emxInit_real_T(&pStruct->OBOn_listgt, 2);
  emxInit_real_T(&pStruct->OBP_listgt, 2);
  emxInit_real_T(&pStruct->OBOff_listgt, 2);
  emxInit_real_T(&pStruct->S_listgt, 2);
  emxInit_real_T(&pStruct->M_listgt, 2);
  emxInit_real_T(&pStruct->PU_listtr, 2);
  emxInit_real_T(&pStruct->ND_listgt, 2);
}

//
// Arguments    : struct_T *pStruct
// Return Type  : void
//
void emxInitStruct_struct_T(struct_T *pStruct)
{
  emxInit_real_T(&pStruct->gt_onsetsok, 2);
  emxInit_real_T(&pStruct->gt_offsetsok, 2);
  emxInit_real_T(&pStruct->ovlaptime, 2);
  emxInit_real_T(&pStruct->ovlaptimepitch, 2);
  pStruct->gt_split.size[0] = 0;
  pStruct->gt_split.size[1] = 0;
  emxInit_real_T(&pStruct->gt_merged, 2);
}

//
// Arguments    : emxArray_boolean_T **pEmxArray
//                int numDimensions
// Return Type  : void
//
void emxInit_boolean_T(emxArray_boolean_T **pEmxArray, int numDimensions)
{
  emxArray_boolean_T *emxArray;
  int i;
  *pEmxArray = (emxArray_boolean_T *)malloc(sizeof(emxArray_boolean_T));
  emxArray = *pEmxArray;
  emxArray->data = (boolean_T *)NULL;
  emxArray->numDimensions = numDimensions;
  emxArray->size = (int *)malloc((unsigned int)(sizeof(int) * numDimensions));
  emxArray->allocatedSize = 0;
  emxArray->canFreeData = true;
  for (i = 0; i < numDimensions; i++) {
    emxArray->size[i] = 0;
  }
}

//
// Arguments    : emxArray_int32_T **pEmxArray
//                int numDimensions
// Return Type  : void
//
void emxInit_int32_T(emxArray_int32_T **pEmxArray, int numDimensions)
{
  emxArray_int32_T *emxArray;
  int i;
  *pEmxArray = (emxArray_int32_T *)malloc(sizeof(emxArray_int32_T));
  emxArray = *pEmxArray;
  emxArray->data = (int *)NULL;
  emxArray->numDimensions = numDimensions;
  emxArray->size = (int *)malloc((unsigned int)(sizeof(int) * numDimensions));
  emxArray->allocatedSize = 0;
  emxArray->canFreeData = true;
  for (i = 0; i < numDimensions; i++) {
    emxArray->size[i] = 0;
  }
}

//
// Arguments    : emxArray_real_T **pEmxArray
//                int numDimensions
// Return Type  : void
//
void emxInit_real_T(emxArray_real_T **pEmxArray, int numDimensions)
{
  emxArray_real_T *emxArray;
  int i;
  *pEmxArray = (emxArray_real_T *)malloc(sizeof(emxArray_real_T));
  emxArray = *pEmxArray;
  emxArray->data = (double *)NULL;
  emxArray->numDimensions = numDimensions;
  emxArray->size = (int *)malloc((unsigned int)(sizeof(int) * numDimensions));
  emxArray->allocatedSize = 0;
  emxArray->canFreeData = true;
  for (i = 0; i < numDimensions; i++) {
    emxArray->size[i] = 0;
  }
}

//
// Arguments    : emxArray_struct_T **pEmxArray
//                int numDimensions
// Return Type  : void
//
void emxInit_struct_T(emxArray_struct_T **pEmxArray, int numDimensions)
{
  emxArray_struct_T *emxArray;
  int i;
  *pEmxArray = (emxArray_struct_T *)malloc(sizeof(emxArray_struct_T));
  emxArray = *pEmxArray;
  emxArray->data = (struct_T *)NULL;
  emxArray->numDimensions = numDimensions;
  emxArray->size = (int *)malloc((unsigned int)(sizeof(int) * numDimensions));
  emxArray->allocatedSize = 0;
  emxArray->canFreeData = true;
  for (i = 0; i < numDimensions; i++) {
    emxArray->size[i] = 0;
  }
}

//
// File trailer for classifyNotes_emxutil.cpp
//
// [EOF]
//
