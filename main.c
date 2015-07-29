/* ----------------------------------------------------------------------------
  main.c

  Copyright (c)
---------------------------------------------------------------------------- */
#include <ga-mpi/GASimpleGA.h>      // we're going to use the simple GA
#include <ga-mpi/GA1DBinStrGenome.h> // and the 1D binary string genome
#include <ga-mpi/std_stream.h>
#include <time.h>
#include <stdint.h>
#include <cstring>
#include <sstream>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <vector>
#include <string>
#include <array>
#include <map>
#include <unistd.h>
#include <chrono>
#include <thread>
#include "rt_nonfinite.h"
#include "classifyNotes.h"
#include "classifyNotes_terminate.h"
#include "classifyNotes_emxAPI.h"
#include "classifyNotes_initialize.h"
#include "mpi.h"

#define cout STD_COUT

#define POPSIZE  100
#define NGEN     300
#define PMUT     0.001
#define PCROSS   0.9

#define ONSET_LIM          0.05   // secs % Default parameters (used in MIREX):
#define DUR_PERCENT_RANGE  20     // percentage
#define MIN_DUR_DIST       0.05   // secs
#define F0_RANGE_IN_CENTS  50     // cents
#define HOPSIZE            0.01   // secs


#define NFOLD_GA        3         // n-fold cross-validation for GA training (upper level)
#define NFOLD_HMM       3         // n-fold cross-validation for HMM training(lower level)

#define FILE_MSDINFO         "train/msdInfo"
#define FILE_GROUNDTRUTH     "groundtruth.mlf"
#define FILE_DICT            "dict"
#define FILE_GRAMMAR         "grammar"
#define FILE_WDNET           "wdnet"
#define FILE_MONOPHONES      "monophones"
#define FILE_TRAIN           "train.scp"
#define FILE_TEST            "test.scp"

#define NOTE_NSTATES       3
#define SIL_NSTATES        2
#define SP_NSTATES         1

#define NOTE_NMIXES       2
#define SIL_NMIXES        1
#define SP_NMIXES         1

int NFEAT;
int mpi_tasks, mpi_rank;

std::string run_dir;
std::string run_number;
std::string config_file;
std::map<std::string, emxArray_real_T *> ground_truths;
std::vector<int> msdInfo;


float Objective(GAGenome &);    // This is the declaration of our obj function.
                                // The definition comes later in the file.
float Objective_exception_free(GAGenome &);

std::string make_proto(int nStates, int nMixes, const std::vector<int> msdInfo, const GA1DBinaryStringGenome &genome);
std::string make_dur_mmf (int nStates, const std::vector<std::string> &monophones);

void read_proto(const std::string &, const std::string &);
void write_macros(const std::string &);
void write_hmmdefs(const std::string &, const std::vector<double> &, const std::vector<std::string> &);

