#include "classifyNotes.h"
#include "classifyNotes_terminate.h"
#include "classifyNotes_emxAPI.h"
#include "classifyNotes_initialize.h"
#include <time.h>
#include <stdint.h>
#include <cstring>
#include <sstream>
#include <cstdlib>
#include <exception>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <map>
#include <unistd.h>
#include <chrono>
#include <thread>
#include "rt_nonfinite.h"

#define ONSET_LIM          0.05   // secs % Default parameters (used in MIREX):
#define DUR_PERCENT_RANGE  20     // percentage
#define MIN_DUR_DIST       0.05   // secs
#define F0_RANGE_IN_CENTS  50     // cents
#define HOPSIZE            0.01   // secs


#define NOTE_NSTATES       3
#define SIL_NSTATES        1

#define NOTE_NMIXES       1
#define SIL_NMIXES        1    // SIL for rests

#define USE_HTK_FLAVOR     0   // 0 for HTS, 1 for HTK
#define HTK_PATH_PREFIX    "" //"/Users/lyang/htk-original/HTKTools/"  // with trailing slash

#define FILE_MSDINFO         "train/msdInfo"
#define FILE_GROUNDTRUTH     "groundtruth.mlf"
#define FILE_DICT            "dict"
#define FILE_GRAMMAR         "grammar"
#define FILE_WDNET           "wdnet"
#define FILE_MONOPHONES      "monophones"


void load_mlf(const char *filename, std::map<std::string, emxArray_real_T *> *map) {
  std::ifstream f(filename);
  std::string line;
  std::string current = "";
  std::vector<std::array<double, 2> > starts_and_ends;
  while (std::getline(f, line)) {
    if (line == "#!MLF!#" || line == ".") {
      // store last file
      if (!current.empty()) {
        //cout << "Storing " << current << " from " << filename << '\n';

        if (map) {
          emxArray_real_T *matrix;
          int size[2] = {static_cast<int>(starts_and_ends.size()), 3};
          matrix = emxCreateND_real_T(2, *(int (*)[2])&size[0]);

          //cout << matrix->size[0] << " " << matrix->size[1] << "\n";

          for (int i = 0; i < matrix->size[0]; i++) {
            matrix->data[i + matrix->size[0]*0] = starts_and_ends[i][0];
            matrix->data[i + matrix->size[0]*1] = starts_and_ends[i][1];
            matrix->data[i + matrix->size[0]*2] = 1;  // unify pitch
            //cout << matrix->data[i + matrix->size[0]*0] << " " << matrix->data[i + matrix->size[0]*1] << " " << matrix->data[i + matrix->size[0]*2] << "\n";
          }
          (*map)[current] = matrix;
        }
      }

      // next line is the filename
      if (!std::getline(f, line)) {
        // file ended.
        break;
      }
      if (line.empty()) {
        // file ended.
        break;
      }

      // prepare new file
      starts_and_ends.clear();
      std::string::size_type start = line.find('/')+1;
      std::string::size_type end = line.find_last_of('.');
      current = line.substr(start, end-start);
      //cout << "Loading " << current << " from " << filename << '\n';

    } else if (!current.empty() && !line.empty()) {
      std::istringstream iss(line);
      int start, end;
      std::string word;
      if (iss >> start >> end >> word) {
        // HTK uses the unit 100ns
        if (word == "NOTE") {
          std::array<double, 2> se = {start/10000000.0, end/10000000.0};
          starts_and_ends.push_back(se);
        }
      } else {
        //cout << "Unrecognized line, ignoring: " << line << "\n";
      }
    }
  }
}


void _inner_my_system(const char *cmd) {
  std::cout << cmd << "\n";std::cout.flush();
  int retcode = system(cmd);
  if (retcode) {
    std::cout << "!!!!!!! ERROR IN EXECUTING " << cmd << "  RETCODE: " << retcode << "\n";
    throw std::exception();
  }
}
void my_system(const char *cmd) {
  _inner_my_system(cmd);
}
void my_system(const std::string &cmd) {
  _inner_my_system(cmd.c_str());
}
void my_system(const std::stringstream &cmd) {
  _inner_my_system(cmd.str().c_str());
}


