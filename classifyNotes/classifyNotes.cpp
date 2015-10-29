//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
// File: classifyNotes.cpp
//
// MATLAB Coder version            : 2.8
// C/C++ source code generated on  : 26-Jun-2015 17:43:32
//

// Include Files
#include "rt_nonfinite.h"
#include "classifyNotes.h"
#include "classifyNotes_emxutil.h"
#include "sum.h"
#include "setdiff_simple.h"
#include "intersect_simple.h"
#include "repmat.h"
#include "foverlap_pitch.h"
#include "foverlap.h"
#include "normalization_factors.h"
#include "notes2matrixnotes.h"

// Function Declarations
static void b_cast(const emxArray_c_struct_T *x, emxArray_struct_T *y);
static void cast(const emxArray_d_struct_T *x, emxArray_b_struct_T *y);

// Function Definitions

//
// Arguments    : const emxArray_c_struct_T *x
//                emxArray_struct_T *y
// Return Type  : void
//
static void b_cast(const emxArray_c_struct_T *x, emxArray_struct_T *y)
{
  int i13;
  int j;
  i13 = y->size[0];
  y->size[0] = x->size[0];
  b_emxEnsureCapacity_struct_T(y, i13);
  for (j = 0; j < x->size[0]; j++) {
    for (i13 = 0; i13 < 3; i13++) {
      y->data[j].data[i13] = x->data[j].data[i13];
    }

    i13 = y->data[j].gt_onsetsok->size[0] * y->data[j].gt_onsetsok->size[1];
    y->data[j].gt_onsetsok->size[0] = 1;
    y->data[j].gt_onsetsok->size[1] = 0;
    emxEnsureCapacity((emxArray__common *)y->data[j].gt_onsetsok, i13, (int)
                      sizeof(double));
    i13 = y->data[j].gt_offsetsok->size[0] * y->data[j].gt_offsetsok->size[1];
    y->data[j].gt_offsetsok->size[0] = 1;
    y->data[j].gt_offsetsok->size[1] = 0;
    emxEnsureCapacity((emxArray__common *)y->data[j].gt_offsetsok, i13, (int)
                      sizeof(double));
    i13 = y->data[j].ovlaptime->size[0] * y->data[j].ovlaptime->size[1];
    y->data[j].ovlaptime->size[0] = 1;
    y->data[j].ovlaptime->size[1] = 0;
    emxEnsureCapacity((emxArray__common *)y->data[j].ovlaptime, i13, (int)sizeof
                      (double));
    i13 = y->data[j].ovlaptimepitch->size[0] * y->data[j].ovlaptimepitch->size[1];
    y->data[j].ovlaptimepitch->size[0] = 1;
    y->data[j].ovlaptimepitch->size[1] = 0;
    emxEnsureCapacity((emxArray__common *)y->data[j].ovlaptimepitch, i13, (int)
                      sizeof(double));
    y->data[j].gt_split.size[0] = 1;
    y->data[j].gt_split.size[1] = 0;
    i13 = y->data[j].gt_merged->size[0] * y->data[j].gt_merged->size[1];
    y->data[j].gt_merged->size[0] = 1;
    y->data[j].gt_merged->size[1] = 0;
    emxEnsureCapacity((emxArray__common *)y->data[j].gt_merged, i13, (int)sizeof
                      (double));
  }
}

//
// Arguments    : const emxArray_d_struct_T *x
//                emxArray_b_struct_T *y
// Return Type  : void
//
static void cast(const emxArray_d_struct_T *x, emxArray_b_struct_T *y)
{
  int i12;
  int j;
  i12 = y->size[0];
  y->size[0] = x->size[0];
  emxEnsureCapacity_struct_T(y, i12);
  for (j = 0; j < x->size[0]; j++) {
    for (i12 = 0; i12 < 3; i12++) {
      y->data[j].data[i12] = x->data[j].data[i12];
    }

    i12 = y->data[j].tr_onsetsok->size[0] * y->data[j].tr_onsetsok->size[1];
    y->data[j].tr_onsetsok->size[0] = 1;
    y->data[j].tr_onsetsok->size[1] = 0;
    emxEnsureCapacity((emxArray__common *)y->data[j].tr_onsetsok, i12, (int)
                      sizeof(double));
    i12 = y->data[j].tr_offsetsok->size[0] * y->data[j].tr_offsetsok->size[1];
    y->data[j].tr_offsetsok->size[0] = 1;
    y->data[j].tr_offsetsok->size[1] = 0;
    emxEnsureCapacity((emxArray__common *)y->data[j].tr_offsetsok, i12, (int)
                      sizeof(double));
    i12 = y->data[j].ovlaptime->size[0] * y->data[j].ovlaptime->size[1];
    y->data[j].ovlaptime->size[0] = 1;
    y->data[j].ovlaptime->size[1] = 0;
    emxEnsureCapacity((emxArray__common *)y->data[j].ovlaptime, i12, (int)sizeof
                      (double));
    i12 = y->data[j].ovlaptimepitch->size[0] * y->data[j].ovlaptimepitch->size[1];
    y->data[j].ovlaptimepitch->size[0] = 1;
    y->data[j].ovlaptimepitch->size[1] = 0;
    emxEnsureCapacity((emxArray__common *)y->data[j].ovlaptimepitch, i12, (int)
                      sizeof(double));
    i12 = y->data[j].tr_split->size[0] * y->data[j].tr_split->size[1];
    y->data[j].tr_split->size[0] = 1;
    y->data[j].tr_split->size[1] = 0;
    emxEnsureCapacity((emxArray__common *)y->data[j].tr_split, i12, (int)sizeof
                      (double));
    y->data[j].tr_merged.size[0] = 1;
    y->data[j].tr_merged.size[1] = 0;
  }
}