void _inner_my_system(const char *cmd) {
  cout << cmd << "\n";cout.flush();
  int retcode = system(cmd);
  if (retcode) {
    cout << "!!!!!!! ERROR IN EXECUTING " << cmd << "  RETCODE: " << retcode << "\n";
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


int
main(int argc, char **argv)
{
  // MPI init
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &mpi_tasks);
  MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);


  clock_t t;
  t = clock();    // start time

  cout << "GA";
  cout << "\n";
  cout << "\n\n"; cout.flush();

// See if we've been given a seed to use (for testing purposes).  When you
// specify a random seed, the evolution will be exactly the same each time
// you use that seed number.

  if (argc < 2) {
    cout << "Please provide run #.\n";
    return -1;
  }

  // seed
  for(int ii=2; ii<argc; ii++) {
    if(strcmp(argv[ii++],"seed") == 0) {
      GARandomSeed((unsigned int)atoi(argv[ii]));
    }
  }

  // load groundtruths
  classifyNotes_initialize();
  load_mlf(FILE_GROUNDTRUTH, &ground_truths);


  // read feature numbers
  std::ifstream fmsdInfo(FILE_MSDINFO);
  int tmp;
  while (fmsdInfo >> tmp) {
    msdInfo.push_back(tmp);
  }
  NFEAT = msdInfo.size();
  cout << "nFeat: " << NFEAT << "\n";


  // config run dir
  run_number = argv[1];
  cout << "Run #: " << run_number << "\n";
  run_dir = "GArun_";run_dir += run_number;

  config_file = run_dir+"/config";


  if (mpi_rank != 0) {   // other threads: wait a bit for main thread
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  } else {  // main thread
    std::string cmd_mkdir = "mkdir ";
    my_system(cmd_mkdir+" "+run_dir);

    std::string cmd = "touch ";
    cmd += run_dir;
    my_system((cmd+"/genes").c_str());
    my_system((cmd+"/results").c_str());


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



    ////// write duration vFloor
    /*std::string fn_hlist = run_dir + "/hlist";
    temp.str("");
    temp << "HList -C " << config_file << " -S train.scp  -h -z > " << fn_hlist;
    cout << temp.str() << "\n"; cout.flush();
    my_system(temp.str().c_str());

    std::ifstream fhlist(fn_hlist);
    std::string base;
    double dsum1 = 0, dsum2 = 0;
    int dnum = 0;
    while (std::getline(fhlist, line)) {
      std::size_t pos1 = line.find(" Source: ");
      std::size_t pos2 = line.find("Num Samples:");

      if (pos1 != std::string::npos) {
        std::size_t pos_end_space = line.find(' ', pos1 + 9);
        std::size_t pos_end_tab = line.find('\t', pos1 + 9);
        std::size_t pos_end = ((pos_end_space != std::string::npos) ? pos_end_space : pos_end_tab);

        std::size_t pos_base_start = line.rfind('/', pos_end) + 1;
        std::size_t pos_base_end = line.rfind('.', pos_end);
        base = line.substr(pos_base_start, pos_base_end-pos_base_start);
        cout << base << " ";
      } else if (pos2 != std::string::npos) {
        std::size_t pos_start = pos2 + 14;
        std::size_t pos_end = line.find("File Format:");
        int nframe = std::stoi(line.substr(pos_start, pos_end-pos_start));
        int nlab = nlabs[base];
        double dtmp = (float)nframe / (float)(nlab * 5); // 5 : nState
        dsum1 += dtmp;
        dsum2 += dtmp * dtmp;
        dnum += 1;
      }
    }
    cout << "\n";
    fhlist.close();
    double dmean = dsum1 / dnum;
    double dvari = dsum2 / dnum - dmean * dmean;

    std::ofstream fvFloorsdur((run_dir+"/hmm0/dur.vFloors").c_str());
    for (int i=1; i<=1; i++) {
      fvFloorsdur << "~v varFloor" << i << "\n"
                  << "<Variance> 1\n"
                  << dvari*0.01 << "\n";
    }
    fvFloorsdur.close();

    std::ofstream fprotodur((run_dir+"/hmm0/dur.average").c_str());
    fprotodur << "~o <STREAMINFO> 1 1\n"
              << "<VECSIZE> 1<NULLD><USER><DIAGC>\n"
              << "~h dur.average\n"
              << "<BEGINHMM>\n"
              << "<NUMSTATES> 3\n"
              << "<STATE> 2\n";
    for (int i=1; i<=1; i++) {
      fprotodur << "  <STREAM> " << i << "\n"
                << "    <MEAN> 1\n"
                << "      " << dmean << "\n"
                << "    <VARIANCE> 1\n"
                << "      " << dvari << "\n";
    }
    fprotodur << "<TRANSP> 3\n"
              << " 0.0 1.0 0.0\n"
              << " 0.0 0.0 1.0\n"
              << " 0.0 0.0 0.0\n"
              << "<ENDHMM>\n";
    fprotodur.close();
    */
  }


// Now create the GA and run it.  First we create a genome of the type that
// we want to use in the GA.  The ga doesn't operate on this genome in the
// optimization - it just uses it to clone a population of genomes.

  GA1DBinaryStringGenome genome(NFEAT, Objective);//_exception_free);

// Now that we have the genome, we create the genetic algorithm and set
// its parameters - number of generations, mutation probability, and crossover
// probability.  And finally we tell it to evolve itself.

  GASimpleGA ga(genome);

  // popsize / mpi_tasks must be an integer
  int popsize = mpi_tasks * int((double)POPSIZE/(double)mpi_tasks+0.999);
  ga.populationSize(POPSIZE);

  ga.nGenerations(NGEN);
  ga.pMutation(PMUT);
  ga.pCrossover(PCROSS);
  ga.elitist(gaTrue);

  if (mpi_rank == 0)
    ga.scoreFilename((run_dir+"/evolution.txt").c_str());
  else
    ga.scoreFilename("/dev/null");
  ga.scoreFrequency(1);
  ga.flushFrequency(1);
  ga.selectScores(GAStatistics::AllScores);
  ga.mpi_rank(mpi_rank);
  ga.mpi_tasks(mpi_tasks);
  ga.evolve();

  if (mpi_rank == 0) {
    // Now we print out the best genome that the GA found.
    cout << "The GA found:\n" << ga.statistics().bestIndividual() << "\n";

    t = clock() - t;
    cout << "Time elapsed: " << (float)t/CLOCKS_PER_SEC << "\n\n";
  }

  // That's it!
  classifyNotes_terminate();
  MPI_Finalize();
  return 0;
}