std::string make_proto(int nStates, int nMixes, const std::vector<int> msdInfo, const std::string &genome, int useHTKFlavor);

std::vector<int> msdInfo;
int NFEAT;
std::map<std::string, emxArray_real_T *> ground_truths;

int
main(int argc, char **argv)
{
  classifyNotes_initialize();
  load_mlf(FILE_GROUNDTRUTH, &ground_truths);

  if (argc < 4) {
    std::cout << "Please provide feature subset genome, train scp, test scp\n";
    return -1;
  }

  std::string genome = argv[1];
  std::string train_scp = argv[2];
  std::string test_scp = argv[3];


  std::stringstream temp;
  std::string working_dir = "GAevaluation_";
  working_dir += genome;

  temp.str("");
  temp << "mkdir " << working_dir;
  my_system(temp);

  // read feature numbers
  std::ifstream fmsdInfo(FILE_MSDINFO);
  int tmp;
  while (fmsdInfo >> tmp) {
    msdInfo.push_back(tmp);
  }
  NFEAT = msdInfo.size();

  std::string config_file = working_dir+"/config";
  // write HTK config file
  std::ofstream htk_config(config_file.c_str());
  htk_config << "APPLYVFLOOR = T" << "\n"
             << "NATURALREADORDER = T" << "\n"
             << "NATURALWRITEORDER = T" << "\n"
             << "VFLOORSCALE = 0.01" << "\n"
    //<< "VFLOORSCALESTR = \"Vector " << NFEAT << "\"" << "\n";
             << "DURVARFLOORPERCENTILE = 1.0" << "\n"
             << "APPLYDURVARFLOOR = T" << "\n";

  htk_config.close();

  /////////////////////////////////////////
  // 0. prepare starting HMMs
  /////////////////////////////////////////
  temp.str("");
  temp << "mkdir " << working_dir << "/hmm0";
  my_system(temp);

  // make proto  note
  std::ofstream fproto_note((working_dir+"/hmm0/proto_note").c_str());
  fproto_note << make_proto(NOTE_NSTATES, NOTE_NMIXES, msdInfo, genome, USE_HTK_FLAVOR);
  fproto_note.close();

  // HCompV
  temp.str("");
  temp << HTK_PATH_PREFIX << "HCompV"
       << " -C " << config_file
       << " -f 0.01"
       << " -m"
       << " -S " << train_scp
       << " -M " << working_dir << "/hmm0"
       << " -o proto_note.average"
       << " " << working_dir << "/hmm0/proto_note";
  my_system(temp);

  // Build init mmf file
  std::ifstream ffeat_proto((working_dir+"/hmm0/proto_note").c_str());
  std::string line;
  std::getline(ffeat_proto, line);
  ffeat_proto.close();
  std::ofstream ffeat_init((working_dir+"/hmm0/proto_note.macro").c_str());
  ffeat_init << line << "\n";
  std::ifstream vFloors((working_dir+"/hmm0/vFloors").c_str());
  while (std::getline(vFloors, line)) {
    ffeat_init << line << "\n";
  }
  ffeat_init.close();


  // make proto  sil
  std::ofstream fproto_sil((working_dir+"/hmm0/proto_sil").c_str());
  fproto_sil << make_proto(SIL_NSTATES, SIL_NMIXES, msdInfo, genome, USE_HTK_FLAVOR);
  fproto_sil.close();

  // HCompV
  temp.str("");
  temp << HTK_PATH_PREFIX << "HCompV"
       << " -C " << config_file
       << " -f 0.01"
       << " -m"
       << " -S " << train_scp
       << " -M " << working_dir << "/hmm0"
       << " -o proto_sil.average"
       << " " << working_dir << "/hmm0/proto_sil";
  my_system(temp);

  // Build init mmf file
  {
    std::ifstream ffeat_proto((working_dir+"/hmm0/proto_sil").c_str());
    std::string line;
    std::getline(ffeat_proto, line);
    ffeat_proto.close();
    std::ofstream ffeat_init((working_dir+"/hmm0/proto_sil.macro").c_str());
    ffeat_init << line << "\n";
    std::ifstream vFloors((working_dir+"/hmm0/vFloors").c_str());
    while (std::getline(vFloors, line)) {
      ffeat_init << line << "\n";
    }
    ffeat_init.close();
  }



  // HInit for all
  temp.str("");
  temp << HTK_PATH_PREFIX << "HInit"
       << " -C " << config_file
       << " -H " << working_dir << "/hmm0/proto_note.macro"
       << " -M " << working_dir << "/hmm0"
       << " -I " << FILE_GROUNDTRUTH
       << " -l NOTE"
       << " -o NOTE"
       << " -S " << train_scp
       << " -w 5000"
       << " " << working_dir << "/hmm0/proto_note";
  my_system(temp);

  temp.str("");
  temp << HTK_PATH_PREFIX << "HInit"
       << " -C " << config_file
       << " -H " << working_dir << "/hmm0/proto_sil.macro"
       << " -M " << working_dir << "/hmm0"
       << " -I " << FILE_GROUNDTRUTH
       << " -l SIL"
       << " -o SIL"
       << " -S " << train_scp
       << " -w 5000"
       << " " << working_dir << "/hmm0/proto_sil";
  my_system(temp);

  // HHEd   make a monophone mmf (feat)
  // [TODO]   tie stream weights
  std::ofstream ffeat_lvf((working_dir+"/hmm0/feat.lvf.hed").c_str());
  ffeat_lvf << "FV " << working_dir << "/hmm0/vFloors" << "\n";
  ffeat_lvf.close();
  temp.str("");
  temp /*<< HTK_PATH_PREFIX*/ << "HHEd"
         << " -C " << config_file
         << " -p"
         << " -i"
         << " -d " << working_dir << "/hmm0"
         << " -w " << working_dir << "/hmm0/feat.monophones"
         << " " << working_dir << "/hmm0/feat.lvf.hed"
         << " " << FILE_MONOPHONES;
  my_system(temp);
  /*
  // Making duration monophone mmf
  std::ofstream fdur_mmf((working_dir+"/hmm0/dur.monophones").c_str());
  std::vector<std::string> monophones = {"ONSET", "NOT_ONSET"};
  fdur_mmf << make_dur_mmf(3, monophones);
  fdur_mmf.close();
  */

  std::string cmd_mkdir = "mkdir ";

  // HERest (0->1) ////////////////////////
  my_system(cmd_mkdir + working_dir + "/hmm1");
  temp.str("");
  temp << HTK_PATH_PREFIX << "HERest"
    //<< " -T 1"
       << " -C " << config_file
       << " -I " << FILE_GROUNDTRUTH
       << " -t 250.0 200.0 10500.0"
       << " -S " << train_scp
       << " -H " << working_dir << "/hmm0/feat.monophones"
    //<< " -N " << working_dir << "/hmm0/dur.monophones"
       << " -M " << working_dir << "/hmm1"
    //<< "  monophones"
       << " " << FILE_MONOPHONES;
  my_system(temp);

  // HERest (1->2) ////////////////////////
  my_system(cmd_mkdir + working_dir + "/hmm2");
  temp.str("");
  temp << HTK_PATH_PREFIX << "HERest"
       << " -C " << config_file
       << " -I " << FILE_GROUNDTRUTH
       << " -t 250.0 200.0 10500.0"
       << " -S " << train_scp
       << " -H " << working_dir << "/hmm1/feat.monophones"
    //<< " -N " << working_dir << "/hmm0/dur.monophones"
       << " -M " << working_dir << "/hmm2"
    //<< "  monophones"
       << " " << FILE_MONOPHONES;
  my_system(temp);

  // HERest (2->3) ////////////////////////
  my_system(cmd_mkdir + working_dir + "/hmm3");
  temp.str("");
  temp << HTK_PATH_PREFIX << "HERest"
       << " -C " << config_file
       << " -I " << FILE_GROUNDTRUTH
       << " -t 250.0 200.0 10500.0"
       << " -S " << train_scp
       << " -H " << working_dir << "/hmm2/feat.monophones"
    //<< " -N " << working_dir << "/hmm0/dur.monophones"
       << " -M " << working_dir << "/hmm3"
    //<< "  monophones"
       << " " << FILE_MONOPHONES;
  my_system(temp);


  // HVite generate test set ////////////////////
  temp.str("");
  temp << HTK_PATH_PREFIX << "HVite"
       << " -C " << config_file
       << " -H " << working_dir << "/hmm3/feat.monophones"
       << " -S " << test_scp
       << " -l '*' "
       << " -i " << working_dir << "/test.mlf"
       << " -w " << FILE_WDNET
       << " -p 0.0"
       << " -s 5.0"
       << " " << FILE_DICT
       << " " << FILE_MONOPHONES;
  my_system(temp);


  // Evaluation Framework ////////////////
  temp.str("");
  temp << working_dir << "/result";
  std::string fnresults = temp.str();

  std::map<std::string, emxArray_real_T *> results;
  load_mlf((working_dir+"/test.mlf").c_str(), &results);

  int sum_N_GT = 0;
  int sum_N_TR = 0;

  int sum_COnPOff_size = 0;
  int sum_COnOff_size = 0;
  int sum_COnP_size = 0;
  int sum_COn_size = 0;

  int sum_OBOn_size = 0;
  int sum_OBP_size = 0;
  int sum_OBOff_size = 0;

  int sum_S_listgt_size = 0;
  //int sum_S_listtr_size = 0;
  int sum_M_listgt_size = 0;
  //int sum_M_listtr_size = 0;
  int sum_PU_size = 0;
  int sum_ND_size = 0;

  int count = 0;

  for(std::map<std::string, emxArray_real_T *>::iterator iterator = results.begin(); iterator != results.end(); iterator++) {  // loop all test excerpts
    std::string key = iterator->first;
    emxArray_real_T *result_matrix = iterator->second;
    emxArray_real_T *gt_matrix = ground_truths[key];

    struct0_T Results;
    emxInit_struct0_T(&Results);
    classifyNotes(gt_matrix, result_matrix, ONSET_LIM, DUR_PERCENT_RANGE, MIN_DUR_DIST, F0_RANGE_IN_CENTS, HOPSIZE, &Results);

    count += 1;
    sum_N_GT += (int) Results.N_GT;
    sum_N_TR += (int) Results.N_TR;

    sum_COnPOff_size += (int) Results.COnPOff_listgt->size[1];
    sum_COnOff_size += (int) Results.COnOff_listgt->size[1];
    sum_COnP_size += (int) Results.COnP_listgt->size[1];
    sum_COn_size += (int) Results.COn_listgt->size[1];

    sum_OBOn_size += (int) Results.OBOn_listgt->size[1];
    sum_OBP_size += (int) Results.OBP_listgt->size[1];
    sum_OBOff_size += (int) Results.OBOff_listgt->size[1];

    sum_S_listgt_size += (int) Results.S_listgt->size[1];
    //sum_S_listtr_size += (int) Results.S_listtr->size[1];
    sum_M_listgt_size += (int) Results.M_listgt->size[1];
    //sum_M_listtr_size += (int) Results.M_listtr->size[1];
    sum_PU_size += (int) Results.PU_listtr->size[1];
    sum_ND_size += (int) Results.ND_listgt->size[1];

    //cout << (int) Results.COnPOff_listgt->size[1] << (int) Results.OBP_listgt->size[1] << (int) Results.COnOff_listgt->size[1] << "\n";

    emxDestroy_struct0_T(Results);
  }

  //// deallocate MATLAB matrices
  for(std::map<std::string, emxArray_real_T *>::iterator iterator = results.begin(); iterator != results.end(); iterator++) {
    emxArray_real_T *matrix = iterator->second;
    emxDestroyArray_real_T(matrix);
  }

  // accumulators for evaluation measures
  double acc_COnPOff_Precision = 0.0;
  double acc_COnPOff_Recall = 0.0;
  double acc_COnPOff_Fmeasure = 0.0;

  double acc_COnOff_Precision = 0.0;
  double acc_COnOff_Recall = 0.0;
  double acc_COnOff_Fmeasure = 0.0;

  double acc_COnP_Precision = 0.0;
  double acc_COnP_Recall = 0.0;
  double acc_COnP_Fmeasure = 0.0;

  double acc_COn_Precision = 0.0;
  double acc_COn_Recall = 0.0;
  double acc_COn_Fmeasure = 0.0;

  double acc_OBOn_rate = 0.0;
  double acc_OBP_rate = 0.0;
  double acc_OBOff_rate = 0.0;

  double acc_S_rate = 0.0;
  //double acc_S_ratio = 0.0;

  double acc_M_rate = 0.0;
  //double acc_M_ratio = 0.0;

  double acc_PU_rate = 0.0;
  double acc_ND_rate = 0.0;

  /// evaluation measures for this fold
  acc_COnPOff_Precision = (double)sum_COnPOff_size / (double)sum_N_GT;
  acc_COnPOff_Recall = (double)sum_COnPOff_size / (double)sum_N_TR;
  acc_COnPOff_Fmeasure = 2.0 * (double)sum_COnPOff_size / (double)(sum_N_GT + sum_N_TR);

  acc_COnOff_Precision = (double)sum_COnOff_size / (double)sum_N_GT;
  acc_COnOff_Recall = (double)sum_COnOff_size / (double)sum_N_TR;
  acc_COnOff_Fmeasure = 2.0 * (double)sum_COnOff_size / (double)(sum_N_GT + sum_N_TR);

  acc_COnP_Precision = (double)sum_COnP_size / (double)sum_N_GT;
  acc_COnP_Recall = (double)sum_COnP_size / (double)sum_N_TR;
  acc_COnP_Fmeasure = 2.0 * (double)sum_COnP_size / (double)(sum_N_GT + sum_N_TR);

  acc_COn_Precision = (double)sum_COn_size / (double)sum_N_GT;
  acc_COn_Recall = (double)sum_COn_size / (double)sum_N_TR;
  acc_COn_Fmeasure = 2.0 * (double)sum_COn_size / (double)(sum_N_GT + sum_N_TR);

  acc_OBOn_rate = (double)sum_OBOn_size / (double)sum_N_GT;
  acc_OBP_rate = (double)sum_OBP_size / (double)sum_N_GT;
  acc_OBOff_rate = (double)sum_OBOff_size / (double)sum_N_GT;

  acc_S_rate = (double)sum_S_listgt_size / (double)sum_N_GT;
  //acc_S_ratio = (double)sum_S_listtr_size / (double)sum_S_listgt_size;

  acc_M_rate = (double)sum_M_listgt_size / (double)sum_N_GT;
  //acc_M_ratio = (double)sum_M_listtr_size / (double)sum_M_listgt_size;

  acc_PU_rate = (double)sum_PU_size / (double)sum_N_TR;
  acc_ND_rate = (double)sum_ND_size / (double)sum_N_GT;

  std::cout << "RESULTS: " << "\n"
            << " " << "COnPOff_P=" << acc_COnPOff_Precision << "\n"
            << " " << "COnPOff_R=" << acc_COnPOff_Recall << "\n"
            << " " << "COnPOff_F=" << acc_COnPOff_Fmeasure << "\n"

    //<< " " << "COnOff_P=" << acc_COnOff_Precision << "\n"
    //<< " " << "COnOff_R=" << acc_COnOff_Recall << "\n"
    //<< " " << "COnOff_F=" << acc_COnOff_Fmeasure << "\n"

            << " " << "COnP_P=" << acc_COnP_Precision << "\n"
            << " " << "COnP_R=" << acc_COnP_Recall << "\n"
            << " " << "COnP_F=" << acc_COnP_Fmeasure << "\n"

    //<< " " << "COn_P=" << acc_COn_Precision << "\n"
    //<< " " << "COn_R=" << acc_COn_Recall << "\n"
    //<< " " << "COn_F=" << acc_COn_Fmeasure << "\n"

            << " " << "OBOn=" << acc_OBOn_rate << "\n"
            << " " << "OBP=" << acc_OBP_rate << "\n"
            << " " << "OBOff=" << acc_OBOff_rate << "\n"

            << " " << "S=" << acc_S_rate << "\n"
            << " " << "M=" << acc_M_rate << "\n"
            << " " << "PU=" << acc_PU_rate << "\n"
            << " " << "ND=" << acc_ND_rate << "\n";
  classifyNotes_terminate();
  return 0;
}