//
// onset_lim = 0.05; %secs % Default parameters (used in MIREX):
// dur_percent_range = 20; % percentage
// min_dur_dist = 0.05; %secs
// f0_range_in_cents = 50; %cents
// Arguments    : const emxArray_real_T *notes_gt
//                const emxArray_real_T *notes_tr
//                double onset_lim
//                double dur_percent_range
//                double min_dur_dist
//                double f0_range_in_cents
//                double hopsize
//                struct0_T *Results
// Return Type  : void
//
void classifyNotes(const emxArray_real_T *notes_gt, const emxArray_real_T
                   *notes_tr, double onset_lim, double dur_percent_range, double
                   min_dur_dist, double f0_range_in_cents, double hopsize,
                   struct0_T *Results)
{
  emxArray_real_T *b_notes_tr;
  int i0;
  int ndbl;
  emxArray_real_T *Moverlapped_pitch;
  emxArray_real_T *b_notes_gt;
  emxArray_real_T *M_refg;
  int sizeMax;
  emxArray_real_T *L_g;
  int cdiff;
  emxArray_real_T *M_t;
  int nb;
  int i;
  emxArray_real_T *M_g;
  emxArray_real_T *L_t;
  emxArray_real_T *Moverlapped;
  emxArray_struct_T *sNote_tr;
  emxArray_c_struct_T *r0;
  int b_i;
  emxArray_int32_T *iwork;
  emxArray_boolean_T *x;
  struct_T b_sNote_tr;
  struct_T c_sNote_tr;
  struct_T d_sNote_tr;
  struct_T e_sNote_tr;
  struct_T f_sNote_tr;
  struct_T g_sNote_tr;
  struct_T h_sNote_tr;
  struct_T i_sNote_tr;
  boolean_T exitg13;
  boolean_T guard6 = false;
  boolean_T exitg12;
  boolean_T guard5 = false;
  emxArray_b_struct_T *sNote_gt;
  emxArray_d_struct_T *r1;
  emxArray_int32_T *idx;
  emxArray_boolean_T *b_x;
  b_struct_T b_sNote_gt;
  b_struct_T c_sNote_gt;
  b_struct_T d_sNote_gt;
  b_struct_T e_sNote_gt;
  b_struct_T f_sNote_gt;
  b_struct_T g_sNote_gt;
  b_struct_T h_sNote_gt;
  b_struct_T i_sNote_gt;
  b_struct_T j_sNote_gt;
  b_struct_T k_sNote_gt;
  b_struct_T l_sNote_gt;
  b_struct_T m_sNote_gt;
  boolean_T exitg11;
  boolean_T guard4 = false;
  boolean_T exitg10;
  boolean_T guard3 = false;
  emxArray_real_T *r2;
  emxArray_real_T *r3;
  int j;
  int i1;
  emxArray_real_T *r4;
  emxArray_real_T *r5;
  double offset;
  double y;
  double durrange;
  int k;
  unsigned int uv0[2];
  int m;
  int c;
  int cr;
  int ic;
  int br;
  int ar;
  int ib;
  int ia;
  emxArray_real_T *Fnotes_tr_split;
  emxArray_real_T *b_M_refg;
  b_struct_T n_sNote_gt;
  b_struct_T o_sNote_gt;
  struct_T j_sNote_tr;
  struct_T k_sNote_tr;
  double nflag;
  boolean_T exitg9;
  boolean_T guard2 = false;
  struct_T l_sNote_tr;
  struct_T m_sNote_tr;
  b_struct_T p_sNote_gt;
  b_struct_T q_sNote_gt;
  boolean_T exitg8;
  boolean_T guard1 = false;
  emxArray_real_T *notes_gt_111;
  emxArray_real_T *notes_gt_100;
  emxArray_real_T *notes_gt_011;
  emxArray_real_T *notes_gt_101;
  emxArray_real_T *notes_gt_110;
  emxArray_real_T *Fnotes_gt_split;
  emxArray_real_T *Fnotes_gt_merged;
  emxArray_real_T *b_Fnotes_gt_merged;
  emxArray_real_T *c_Fnotes_gt_merged;
  emxArray_real_T *d_Fnotes_gt_merged;
  emxArray_real_T *e_Fnotes_gt_merged;
  int n;
  int apnd;
  int absb;
  int nm1d2;
  static const signed char C[15] = { 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 0
  };

  int na;
  boolean_T p;
  int i2;
  int pEnd;
  int b_p;
  int q;
  int qEnd;
  int kEnd;
  int b_k;
  int nNaN;
  int nInf;
  int b_nb;
  int khi;
  double c_x;
  int32_T exitg7;
  double absxk;
  double r;
  int exponent;
  int b_i2;
  emxArray_real_T *notes_gt_011b;
  emxArray_real_T *notes_gt_101b;
  emxArray_real_T *notes_gt_110b;
  emxArray_real_T *Fnotes_tr_merged;
  emxArray_real_T *Fnotes_tr_detected;
  emxArray_real_T *Fnotes_gt_detected;
  emxArray_real_T *r6;
  emxArray_real_T *r7;
  emxArray_real_T *S_listgt;
  int32_T exitg6;
  int b_exponent;
  int32_T exitg5;
  int c_exponent;
  emxArray_real_T *M_listgt;
  int32_T exitg4;
  int d_exponent;
  int32_T exitg3;
  int e_exponent;
  int32_T exitg2;
  int f_exponent;
  int i3;
  int32_T exitg1;
  int g_exponent;
  int i4;
  int i5;
  emxInit_real_T(&b_notes_tr, 2);

  // hopsize=0.01;
  //  Author: Emilio Molina (emm@ic.uma.es)
  //  23/09/2014
  //  In case you use this software tool, please cite the following paper:
  //  [1] Molina, E., Barbancho A. M., Tardon, L. J., Barbancho, I., "Evaluation 
  //  framework for automatic singing transcription", Proceedings of ISMIR 2014
  //
  //  Please, refer to the README.txt for more information about the license
  //  issues of this software tool.
  //  ----------------------------------------------------------------------
  //
  //  Results = classifyNotes(fileTranscription,fileGroundTruth) return a set
  //  of evaluation measures (within a struct variable) that represents the
  //  transcription performance of fileTranscription (MIDI or ASCII-formatted
  //  file) with respect to fileGroundTruth.
  //
  //  -- INPUTS --------------------------------
  //  Both fileTranscription and fileGroundTruth are monophonic melodies. Two
  //  formats are accepted: (1) monophonic, one track MIDI file or (2)
  //  ASCII-formatted in three columns as follows:
  //
  //           Onset (seconds) - Offset (seconds) - Pitch (MIDI number)
  //
  //  Note that, is ASCII-formatted files, the 3rd columns 'Pitch' may contain
  //  non-integer values.
  //
  //  -- OUTPUT -------------------------------
  //  The ouput Results is a struct containing all the evaluation measures
  //  described in [1]:
  //
  //  Results.Dur_GT --> Duration of fileGroundTruth
  //  Results.Dur_TR --> Duration of fileTranscription
  //  Results.N_GT --> No. of notes in fileGroundTruth
  //  Results.N_TR --> No. of notes in fileTranscription
  //    COnPOff: Correct Onset, Pitch & Offset
  //  Results.COnPOff_listgt
  //  Results.COnPOff_Precision
  //  Results.COnPOff_Recall
  //  Results.COnPOff_Fmeasure
  //    COnP: Correct Onset, Pitch
  //  Results.COnP_listgt
  //  Results.COnP_Precision
  //  Results.COnP_Recall
  //  Results.COnP_Fmeasure
  //    COn: Correct Onset
  //  Results.COn_listgt
  //  Results.COn_Precision
  //  Results.COn_Recall
  //  Results.COn_Fmeasure
  //    OBOn: Only Bad Onset (i.e. Correct Pitch&Offset, Wrong Onset)
  //  Results.OBOn_listgt
  //  Results.OBOn_rategt
  //    OBP: Only Bad Pitch (i.e. Correct Onset&Offset, Wrong Pitch)
  //  Results.OBP_listgt
  //  Results.OBP_rategt
  //    OBOff: Only Bad Offset (i.e. Correct Onset&Pitch, Wrong Offset)
  //  Results.OBOff_listgt
  //  Results.OBOff_rategt
  //    S: Split
  //  Results.S_listgt
  //  Results.S_rategt
  //  Results.S_ratio
  //    M: Merged
  //  Results.M_listgt
  //  Results.M_rategt
  //  Results.M_ratio
  //    PU: Spurious
  //  Results.PU_listtr
  //  Results.PU_ratetr
  //    ND: Non-Detected
  //  Results.ND_listgt
  //  Results.ND_rategt
  //
  //  Please, refer to evaluation.m in order to analyse a complete dataset.
  i0 = b_notes_tr->size[0] * b_notes_tr->size[1];
  b_notes_tr->size[0] = notes_tr->size[0];
  b_notes_tr->size[1] = 3;
  emxEnsureCapacity((emxArray__common *)b_notes_tr, i0, (int)sizeof(double));
  ndbl = notes_tr->size[0] * notes_tr->size[1];
  for (i0 = 0; i0 < ndbl; i0++) {
    b_notes_tr->data[i0] = notes_tr->data[i0];
  }

  emxInit_real_T(&Moverlapped_pitch, 2);
  emxInit_real_T(&b_notes_gt, 2);
  notes2matrixnotes(b_notes_tr, hopsize, Moverlapped_pitch);
  i0 = b_notes_gt->size[0] * b_notes_gt->size[1];
  b_notes_gt->size[0] = notes_gt->size[0];
  b_notes_gt->size[1] = 3;
  emxEnsureCapacity((emxArray__common *)b_notes_gt, i0, (int)sizeof(double));
  ndbl = notes_gt->size[0] * notes_gt->size[1];
  for (i0 = 0; i0 < ndbl; i0++) {
    b_notes_gt->data[i0] = notes_gt->data[i0];
  }

  emxInit_real_T(&M_refg, 2);
  notes2matrixnotes(b_notes_gt, hopsize, M_refg);
  if (Moverlapped_pitch->size[1] >= M_refg->size[1]) {
    sizeMax = Moverlapped_pitch->size[1];
  } else {
    sizeMax = M_refg->size[1];
  }

  emxInit_real_T(&L_g, 2);
  cdiff = Moverlapped_pitch->size[0];
  i0 = L_g->size[0] * L_g->size[1];
  L_g->size[0] = cdiff;
  emxEnsureCapacity((emxArray__common *)L_g, i0, (int)sizeof(double));
  cdiff = sizeMax - Moverlapped_pitch->size[1];
  i0 = L_g->size[0] * L_g->size[1];
  L_g->size[1] = cdiff;
  emxEnsureCapacity((emxArray__common *)L_g, i0, (int)sizeof(double));
  ndbl = Moverlapped_pitch->size[0] * (sizeMax - Moverlapped_pitch->size[1]);
  for (i0 = 0; i0 < ndbl; i0++) {
    L_g->data[i0] = 0.0;
  }

  emxInit_real_T(&M_t, 2);
  i0 = M_t->size[0] * M_t->size[1];
  M_t->size[0] = Moverlapped_pitch->size[0];
  M_t->size[1] = Moverlapped_pitch->size[1] + L_g->size[1];
  emxEnsureCapacity((emxArray__common *)M_t, i0, (int)sizeof(double));
  ndbl = Moverlapped_pitch->size[1];
  for (i0 = 0; i0 < ndbl; i0++) {
    nb = Moverlapped_pitch->size[0];
    for (i = 0; i < nb; i++) {
      M_t->data[i + M_t->size[0] * i0] = Moverlapped_pitch->data[i +
        Moverlapped_pitch->size[0] * i0];
    }
  }

  ndbl = L_g->size[1];
  for (i0 = 0; i0 < ndbl; i0++) {
    nb = L_g->size[0];
    for (i = 0; i < nb; i++) {
      M_t->data[i + M_t->size[0] * (i0 + Moverlapped_pitch->size[1])] =
        L_g->data[i + L_g->size[0] * i0];
    }
  }

  cdiff = M_refg->size[0];
  i0 = L_g->size[0] * L_g->size[1];
  L_g->size[0] = cdiff;
  emxEnsureCapacity((emxArray__common *)L_g, i0, (int)sizeof(double));
  cdiff = sizeMax - M_refg->size[1];
  i0 = L_g->size[0] * L_g->size[1];
  L_g->size[1] = cdiff;
  emxEnsureCapacity((emxArray__common *)L_g, i0, (int)sizeof(double));
  ndbl = M_refg->size[0] * (sizeMax - M_refg->size[1]);
  for (i0 = 0; i0 < ndbl; i0++) {
    L_g->data[i0] = 0.0;
  }

  emxInit_real_T(&M_g, 2);
  i0 = M_g->size[0] * M_g->size[1];
  M_g->size[0] = M_refg->size[0];
  M_g->size[1] = M_refg->size[1] + L_g->size[1];
  emxEnsureCapacity((emxArray__common *)M_g, i0, (int)sizeof(double));
  ndbl = M_refg->size[1];
  for (i0 = 0; i0 < ndbl; i0++) {
    nb = M_refg->size[0];
    for (i = 0; i < nb; i++) {
      M_g->data[i + M_g->size[0] * i0] = M_refg->data[i + M_refg->size[0] * i0];
    }
  }

  ndbl = L_g->size[1];
  for (i0 = 0; i0 < ndbl; i0++) {
    nb = L_g->size[0];
    for (i = 0; i < nb; i++) {
      M_g->data[i + M_g->size[0] * (i0 + M_refg->size[1])] = L_g->data[i +
        L_g->size[0] * i0];
    }
  }

  emxInit_real_T(&L_t, 2);
  emxInit_real_T(&Moverlapped, 2);
  emxInit_struct_T(&sNote_tr, 1);
  c_emxInit_struct_T(&r0, 1);
  normalization_factors(M_g, L_g);

  // Normalize to duration of gt notes
  normalization_factors(M_t, L_t);

  // Normalize to duration of transcribed notes
  foverlap(M_g, M_t, Moverlapped);

  // Find which notes overlap
  foverlap_pitch(M_g, M_t, f0_range_in_cents, Moverlapped_pitch);

  // Find which notes overlap
  repmat((double)b_notes_tr->size[0], r0);
  b_cast(r0, sNote_tr);
  b_i = 0;
  c_emxFree_struct_T(&r0);
  emxFree_real_T(&M_g);
  emxFree_real_T(&M_t);
  b_emxInit_int32_T(&iwork, 1);
  emxInit_boolean_T(&x, 1);
  emxInitStruct_struct_T(&b_sNote_tr);
  emxInitStruct_struct_T(&c_sNote_tr);
  emxInitStruct_struct_T(&d_sNote_tr);
  emxInitStruct_struct_T(&e_sNote_tr);
  emxInitStruct_struct_T(&f_sNote_tr);
  emxInitStruct_struct_T(&g_sNote_tr);
  emxInitStruct_struct_T(&h_sNote_tr);
  emxInitStruct_struct_T(&i_sNote_tr);
  while (b_i <= b_notes_tr->size[0] - 1) {
    for (i0 = 0; i0 < 3; i0++) {
      sNote_tr->data[b_i].data[i0] = b_notes_tr->data[b_i + b_notes_tr->size[0] *
        i0];
    }

    i0 = sNote_tr->data[b_i].gt_onsetsok->size[0] * sNote_tr->data[b_i].
      gt_onsetsok->size[1];
    sNote_tr->data[b_i].gt_onsetsok->size[0] = 1;
    sNote_tr->data[b_i].gt_onsetsok->size[1] = 0;
    emxEnsureCapacity((emxArray__common *)sNote_tr->data[b_i].gt_onsetsok, i0,
                      (int)sizeof(double));
    emxCopyStruct_struct_T(&b_sNote_tr, &sNote_tr->data[b_i]);
    emxCopyStruct_struct_T(&c_sNote_tr, &sNote_tr->data[b_i]);
    i0 = sNote_tr->data[b_i].gt_offsetsok->size[0] * sNote_tr->data[b_i].
      gt_offsetsok->size[1];
    sNote_tr->data[b_i].gt_offsetsok->size[0] = 1;
    sNote_tr->data[b_i].gt_offsetsok->size[1] = 0;
    emxEnsureCapacity((emxArray__common *)sNote_tr->data[b_i].gt_offsetsok, i0,
                      (int)sizeof(double));
    emxCopyStruct_struct_T(&d_sNote_tr, &sNote_tr->data[b_i]);
    emxCopyStruct_struct_T(&e_sNote_tr, &sNote_tr->data[b_i]);
    ndbl = Moverlapped->size[0];
    i0 = x->size[0];
    x->size[0] = ndbl;
    emxEnsureCapacity((emxArray__common *)x, i0, (int)sizeof(boolean_T));
    for (i0 = 0; i0 < ndbl; i0++) {
      x->data[i0] = (Moverlapped->data[i0 + Moverlapped->size[0] * b_i] > 0.0);
    }

    nb = x->size[0];
    ndbl = 0;
    i0 = iwork->size[0];
    iwork->size[0] = x->size[0];
    emxEnsureCapacity((emxArray__common *)iwork, i0, (int)sizeof(int));
    cdiff = 1;
    exitg13 = false;
    while ((!exitg13) && (cdiff <= nb)) {
      guard6 = false;
      if (x->data[cdiff - 1]) {
        ndbl++;
        iwork->data[ndbl - 1] = cdiff;
        if (ndbl >= nb) {
          exitg13 = true;
        } else {
          guard6 = true;
        }
      } else {
        guard6 = true;
      }

      if (guard6) {
        cdiff++;
      }
    }

    if (x->size[0] == 1) {
      if (ndbl == 0) {
        i0 = iwork->size[0];
        iwork->size[0] = 0;
        emxEnsureCapacity((emxArray__common *)iwork, i0, (int)sizeof(int));
      }
    } else {
      i0 = iwork->size[0];
      if (1 > ndbl) {
        iwork->size[0] = 0;
      } else {
        iwork->size[0] = ndbl;
      }

      emxEnsureCapacity((emxArray__common *)iwork, i0, (int)sizeof(int));
    }

    i0 = sNote_tr->data[b_i].ovlaptime->size[0] * sNote_tr->data[b_i].
      ovlaptime->size[1];
    sNote_tr->data[b_i].ovlaptime->size[0] = 1;
    sNote_tr->data[b_i].ovlaptime->size[1] = iwork->size[0];
    emxEnsureCapacity((emxArray__common *)sNote_tr->data[b_i].ovlaptime, i0,
                      (int)sizeof(double));
    ndbl = iwork->size[0];
    for (i0 = 0; i0 < ndbl; i0++) {
      sNote_tr->data[b_i].ovlaptime->data[sNote_tr->data[b_i].ovlaptime->size[0]
        * i0] = iwork->data[i0];
    }

    ndbl = Moverlapped_pitch->size[0];
    i0 = x->size[0];
    x->size[0] = ndbl;
    emxEnsureCapacity((emxArray__common *)x, i0, (int)sizeof(boolean_T));
    for (i0 = 0; i0 < ndbl; i0++) {
      x->data[i0] = (Moverlapped_pitch->data[i0 + Moverlapped_pitch->size[0] *
                     b_i] > 0.0);
    }

    nb = x->size[0];
    ndbl = 0;
    i0 = iwork->size[0];
    iwork->size[0] = x->size[0];
    emxEnsureCapacity((emxArray__common *)iwork, i0, (int)sizeof(int));
    cdiff = 1;
    exitg12 = false;
    while ((!exitg12) && (cdiff <= nb)) {
      guard5 = false;
      if (x->data[cdiff - 1]) {
        ndbl++;
        iwork->data[ndbl - 1] = cdiff;
        if (ndbl >= nb) {
          exitg12 = true;
        } else {
          guard5 = true;
        }
      } else {
        guard5 = true;
      }

      if (guard5) {
        cdiff++;
      }
    }

    if (x->size[0] == 1) {
      if (ndbl == 0) {
        i0 = iwork->size[0];
        iwork->size[0] = 0;
        emxEnsureCapacity((emxArray__common *)iwork, i0, (int)sizeof(int));
      }
    } else {
      i0 = iwork->size[0];
      if (1 > ndbl) {
        iwork->size[0] = 0;
      } else {
        iwork->size[0] = ndbl;
      }

      emxEnsureCapacity((emxArray__common *)iwork, i0, (int)sizeof(int));
    }

    i0 = sNote_tr->data[b_i].ovlaptimepitch->size[0] * sNote_tr->data[b_i].
      ovlaptimepitch->size[1];
    sNote_tr->data[b_i].ovlaptimepitch->size[0] = 1;
    sNote_tr->data[b_i].ovlaptimepitch->size[1] = iwork->size[0];
    emxEnsureCapacity((emxArray__common *)sNote_tr->data[b_i].ovlaptimepitch, i0,
                      (int)sizeof(double));
    ndbl = iwork->size[0];
    for (i0 = 0; i0 < ndbl; i0++) {
      sNote_tr->data[b_i].ovlaptimepitch->data[sNote_tr->data[b_i].
        ovlaptimepitch->size[0] * i0] = iwork->data[i0];
    }

    sNote_tr->data[b_i].gt_split.size[0] = 1;
    sNote_tr->data[b_i].gt_split.size[1] = 0;
    emxCopyStruct_struct_T(&f_sNote_tr, &sNote_tr->data[b_i]);
    emxCopyStruct_struct_T(&g_sNote_tr, &sNote_tr->data[b_i]);
    i0 = sNote_tr->data[b_i].gt_merged->size[0] * sNote_tr->data[b_i].
      gt_merged->size[1];
    sNote_tr->data[b_i].gt_merged->size[0] = 1;
    sNote_tr->data[b_i].gt_merged->size[1] = 0;
    emxEnsureCapacity((emxArray__common *)sNote_tr->data[b_i].gt_merged, i0,
                      (int)sizeof(double));
    emxCopyStruct_struct_T(&h_sNote_tr, &sNote_tr->data[b_i]);
    emxCopyStruct_struct_T(&i_sNote_tr, &sNote_tr->data[b_i]);
    b_i++;
  }

  emxFreeStruct_struct_T(&i_sNote_tr);
  emxFreeStruct_struct_T(&h_sNote_tr);
  emxFreeStruct_struct_T(&g_sNote_tr);
  emxFreeStruct_struct_T(&f_sNote_tr);
  emxFreeStruct_struct_T(&e_sNote_tr);
  emxFreeStruct_struct_T(&d_sNote_tr);
  emxFreeStruct_struct_T(&c_sNote_tr);
  emxFreeStruct_struct_T(&b_sNote_tr);
  b_emxInit_struct_T(&sNote_gt, 1);
  d_emxInit_struct_T(&r1, 1);
  b_repmat((double)b_notes_gt->size[0], r1);
  cast(r1, sNote_gt);
  b_i = 0;
  d_emxFree_struct_T(&r1);
  emxInit_int32_T(&idx, 2);
  b_emxInit_boolean_T(&b_x, 2);
  b_emxInitStruct_struct_T(&b_sNote_gt);
  b_emxInitStruct_struct_T(&c_sNote_gt);
  b_emxInitStruct_struct_T(&d_sNote_gt);
  b_emxInitStruct_struct_T(&e_sNote_gt);
  b_emxInitStruct_struct_T(&f_sNote_gt);
  b_emxInitStruct_struct_T(&g_sNote_gt);
  b_emxInitStruct_struct_T(&h_sNote_gt);
  b_emxInitStruct_struct_T(&i_sNote_gt);
  b_emxInitStruct_struct_T(&j_sNote_gt);
  b_emxInitStruct_struct_T(&k_sNote_gt);
  b_emxInitStruct_struct_T(&l_sNote_gt);
  b_emxInitStruct_struct_T(&m_sNote_gt);
  while (b_i <= b_notes_gt->size[0] - 1) {
    for (i0 = 0; i0 < 3; i0++) {
      sNote_gt->data[b_i].data[i0] = b_notes_gt->data[b_i + b_notes_gt->size[0] *
        i0];
    }

    i0 = sNote_gt->data[b_i].tr_onsetsok->size[0] * sNote_gt->data[b_i].
      tr_onsetsok->size[1];
    sNote_gt->data[b_i].tr_onsetsok->size[0] = 1;
    sNote_gt->data[b_i].tr_onsetsok->size[1] = 0;
    emxEnsureCapacity((emxArray__common *)sNote_gt->data[b_i].tr_onsetsok, i0,
                      (int)sizeof(double));
    b_emxCopyStruct_struct_T(&b_sNote_gt, &sNote_gt->data[b_i]);
    b_emxCopyStruct_struct_T(&c_sNote_gt, &sNote_gt->data[b_i]);
    i0 = sNote_gt->data[b_i].tr_offsetsok->size[0] * sNote_gt->data[b_i].
      tr_offsetsok->size[1];
    sNote_gt->data[b_i].tr_offsetsok->size[0] = 1;
    sNote_gt->data[b_i].tr_offsetsok->size[1] = 0;
    emxEnsureCapacity((emxArray__common *)sNote_gt->data[b_i].tr_offsetsok, i0,
                      (int)sizeof(double));
    b_emxCopyStruct_struct_T(&d_sNote_gt, &sNote_gt->data[b_i]);
    b_emxCopyStruct_struct_T(&e_sNote_gt, &sNote_gt->data[b_i]);
    ndbl = Moverlapped->size[1];
    i0 = b_x->size[0] * b_x->size[1];
    b_x->size[0] = 1;
    b_x->size[1] = ndbl;
    emxEnsureCapacity((emxArray__common *)b_x, i0, (int)sizeof(boolean_T));
    for (i0 = 0; i0 < ndbl; i0++) {
      b_x->data[b_x->size[0] * i0] = (Moverlapped->data[b_i + Moverlapped->size
        [0] * i0] > 0.0);
    }

    nb = b_x->size[1];
    ndbl = 0;
    i0 = idx->size[0] * idx->size[1];
    idx->size[0] = 1;
    idx->size[1] = b_x->size[1];
    emxEnsureCapacity((emxArray__common *)idx, i0, (int)sizeof(int));
    cdiff = 1;
    exitg11 = false;
    while ((!exitg11) && (cdiff <= nb)) {
      guard4 = false;
      if (b_x->data[cdiff - 1]) {
        ndbl++;
        idx->data[ndbl - 1] = cdiff;
        if (ndbl >= nb) {
          exitg11 = true;
        } else {
          guard4 = true;
        }
      } else {
        guard4 = true;
      }

      if (guard4) {
        cdiff++;
      }
    }

    if (b_x->size[1] == 1) {
      if (ndbl == 0) {
        i0 = idx->size[0] * idx->size[1];
        idx->size[0] = 1;
        idx->size[1] = 0;
        emxEnsureCapacity((emxArray__common *)idx, i0, (int)sizeof(int));
      }
    } else {
      i0 = idx->size[0] * idx->size[1];
      if (1 > ndbl) {
        idx->size[1] = 0;
      } else {
        idx->size[1] = ndbl;
      }

      emxEnsureCapacity((emxArray__common *)idx, i0, (int)sizeof(int));
    }

    i0 = sNote_gt->data[b_i].ovlaptime->size[0] * sNote_gt->data[b_i].
      ovlaptime->size[1];
    sNote_gt->data[b_i].ovlaptime->size[0] = 1;
    sNote_gt->data[b_i].ovlaptime->size[1] = idx->size[1];
    emxEnsureCapacity((emxArray__common *)sNote_gt->data[b_i].ovlaptime, i0,
                      (int)sizeof(double));
    b_emxCopyStruct_struct_T(&f_sNote_gt, &sNote_gt->data[b_i]);
    b_emxCopyStruct_struct_T(&g_sNote_gt, &sNote_gt->data[b_i]);
    ndbl = idx->size[0] * idx->size[1];
    for (i0 = 0; i0 < ndbl; i0++) {
      sNote_gt->data[b_i].ovlaptime->data[i0] = idx->data[i0];
    }

    ndbl = Moverlapped_pitch->size[1];
    i0 = b_x->size[0] * b_x->size[1];
    b_x->size[0] = 1;
    b_x->size[1] = ndbl;
    emxEnsureCapacity((emxArray__common *)b_x, i0, (int)sizeof(boolean_T));
    for (i0 = 0; i0 < ndbl; i0++) {
      b_x->data[b_x->size[0] * i0] = (Moverlapped_pitch->data[b_i +
        Moverlapped_pitch->size[0] * i0] > 0.0);
    }

    nb = b_x->size[1];
    ndbl = 0;
    i0 = idx->size[0] * idx->size[1];
    idx->size[0] = 1;
    idx->size[1] = b_x->size[1];
    emxEnsureCapacity((emxArray__common *)idx, i0, (int)sizeof(int));
    cdiff = 1;
    exitg10 = false;
    while ((!exitg10) && (cdiff <= nb)) {
      guard3 = false;
      if (b_x->data[cdiff - 1]) {
        ndbl++;
        idx->data[ndbl - 1] = cdiff;
        if (ndbl >= nb) {
          exitg10 = true;
        } else {
          guard3 = true;
        }
      } else {
        guard3 = true;
      }

      if (guard3) {
        cdiff++;
      }
    }

    if (b_x->size[1] == 1) {
      if (ndbl == 0) {
        i0 = idx->size[0] * idx->size[1];
        idx->size[0] = 1;
        idx->size[1] = 0;
        emxEnsureCapacity((emxArray__common *)idx, i0, (int)sizeof(int));
      }
    } else {
      i0 = idx->size[0] * idx->size[1];
      if (1 > ndbl) {
        idx->size[1] = 0;
      } else {
        idx->size[1] = ndbl;
      }

      emxEnsureCapacity((emxArray__common *)idx, i0, (int)sizeof(int));
    }

    i0 = sNote_gt->data[b_i].ovlaptimepitch->size[0] * sNote_gt->data[b_i].
      ovlaptimepitch->size[1];
    sNote_gt->data[b_i].ovlaptimepitch->size[0] = 1;
    sNote_gt->data[b_i].ovlaptimepitch->size[1] = idx->size[1];
    emxEnsureCapacity((emxArray__common *)sNote_gt->data[b_i].ovlaptimepitch, i0,
                      (int)sizeof(double));
    b_emxCopyStruct_struct_T(&h_sNote_gt, &sNote_gt->data[b_i]);
    b_emxCopyStruct_struct_T(&i_sNote_gt, &sNote_gt->data[b_i]);
    ndbl = idx->size[0] * idx->size[1];
    for (i0 = 0; i0 < ndbl; i0++) {
      sNote_gt->data[b_i].ovlaptimepitch->data[i0] = idx->data[i0];
    }

    i0 = sNote_gt->data[b_i].tr_split->size[0] * sNote_gt->data[b_i]
      .tr_split->size[1];
    sNote_gt->data[b_i].tr_split->size[0] = 1;
    sNote_gt->data[b_i].tr_split->size[1] = 0;
    emxEnsureCapacity((emxArray__common *)sNote_gt->data[b_i].tr_split, i0, (int)
                      sizeof(double));
    b_emxCopyStruct_struct_T(&j_sNote_gt, &sNote_gt->data[b_i]);
    b_emxCopyStruct_struct_T(&k_sNote_gt, &sNote_gt->data[b_i]);
    sNote_gt->data[b_i].tr_merged.size[0] = 1;
    sNote_gt->data[b_i].tr_merged.size[1] = 0;
    b_emxCopyStruct_struct_T(&l_sNote_gt, &sNote_gt->data[b_i]);
    b_emxCopyStruct_struct_T(&m_sNote_gt, &sNote_gt->data[b_i]);
    b_i++;
  }

  b_emxFreeStruct_struct_T(&m_sNote_gt);
  b_emxFreeStruct_struct_T(&l_sNote_gt);
  b_emxFreeStruct_struct_T(&k_sNote_gt);
  b_emxFreeStruct_struct_T(&j_sNote_gt);
  b_emxFreeStruct_struct_T(&i_sNote_gt);
  b_emxFreeStruct_struct_T(&h_sNote_gt);
  b_emxFreeStruct_struct_T(&g_sNote_gt);
  b_emxFreeStruct_struct_T(&f_sNote_gt);
  b_emxFreeStruct_struct_T(&e_sNote_gt);
  b_emxFreeStruct_struct_T(&d_sNote_gt);
  b_emxFreeStruct_struct_T(&c_sNote_gt);
  b_emxFreeStruct_struct_T(&b_sNote_gt);

  // Find close onsets
  i0 = sNote_tr->size[0];
  b_i = 0;
  emxInit_real_T(&r2, 2);
  emxInit_real_T(&r3, 2);
  while (b_i <= i0 - 1) {
    i = sNote_gt->size[0];
    for (j = 0; j < i; j++) {
      if (fabs(sNote_gt->data[j].data[0] - sNote_tr->data[b_i].data[0]) <=
          onset_lim) {
        i1 = r2->size[0] * r2->size[1];
        r2->size[0] = 1;
        r2->size[1] = sNote_tr->data[b_i].gt_onsetsok->size[1];
        emxEnsureCapacity((emxArray__common *)r2, i1, (int)sizeof(double));
        ndbl = sNote_tr->data[b_i].gt_onsetsok->size[0] * sNote_tr->data[b_i].
          gt_onsetsok->size[1];
        for (i1 = 0; i1 < ndbl; i1++) {
          r2->data[i1] = sNote_tr->data[b_i].gt_onsetsok->data[i1];
        }

        nb = r2->size[1];
        i1 = sNote_tr->data[b_i].gt_onsetsok->size[0] * sNote_tr->data[b_i].
          gt_onsetsok->size[1];
        sNote_tr->data[b_i].gt_onsetsok->size[1] = nb + 1;
        emxEnsureCapacity((emxArray__common *)sNote_tr->data[b_i].gt_onsetsok,
                          i1, (int)sizeof(double));
        sNote_tr->data[b_i].gt_onsetsok->data[nb] = 1.0 + (double)j;
        i1 = r3->size[0] * r3->size[1];
        r3->size[0] = 1;
        r3->size[1] = sNote_gt->data[j].tr_onsetsok->size[1];
        emxEnsureCapacity((emxArray__common *)r3, i1, (int)sizeof(double));
        ndbl = sNote_gt->data[j].tr_onsetsok->size[0] * sNote_gt->data[j].
          tr_onsetsok->size[1];
        for (i1 = 0; i1 < ndbl; i1++) {
          r3->data[i1] = sNote_gt->data[j].tr_onsetsok->data[i1];
        }

        nb = r3->size[1];
        i1 = sNote_gt->data[j].tr_onsetsok->size[0] * sNote_gt->data[j].
          tr_onsetsok->size[1];
        sNote_gt->data[j].tr_onsetsok->size[1] = nb + 1;
        emxEnsureCapacity((emxArray__common *)sNote_gt->data[j].tr_onsetsok, i1,
                          (int)sizeof(double));
        sNote_gt->data[j].tr_onsetsok->data[nb] = 1.0 + (double)b_i;
      }
    }

    b_i++;
  }

  emxFree_real_T(&r3);
  emxFree_real_T(&r2);

  // Find close offsets
  i0 = sNote_tr->size[0];
  b_i = 0;
  emxInit_real_T(&r4, 2);
  emxInit_real_T(&r5, 2);
  while (b_i <= i0 - 1) {
    i = sNote_gt->size[0];
    for (j = 0; j < i; j++) {
      offset = sNote_tr->data[b_i].data[0] + sNote_tr->data[b_i].data[1];
      y = sNote_gt->data[j].data[1] * dur_percent_range / 100.0;
      if ((min_dur_dist >= y) || rtIsNaN(y)) {
        durrange = min_dur_dist;
      } else {
        durrange = y;
      }

      if ((offset >= (sNote_gt->data[j].data[0] + sNote_gt->data[j].data[1]) -
           durrange) && (offset <= (sNote_gt->data[j].data[0] + sNote_gt->data[j]
            .data[1]) + durrange)) {
        i1 = r4->size[0] * r4->size[1];
        r4->size[0] = 1;
        r4->size[1] = sNote_tr->data[b_i].gt_offsetsok->size[1];
        emxEnsureCapacity((emxArray__common *)r4, i1, (int)sizeof(double));
        ndbl = sNote_tr->data[b_i].gt_offsetsok->size[0] * sNote_tr->data[b_i].
          gt_offsetsok->size[1];
        for (i1 = 0; i1 < ndbl; i1++) {
          r4->data[i1] = sNote_tr->data[b_i].gt_offsetsok->data[i1];
        }

        nb = r4->size[1];
        i1 = sNote_tr->data[b_i].gt_offsetsok->size[0] * sNote_tr->data[b_i].
          gt_offsetsok->size[1];
        sNote_tr->data[b_i].gt_offsetsok->size[1] = nb + 1;
        emxEnsureCapacity((emxArray__common *)sNote_tr->data[b_i].gt_offsetsok,
                          i1, (int)sizeof(double));
        sNote_tr->data[b_i].gt_offsetsok->data[nb] = 1.0 + (double)j;
        i1 = r5->size[0] * r5->size[1];
        r5->size[0] = 1;
        r5->size[1] = sNote_gt->data[j].tr_offsetsok->size[1];
        emxEnsureCapacity((emxArray__common *)r5, i1, (int)sizeof(double));
        ndbl = sNote_gt->data[j].tr_offsetsok->size[0] * sNote_gt->data[j].
          tr_offsetsok->size[1];
        for (i1 = 0; i1 < ndbl; i1++) {
          r5->data[i1] = sNote_gt->data[j].tr_offsetsok->data[i1];
        }

        nb = r5->size[1];
        i1 = sNote_gt->data[j].tr_offsetsok->size[0] * sNote_gt->data[j].
          tr_offsetsok->size[1];
        sNote_gt->data[j].tr_offsetsok->size[1] = nb + 1;
        emxEnsureCapacity((emxArray__common *)sNote_gt->data[j].tr_offsetsok, i1,
                          (int)sizeof(double));
        sNote_gt->data[j].tr_offsetsok->data[nb] = 1.0 + (double)b_i;
      }
    }

    b_i++;
  }

  emxFree_real_T(&r5);
  emxFree_real_T(&r4);

  // Find split notes
  if ((L_g->size[1] == 1) || (Moverlapped->size[0] == 1)) {
    i0 = M_refg->size[0] * M_refg->size[1];
    M_refg->size[0] = L_g->size[0];
    M_refg->size[1] = Moverlapped->size[1];
    emxEnsureCapacity((emxArray__common *)M_refg, i0, (int)sizeof(double));
    ndbl = L_g->size[0];
    for (i0 = 0; i0 < ndbl; i0++) {
      nb = Moverlapped->size[1];
      for (i = 0; i < nb; i++) {
        M_refg->data[i0 + M_refg->size[0] * i] = 0.0;
        cdiff = L_g->size[1];
        for (i1 = 0; i1 < cdiff; i1++) {
          M_refg->data[i0 + M_refg->size[0] * i] += L_g->data[i0 + L_g->size[0] *
            i1] * Moverlapped->data[i1 + Moverlapped->size[0] * i];
        }
      }
    }
  } else {
    k = L_g->size[1];
    uv0[0] = (unsigned int)L_g->size[0];
    uv0[1] = (unsigned int)Moverlapped->size[1];
    m = L_g->size[0];
    i0 = M_refg->size[0] * M_refg->size[1];
    M_refg->size[0] = (int)uv0[0];
    emxEnsureCapacity((emxArray__common *)M_refg, i0, (int)sizeof(double));
    i0 = M_refg->size[0] * M_refg->size[1];
    M_refg->size[1] = (int)uv0[1];
    emxEnsureCapacity((emxArray__common *)M_refg, i0, (int)sizeof(double));
    ndbl = (int)uv0[0] * (int)uv0[1];
    for (i0 = 0; i0 < ndbl; i0++) {
      M_refg->data[i0] = 0.0;
    }

    if ((L_g->size[0] == 0) || (Moverlapped->size[1] == 0)) {
    } else {
      c = L_g->size[0] * (Moverlapped->size[1] - 1);
      cr = 0;
      while ((m > 0) && (cr <= c)) {
        i0 = cr + m;
        for (ic = cr; ic + 1 <= i0; ic++) {
          M_refg->data[ic] = 0.0;
        }

        cr += m;
      }

      br = 0;
      cr = 0;
      while ((m > 0) && (cr <= c)) {
        ar = 0;
        i0 = br + k;
        for (ib = br; ib + 1 <= i0; ib++) {
          if (Moverlapped->data[ib] != 0.0) {
            ia = ar;
            i = cr + m;
            for (ic = cr; ic + 1 <= i; ic++) {
              ia++;
              M_refg->data[ic] += Moverlapped->data[ib] * L_g->data[ia - 1];
            }
          }

          ar += m;
        }

        br += k;
        cr += m;
      }
    }
  }

  emxFree_real_T(&L_g);
  if ((Moverlapped->size[1] == 1) || (L_t->size[0] == 1)) {
    i0 = Moverlapped_pitch->size[0] * Moverlapped_pitch->size[1];
    Moverlapped_pitch->size[0] = Moverlapped->size[0];
    Moverlapped_pitch->size[1] = L_t->size[1];
    emxEnsureCapacity((emxArray__common *)Moverlapped_pitch, i0, (int)sizeof
                      (double));
    ndbl = Moverlapped->size[0];
    for (i0 = 0; i0 < ndbl; i0++) {
      nb = L_t->size[1];
      for (i = 0; i < nb; i++) {
        Moverlapped_pitch->data[i0 + Moverlapped_pitch->size[0] * i] = 0.0;
        cdiff = Moverlapped->size[1];
        for (i1 = 0; i1 < cdiff; i1++) {
          Moverlapped_pitch->data[i0 + Moverlapped_pitch->size[0] * i] +=
            Moverlapped->data[i0 + Moverlapped->size[0] * i1] * L_t->data[i1 +
            L_t->size[0] * i];
        }
      }
    }
  } else {
    k = Moverlapped->size[1];
    uv0[0] = (unsigned int)Moverlapped->size[0];
    uv0[1] = (unsigned int)L_t->size[1];
    m = Moverlapped->size[0];
    i0 = Moverlapped_pitch->size[0] * Moverlapped_pitch->size[1];
    Moverlapped_pitch->size[0] = (int)uv0[0];
    emxEnsureCapacity((emxArray__common *)Moverlapped_pitch, i0, (int)sizeof
                      (double));
    i0 = Moverlapped_pitch->size[0] * Moverlapped_pitch->size[1];
    Moverlapped_pitch->size[1] = (int)uv0[1];
    emxEnsureCapacity((emxArray__common *)Moverlapped_pitch, i0, (int)sizeof
                      (double));
    ndbl = (int)uv0[0] * (int)uv0[1];
    for (i0 = 0; i0 < ndbl; i0++) {
      Moverlapped_pitch->data[i0] = 0.0;
    }

    if ((Moverlapped->size[0] == 0) || (L_t->size[1] == 0)) {
    } else {
      c = Moverlapped->size[0] * (L_t->size[1] - 1);
      cr = 0;
      while ((m > 0) && (cr <= c)) {
        i0 = cr + m;
        for (ic = cr; ic + 1 <= i0; ic++) {
          Moverlapped_pitch->data[ic] = 0.0;
        }

        cr += m;
      }

      br = 0;
      cr = 0;
      while ((m > 0) && (cr <= c)) {
        ar = 0;
        i0 = br + k;
        for (ib = br; ib + 1 <= i0; ib++) {
          if (L_t->data[ib] != 0.0) {
            ia = ar;
            i = cr + m;
            for (ic = cr; ic + 1 <= i; ic++) {
              ia++;
              Moverlapped_pitch->data[ic] += L_t->data[ib] * Moverlapped->
                data[ia - 1];
            }
          }

          ar += m;
        }

        br += k;
        cr += m;
      }
    }
  }

  emxFree_real_T(&Moverlapped);
  emxFree_real_T(&L_t);
  b_i = 0;
  emxInit_real_T(&Fnotes_tr_split, 2);
  emxInit_real_T(&b_M_refg, 2);
  b_emxInitStruct_struct_T(&n_sNote_gt);
  b_emxInitStruct_struct_T(&o_sNote_gt);
  emxInitStruct_struct_T(&j_sNote_tr);
  emxInitStruct_struct_T(&k_sNote_tr);
  while (b_i <= M_refg->size[0] - 1) {
    nflag = 0.0;
    for (j = 0; j < M_refg->size[1]; j++) {
      // The t% of the segment must overlap with the ref.
      if (Moverlapped_pitch->data[b_i + Moverlapped_pitch->size[0] * j] > 0.4) {
        nflag++;
      }
    }

    if (nflag > 1.0) {
      //  All the short segments together must overlap the t% of the ref.
      ndbl = M_refg->size[1];
      i0 = b_M_refg->size[0] * b_M_refg->size[1];
      b_M_refg->size[0] = 1;
      b_M_refg->size[1] = ndbl;
      emxEnsureCapacity((emxArray__common *)b_M_refg, i0, (int)sizeof(double));
      for (i0 = 0; i0 < ndbl; i0++) {
        b_M_refg->data[b_M_refg->size[0] * i0] = M_refg->data[b_i + M_refg->
          size[0] * i0];
      }

      if (sum(b_M_refg) > 0.4) {
        ndbl = Moverlapped_pitch->size[1];
        i0 = b_x->size[0] * b_x->size[1];
        b_x->size[0] = 1;
        b_x->size[1] = ndbl;
        emxEnsureCapacity((emxArray__common *)b_x, i0, (int)sizeof(boolean_T));
        for (i0 = 0; i0 < ndbl; i0++) {
          b_x->data[b_x->size[0] * i0] = (Moverlapped_pitch->data[b_i +
            Moverlapped_pitch->size[0] * i0] > 0.4);
        }

        nb = b_x->size[1];
        ndbl = 0;
        i0 = idx->size[0] * idx->size[1];
        idx->size[0] = 1;
        idx->size[1] = b_x->size[1];
        emxEnsureCapacity((emxArray__common *)idx, i0, (int)sizeof(int));
        cdiff = 1;
        exitg9 = false;
        while ((!exitg9) && (cdiff <= nb)) {
          guard2 = false;
          if (b_x->data[cdiff - 1]) {
            ndbl++;
            idx->data[ndbl - 1] = cdiff;
            if (ndbl >= nb) {
              exitg9 = true;
            } else {
              guard2 = true;
            }
          } else {
            guard2 = true;
          }

          if (guard2) {
            cdiff++;
          }
        }

        if (b_x->size[1] == 1) {
          if (ndbl == 0) {
            i0 = idx->size[0] * idx->size[1];
            idx->size[0] = 1;
            idx->size[1] = 0;
            emxEnsureCapacity((emxArray__common *)idx, i0, (int)sizeof(int));
          }
        } else {
          i0 = idx->size[0] * idx->size[1];
          if (1 > ndbl) {
            idx->size[1] = 0;
          } else {
            idx->size[1] = ndbl;
          }

          emxEnsureCapacity((emxArray__common *)idx, i0, (int)sizeof(int));
        }

        i0 = Fnotes_tr_split->size[0] * Fnotes_tr_split->size[1];
        Fnotes_tr_split->size[0] = 1;
        Fnotes_tr_split->size[1] = idx->size[1];
        emxEnsureCapacity((emxArray__common *)Fnotes_tr_split, i0, (int)sizeof
                          (double));
        ndbl = idx->size[0] * idx->size[1];
        for (i0 = 0; i0 < ndbl; i0++) {
          Fnotes_tr_split->data[i0] = idx->data[i0];
        }

        i0 = sNote_gt->data[b_i].tr_split->size[0] * sNote_gt->data[b_i].
          tr_split->size[1];
        sNote_gt->data[b_i].tr_split->size[0] = 1;
        sNote_gt->data[b_i].tr_split->size[1] = Fnotes_tr_split->size[1];
        emxEnsureCapacity((emxArray__common *)sNote_gt->data[b_i].tr_split, i0,
                          (int)sizeof(double));
        b_emxCopyStruct_struct_T(&n_sNote_gt, &sNote_gt->data[b_i]);
        b_emxCopyStruct_struct_T(&o_sNote_gt, &sNote_gt->data[b_i]);
        ndbl = Fnotes_tr_split->size[0] * Fnotes_tr_split->size[1];
        for (i0 = 0; i0 < ndbl; i0++) {
          sNote_gt->data[b_i].tr_split->data[i0] = Fnotes_tr_split->data[i0];
        }

        for (j = 0; j < Fnotes_tr_split->size[1]; j++) {
          sNote_tr->data[(int)Fnotes_tr_split->data[j] - 1].gt_split.size[0] = 1;
          sNote_tr->data[(int)Fnotes_tr_split->data[j] - 1].gt_split.size[1] = 1;
          emxCopyStruct_struct_T(&j_sNote_tr, &sNote_tr->data[(int)
            Fnotes_tr_split->data[j] - 1]);
          emxCopyStruct_struct_T(&k_sNote_tr, &sNote_tr->data[(int)
            Fnotes_tr_split->data[j] - 1]);
          sNote_tr->data[(int)Fnotes_tr_split->data[j] - 1].gt_split.data[0] =
            1.0 + (double)b_i;
        }
      }
    }

    b_i++;
  }

  emxFreeStruct_struct_T(&k_sNote_tr);
  emxFreeStruct_struct_T(&j_sNote_tr);
  b_emxFreeStruct_struct_T(&o_sNote_gt);
  b_emxFreeStruct_struct_T(&n_sNote_gt);
  emxFree_real_T(&b_M_refg);
  emxFree_boolean_T(&b_x);

  // Find merged notes
  j = 0;
  emxInitStruct_struct_T(&l_sNote_tr);
  emxInitStruct_struct_T(&m_sNote_tr);
  b_emxInitStruct_struct_T(&p_sNote_gt);
  b_emxInitStruct_struct_T(&q_sNote_gt);
  while (j <= Moverlapped_pitch->size[1] - 1) {
    nflag = 0.0;
    for (b_i = 0; b_i < Moverlapped_pitch->size[0]; b_i++) {
      if (M_refg->data[b_i + M_refg->size[0] * j] > 0.4) {
        nflag++;
      }
    }

    if (nflag > 1.0) {
      ndbl = M_refg->size[0];
      i0 = x->size[0];
      x->size[0] = ndbl;
      emxEnsureCapacity((emxArray__common *)x, i0, (int)sizeof(boolean_T));
      for (i0 = 0; i0 < ndbl; i0++) {
        x->data[i0] = (M_refg->data[i0 + M_refg->size[0] * j] > 0.4);
      }

      nb = x->size[0];
      ndbl = 0;
      i0 = iwork->size[0];
      iwork->size[0] = x->size[0];
      emxEnsureCapacity((emxArray__common *)iwork, i0, (int)sizeof(int));
      cdiff = 1;
      exitg8 = false;
      while ((!exitg8) && (cdiff <= nb)) {
        guard1 = false;
        if (x->data[cdiff - 1]) {
          ndbl++;
          iwork->data[ndbl - 1] = cdiff;
          if (ndbl >= nb) {
            exitg8 = true;
          } else {
            guard1 = true;
          }
        } else {
          guard1 = true;
        }

        if (guard1) {
          cdiff++;
        }
      }

      if (x->size[0] == 1) {
        if (ndbl == 0) {
          i0 = iwork->size[0];
          iwork->size[0] = 0;
          emxEnsureCapacity((emxArray__common *)iwork, i0, (int)sizeof(int));
        }
      } else {
        i0 = iwork->size[0];
        if (1 > ndbl) {
          iwork->size[0] = 0;
        } else {
          iwork->size[0] = ndbl;
        }

        emxEnsureCapacity((emxArray__common *)iwork, i0, (int)sizeof(int));
      }

      i0 = Fnotes_tr_split->size[0] * Fnotes_tr_split->size[1];
      Fnotes_tr_split->size[0] = 1;
      Fnotes_tr_split->size[1] = iwork->size[0];
      emxEnsureCapacity((emxArray__common *)Fnotes_tr_split, i0, (int)sizeof
                        (double));
      ndbl = iwork->size[0];
      for (i0 = 0; i0 < ndbl; i0++) {
        Fnotes_tr_split->data[Fnotes_tr_split->size[0] * i0] = iwork->data[i0];
      }

      i0 = sNote_tr->data[j].gt_merged->size[0] * sNote_tr->data[j]
        .gt_merged->size[1];
      sNote_tr->data[j].gt_merged->size[0] = 1;
      sNote_tr->data[j].gt_merged->size[1] = Fnotes_tr_split->size[1];
      emxEnsureCapacity((emxArray__common *)sNote_tr->data[j].gt_merged, i0,
                        (int)sizeof(double));
      emxCopyStruct_struct_T(&l_sNote_tr, &sNote_tr->data[j]);
      emxCopyStruct_struct_T(&m_sNote_tr, &sNote_tr->data[j]);
      ndbl = Fnotes_tr_split->size[0] * Fnotes_tr_split->size[1];
      for (i0 = 0; i0 < ndbl; i0++) {
        sNote_tr->data[j].gt_merged->data[i0] = Fnotes_tr_split->data[i0];
      }

      for (b_i = 0; b_i < Fnotes_tr_split->size[1]; b_i++) {
        sNote_gt->data[(int)Fnotes_tr_split->data[b_i] - 1].tr_merged.size[0] =
          1;
        sNote_gt->data[(int)Fnotes_tr_split->data[b_i] - 1].tr_merged.size[1] =
          1;
        b_emxCopyStruct_struct_T(&p_sNote_gt, &sNote_gt->data[(int)
          Fnotes_tr_split->data[b_i] - 1]);
        b_emxCopyStruct_struct_T(&q_sNote_gt, &sNote_gt->data[(int)
          Fnotes_tr_split->data[b_i] - 1]);
        sNote_gt->data[(int)Fnotes_tr_split->data[b_i] - 1].tr_merged.data[0] =
          1.0 + (double)j;
      }
    }

    j++;
  }

  b_emxFreeStruct_struct_T(&q_sNote_gt);
  b_emxFreeStruct_struct_T(&p_sNote_gt);
  emxFreeStruct_struct_T(&m_sNote_tr);
  emxFreeStruct_struct_T(&l_sNote_tr);
  emxFree_boolean_T(&x);
  emxFree_real_T(&M_refg);
  emxFree_real_T(&Moverlapped_pitch);
  emxInit_real_T(&notes_gt_111, 2);
  emxInit_real_T(&notes_gt_100, 2);
  emxInit_real_T(&notes_gt_011, 2);
  emxInit_real_T(&notes_gt_101, 2);
  emxInit_real_T(&notes_gt_110, 2);

  // C=[ONSET_OK OFFSET_OK PITCH_OK]
  i0 = notes_gt_111->size[0] * notes_gt_111->size[1];
  notes_gt_111->size[0] = 1;
  notes_gt_111->size[1] = 0;
  emxEnsureCapacity((emxArray__common *)notes_gt_111, i0, (int)sizeof(double));
  i0 = notes_gt_100->size[0] * notes_gt_100->size[1];
  notes_gt_100->size[0] = 1;
  notes_gt_100->size[1] = 0;
  emxEnsureCapacity((emxArray__common *)notes_gt_100, i0, (int)sizeof(double));
  i0 = notes_gt_011->size[0] * notes_gt_011->size[1];
  notes_gt_011->size[0] = 1;
  notes_gt_011->size[1] = 0;
  emxEnsureCapacity((emxArray__common *)notes_gt_011, i0, (int)sizeof(double));
  i0 = notes_gt_101->size[0] * notes_gt_101->size[1];
  notes_gt_101->size[0] = 1;
  notes_gt_101->size[1] = 0;
  emxEnsureCapacity((emxArray__common *)notes_gt_101, i0, (int)sizeof(double));
  i0 = notes_gt_110->size[0] * notes_gt_110->size[1];
  notes_gt_110->size[0] = 1;
  notes_gt_110->size[1] = 0;
  emxEnsureCapacity((emxArray__common *)notes_gt_110, i0, (int)sizeof(double));
  emxInit_real_T(&Fnotes_gt_split, 2);
  emxInit_real_T(&Fnotes_gt_merged, 2);
  emxInit_real_T(&b_Fnotes_gt_merged, 2);
  emxInit_real_T(&c_Fnotes_gt_merged, 2);
  emxInit_real_T(&d_Fnotes_gt_merged, 2);
  emxInit_real_T(&e_Fnotes_gt_merged, 2);
  for (c = 0; c < 5; c++) {
    i0 = Fnotes_tr_split->size[0] * Fnotes_tr_split->size[1];
    Fnotes_tr_split->size[0] = 1;
    Fnotes_tr_split->size[1] = 0;
    emxEnsureCapacity((emxArray__common *)Fnotes_tr_split, i0, (int)sizeof
                      (double));
    for (b_i = 0; b_i < sNote_tr->size[0]; b_i++) {
      if (sNote_gt->size[0] < 1) {
        n = -1;
        apnd = 0;
      } else {
        ndbl = (int)floor(((double)sNote_gt->size[0] - 1.0) + 0.5);
        apnd = ndbl + 1;
        cdiff = (ndbl - sNote_gt->size[0]) + 1;
        absb = sNote_gt->size[0];
        if (fabs((double)cdiff) < 4.4408920985006262E-16 * (double)absb) {
          ndbl++;
          apnd = sNote_gt->size[0];
        } else if (cdiff > 0) {
          apnd = ndbl;
        } else {
          ndbl++;
        }

        n = ndbl - 1;
      }

      i0 = Fnotes_gt_merged->size[0] * Fnotes_gt_merged->size[1];
      Fnotes_gt_merged->size[0] = 1;
      Fnotes_gt_merged->size[1] = n + 1;
      emxEnsureCapacity((emxArray__common *)Fnotes_gt_merged, i0, (int)sizeof
                        (double));
      if (n + 1 > 0) {
        Fnotes_gt_merged->data[0] = 1.0;
        if (n + 1 > 1) {
          Fnotes_gt_merged->data[n] = apnd;
          nm1d2 = (n + (n < 0)) >> 1;
          for (k = 1; k < nm1d2; k++) {
            Fnotes_gt_merged->data[k] = 1.0 + (double)k;
            Fnotes_gt_merged->data[n - k] = apnd - k;
          }

          if (nm1d2 << 1 == n) {
            Fnotes_gt_merged->data[nm1d2] = (1.0 + (double)apnd) / 2.0;
          } else {
            Fnotes_gt_merged->data[nm1d2] = 1.0 + (double)nm1d2;
            Fnotes_gt_merged->data[nm1d2 + 1] = apnd - nm1d2;
          }
        }
      }

      if (C[c] == 1) {
        i0 = e_Fnotes_gt_merged->size[0] * e_Fnotes_gt_merged->size[1];
        e_Fnotes_gt_merged->size[0] = 1;
        e_Fnotes_gt_merged->size[1] = Fnotes_gt_merged->size[1];
        emxEnsureCapacity((emxArray__common *)e_Fnotes_gt_merged, i0, (int)
                          sizeof(double));
        ndbl = Fnotes_gt_merged->size[0] * Fnotes_gt_merged->size[1];
        for (i0 = 0; i0 < ndbl; i0++) {
          e_Fnotes_gt_merged->data[i0] = Fnotes_gt_merged->data[i0];
        }

        intersect_simple(e_Fnotes_gt_merged, sNote_tr->data[b_i].gt_onsetsok,
                         Fnotes_gt_merged);
      }

      if (C[5 + c] == 1) {
        i0 = d_Fnotes_gt_merged->size[0] * d_Fnotes_gt_merged->size[1];
        d_Fnotes_gt_merged->size[0] = 1;
        d_Fnotes_gt_merged->size[1] = Fnotes_gt_merged->size[1];
        emxEnsureCapacity((emxArray__common *)d_Fnotes_gt_merged, i0, (int)
                          sizeof(double));
        ndbl = Fnotes_gt_merged->size[0] * Fnotes_gt_merged->size[1];
        for (i0 = 0; i0 < ndbl; i0++) {
          d_Fnotes_gt_merged->data[i0] = Fnotes_gt_merged->data[i0];
        }

        intersect_simple(d_Fnotes_gt_merged, sNote_tr->data[b_i].gt_offsetsok,
                         Fnotes_gt_merged);
      }

      if (C[10 + c] == 1) {
        i0 = c_Fnotes_gt_merged->size[0] * c_Fnotes_gt_merged->size[1];
        c_Fnotes_gt_merged->size[0] = 1;
        c_Fnotes_gt_merged->size[1] = Fnotes_gt_merged->size[1];
        emxEnsureCapacity((emxArray__common *)c_Fnotes_gt_merged, i0, (int)
                          sizeof(double));
        ndbl = Fnotes_gt_merged->size[0] * Fnotes_gt_merged->size[1];
        for (i0 = 0; i0 < ndbl; i0++) {
          c_Fnotes_gt_merged->data[i0] = Fnotes_gt_merged->data[i0];
        }

        intersect_simple(c_Fnotes_gt_merged, sNote_tr->data[b_i].ovlaptimepitch,
                         Fnotes_gt_merged);
      }

      // Only one ground-truth <-> Transcribed note association
      i0 = b_Fnotes_gt_merged->size[0] * b_Fnotes_gt_merged->size[1];
      b_Fnotes_gt_merged->size[0] = 1;
      b_Fnotes_gt_merged->size[1] = Fnotes_gt_merged->size[1];
      emxEnsureCapacity((emxArray__common *)b_Fnotes_gt_merged, i0, (int)sizeof
                        (double));
      ndbl = Fnotes_gt_merged->size[0] * Fnotes_gt_merged->size[1];
      for (i0 = 0; i0 < ndbl; i0++) {
        b_Fnotes_gt_merged->data[i0] = Fnotes_gt_merged->data[i0];
      }

      setdiff_simple(b_Fnotes_gt_merged, Fnotes_tr_split, Fnotes_gt_merged);

      // Ignore if already considered
      if (!(Fnotes_gt_merged->size[1] == 0)) {
        i0 = Fnotes_gt_split->size[0] * Fnotes_gt_split->size[1];
        Fnotes_gt_split->size[0] = 1;
        Fnotes_gt_split->size[1] = Fnotes_tr_split->size[1] + 1;
        emxEnsureCapacity((emxArray__common *)Fnotes_gt_split, i0, (int)sizeof
                          (double));
        ndbl = Fnotes_tr_split->size[1];
        for (i0 = 0; i0 < ndbl; i0++) {
          Fnotes_gt_split->data[Fnotes_gt_split->size[0] * i0] =
            Fnotes_tr_split->data[Fnotes_tr_split->size[0] * i0];
        }

        Fnotes_gt_split->data[Fnotes_gt_split->size[0] * Fnotes_tr_split->size[1]]
          = Fnotes_gt_merged->data[0];
        na = Fnotes_gt_split->size[1];
        i0 = idx->size[0] * idx->size[1];
        idx->size[0] = 1;
        emxEnsureCapacity((emxArray__common *)idx, i0, (int)sizeof(int));
        nb = Fnotes_gt_split->size[1];
        i0 = idx->size[0] * idx->size[1];
        idx->size[1] = nb;
        emxEnsureCapacity((emxArray__common *)idx, i0, (int)sizeof(int));
        ndbl = Fnotes_gt_split->size[1];
        for (i0 = 0; i0 < ndbl; i0++) {
          idx->data[i0] = 0;
        }

        n = Fnotes_gt_split->size[1] + 1;
        nb = Fnotes_gt_split->size[1];
        i0 = iwork->size[0];
        iwork->size[0] = nb;
        emxEnsureCapacity((emxArray__common *)iwork, i0, (int)sizeof(int));
        for (k = 1; k <= n - 2; k += 2) {
          if ((Fnotes_gt_split->data[k - 1] <= Fnotes_gt_split->data[k]) ||
              rtIsNaN(Fnotes_gt_split->data[k])) {
            p = true;
          } else {
            p = false;
          }

          if (p) {
            idx->data[k - 1] = k;
            idx->data[k] = k + 1;
          } else {
            idx->data[k - 1] = k + 1;
            idx->data[k] = k;
          }
        }

        if ((Fnotes_gt_split->size[1] & 1) != 0) {
          idx->data[Fnotes_gt_split->size[1] - 1] = Fnotes_gt_split->size[1];
        }

        i = 2;
        while (i < n - 1) {
          i2 = i << 1;
          j = 1;
          for (pEnd = 1 + i; pEnd < n; pEnd = qEnd + i) {
            b_p = j;
            q = pEnd - 1;
            qEnd = j + i2;
            if (qEnd > n) {
              qEnd = n;
            }

            k = 0;
            kEnd = qEnd - j;
            while (k + 1 <= kEnd) {
              if ((Fnotes_gt_split->data[idx->data[b_p - 1] - 1] <=
                   Fnotes_gt_split->data[idx->data[q] - 1]) || rtIsNaN
                  (Fnotes_gt_split->data[idx->data[q] - 1])) {
                p = true;
              } else {
                p = false;
              }

              if (p) {
                iwork->data[k] = idx->data[b_p - 1];
                b_p++;
                if (b_p == pEnd) {
                  while (q + 1 < qEnd) {
                    k++;
                    iwork->data[k] = idx->data[q];
                    q++;
                  }
                }
              } else {
                iwork->data[k] = idx->data[q];
                q++;
                if (q + 1 == qEnd) {
                  while (b_p < pEnd) {
                    k++;
                    iwork->data[k] = idx->data[b_p - 1];
                    b_p++;
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

        for (i0 = 0; i0 < 2; i0++) {
          uv0[i0] = (unsigned int)Fnotes_gt_split->size[i0];
        }

        i0 = Fnotes_tr_split->size[0] * Fnotes_tr_split->size[1];
        Fnotes_tr_split->size[0] = 1;
        Fnotes_tr_split->size[1] = (int)uv0[1];
        emxEnsureCapacity((emxArray__common *)Fnotes_tr_split, i0, (int)sizeof
                          (double));
        for (k = 0; k + 1 <= na; k++) {
          Fnotes_tr_split->data[k] = Fnotes_gt_split->data[idx->data[k] - 1];
        }

        k = 0;
        while ((k + 1 <= na) && rtIsInf(Fnotes_tr_split->data[k]) &&
               (Fnotes_tr_split->data[k] < 0.0)) {
          k++;
        }

        b_k = k;
        k = Fnotes_gt_split->size[1];
        while ((k >= 1) && rtIsNaN(Fnotes_tr_split->data[k - 1])) {
          k--;
        }

        nNaN = Fnotes_gt_split->size[1] - k;
        while ((k >= 1) && rtIsInf(Fnotes_tr_split->data[k - 1]) &&
               (Fnotes_tr_split->data[k - 1] > 0.0)) {
          k--;
        }

        nInf = (Fnotes_gt_split->size[1] - k) - nNaN;
        b_nb = -1;
        if (b_k > 0) {
          b_nb = 0;
        }

        khi = (b_k + k) - b_k;
        while (b_k + 1 <= khi) {
          c_x = Fnotes_tr_split->data[b_k];
          do {
            exitg7 = 0;
            b_k++;
            if (b_k + 1 > khi) {
              exitg7 = 1;
            } else {
              absxk = fabs(c_x / 2.0);
              if ((!rtIsInf(absxk)) && (!rtIsNaN(absxk))) {
                if (absxk <= 2.2250738585072014E-308) {
                  r = 4.94065645841247E-324;
                } else {
                  frexp(absxk, &exponent);
                  r = ldexp(1.0, exponent - 53);
                }
              } else {
                r = rtNaN;
              }

              if ((fabs(c_x - Fnotes_tr_split->data[b_k]) < r) || (rtIsInf
                   (Fnotes_tr_split->data[b_k]) && rtIsInf(c_x) &&
                   ((Fnotes_tr_split->data[b_k] > 0.0) == (c_x > 0.0)))) {
                p = true;
              } else {
                p = false;
              }

              if (!p) {
                exitg7 = 1;
              }
            }
          } while (exitg7 == 0);

          b_nb++;
          Fnotes_tr_split->data[b_nb] = c_x;
        }

        if (nInf > 0) {
          b_nb++;
          Fnotes_tr_split->data[b_nb] = Fnotes_tr_split->data[khi];
        }

        b_k = khi + nInf;
        for (j = 1; j <= nNaN; j++) {
          b_nb++;
          Fnotes_tr_split->data[b_nb] = Fnotes_tr_split->data[(b_k + j) - 1];
        }

        i0 = Fnotes_tr_split->size[0] * Fnotes_tr_split->size[1];
        if (1 > b_nb + 1) {
          b_i2 = -1;
        } else {
          b_i2 = b_nb;
        }

        Fnotes_tr_split->size[1] = b_i2 + 1;
        emxEnsureCapacity((emxArray__common *)Fnotes_tr_split, i0, (int)sizeof
                          (double));
      }
    }

    if (1 + c == 1) {
      i0 = notes_gt_111->size[0] * notes_gt_111->size[1];
      notes_gt_111->size[0] = 1;
      notes_gt_111->size[1] = Fnotes_tr_split->size[1];
      emxEnsureCapacity((emxArray__common *)notes_gt_111, i0, (int)sizeof(double));
      ndbl = Fnotes_tr_split->size[0] * Fnotes_tr_split->size[1];
      for (i0 = 0; i0 < ndbl; i0++) {
        notes_gt_111->data[i0] = Fnotes_tr_split->data[i0];
      }
    } else if (1 + c == 2) {
      i0 = notes_gt_100->size[0] * notes_gt_100->size[1];
      notes_gt_100->size[0] = 1;
      notes_gt_100->size[1] = Fnotes_tr_split->size[1];
      emxEnsureCapacity((emxArray__common *)notes_gt_100, i0, (int)sizeof(double));
      ndbl = Fnotes_tr_split->size[0] * Fnotes_tr_split->size[1];
      for (i0 = 0; i0 < ndbl; i0++) {
        notes_gt_100->data[i0] = Fnotes_tr_split->data[i0];
      }
    } else if (1 + c == 3) {
      i0 = notes_gt_011->size[0] * notes_gt_011->size[1];
      notes_gt_011->size[0] = 1;
      notes_gt_011->size[1] = Fnotes_tr_split->size[1];
      emxEnsureCapacity((emxArray__common *)notes_gt_011, i0, (int)sizeof(double));
      ndbl = Fnotes_tr_split->size[0] * Fnotes_tr_split->size[1];
      for (i0 = 0; i0 < ndbl; i0++) {
        notes_gt_011->data[i0] = Fnotes_tr_split->data[i0];
      }
    } else if (1 + c == 4) {
      i0 = notes_gt_101->size[0] * notes_gt_101->size[1];
      notes_gt_101->size[0] = 1;
      notes_gt_101->size[1] = Fnotes_tr_split->size[1];
      emxEnsureCapacity((emxArray__common *)notes_gt_101, i0, (int)sizeof(double));
      ndbl = Fnotes_tr_split->size[0] * Fnotes_tr_split->size[1];
      for (i0 = 0; i0 < ndbl; i0++) {
        notes_gt_101->data[i0] = Fnotes_tr_split->data[i0];
      }
    } else {
      i0 = notes_gt_110->size[0] * notes_gt_110->size[1];
      notes_gt_110->size[0] = 1;
      notes_gt_110->size[1] = Fnotes_tr_split->size[1];
      emxEnsureCapacity((emxArray__common *)notes_gt_110, i0, (int)sizeof(double));
      ndbl = Fnotes_tr_split->size[0] * Fnotes_tr_split->size[1];
      for (i0 = 0; i0 < ndbl; i0++) {
        notes_gt_110->data[i0] = Fnotes_tr_split->data[i0];
      }
    }
  }

  emxFree_real_T(&e_Fnotes_gt_merged);
  emxFree_real_T(&d_Fnotes_gt_merged);
  emxFree_real_T(&c_Fnotes_gt_merged);
  emxFree_real_T(&b_Fnotes_gt_merged);
  emxInit_real_T(&notes_gt_011b, 2);
  emxInit_real_T(&notes_gt_101b, 2);
  emxInit_real_T(&notes_gt_110b, 2);
  emxInit_real_T(&Fnotes_tr_merged, 2);
  emxInit_real_T(&Fnotes_tr_detected, 2);
  emxInit_real_T(&Fnotes_gt_detected, 2);
  setdiff_simple(notes_gt_011, notes_gt_111, notes_gt_011b);
  setdiff_simple(notes_gt_101, notes_gt_111, notes_gt_101b);
  setdiff_simple(notes_gt_110, notes_gt_111, notes_gt_110b);
  i0 = Fnotes_tr_split->size[0] * Fnotes_tr_split->size[1];
  Fnotes_tr_split->size[0] = 1;
  Fnotes_tr_split->size[1] = 0;
  emxEnsureCapacity((emxArray__common *)Fnotes_tr_split, i0, (int)sizeof(double));
  i0 = Fnotes_gt_split->size[0] * Fnotes_gt_split->size[1];
  Fnotes_gt_split->size[0] = 1;
  Fnotes_gt_split->size[1] = 0;
  emxEnsureCapacity((emxArray__common *)Fnotes_gt_split, i0, (int)sizeof(double));
  i0 = Fnotes_tr_merged->size[0] * Fnotes_tr_merged->size[1];
  Fnotes_tr_merged->size[0] = 1;
  Fnotes_tr_merged->size[1] = 0;
  emxEnsureCapacity((emxArray__common *)Fnotes_tr_merged, i0, (int)sizeof(double));
  i0 = Fnotes_gt_merged->size[0] * Fnotes_gt_merged->size[1];
  Fnotes_gt_merged->size[0] = 1;
  Fnotes_gt_merged->size[1] = 0;
  emxEnsureCapacity((emxArray__common *)Fnotes_gt_merged, i0, (int)sizeof(double));
  i0 = Fnotes_tr_detected->size[0] * Fnotes_tr_detected->size[1];
  Fnotes_tr_detected->size[0] = 1;
  Fnotes_tr_detected->size[1] = 0;
  emxEnsureCapacity((emxArray__common *)Fnotes_tr_detected, i0, (int)sizeof
                    (double));
  i0 = Fnotes_gt_detected->size[0] * Fnotes_gt_detected->size[1];
  Fnotes_gt_detected->size[0] = 1;
  Fnotes_gt_detected->size[1] = 0;
  emxEnsureCapacity((emxArray__common *)Fnotes_gt_detected, i0, (int)sizeof
                    (double));
  b_i = 0;
  emxInit_real_T(&r6, 2);
  emxInit_real_T(&r7, 2);
  while (b_i <= sNote_tr->size[0] - 1) {
    cdiff = sNote_tr->data[b_i].gt_split.size[1];
    nb = Fnotes_gt_split->size[1];
    i0 = Fnotes_gt_split->size[0] * Fnotes_gt_split->size[1];
    Fnotes_gt_split->size[1] = nb + cdiff;
    emxEnsureCapacity((emxArray__common *)Fnotes_gt_split, i0, (int)sizeof
                      (double));
    for (i0 = 0; i0 < cdiff; i0++) {
      Fnotes_gt_split->data[nb + i0] = sNote_tr->data[b_i].gt_split.data[i0];
    }

    if (!(sNote_tr->data[b_i].gt_split.size[1] == 0)) {
      nb = Fnotes_tr_split->size[1];
      i0 = Fnotes_tr_split->size[0] * Fnotes_tr_split->size[1];
      Fnotes_tr_split->size[1] = nb + 1;
      emxEnsureCapacity((emxArray__common *)Fnotes_tr_split, i0, (int)sizeof
                        (double));
      Fnotes_tr_split->data[nb] = 1.0 + (double)b_i;
    }

    i0 = r6->size[0] * r6->size[1];
    r6->size[0] = 1;
    r6->size[1] = sNote_tr->data[b_i].ovlaptime->size[1];
    emxEnsureCapacity((emxArray__common *)r6, i0, (int)sizeof(double));
    ndbl = sNote_tr->data[b_i].ovlaptime->size[0] * sNote_tr->data[b_i].
      ovlaptime->size[1];
    for (i0 = 0; i0 < ndbl; i0++) {
      r6->data[i0] = sNote_tr->data[b_i].ovlaptime->data[i0];
    }

    nb = Fnotes_gt_detected->size[1];
    cdiff = r6->size[1];
    i0 = Fnotes_gt_detected->size[0] * Fnotes_gt_detected->size[1];
    Fnotes_gt_detected->size[1] = nb + cdiff;
    emxEnsureCapacity((emxArray__common *)Fnotes_gt_detected, i0, (int)sizeof
                      (double));
    for (i0 = 0; i0 < cdiff; i0++) {
      Fnotes_gt_detected->data[nb + i0] = sNote_tr->data[b_i].ovlaptime->data[i0];
    }

    i0 = r7->size[0] * r7->size[1];
    r7->size[0] = 1;
    r7->size[1] = sNote_tr->data[b_i].gt_merged->size[1];
    emxEnsureCapacity((emxArray__common *)r7, i0, (int)sizeof(double));
    ndbl = sNote_tr->data[b_i].gt_merged->size[0] * sNote_tr->data[b_i].
      gt_merged->size[1];
    for (i0 = 0; i0 < ndbl; i0++) {
      r7->data[i0] = sNote_tr->data[b_i].gt_merged->data[i0];
    }

    nb = Fnotes_gt_merged->size[1];
    cdiff = r7->size[1];
    i0 = Fnotes_gt_merged->size[0] * Fnotes_gt_merged->size[1];
    Fnotes_gt_merged->size[1] = nb + cdiff;
    emxEnsureCapacity((emxArray__common *)Fnotes_gt_merged, i0, (int)sizeof
                      (double));
    for (i0 = 0; i0 < cdiff; i0++) {
      Fnotes_gt_merged->data[nb + i0] = sNote_tr->data[b_i].gt_merged->data[i0];
    }

    if (!(sNote_tr->data[b_i].gt_merged->size[1] == 0)) {
      nb = Fnotes_tr_merged->size[1];
      i0 = Fnotes_tr_merged->size[0] * Fnotes_tr_merged->size[1];
      Fnotes_tr_merged->size[1] = nb + 1;
      emxEnsureCapacity((emxArray__common *)Fnotes_tr_merged, i0, (int)sizeof
                        (double));
      Fnotes_tr_merged->data[nb] = 1.0 + (double)b_i;
    }

    if (!(sNote_tr->data[b_i].ovlaptime->size[1] == 0)) {
      nb = Fnotes_tr_detected->size[1];
      i0 = Fnotes_tr_detected->size[0] * Fnotes_tr_detected->size[1];
      Fnotes_tr_detected->size[1] = nb + 1;
      emxEnsureCapacity((emxArray__common *)Fnotes_tr_detected, i0, (int)sizeof
                        (double));
      Fnotes_tr_detected->data[nb] = 1.0 + (double)b_i;
    }

    b_i++;
  }

  emxFree_real_T(&r7);
  emxFree_real_T(&r6);
  na = Fnotes_gt_split->size[1];
  i0 = idx->size[0] * idx->size[1];
  idx->size[0] = 1;
  emxEnsureCapacity((emxArray__common *)idx, i0, (int)sizeof(int));
  nb = Fnotes_gt_split->size[1];
  i0 = idx->size[0] * idx->size[1];
  idx->size[1] = nb;
  emxEnsureCapacity((emxArray__common *)idx, i0, (int)sizeof(int));
  ndbl = Fnotes_gt_split->size[1];
  for (i0 = 0; i0 < ndbl; i0++) {
    idx->data[i0] = 0;
  }

  if (Fnotes_gt_split->size[1] == 0) {
  } else {
    n = Fnotes_gt_split->size[1] + 1;
    nb = Fnotes_gt_split->size[1];
    i0 = iwork->size[0];
    iwork->size[0] = nb;
    emxEnsureCapacity((emxArray__common *)iwork, i0, (int)sizeof(int));
    for (k = 1; k <= n - 2; k += 2) {
      if ((Fnotes_gt_split->data[k - 1] <= Fnotes_gt_split->data[k]) || rtIsNaN
          (Fnotes_gt_split->data[k])) {
        p = true;
      } else {
        p = false;
      }

      if (p) {
        idx->data[k - 1] = k;
        idx->data[k] = k + 1;
      } else {
        idx->data[k - 1] = k + 1;
        idx->data[k] = k;
      }
    }

    if ((Fnotes_gt_split->size[1] & 1) != 0) {
      idx->data[Fnotes_gt_split->size[1] - 1] = Fnotes_gt_split->size[1];
    }

    b_i = 2;
    while (b_i < n - 1) {
      i2 = b_i << 1;
      j = 1;
      for (pEnd = 1 + b_i; pEnd < n; pEnd = qEnd + b_i) {
        b_p = j;
        q = pEnd - 1;
        qEnd = j + i2;
        if (qEnd > n) {
          qEnd = n;
        }

        k = 0;
        kEnd = qEnd - j;
        while (k + 1 <= kEnd) {
          if ((Fnotes_gt_split->data[idx->data[b_p - 1] - 1] <=
               Fnotes_gt_split->data[idx->data[q] - 1]) || rtIsNaN
              (Fnotes_gt_split->data[idx->data[q] - 1])) {
            p = true;
          } else {
            p = false;
          }

          if (p) {
            iwork->data[k] = idx->data[b_p - 1];
            b_p++;
            if (b_p == pEnd) {
              while (q + 1 < qEnd) {
                k++;
                iwork->data[k] = idx->data[q];
                q++;
              }
            }
          } else {
            iwork->data[k] = idx->data[q];
            q++;
            if (q + 1 == qEnd) {
              while (b_p < pEnd) {
                k++;
                iwork->data[k] = idx->data[b_p - 1];
                b_p++;
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

      b_i = i2;
    }
  }

  for (i0 = 0; i0 < 2; i0++) {
    uv0[i0] = (unsigned int)Fnotes_gt_split->size[i0];
  }

  emxInit_real_T(&S_listgt, 2);
  i0 = S_listgt->size[0] * S_listgt->size[1];
  S_listgt->size[0] = 1;
  S_listgt->size[1] = (int)uv0[1];
  emxEnsureCapacity((emxArray__common *)S_listgt, i0, (int)sizeof(double));
  for (k = 0; k + 1 <= na; k++) {
    S_listgt->data[k] = Fnotes_gt_split->data[idx->data[k] - 1];
  }

  k = 0;
  while ((k + 1 <= na) && rtIsInf(S_listgt->data[k]) && (S_listgt->data[k] < 0.0))
  {
    k++;
  }

  b_k = k;
  k = Fnotes_gt_split->size[1];
  while ((k >= 1) && rtIsNaN(S_listgt->data[k - 1])) {
    k--;
  }

  nNaN = Fnotes_gt_split->size[1] - k;
  while ((k >= 1) && rtIsInf(S_listgt->data[k - 1]) && (S_listgt->data[k - 1] >
          0.0)) {
    k--;
  }

  nInf = (Fnotes_gt_split->size[1] - k) - nNaN;
  b_nb = -1;
  if (b_k > 0) {
    b_nb = 0;
  }

  khi = (b_k + k) - b_k;
  while (b_k + 1 <= khi) {
    c_x = S_listgt->data[b_k];
    do {
      exitg6 = 0;
      b_k++;
      if (b_k + 1 > khi) {
        exitg6 = 1;
      } else {
        absxk = fabs(c_x / 2.0);
        if ((!rtIsInf(absxk)) && (!rtIsNaN(absxk))) {
          if (absxk <= 2.2250738585072014E-308) {
            r = 4.94065645841247E-324;
          } else {
            frexp(absxk, &b_exponent);
            r = ldexp(1.0, b_exponent - 53);
          }
        } else {
          r = rtNaN;
        }

        if ((fabs(c_x - S_listgt->data[b_k]) < r) || (rtIsInf(S_listgt->data[b_k])
             && rtIsInf(c_x) && ((S_listgt->data[b_k] > 0.0) == (c_x > 0.0)))) {
          p = true;
        } else {
          p = false;
        }

        if (!p) {
          exitg6 = 1;
        }
      }
    } while (exitg6 == 0);

    b_nb++;
    S_listgt->data[b_nb] = c_x;
  }

  if (nInf > 0) {
    b_nb++;
    S_listgt->data[b_nb] = S_listgt->data[khi];
  }

  b_k = khi + nInf;
  for (j = 1; j <= nNaN; j++) {
    b_nb++;
    S_listgt->data[b_nb] = S_listgt->data[(b_k + j) - 1];
  }

  if (1 > b_nb + 1) {
    i0 = 0;
  } else {
    i0 = b_nb + 1;
  }

  i = S_listgt->size[0] * S_listgt->size[1];
  S_listgt->size[1] = i0;
  emxEnsureCapacity((emxArray__common *)S_listgt, i, (int)sizeof(double));
  na = Fnotes_tr_split->size[1];
  i = idx->size[0] * idx->size[1];
  idx->size[0] = 1;
  emxEnsureCapacity((emxArray__common *)idx, i, (int)sizeof(int));
  nb = Fnotes_tr_split->size[1];
  i = idx->size[0] * idx->size[1];
  idx->size[1] = nb;
  emxEnsureCapacity((emxArray__common *)idx, i, (int)sizeof(int));
  ndbl = Fnotes_tr_split->size[1];
  for (i = 0; i < ndbl; i++) {
    idx->data[i] = 0;
  }

  if (Fnotes_tr_split->size[1] == 0) {
  } else {
    n = Fnotes_tr_split->size[1] + 1;
    nb = Fnotes_tr_split->size[1];
    i = iwork->size[0];
    iwork->size[0] = nb;
    emxEnsureCapacity((emxArray__common *)iwork, i, (int)sizeof(int));
    for (k = 1; k <= n - 2; k += 2) {
      p = ((int)Fnotes_tr_split->data[k - 1] <= (int)Fnotes_tr_split->data[k]);
      if (p) {
        idx->data[k - 1] = k;
        idx->data[k] = k + 1;
      } else {
        idx->data[k - 1] = k + 1;
        idx->data[k] = k;
      }
    }

    if ((Fnotes_tr_split->size[1] & 1) != 0) {
      idx->data[Fnotes_tr_split->size[1] - 1] = Fnotes_tr_split->size[1];
    }

    b_i = 2;
    while (b_i < n - 1) {
      i2 = b_i << 1;
      j = 1;
      for (pEnd = 1 + b_i; pEnd < n; pEnd = qEnd + b_i) {
        b_p = j;
        q = pEnd;
        qEnd = j + i2;
        if (qEnd > n) {
          qEnd = n;
        }

        k = 0;
        kEnd = qEnd - j;
        while (k + 1 <= kEnd) {
          p = ((int)Fnotes_tr_split->data[idx->data[b_p - 1] - 1] <= (int)
               Fnotes_tr_split->data[idx->data[q - 1] - 1]);
          if (p) {
            iwork->data[k] = idx->data[b_p - 1];
            b_p++;
            if (b_p == pEnd) {
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
              while (b_p < pEnd) {
                k++;
                iwork->data[k] = idx->data[b_p - 1];
                b_p++;
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

      b_i = i2;
    }
  }

  for (i = 0; i < 2; i++) {
    uv0[i] = (unsigned int)Fnotes_tr_split->size[i];
  }

  i = notes_gt_011->size[0] * notes_gt_011->size[1];
  notes_gt_011->size[0] = 1;
  notes_gt_011->size[1] = (int)uv0[1];
  emxEnsureCapacity((emxArray__common *)notes_gt_011, i, (int)sizeof(double));
  for (k = 0; k + 1 <= na; k++) {
    notes_gt_011->data[k] = Fnotes_tr_split->data[idx->data[k] - 1];
  }

  k = Fnotes_tr_split->size[1];
  nNaN = Fnotes_tr_split->size[1] - k;
  nInf = (Fnotes_tr_split->size[1] - k) - nNaN;
  b_nb = 0;
  b_k = 0;
  while (b_k + 1 <= k) {
    cdiff = (int)notes_gt_011->data[b_k];
    do {
      exitg5 = 0;
      b_k++;
      if (b_k + 1 > k) {
        exitg5 = 1;
      } else {
        frexp((double)cdiff / 2.0, &c_exponent);
        p = (fabs((double)cdiff - notes_gt_011->data[b_k]) < ldexp(1.0,
              c_exponent - 53));
        if (!p) {
          exitg5 = 1;
        }
      }
    } while (exitg5 == 0);

    b_nb++;
    notes_gt_011->data[b_nb - 1] = cdiff;
  }

  if (nInf > 0) {
    b_nb++;
  }

  for (j = 1; j <= nNaN; j++) {
    b_nb++;
  }

  na = Fnotes_gt_merged->size[1];
  i = idx->size[0] * idx->size[1];
  idx->size[0] = 1;
  emxEnsureCapacity((emxArray__common *)idx, i, (int)sizeof(int));
  nb = Fnotes_gt_merged->size[1];
  i = idx->size[0] * idx->size[1];
  idx->size[1] = nb;
  emxEnsureCapacity((emxArray__common *)idx, i, (int)sizeof(int));
  ndbl = Fnotes_gt_merged->size[1];
  for (i = 0; i < ndbl; i++) {
    idx->data[i] = 0;
  }

  if (Fnotes_gt_merged->size[1] == 0) {
  } else {
    n = Fnotes_gt_merged->size[1] + 1;
    nb = Fnotes_gt_merged->size[1];
    i = iwork->size[0];
    iwork->size[0] = nb;
    emxEnsureCapacity((emxArray__common *)iwork, i, (int)sizeof(int));
    for (k = 1; k <= n - 2; k += 2) {
      if ((Fnotes_gt_merged->data[k - 1] <= Fnotes_gt_merged->data[k]) ||
          rtIsNaN(Fnotes_gt_merged->data[k])) {
        p = true;
      } else {
        p = false;
      }

      if (p) {
        idx->data[k - 1] = k;
        idx->data[k] = k + 1;
      } else {
        idx->data[k - 1] = k + 1;
        idx->data[k] = k;
      }
    }

    if ((Fnotes_gt_merged->size[1] & 1) != 0) {
      idx->data[Fnotes_gt_merged->size[1] - 1] = Fnotes_gt_merged->size[1];
    }

    b_i = 2;
    while (b_i < n - 1) {
      i2 = b_i << 1;
      j = 1;
      for (pEnd = 1 + b_i; pEnd < n; pEnd = qEnd + b_i) {
        b_p = j;
        q = pEnd - 1;
        qEnd = j + i2;
        if (qEnd > n) {
          qEnd = n;
        }

        k = 0;
        kEnd = qEnd - j;
        while (k + 1 <= kEnd) {
          if ((Fnotes_gt_merged->data[idx->data[b_p - 1] - 1] <=
               Fnotes_gt_merged->data[idx->data[q] - 1]) || rtIsNaN
              (Fnotes_gt_merged->data[idx->data[q] - 1])) {
            p = true;
          } else {
            p = false;
          }

          if (p) {
            iwork->data[k] = idx->data[b_p - 1];
            b_p++;
            if (b_p == pEnd) {
              while (q + 1 < qEnd) {
                k++;
                iwork->data[k] = idx->data[q];
                q++;
              }
            }
          } else {
            iwork->data[k] = idx->data[q];
            q++;
            if (q + 1 == qEnd) {
              while (b_p < pEnd) {
                k++;
                iwork->data[k] = idx->data[b_p - 1];
                b_p++;
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

      b_i = i2;
    }
  }

  for (i = 0; i < 2; i++) {
    uv0[i] = (unsigned int)Fnotes_gt_merged->size[i];
  }

  emxInit_real_T(&M_listgt, 2);
  i = M_listgt->size[0] * M_listgt->size[1];
  M_listgt->size[0] = 1;
  M_listgt->size[1] = (int)uv0[1];
  emxEnsureCapacity((emxArray__common *)M_listgt, i, (int)sizeof(double));
  for (k = 0; k + 1 <= na; k++) {
    M_listgt->data[k] = Fnotes_gt_merged->data[idx->data[k] - 1];
  }

  k = 0;
  while ((k + 1 <= na) && rtIsInf(M_listgt->data[k]) && (M_listgt->data[k] < 0.0))
  {
    k++;
  }

  b_k = k;
  k = Fnotes_gt_merged->size[1];
  while ((k >= 1) && rtIsNaN(M_listgt->data[k - 1])) {
    k--;
  }

  nNaN = Fnotes_gt_merged->size[1] - k;
  while ((k >= 1) && rtIsInf(M_listgt->data[k - 1]) && (M_listgt->data[k - 1] >
          0.0)) {
    k--;
  }

  nInf = (Fnotes_gt_merged->size[1] - k) - nNaN;
  nb = -1;
  if (b_k > 0) {
    nb = 0;
  }

  khi = (b_k + k) - b_k;
  while (b_k + 1 <= khi) {
    c_x = M_listgt->data[b_k];
    do {
      exitg4 = 0;
      b_k++;
      if (b_k + 1 > khi) {
        exitg4 = 1;
      } else {
        absxk = fabs(c_x / 2.0);
        if ((!rtIsInf(absxk)) && (!rtIsNaN(absxk))) {
          if (absxk <= 2.2250738585072014E-308) {
            r = 4.94065645841247E-324;
          } else {
            frexp(absxk, &d_exponent);
            r = ldexp(1.0, d_exponent - 53);
          }
        } else {
          r = rtNaN;
        }

        if ((fabs(c_x - M_listgt->data[b_k]) < r) || (rtIsInf(M_listgt->data[b_k])
             && rtIsInf(c_x) && ((M_listgt->data[b_k] > 0.0) == (c_x > 0.0)))) {
          p = true;
        } else {
          p = false;
        }

        if (!p) {
          exitg4 = 1;
        }
      }
    } while (exitg4 == 0);

    nb++;
    M_listgt->data[nb] = c_x;
  }

  if (nInf > 0) {
    nb++;
    M_listgt->data[nb] = M_listgt->data[khi];
  }

  b_k = khi + nInf;
  for (j = 1; j <= nNaN; j++) {
    nb++;
    M_listgt->data[nb] = M_listgt->data[(b_k + j) - 1];
  }

  if (1 > nb + 1) {
    i = 0;
  } else {
    i = nb + 1;
  }

  i1 = M_listgt->size[0] * M_listgt->size[1];
  M_listgt->size[1] = i;
  emxEnsureCapacity((emxArray__common *)M_listgt, i1, (int)sizeof(double));
  na = Fnotes_tr_merged->size[1];
  i1 = idx->size[0] * idx->size[1];
  idx->size[0] = 1;
  emxEnsureCapacity((emxArray__common *)idx, i1, (int)sizeof(int));
  nb = Fnotes_tr_merged->size[1];
  i1 = idx->size[0] * idx->size[1];
  idx->size[1] = nb;
  emxEnsureCapacity((emxArray__common *)idx, i1, (int)sizeof(int));
  ndbl = Fnotes_tr_merged->size[1];
  for (i1 = 0; i1 < ndbl; i1++) {
    idx->data[i1] = 0;
  }

  if (Fnotes_tr_merged->size[1] == 0) {
  } else {
    n = Fnotes_tr_merged->size[1] + 1;
    nb = Fnotes_tr_merged->size[1];
    i1 = iwork->size[0];
    iwork->size[0] = nb;
    emxEnsureCapacity((emxArray__common *)iwork, i1, (int)sizeof(int));
    for (k = 1; k <= n - 2; k += 2) {
      p = ((int)Fnotes_tr_merged->data[k - 1] <= (int)Fnotes_tr_merged->data[k]);
      if (p) {
        idx->data[k - 1] = k;
        idx->data[k] = k + 1;
      } else {
        idx->data[k - 1] = k + 1;
        idx->data[k] = k;
      }
    }

    if ((Fnotes_tr_merged->size[1] & 1) != 0) {
      idx->data[Fnotes_tr_merged->size[1] - 1] = Fnotes_tr_merged->size[1];
    }

    b_i = 2;
    while (b_i < n - 1) {
      i2 = b_i << 1;
      j = 1;
      for (pEnd = 1 + b_i; pEnd < n; pEnd = qEnd + b_i) {
        b_p = j;
        q = pEnd;
        qEnd = j + i2;
        if (qEnd > n) {
          qEnd = n;
        }

        k = 0;
        kEnd = qEnd - j;
        while (k + 1 <= kEnd) {
          p = ((int)Fnotes_tr_merged->data[idx->data[b_p - 1] - 1] <= (int)
               Fnotes_tr_merged->data[idx->data[q - 1] - 1]);
          if (p) {
            iwork->data[k] = idx->data[b_p - 1];
            b_p++;
            if (b_p == pEnd) {
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
              while (b_p < pEnd) {
                k++;
                iwork->data[k] = idx->data[b_p - 1];
                b_p++;
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

      b_i = i2;
    }
  }

  for (i1 = 0; i1 < 2; i1++) {
    uv0[i1] = (unsigned int)Fnotes_tr_merged->size[i1];
  }

  i1 = Fnotes_tr_split->size[0] * Fnotes_tr_split->size[1];
  Fnotes_tr_split->size[0] = 1;
  Fnotes_tr_split->size[1] = (int)uv0[1];
  emxEnsureCapacity((emxArray__common *)Fnotes_tr_split, i1, (int)sizeof(double));
  for (k = 0; k + 1 <= na; k++) {
    Fnotes_tr_split->data[k] = Fnotes_tr_merged->data[idx->data[k] - 1];
  }

  k = Fnotes_tr_merged->size[1];
  nNaN = Fnotes_tr_merged->size[1] - k;
  nInf = (Fnotes_tr_merged->size[1] - k) - nNaN;
  nb = 0;
  emxFree_real_T(&Fnotes_tr_merged);
  b_k = 0;
  while (b_k + 1 <= k) {
    cdiff = (int)Fnotes_tr_split->data[b_k];
    do {
      exitg3 = 0;
      b_k++;
      if (b_k + 1 > k) {
        exitg3 = 1;
      } else {
        frexp((double)cdiff / 2.0, &e_exponent);
        p = (fabs((double)cdiff - Fnotes_tr_split->data[b_k]) < ldexp(1.0,
              e_exponent - 53));
        if (!p) {
          exitg3 = 1;
        }
      }
    } while (exitg3 == 0);

    nb++;
    Fnotes_tr_split->data[nb - 1] = cdiff;
  }

  if (nInf > 0) {
    nb++;
  }

  for (j = 1; j <= nNaN; j++) {
    nb++;
  }

  if (1 > nb) {
    i1 = 0;
  } else {
    i1 = nb;
  }

  if (sNote_gt->size[0] < 1) {
    n = -1;
    apnd = 0;
  } else {
    ndbl = (int)floor(((double)sNote_gt->size[0] - 1.0) + 0.5);
    apnd = ndbl + 1;
    cdiff = (ndbl - sNote_gt->size[0]) + 1;
    absb = sNote_gt->size[0];
    if (fabs((double)cdiff) < 4.4408920985006262E-16 * (double)absb) {
      ndbl++;
      apnd = sNote_gt->size[0];
    } else if (cdiff > 0) {
      apnd = ndbl;
    } else {
      ndbl++;
    }

    n = ndbl - 1;
  }

  cdiff = Fnotes_gt_split->size[0] * Fnotes_gt_split->size[1];
  Fnotes_gt_split->size[0] = 1;
  Fnotes_gt_split->size[1] = n + 1;
  emxEnsureCapacity((emxArray__common *)Fnotes_gt_split, cdiff, (int)sizeof
                    (double));
  if (n + 1 > 0) {
    Fnotes_gt_split->data[0] = 1.0;
    if (n + 1 > 1) {
      Fnotes_gt_split->data[n] = apnd;
      nm1d2 = (n + (n < 0)) >> 1;
      for (k = 1; k < nm1d2; k++) {
        Fnotes_gt_split->data[k] = 1.0 + (double)k;
        Fnotes_gt_split->data[n - k] = apnd - k;
      }

      if (nm1d2 << 1 == n) {
        Fnotes_gt_split->data[nm1d2] = (1.0 + (double)apnd) / 2.0;
      } else {
        Fnotes_gt_split->data[nm1d2] = 1.0 + (double)nm1d2;
        Fnotes_gt_split->data[nm1d2 + 1] = apnd - nm1d2;
      }
    }
  }

  na = Fnotes_gt_detected->size[1];
  cdiff = idx->size[0] * idx->size[1];
  idx->size[0] = 1;
  emxEnsureCapacity((emxArray__common *)idx, cdiff, (int)sizeof(int));
  nb = Fnotes_gt_detected->size[1];
  cdiff = idx->size[0] * idx->size[1];
  idx->size[1] = nb;
  emxEnsureCapacity((emxArray__common *)idx, cdiff, (int)sizeof(int));
  ndbl = Fnotes_gt_detected->size[1];
  for (cdiff = 0; cdiff < ndbl; cdiff++) {
    idx->data[cdiff] = 0;
  }

  if (Fnotes_gt_detected->size[1] == 0) {
  } else {
    n = Fnotes_gt_detected->size[1] + 1;
    nb = Fnotes_gt_detected->size[1];
    cdiff = iwork->size[0];
    iwork->size[0] = nb;
    emxEnsureCapacity((emxArray__common *)iwork, cdiff, (int)sizeof(int));
    for (k = 1; k <= n - 2; k += 2) {
      if ((Fnotes_gt_detected->data[k - 1] <= Fnotes_gt_detected->data[k]) ||
          rtIsNaN(Fnotes_gt_detected->data[k])) {
        p = true;
      } else {
        p = false;
      }

      if (p) {
        idx->data[k - 1] = k;
        idx->data[k] = k + 1;
      } else {
        idx->data[k - 1] = k + 1;
        idx->data[k] = k;
      }
    }

    if ((Fnotes_gt_detected->size[1] & 1) != 0) {
      idx->data[Fnotes_gt_detected->size[1] - 1] = Fnotes_gt_detected->size[1];
    }

    b_i = 2;
    while (b_i < n - 1) {
      i2 = b_i << 1;
      j = 1;
      for (pEnd = 1 + b_i; pEnd < n; pEnd = qEnd + b_i) {
        b_p = j;
        q = pEnd - 1;
        qEnd = j + i2;
        if (qEnd > n) {
          qEnd = n;
        }

        k = 0;
        kEnd = qEnd - j;
        while (k + 1 <= kEnd) {
          if ((Fnotes_gt_detected->data[idx->data[b_p - 1] - 1] <=
               Fnotes_gt_detected->data[idx->data[q] - 1]) || rtIsNaN
              (Fnotes_gt_detected->data[idx->data[q] - 1])) {
            p = true;
          } else {
            p = false;
          }

          if (p) {
            iwork->data[k] = idx->data[b_p - 1];
            b_p++;
            if (b_p == pEnd) {
              while (q + 1 < qEnd) {
                k++;
                iwork->data[k] = idx->data[q];
                q++;
              }
            }
          } else {
            iwork->data[k] = idx->data[q];
            q++;
            if (q + 1 == qEnd) {
              while (b_p < pEnd) {
                k++;
                iwork->data[k] = idx->data[b_p - 1];
                b_p++;
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

      b_i = i2;
    }
  }

  for (cdiff = 0; cdiff < 2; cdiff++) {
    uv0[cdiff] = (unsigned int)Fnotes_gt_detected->size[cdiff];
  }

  cdiff = Fnotes_tr_split->size[0] * Fnotes_tr_split->size[1];
  Fnotes_tr_split->size[0] = 1;
  Fnotes_tr_split->size[1] = (int)uv0[1];
  emxEnsureCapacity((emxArray__common *)Fnotes_tr_split, cdiff, (int)sizeof
                    (double));
  for (k = 0; k + 1 <= na; k++) {
    Fnotes_tr_split->data[k] = Fnotes_gt_detected->data[idx->data[k] - 1];
  }

  k = 0;
  while ((k + 1 <= na) && rtIsInf(Fnotes_tr_split->data[k]) &&
         (Fnotes_tr_split->data[k] < 0.0)) {
    k++;
  }

  b_k = k;
  k = Fnotes_gt_detected->size[1];
  while ((k >= 1) && rtIsNaN(Fnotes_tr_split->data[k - 1])) {
    k--;
  }

  nNaN = Fnotes_gt_detected->size[1] - k;
  while ((k >= 1) && rtIsInf(Fnotes_tr_split->data[k - 1]) &&
         (Fnotes_tr_split->data[k - 1] > 0.0)) {
    k--;
  }

  nInf = (Fnotes_gt_detected->size[1] - k) - nNaN;
  nb = -1;
  emxFree_real_T(&Fnotes_gt_detected);
  if (b_k > 0) {
    nb = 0;
  }

  khi = (b_k + k) - b_k;
  while (b_k + 1 <= khi) {
    c_x = Fnotes_tr_split->data[b_k];
    do {
      exitg2 = 0;
      b_k++;
      if (b_k + 1 > khi) {
        exitg2 = 1;
      } else {
        absxk = fabs(c_x / 2.0);
        if ((!rtIsInf(absxk)) && (!rtIsNaN(absxk))) {
          if (absxk <= 2.2250738585072014E-308) {
            r = 4.94065645841247E-324;
          } else {
            frexp(absxk, &f_exponent);
            r = ldexp(1.0, f_exponent - 53);
          }
        } else {
          r = rtNaN;
        }

        if ((fabs(c_x - Fnotes_tr_split->data[b_k]) < r) || (rtIsInf
             (Fnotes_tr_split->data[b_k]) && rtIsInf(c_x) &&
             ((Fnotes_tr_split->data[b_k] > 0.0) == (c_x > 0.0)))) {
          p = true;
        } else {
          p = false;
        }

        if (!p) {
          exitg2 = 1;
        }
      }
    } while (exitg2 == 0);

    nb++;
    Fnotes_tr_split->data[nb] = c_x;
  }

  if (nInf > 0) {
    nb++;
    Fnotes_tr_split->data[nb] = Fnotes_tr_split->data[khi];
  }

  b_k = khi + nInf;
  for (j = 1; j <= nNaN; j++) {
    nb++;
    Fnotes_tr_split->data[nb] = Fnotes_tr_split->data[(b_k + j) - 1];
  }

  cdiff = Fnotes_tr_split->size[0] * Fnotes_tr_split->size[1];
  if (1 > nb + 1) {
    i3 = -1;
  } else {
    i3 = nb;
  }

  Fnotes_tr_split->size[1] = i3 + 1;
  emxEnsureCapacity((emxArray__common *)Fnotes_tr_split, cdiff, (int)sizeof
                    (double));
  setdiff_simple(Fnotes_gt_split, Fnotes_tr_split, notes_gt_011);
  if (sNote_tr->size[0] < 1) {
    n = -1;
    apnd = 0;
  } else {
    ndbl = (int)floor(((double)sNote_tr->size[0] - 1.0) + 0.5);
    apnd = ndbl + 1;
    cdiff = (ndbl - sNote_tr->size[0]) + 1;
    absb = sNote_tr->size[0];
    if (fabs((double)cdiff) < 4.4408920985006262E-16 * (double)absb) {
      ndbl++;
      apnd = sNote_tr->size[0];
    } else if (cdiff > 0) {
      apnd = ndbl;
    } else {
      ndbl++;
    }

    n = ndbl - 1;
  }

  cdiff = Fnotes_gt_split->size[0] * Fnotes_gt_split->size[1];
  Fnotes_gt_split->size[0] = 1;
  Fnotes_gt_split->size[1] = n + 1;
  emxEnsureCapacity((emxArray__common *)Fnotes_gt_split, cdiff, (int)sizeof
                    (double));
  if (n + 1 > 0) {
    Fnotes_gt_split->data[0] = 1.0;
    if (n + 1 > 1) {
      Fnotes_gt_split->data[n] = apnd;
      nm1d2 = (n + (n < 0)) >> 1;
      for (k = 1; k < nm1d2; k++) {
        Fnotes_gt_split->data[k] = 1.0 + (double)k;
        Fnotes_gt_split->data[n - k] = apnd - k;
      }

      if (nm1d2 << 1 == n) {
        Fnotes_gt_split->data[nm1d2] = (1.0 + (double)apnd) / 2.0;
      } else {
        Fnotes_gt_split->data[nm1d2] = 1.0 + (double)nm1d2;
        Fnotes_gt_split->data[nm1d2 + 1] = apnd - nm1d2;
      }
    }
  }

  na = Fnotes_tr_detected->size[1];
  cdiff = idx->size[0] * idx->size[1];
  idx->size[0] = 1;
  emxEnsureCapacity((emxArray__common *)idx, cdiff, (int)sizeof(int));
  nb = Fnotes_tr_detected->size[1];
  cdiff = idx->size[0] * idx->size[1];
  idx->size[1] = nb;
  emxEnsureCapacity((emxArray__common *)idx, cdiff, (int)sizeof(int));
  ndbl = Fnotes_tr_detected->size[1];
  for (cdiff = 0; cdiff < ndbl; cdiff++) {
    idx->data[cdiff] = 0;
  }

  if (Fnotes_tr_detected->size[1] == 0) {
  } else {
    n = Fnotes_tr_detected->size[1] + 1;
    nb = Fnotes_tr_detected->size[1];
    cdiff = iwork->size[0];
    iwork->size[0] = nb;
    emxEnsureCapacity((emxArray__common *)iwork, cdiff, (int)sizeof(int));
    for (k = 1; k <= n - 2; k += 2) {
      p = ((int)Fnotes_tr_detected->data[k - 1] <= (int)Fnotes_tr_detected->
           data[k]);
      if (p) {
        idx->data[k - 1] = k;
        idx->data[k] = k + 1;
      } else {
        idx->data[k - 1] = k + 1;
        idx->data[k] = k;
      }
    }

    if ((Fnotes_tr_detected->size[1] & 1) != 0) {
      idx->data[Fnotes_tr_detected->size[1] - 1] = Fnotes_tr_detected->size[1];
    }

    b_i = 2;
    while (b_i < n - 1) {
      i2 = b_i << 1;
      j = 1;
      for (pEnd = 1 + b_i; pEnd < n; pEnd = qEnd + b_i) {
        b_p = j;
        q = pEnd;
        qEnd = j + i2;
        if (qEnd > n) {
          qEnd = n;
        }

        k = 0;
        kEnd = qEnd - j;
        while (k + 1 <= kEnd) {
          p = ((int)Fnotes_tr_detected->data[idx->data[b_p - 1] - 1] <= (int)
               Fnotes_tr_detected->data[idx->data[q - 1] - 1]);
          if (p) {
            iwork->data[k] = idx->data[b_p - 1];
            b_p++;
            if (b_p == pEnd) {
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
              while (b_p < pEnd) {
                k++;
                iwork->data[k] = idx->data[b_p - 1];
                b_p++;
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

      b_i = i2;
    }
  }

  emxFree_int32_T(&iwork);
  for (cdiff = 0; cdiff < 2; cdiff++) {
    uv0[cdiff] = (unsigned int)Fnotes_tr_detected->size[cdiff];
  }

  cdiff = Fnotes_tr_split->size[0] * Fnotes_tr_split->size[1];
  Fnotes_tr_split->size[0] = 1;
  Fnotes_tr_split->size[1] = (int)uv0[1];
  emxEnsureCapacity((emxArray__common *)Fnotes_tr_split, cdiff, (int)sizeof
                    (double));
  for (k = 0; k + 1 <= na; k++) {
    Fnotes_tr_split->data[k] = Fnotes_tr_detected->data[idx->data[k] - 1];
  }

  emxFree_int32_T(&idx);
  k = Fnotes_tr_detected->size[1];
  nNaN = Fnotes_tr_detected->size[1] - k;
  nInf = (Fnotes_tr_detected->size[1] - k) - nNaN;
  nb = -1;
  emxFree_real_T(&Fnotes_tr_detected);
  b_k = 0;
  while (b_k + 1 <= k) {
    cdiff = (int)Fnotes_tr_split->data[b_k];
    do {
      exitg1 = 0;
      b_k++;
      if (b_k + 1 > k) {
        exitg1 = 1;
      } else {
        frexp((double)cdiff / 2.0, &g_exponent);
        p = (fabs((double)cdiff - Fnotes_tr_split->data[b_k]) < ldexp(1.0,
              g_exponent - 53));
        if (!p) {
          exitg1 = 1;
        }
      }
    } while (exitg1 == 0);

    nb++;
    Fnotes_tr_split->data[nb] = cdiff;
  }

  if (nInf > 0) {
    nb++;
    Fnotes_tr_split->data[nb] = Fnotes_tr_split->data[k];
  }

  b_k = k + nInf;
  for (j = 1; j <= nNaN; j++) {
    nb++;
    Fnotes_tr_split->data[nb] = Fnotes_tr_split->data[(b_k + j) - 1];
  }

  cdiff = Fnotes_tr_split->size[0] * Fnotes_tr_split->size[1];
  if (1 > nb + 1) {
    i4 = -1;
  } else {
    i4 = nb;
  }

  Fnotes_tr_split->size[1] = i4 + 1;
  emxEnsureCapacity((emxArray__common *)Fnotes_tr_split, cdiff, (int)sizeof
                    (double));
  setdiff_simple(Fnotes_gt_split, Fnotes_tr_split, Fnotes_gt_merged);

  //  ---- Write output struct Results:
  Results->Dur_GT = b_notes_gt->data[b_notes_gt->size[0] - 1] + b_notes_gt->
    data[(b_notes_gt->size[0] + b_notes_gt->size[0]) - 1];
  Results->Dur_TR = b_notes_tr->data[b_notes_tr->size[0] - 1] + b_notes_tr->
    data[(b_notes_tr->size[0] + b_notes_tr->size[0]) - 1];
  Results->N_GT = sNote_gt->size[0];
  Results->N_TR = sNote_tr->size[0];
  cdiff = Results->COnPOff_listgt->size[0] * Results->COnPOff_listgt->size[1];
  Results->COnPOff_listgt->size[0] = 1;
  Results->COnPOff_listgt->size[1] = notes_gt_111->size[1];
  emxEnsureCapacity((emxArray__common *)Results->COnPOff_listgt, cdiff, (int)
                    sizeof(double));
  ndbl = notes_gt_111->size[0] * notes_gt_111->size[1];
  emxFree_real_T(&b_notes_gt);
  emxFree_real_T(&b_notes_tr);
  emxFree_real_T(&Fnotes_gt_split);
  emxFree_real_T(&Fnotes_tr_split);
  for (cdiff = 0; cdiff < ndbl; cdiff++) {
    Results->COnPOff_listgt->data[cdiff] = notes_gt_111->data[cdiff];
  }

  Results->COnPOff_Precision = (double)notes_gt_111->size[1] / (double)
    sNote_gt->size[0];
  Results->COnPOff_Recall = (double)notes_gt_111->size[1] / (double)
    sNote_tr->size[0];
  Results->COnPOff_Fmeasure = 2.0 * (double)notes_gt_111->size[1] / (double)
    ((unsigned int)sNote_gt->size[0] + sNote_tr->size[0]);
  cdiff = Results->COnOff_listgt->size[0] * Results->COnOff_listgt->size[1];
  Results->COnOff_listgt->size[0] = 1;
  Results->COnOff_listgt->size[1] = notes_gt_110->size[1];
  emxEnsureCapacity((emxArray__common *)Results->COnOff_listgt, cdiff, (int)
                    sizeof(double));
  ndbl = notes_gt_110->size[0] * notes_gt_110->size[1];
  emxFree_real_T(&notes_gt_111);
  for (cdiff = 0; cdiff < ndbl; cdiff++) {
    Results->COnOff_listgt->data[cdiff] = notes_gt_110->data[cdiff];
  }

  Results->COnOff_Precision = (double)notes_gt_110->size[1] / (double)
    sNote_gt->size[0];
  Results->COnOff_Recall = (double)notes_gt_110->size[1] / (double)
    sNote_tr->size[0];
  Results->COnOff_Fmeasure = 2.0 * (double)notes_gt_110->size[1] / (double)
    ((unsigned int)sNote_gt->size[0] + sNote_tr->size[0]);
  cdiff = Results->COnP_listgt->size[0] * Results->COnP_listgt->size[1];
  Results->COnP_listgt->size[0] = 1;
  Results->COnP_listgt->size[1] = notes_gt_101->size[1];
  emxEnsureCapacity((emxArray__common *)Results->COnP_listgt, cdiff, (int)sizeof
                    (double));
  ndbl = notes_gt_101->size[0] * notes_gt_101->size[1];
  emxFree_real_T(&notes_gt_110);
  for (cdiff = 0; cdiff < ndbl; cdiff++) {
    Results->COnP_listgt->data[cdiff] = notes_gt_101->data[cdiff];
  }

  Results->COnP_Precision = (double)notes_gt_101->size[1] / (double)
    sNote_gt->size[0];
  Results->COnP_Recall = (double)notes_gt_101->size[1] / (double)sNote_tr->size
    [0];
  Results->COnP_Fmeasure = 2.0 * (double)notes_gt_101->size[1] / (double)
    ((unsigned int)sNote_gt->size[0] + sNote_tr->size[0]);
  cdiff = Results->COn_listgt->size[0] * Results->COn_listgt->size[1];
  Results->COn_listgt->size[0] = 1;
  Results->COn_listgt->size[1] = notes_gt_100->size[1];
  emxEnsureCapacity((emxArray__common *)Results->COn_listgt, cdiff, (int)sizeof
                    (double));
  ndbl = notes_gt_100->size[0] * notes_gt_100->size[1];
  emxFree_real_T(&notes_gt_101);
  for (cdiff = 0; cdiff < ndbl; cdiff++) {
    Results->COn_listgt->data[cdiff] = notes_gt_100->data[cdiff];
  }

  Results->COn_Precision = (double)notes_gt_100->size[1] / (double)
    sNote_gt->size[0];
  Results->COn_Recall = (double)notes_gt_100->size[1] / (double)sNote_tr->size[0];
  Results->COn_Fmeasure = 2.0 * (double)notes_gt_100->size[1] / (double)
    ((unsigned int)sNote_gt->size[0] + sNote_tr->size[0]);
  cdiff = Results->OBOn_listgt->size[0] * Results->OBOn_listgt->size[1];
  Results->OBOn_listgt->size[0] = 1;
  Results->OBOn_listgt->size[1] = notes_gt_011b->size[1];
  emxEnsureCapacity((emxArray__common *)Results->OBOn_listgt, cdiff, (int)sizeof
                    (double));
  ndbl = notes_gt_011b->size[0] * notes_gt_011b->size[1];
  emxFree_real_T(&notes_gt_100);
  for (cdiff = 0; cdiff < ndbl; cdiff++) {
    Results->OBOn_listgt->data[cdiff] = notes_gt_011b->data[cdiff];
  }

  Results->OBOn_rategt = (double)notes_gt_011b->size[1] / (double)sNote_gt->
    size[0];
  cdiff = Results->OBP_listgt->size[0] * Results->OBP_listgt->size[1];
  Results->OBP_listgt->size[0] = 1;
  Results->OBP_listgt->size[1] = notes_gt_110b->size[1];
  emxEnsureCapacity((emxArray__common *)Results->OBP_listgt, cdiff, (int)sizeof
                    (double));
  ndbl = notes_gt_110b->size[0] * notes_gt_110b->size[1];
  emxFree_real_T(&notes_gt_011b);
  for (cdiff = 0; cdiff < ndbl; cdiff++) {
    Results->OBP_listgt->data[cdiff] = notes_gt_110b->data[cdiff];
  }

  Results->OBP_rategt = (double)notes_gt_110b->size[1] / (double)sNote_gt->size
    [0];
  cdiff = Results->OBOff_listgt->size[0] * Results->OBOff_listgt->size[1];
  Results->OBOff_listgt->size[0] = 1;
  Results->OBOff_listgt->size[1] = notes_gt_101b->size[1];
  emxEnsureCapacity((emxArray__common *)Results->OBOff_listgt, cdiff, (int)
                    sizeof(double));
  ndbl = notes_gt_101b->size[0] * notes_gt_101b->size[1];
  emxFree_real_T(&notes_gt_110b);
  for (cdiff = 0; cdiff < ndbl; cdiff++) {
    Results->OBOff_listgt->data[cdiff] = notes_gt_101b->data[cdiff];
  }

  Results->OBOff_rategt = (double)notes_gt_101b->size[1] / (double)
    sNote_gt->size[0];
  cdiff = Results->S_listgt->size[0] * Results->S_listgt->size[1];
  Results->S_listgt->size[0] = 1;
  Results->S_listgt->size[1] = S_listgt->size[1];
  emxEnsureCapacity((emxArray__common *)Results->S_listgt, cdiff, (int)sizeof
                    (double));
  ndbl = S_listgt->size[0] * S_listgt->size[1];
  emxFree_real_T(&notes_gt_101b);
  for (cdiff = 0; cdiff < ndbl; cdiff++) {
    Results->S_listgt->data[cdiff] = S_listgt->data[cdiff];
  }

  emxFree_real_T(&S_listgt);
  Results->S_rategt = (double)i0 / (double)sNote_gt->size[0];
  if (1 > b_nb) {
    i5 = 0;
  } else {
    i5 = b_nb;
  }

  Results->S_ratio = (double)i5 / (double)i0;
  i0 = Results->M_listgt->size[0] * Results->M_listgt->size[1];
  Results->M_listgt->size[0] = 1;
  Results->M_listgt->size[1] = M_listgt->size[1];
  emxEnsureCapacity((emxArray__common *)Results->M_listgt, i0, (int)sizeof
                    (double));
  ndbl = M_listgt->size[0] * M_listgt->size[1];
  for (i0 = 0; i0 < ndbl; i0++) {
    Results->M_listgt->data[i0] = M_listgt->data[i0];
  }

  emxFree_real_T(&M_listgt);
  Results->M_rategt = (double)i / (double)sNote_gt->size[0];
  Results->M_ratio = (double)i1 / (double)i;
  i0 = Results->PU_listtr->size[0] * Results->PU_listtr->size[1];
  Results->PU_listtr->size[0] = 1;
  Results->PU_listtr->size[1] = Fnotes_gt_merged->size[1];
  emxEnsureCapacity((emxArray__common *)Results->PU_listtr, i0, (int)sizeof
                    (double));
  ndbl = Fnotes_gt_merged->size[0] * Fnotes_gt_merged->size[1];
  for (i0 = 0; i0 < ndbl; i0++) {
    Results->PU_listtr->data[i0] = Fnotes_gt_merged->data[i0];
  }

  emxFree_real_T(&Fnotes_gt_merged);
  Results->PU_ratetr = (double)i1 / (double)sNote_tr->size[0];
  i0 = Results->ND_listgt->size[0] * Results->ND_listgt->size[1];
  Results->ND_listgt->size[0] = 1;
  Results->ND_listgt->size[1] = notes_gt_011->size[1];
  emxEnsureCapacity((emxArray__common *)Results->ND_listgt, i0, (int)sizeof
                    (double));
  ndbl = notes_gt_011->size[0] * notes_gt_011->size[1];
  emxFree_struct_T(&sNote_tr);
  for (i0 = 0; i0 < ndbl; i0++) {
    Results->ND_listgt->data[i0] = notes_gt_011->data[i0];
  }

  Results->ND_rategt = (double)notes_gt_011->size[1] / (double)sNote_gt->size[0];
  emxFree_real_T(&notes_gt_011);
  b_emxFree_struct_T(&sNote_gt);
}

//
// File trailer for classifyNotes.cpp
//
// [EOF]
//