float Objective_exception_free(GAGenome &g) {
  try {
    return Objective(g);
  } catch (std::exception &e) {
    cout << e.what() << "\n";
    return 0.0;
  }
}


float
Objective(GAGenome &g) {
  GA1DBinaryStringGenome &genome = (GA1DBinaryStringGenome &)g;
  std::stringstream temp;

  clock_t t;
  t = clock();
  uintmax_t id_thread = (uintmax_t)t;
  temp << mpi_rank << "_" << id_thread;
  std::string id = temp.str();
  std::string working_dir = run_dir + "/" + id;

  temp.str("");
  temp << "mkdir " << working_dir;
  my_system(temp);

  cout << run_number << " " << id << " " << genome << "\n";

  //////// write genes
  {
    std::vector<double> weights;
    std::stringstream gene_ss;
    for (int i=0; i<NFEAT; i++) {
      gene_ss << genome.gene(i);
      weights.push_back(genome.gene(i));
    }
    temp.str("");
    temp << "echo " << "\"" << id << " " << gene_ss.str() << "\" >> " << run_dir << "/genes";
    cout << temp.str() << "\n"; cout.flush();
    my_system(temp);
  }


  /////////////////////////////////////////
  // 0. prepare starting HMMs
  /////////////////////////////////////////
  temp.str("");
  temp << "mkdir " << working_dir << "/hmm0";
  my_system(temp);

  // make proto  note
  std::ofstream fproto_note((working_dir+"/hmm0/proto_note").c_str());
  fproto_note << make_proto(NOTE_NSTATES, NOTE_NMIXES, msdInfo, genome);
  fproto_note.close();

  // HCompV
  temp.str("");
  temp << "HCompV"
       << " -C " << config_file
       << " -f 0.01"
       << " -m"
       << " -S " << FILE_TRAIN
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
  fproto_sil << make_proto(SIL_NSTATES, SIL_NMIXES, msdInfo, genome);
  fproto_sil.close();

  // HCompV
  temp.str("");
  temp << "HCompV"
       << " -C " << config_file
       << " -f 0.01"
       << " -m"
       << " -S " << FILE_TRAIN
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



  // make proto
  std::ofstream fproto_sp((working_dir+"/hmm0/proto_sp").c_str());
  fproto_sp << make_proto(SP_NSTATES, SP_NMIXES, msdInfo, genome);
  fproto_sp.close();

  // HCompV
  temp.str("");
  temp << "HCompV"
       << " -C " << config_file
       << " -f 0.01"
       << " -m"
       << " -S " << FILE_TRAIN
       << " -M " << working_dir << "/hmm0"
       << " -o proto_sp.average"
       << " " << working_dir << "/hmm0/proto_sp";
  my_system(temp);

  // Build init mmf file
  {
    std::ifstream ffeat_proto((working_dir+"/hmm0/proto_sp").c_str());
    std::string line;
    std::getline(ffeat_proto, line);
    ffeat_proto.close();
    std::ofstream ffeat_init((working_dir+"/hmm0/proto_sp.macro").c_str());
    ffeat_init << line << "\n";
    std::ifstream vFloors((working_dir+"/hmm0/vFloors").c_str());
    while (std::getline(vFloors, line)) {
      ffeat_init << line << "\n";
    }
    ffeat_init.close();
  }


  // HInit for all
  temp.str("");
  temp << "HInit"
       << " -C " << config_file
       << " -H " << working_dir << "/hmm0/proto_note.macro"
       << " -M " << working_dir << "/hmm0"
       << " -I " << FILE_GROUNDTRUTH
       << " -l NOTE"
       << " -o NOTE"
       << " -S " << FILE_TRAIN
       << " -w 5000"
       << " " << working_dir << "/hmm0/proto_note";
  my_system(temp);

  temp.str("");
  temp << "HInit"
       << " -C " << config_file
       << " -H " << working_dir << "/hmm0/proto_sil.macro"
       << " -M " << working_dir << "/hmm0"
       << " -I " << FILE_GROUNDTRUTH
       << " -l SIL"
       << " -o SIL"
       << " -S " << FILE_TRAIN
       << " -w 5000"
       << " " << working_dir << "/hmm0/proto_sil";
  my_system(temp);

  temp.str("");
  temp << "HInit"
       << " -C " << config_file
       << " -H " << working_dir << "/hmm0/proto_sp.macro"
       << " -M " << working_dir << "/hmm0"
       << " -I " << FILE_GROUNDTRUTH
       << " -l SP"
       << " -o SP"
       << " -S " << FILE_TRAIN
       << " -w 5000"
       << " " << working_dir << "/hmm0/proto_sp";
  my_system(temp);

  // HHEd   make a monophone mmf (feat)
  // [TODO]   tie stream weights
  std::ofstream ffeat_lvf((working_dir+"/hmm0/feat.lvf.hed").c_str());
  ffeat_lvf << "FV " << working_dir << "/hmm0/vFloors" << "\n";
  ffeat_lvf.close();
  temp.str("");
  temp << "HHEd"
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
  temp << "HERest"
    //<< " -T 1"
       << " -C " << config_file
       << " -I " << FILE_GROUNDTRUTH
       << " -t 250.0 200.0 10500.0"
       << " -S " << FILE_TRAIN
       << " -H " << working_dir << "/hmm0/feat.monophones"
    //<< " -N " << working_dir << "/hmm0/dur.monophones"
       << " -M " << working_dir << "/hmm1"
    //<< "  monophones"
       << " " << FILE_MONOPHONES;
  my_system(temp);

  // HERest (1->2) ////////////////////////
  my_system(cmd_mkdir + working_dir + "/hmm2");
  temp.str("");
  temp << "HERest"
       << " -C " << config_file
       << " -I " << FILE_GROUNDTRUTH
       << " -t 250.0 200.0 10500.0"
       << " -S " << FILE_TRAIN
       << " -H " << working_dir << "/hmm1/feat.monophones"
    //<< " -N " << working_dir << "/hmm0/dur.monophones"
       << " -M " << working_dir << "/hmm2"
    //<< "  monophones"
       << " " << FILE_MONOPHONES;
  my_system(temp);

  // HERest (2->3) ////////////////////////
  my_system(cmd_mkdir + working_dir + "/hmm3");
  temp.str("");
  temp << "HERest"
       << " -C " << config_file
       << " -I " << FILE_GROUNDTRUTH
       << " -t 250.0 200.0 10500.0"
       << " -S " << FILE_TRAIN
       << " -H " << working_dir << "/hmm2/feat.monophones"
    //<< " -N " << working_dir << "/hmm0/dur.monophones"
       << " -M " << working_dir << "/hmm3"
    //<< "  monophones"
       << " " << FILE_MONOPHONES;
  my_system(temp);


  // HVite generate test set ////////////////////
  temp.str("");
  temp << "HVite"
       << " -C " << config_file
       << " -H " << working_dir << "/hmm3/feat.monophones"
       << " -S " << FILE_TEST
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
  int sum_COn_size = 0;
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
    sum_COn_size += (int) Results.COn_listgt->size[1];

    /*
    cout << "N GT: " << Results.N_GT << "\n"
         << "N TR: " << Results.N_TR << "\n"
         << "Correct Onset & Offset: \n"
         << "  Count: " << " " << Results.COnPOff_listgt->size[1] << "\n"
         << "  Precision: " << Results.COnPOff_Precision << "\n"
         << "  Recall: " << Results.COnPOff_Recall << "\n"
         << "  Fmeasure: " << Results.COnPOff_Fmeasure << "\n"
         << "Correct Onset: \n"
         << "  Count: " << " " << Results.COnP_listgt->size[1] << "\n"
         << "  Precision: " << Results.COnP_Precision << "\n"
         << "  Recall: " << Results.COnP_Recall << "\n"
         << "  Fmeasure: " << Results.COnP_Fmeasure << "\n"
         << "Correct Onset (2): \n"
         << "  Count: " << " " << Results.COn_listgt->size[1] << "\n"
         << "  Precision: " << Results.COn_Precision << "\n"
         << "  Recall: " << Results.COn_Recall << "\n"
         << "  Fmeasure: " << Results.COn_Fmeasure << "\n"
         << "Wrong Pitch: \n"
         << "  Rate: " << Results.OBP_rategt << "\n";
    */

    emxDestroy_struct0_T(Results);
  }


  //// deallocate MATLAB matrices
  for(std::map<std::string, emxArray_real_T *>::iterator iterator = results.begin(); iterator != results.end(); iterator++) {
    emxArray_real_T *matrix = iterator->second;
    emxDestroyArray_real_T(matrix);
  }

  double f_measure = 2.0 * sum_COn_size / (sum_N_GT + sum_N_TR);
  cout << "Score: " << f_measure << "\n";


  //////// write results
  temp.str("");
  temp << "echo " << "\"" << id << " " << f_measure << "\" >> " << run_dir << "/results";
  cout << temp.str() << "\n"; cout.flush();
  my_system(temp.str().c_str());

  return f_measure;
}