std::string make_proto(int nStates, int nMixes, const std::vector<int> msdInfo, const std::string &genome, int useHTKFlavor) {
  std::stringstream proto;
  proto << "~o <VecSize> " << NFEAT << " <USER><DIAGC> "
        << "   <StreamInfo> " << NFEAT;
  for (int i=0; i<NFEAT; i++) {
    proto << " " << "1";
  }
  if (!useHTKFlavor) {
    proto << " "
          << "   <MSDInfo> " << NFEAT;
    for (int i=0; i<NFEAT; i++) {
      proto << " " << msdInfo[i];
    }
  }
  proto << "\n"
        << " <BeginHMM>\n"
        << "   <NumStates> " << nStates+2 << "\n";

  // States
  for (int k=2;k<=nStates+1;k++) {
    proto << "   <State> " << k << "\n";

    if (useHTKFlavor) {
      // if HTK flavor, write msdinfo here as Mix numbers
      proto << "     <NumMixes> ";
      for (int i=0; i<NFEAT; i++) {
        if (msdInfo[i]) { // is MSD: one more mix
          proto << nMixes+1 << " ";
        } else {
          proto << nMixes << " ";
        }
      }
      proto << "\n";
    }

    // SWeights
    proto << "     <SWeights> " << NFEAT;
    for (int i=0; i<NFEAT; i++) {
      proto << " " << genome.at(i);
    }
    proto << "\n";

    // Values
    for (int i=0; i<NFEAT; i++) {
      if (msdInfo[i] == 0) {
        float mixprob = 1.0/nMixes;
        proto << "      <Stream> " << i+1 << "\n";
        if (!useHTKFlavor) {
          proto << "        <NumMixes> " << nMixes << "\n";
        }
        for (int j=1; j<=nMixes; j++) {
          proto << "          <Mixture> " << j << " " << mixprob << "\n"
                << "            <Mean> 1 0.0\n"
                << "            <Variance> 1 1.0\n";
        }
      } else {
        float mixprob = 1.0/(nMixes+1);
        proto << "      <Stream> " << i+1 << "\n";
        if (!useHTKFlavor) {
          proto << "        <NumMixes> " << nMixes+1 << "\n";
        }
        for (int j=1; j<=nMixes; j++) {
          proto << "          <Mixture> " << j << " " << mixprob << "\n"
                << "            <Mean> 1 0.0\n"
                << "            <Variance> 1 1.0\n";
        }
        if (!useHTKFlavor) {
          proto << "          <Mixture> " << nMixes+1 << " " << mixprob << "\n"
                << "            <Mean> 0\n"
                << "            <Variance> 0\n";
        } else {
          proto << "          <Mixture> " << nMixes+1 << " " << mixprob << "\n"
                << "            <Mean> 1 1.0\n"
                << "            <Variance> 1 1.0\n";
        }
      }
    }
  }

  // TransP (left to right)
  proto << "   <TransP> " << nStates+2 << "\n"
        << "     0.0 1.0";
  for (int k=2;k<nStates+2;k++) {
    proto << " 0.0";
  }
  proto << "\n";

  double self_transition_p[5] = {0.9, 0.99, 0.999, 0.9999, 0.99999};

  for (int k=2;k<=nStates+1;k++) {
    proto << "    ";
    for (int i=1;i<k;i++) {
      proto << " 0.0";
    }
    proto << " " << self_transition_p[k-2] << " " << 1-self_transition_p[k-2];
    for (int i=k+1;i<nStates+2;i++) {
      proto << " 0.0";
    }
    proto << "\n";
  }
  proto << "    ";
  for (int j=0; j<nStates+2; j++) {
    proto << " 0.0";
  }
  proto << "\n";
  proto << " <EndHMM>\n";
  return proto.str();
}