std::string make_proto(int nStates, int nMixes, const std::vector<int> msdInfo, const GA1DBinaryStringGenome &genome) {
  std::stringstream proto;
  proto << "~o <VecSize> " << NFEAT << " <USER><DIAGC> "
        << "   <StreamInfo> " << NFEAT;
  for (int i=0; i<NFEAT; i++) {
    proto << " " << "1";
  }
  proto << " "
        << "   <MSDInfo> " << NFEAT;
  for (int i=0; i<NFEAT; i++) {
    proto << " " << msdInfo[i];
  }
  proto << "\n"
        << " <BeginHMM>\n"
        << "   <NumStates> " << nStates+2 << "\n";

  // States
  for (int k=2;k<=nStates+1;k++) {
    // SWeights
    proto << "   <State> " << k << "\n"
          << "     <SWeights> " << NFEAT;
    for (int i=0; i<NFEAT; i++) {
      proto << " " << genome.gene(i);
    }
    proto << "\n";

    // Values
    for (int i=0; i<NFEAT; i++) {
      if (msdInfo[i] == 0) {
        float mixprob = 1.0/nMixes;
        proto << "      <Stream> " << i+1 << "\n"
              << "        <NumMixes> " << nMixes << "\n";
        for (int j=1; j<=nMixes; j++) {
          proto << "          <Mixture> " << j << " " << mixprob << "\n"
                << "            <Mean> 1 0.0\n"
                << "            <Variance> 1 1.0\n";
        }
      } else {
        float mixprob = 1.0/(nMixes+1);
        proto << "      <Stream> " << i+1 << "\n"
              << "        <NumMixes> " << nMixes+1 << "\n";
        for (int j=1; j<=nMixes; j++) {
          proto << "          <Mixture> " << j << " " << mixprob << "\n"
                << "            <Mean> 1 0.0\n"
                << "            <Variance> 1 1.0\n";
        }
        proto << "          <Mixture> " << nMixes+1 << " " << mixprob << "\n"
              << "            <Mean> 0\n"
              << "            <Variance> 0\n";
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

/*
std::string make_dur_mmf (int nStates, const std::vector<std::string> &monophones) {
  std::stringstream s;
  s << "~o\n"
    << "<STREAMINFO> " << nStates;
  for (int i=0; i<nStates; i++) s << " " << "1";
  s << "\n"
    << "<MSDINFO> " << nStates;
  for (int i=0; i<nStates; i++) s << " " << "0";
  s << "\n"
    << "<VECSIZE> " << nStates << " <NULLD><DIAGC><USER>\n";

  s << "~v \"varFloor1\"\n"
    << "<VARIANCE> 1 5e+03\n"
    << "~v \"varFloor2\"\n"
    << "<VARIANCE> 1 5e+03\n"
    << "~v \"varFloor3\"\n"
    << "<VARIANCE> 1 5e+03\n";

  for (int i=0; i<monophones.size(); i++) {
    s << "~h " << '"' << monophones[i] << '"' << "\n"
      << "<BEGINHMM>\n"
      << "  <NUMSTATES> 3\n"
      << "    <STATE> 2\n";
    for (int j=1; j<=nStates; j++) {
      if (monophones[i].compare("ONSET") == 0) {
        s << "    <STREAM> " << j << "\n"
          << "       <MEAN> 1\n"
          << "         " << ONSET_DUR/3.0 << "\n"
          << "       <VARIANCE> 1\n"
          << "         " << ONSET_DUR/9.0 << "\n";
      } else {
        s << "    <STREAM> " << j << "\n"
          << "       <MEAN> 1\n"
          << "         " << 1.0 << "\n"
          << "       <VARIANCE> 1\n"
          << "         " << 3.0 << "\n";
      }
    }
    s << "  <TRANSP> " << nStates << "\n";
    for (int j=1; j<nStates; j++) {
      s << "    ";
      for (int k=0; k<j; k++) {
        s << " 0";
      }
      s << " 1";
      for (int k=j+1; k<nStates; k++) {
        s << " 0";
      }
      s << "\n";
    }
    s << "    ";
    for (int j=0; j<nStates; j++) {
      s << " 0";
    }
    s << "\n";
    s << "<ENDHMM>\n";
  }
  return s.str();
}
*/
