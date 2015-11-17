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
#include <algorithm>
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
#define NGEN     100
#define PMUT     0.001
#define PCROSS   0.9

#define ONSET_LIM          0.05   // secs % Default parameters (used in MIREX):
#define DUR_PERCENT_RANGE  20     // percentage
#define MIN_DUR_DIST       0.05   // secs
#define F0_RANGE_IN_CENTS  50     // cents
#define HOPSIZE            0.01   // secs


//#define NFOLD        3         // n-fold cross-validation for splitting the given training set.
#define OUTER_TRAINING_SET_PERCENT    0.25   // 25% as outer training set

#define FILE_MSDINFO         "train/msdInfo"
#define FILE_GROUNDTRUTH     "groundtruth.mlf"
#define FILE_DICT            "dict"
#define FILE_GRAMMAR         "grammar"
#define FILE_WDNET           "wdnet"
#define FILE_MONOPHONES      "monophones"

std::string FILE_TRAIN = "";
std::vector<std::string> train_set;
std::string inner_scp;
std::string outer_scp;

#define NOTE_NSTATES       3
#define SIL_NSTATES        1
#define SP_NSTATES        1

#define NOTE_NMIXES       1
#define SIL_NMIXES        1
#define SP_NMIXES         1

#define USE_HTK_FLAVOR     0   // 0 for HTS, 1 for HTK
//#define LOCAL_SHUFFLE      0

#define HTK_PATH_PREFIX    "" //"/Users/lyang/htk-original/HTKTools/"  // with trailing slash

int NFEAT;
int mpi_tasks, mpi_rank;
int gene_count = 0;  // count progress

std::string run_dir;
std::string run_number;
std::string config_file;
std::map<std::string, emxArray_real_T *> ground_truths;
std::vector<int> msdInfo;
std::string log_file;

void log( const std::string &text )
{
    std::ofstream flog(
        log_file, std::ios_base::app );
    flog << text;
}
const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}

float Objective(GAGenome &);    // This is the declaration of our obj function.
                                // The definition comes later in the file.
float Objective_exception_free(GAGenome &);

std::string make_proto(int nStates, int nMixes, const std::vector<int> msdInfo, const GA1DBinaryStringGenome &genome, int useHTKFlavor);
std::string make_dur_mmf (int nStates, const std::vector<std::string> &monophones);

void read_proto(const std::string &, const std::string &);
void write_macros(const std::string &);
void write_hmmdefs(const std::string &, const std::vector<double> &, const std::vector<std::string> &);

std::string genome2str(const GA1DBinaryStringGenome &);

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


class MyGASimpleGA: public GASimpleGA {
  // extend original class to write results for genes
  using GASimpleGA::GASimpleGA;

 public:
  std::string my_generation_file;

  virtual void evolve(unsigned int seed=0){
    initialize(seed);
    while(!done()){
      if (!my_generation_file.empty()) {
        my_write_genes_in_generation();// write exact genes
      }
      step();
    }
    if(stats.flushFrequency() > 0) stats.flushScores();
  }
  void my_write_genes_in_generation() {
    std::ofstream gen_file(
        my_generation_file, std::ios_base::out | std::ios_base::app );
    gen_file << stats.generation() << " ";
    for (int i=0; i<pop->size(); i++) {
      gen_file << " " << genome2str((const GA1DBinaryStringGenome &)pop->individual(i));
    }
    gen_file << "\n";
  }
};


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

// See if we've been given a seed to use (for testing purposes).  When you
// specify a random seed, the evolution will be exactly the same each time
// you use that seed number.

  if (argc < 4) {
    cout << "Please provide run #, seed, train scp\n";
    return -1;
  }

  std::srand(unsigned(atoi(argv[2])));
  GARandomSeed((unsigned int)atoi(argv[2]));

  // config run dir
  run_number = argv[1];
  run_dir = "GArun_";run_dir += run_number;

  config_file = run_dir+"/config";
  log_file = run_dir+"/log";
  inner_scp = run_dir+"/inner_train.scp";
  outer_scp = run_dir+"/outer_train.scp";


  // read train set
  FILE_TRAIN = argv[3];
  {
    std::ifstream f(FILE_TRAIN);
    std::string line;
    while ( std::getline(f, line) ) {
      if ( !line.empty() )
        train_set.push_back(line);
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



  if (mpi_rank != 0) {   // other threads: wait a bit for main thread
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
  } else {  // main thread
    std::string cmd_mkdir = "mkdir ";
    my_system(cmd_mkdir+" "+run_dir);

    std::string cmd = "touch ";
    cmd += run_dir;
    my_system((cmd+"/genes").c_str());
    my_system((cmd+"/results").c_str());

    // Split into inner and outer sets.
    std::random_shuffle(train_set.begin(), train_set.end());  // global shuffle
    std::ofstream finner_scp(inner_scp.c_str());
    std::ofstream fouter_scp(outer_scp.c_str());
    for (std::vector<std::string>::size_type i = 0; i != train_set.size(); i++) {
      if (i < train_set.size()*OUTER_TRAINING_SET_PERCENT) {
        fouter_scp << train_set[i] << "\n";
      } else {
        finner_scp << train_set[i] << "\n";
      }
    }
    finner_scp.close();
    fouter_scp.close();

    log("\n\n");
    log("===================================\n");
    std::stringstream temp;
    temp << "Start time: " << currentDateTime() << "\n"
         << "Run #: " << run_number << "\n"
         << "Random seed: " << argv[2] << "\n"
         << "NFEAT: " << NFEAT << "\n"
         << "Train set size: " << train_set.size() << "\n";
    log(temp.str());


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

  GA1DBinaryStringGenome genome(NFEAT, Objective_exception_free);

// Now that we have the genome, we create the genetic algorithm and set
// its parameters - number of generations, mutation probability, and crossover
// probability.  And finally we tell it to evolve itself.

  MyGASimpleGA ga(genome);

  // popsize / mpi_tasks must be an integer
  int popsize = mpi_tasks * int((double)POPSIZE/(double)mpi_tasks+0.999);
  ga.populationSize(popsize);

  ga.nGenerations(NGEN);
  ga.pMutation(PMUT);
  ga.pCrossover(PCROSS);
  ga.elitist(gaTrue);

  if (mpi_rank == 0) {
    ga.my_generation_file = run_dir+"/generationInfo.txt";
    ga.scoreFilename((run_dir+"/evolution.txt").c_str());
  } else {
    ga.scoreFilename("/dev/null");
  }
  ga.scoreFrequency(1);
  ga.flushFrequency(1);
  ga.selectScores(GAStatistics::AllScores);
  ga.mpi_rank(mpi_rank);
  ga.mpi_tasks(mpi_tasks);
  ga.evolve();

  if (mpi_rank == 0) {
    // Now we print out the best genome that the GA found.
    std::stringstream temp;
    temp << "Best candidate: " << ga.statistics().bestIndividual() << "\n"
         << "End time: " << currentDateTime() << "\n";
    log(temp.str());


    std::ofstream fbest(
        run_dir+"/best_genome", std::ios_base::out | std::ios_base::app );
    fbest << ga.statistics().bestIndividual();
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

  gene_count += 1;

  //if (mpi_rank == 0) {
    temp.str("");
    temp << gene_count << "(on thread #" << mpi_rank << ") start: " << currentDateTime() << "\n";
    log(temp.str());
    //}

  //////// write genes
  std::stringstream gene_ss;
  std::vector<double> weights;
  for (int i=0; i<NFEAT; i++) {
    gene_ss << genome.gene(i);
    weights.push_back(genome.gene(i));
  }
  temp.str("");
  temp << "echo " << "\"" << id << " " << gene_ss.str() << "\" >> " << run_dir << "/genes";
  cout << temp.str() << "\n"; cout.flush();
  my_system(temp);

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

  /*
  // split folds randomly
  if (LOCAL_SHUFFLE) {
    std::random_shuffle(train_set.begin(), train_set.end());
  }
  int fold_size = train_set.size() / (int)NFOLD;
  int n_folds_with_one_more = train_set.size() % (int)NFOLD;
  int fold_pointer = 0;
  */
  int NFOLD = 1;  // remove it

  for (int l=1; l<=NFOLD; l++) {
    // mkdir
    temp.str("");
    temp << run_dir << "/" << id ;//<< "_fold" << l;
    std::string working_dir = temp.str();

    temp.str("");
    temp << "mkdir " << working_dir;
    my_system(temp);
    /*
    // set up outer/inner training sets
    // outer: fold_pointer..fold_pointer+this_fold_size
    // inner: 0..fold_pointer + fold_pointer+this_fold_size..train_set.size()
    int this_fold_size = (l<=n_folds_with_one_more) ? (fold_size+1) : fold_size;
    std::string inner_scp = working_dir + "/inner.scp";
    std::string outer_scp = working_dir + "/outer.scp";
    std::ofstream finner_scp(inner_scp.c_str());
    std::ofstream fouter_scp(outer_scp.c_str());

    for (std::vector<std::string>::size_type i = 0; i != train_set.size(); i++) {
      if (i < fold_pointer) {
        finner_scp << train_set[i] << "\n";
      } else if (i < fold_pointer+this_fold_size) {
        fouter_scp << train_set[i] << "\n";
      } else {
        finner_scp << train_set[i] << "\n";
      }
    }
    fold_pointer += this_fold_size;

    finner_scp.close();
    fouter_scp.close();*/

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
         << " -S " << inner_scp
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
         << " -S " << inner_scp
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
    fproto_sp << make_proto(SP_NSTATES, SP_NMIXES, msdInfo, genome, USE_HTK_FLAVOR);
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
    temp << HTK_PATH_PREFIX << "HInit"
         << " -C " << config_file
         << " -H " << working_dir << "/hmm0/proto_note.macro"
         << " -M " << working_dir << "/hmm0"
         << " -I " << FILE_GROUNDTRUTH
         << " -l NOTE"
         << " -o NOTE"
         << " -S " << inner_scp
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
         << " -S " << inner_scp
         << " -w 5000"
         << " " << working_dir << "/hmm0/proto_sil";
    my_system(temp);

    temp.str("");
    temp << HTK_PATH_PREFIX << "HInit"
         << " -C " << config_file
         << " -H " << working_dir << "/hmm0/proto_sp.macro"
         << " -M " << working_dir << "/hmm0"
         << " -I " << FILE_GROUNDTRUTH
         << " -l SP"
         << " -o SP"
         << " -S " << inner_scp
         << " -w 5000"
         << " " << working_dir << "/hmm0/proto_sp";
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
         << " -S " << inner_scp
         << " -H " << working_dir << "/hmm0/feat.monophones"
      //<< " -N " << working_dir << "/hmm0/dur.monophones"
         << " -M " << working_dir << "/hmm1"
      //<< "  monophones"
         << " " << FILE_MONOPHONES;
    my_system(temp);

    // HERest (1->2) ////////////////////////
    my_system(cmd_mkdir + working_dir + "/hmm3");
    temp.str("");
    temp << HTK_PATH_PREFIX << "HERest"
         << " -C " << config_file
         << " -I " << FILE_GROUNDTRUTH
         << " -t 250.0 200.0 10500.0"
         << " -S " << inner_scp
         << " -H " << working_dir << "/hmm1/feat.monophones"
      //<< " -N " << working_dir << "/hmm0/dur.monophones"
         << " -M " << working_dir << "/hmm3"
      //<< "  monophones"
         << " " << FILE_MONOPHONES;
    my_system(temp);
    /*
    // HERest (2->3) ////////////////////////
    my_system(cmd_mkdir + working_dir + "/hmm3");
    temp.str("");
    temp << HTK_PATH_PREFIX << "HERest"
         << " -C " << config_file
         << " -I " << FILE_GROUNDTRUTH
         << " -t 250.0 200.0 10500.0"
         << " -S " << inner_scp
         << " -H " << working_dir << "/hmm2/feat.monophones"
      //<< " -N " << working_dir << "/hmm0/dur.monophones"
         << " -M " << working_dir << "/hmm3"
      //<< "  monophones"
         << " " << FILE_MONOPHONES;
    my_system(temp);
    */

    // HVite generate test set ////////////////////
    temp.str("");
    temp << HTK_PATH_PREFIX << "HVite"
         << " -C " << config_file
         << " -H " << working_dir << "/hmm3/feat.monophones"
         << " -S " << outer_scp
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

    /// evaluation measures for this fold
    acc_COnPOff_Precision += (double)sum_COnPOff_size / (double)sum_N_GT;
    acc_COnPOff_Recall += (double)sum_COnPOff_size / (double)sum_N_TR;
    acc_COnPOff_Fmeasure += 2.0 * (double)sum_COnPOff_size / (double)(sum_N_GT + sum_N_TR);

    acc_COnOff_Precision += (double)sum_COnOff_size / (double)sum_N_GT;
    acc_COnOff_Recall += (double)sum_COnOff_size / (double)sum_N_TR;
    acc_COnOff_Fmeasure += 2.0 * (double)sum_COnOff_size / (double)(sum_N_GT + sum_N_TR);

    acc_COnP_Precision += (double)sum_COnP_size / (double)sum_N_GT;
    acc_COnP_Recall += (double)sum_COnP_size / (double)sum_N_TR;
    acc_COnP_Fmeasure += 2.0 * (double)sum_COnP_size / (double)(sum_N_GT + sum_N_TR);

    acc_COn_Precision += (double)sum_COn_size / (double)sum_N_GT;
    acc_COn_Recall += (double)sum_COn_size / (double)sum_N_TR;
    acc_COn_Fmeasure += 2.0 * (double)sum_COn_size / (double)(sum_N_GT + sum_N_TR);

    acc_OBOn_rate += (double)sum_OBOn_size / (double)sum_N_GT;
    acc_OBP_rate += (double)sum_OBP_size / (double)sum_N_GT;
    acc_OBOff_rate += (double)sum_OBOff_size / (double)sum_N_GT;

    acc_S_rate += (double)sum_S_listgt_size / (double)sum_N_GT;
    //acc_S_ratio += (double)sum_S_listtr_size / (double)sum_S_listgt_size;

    acc_M_rate += (double)sum_M_listgt_size / (double)sum_N_GT;
    //acc_M_ratio += (double)sum_M_listtr_size / (double)sum_M_listgt_size;

    acc_PU_rate += (double)sum_PU_size / (double)sum_N_TR;
    acc_ND_rate += (double)sum_ND_size / (double)sum_N_GT;
  }
  // average scores.
  double avg_COnPOff_Precision = acc_COnPOff_Precision / NFOLD;
  double avg_COnPOff_Recall = acc_COnPOff_Recall / NFOLD;
  double avg_COnPOff_Fmeasure = acc_COnPOff_Fmeasure / NFOLD;

  double avg_COnOff_Precision = acc_COnOff_Precision / NFOLD;
  double avg_COnOff_Recall = acc_COnOff_Recall / NFOLD;
  double avg_COnOff_Fmeasure = acc_COnOff_Fmeasure / NFOLD;

  double avg_COnP_Precision = acc_COnP_Precision / NFOLD;
  double avg_COnP_Recall = acc_COnP_Recall / NFOLD;
  double avg_COnP_Fmeasure = acc_COnP_Fmeasure / NFOLD;

  double avg_COn_Precision = acc_COn_Precision / NFOLD;
  double avg_COn_Recall = acc_COn_Recall / NFOLD;
  double avg_COn_Fmeasure = acc_COn_Fmeasure / NFOLD;

  double avg_OBOn_rate = acc_OBOn_rate / NFOLD;
  double avg_OBP_rate = acc_OBP_rate / NFOLD;
  double avg_OBOff_rate = acc_OBOff_rate / NFOLD;

  double avg_S_rate = acc_S_rate / NFOLD;
  //double avg_S_ratio = acc_S_ratio / NFOLD;

  double avg_M_rate = acc_M_rate / NFOLD;
  //double avg_M_ratio = acc_M_ratio / NFOLD;

  double avg_PU_rate = acc_PU_rate / NFOLD;
  double avg_ND_rate = acc_ND_rate / NFOLD;

  //////// write results
  temp.str("");
  temp << "echo " << "\"" << gene_ss.str()
       << " " << "COnPOff_P=" << avg_COnPOff_Precision
       << " " << "COnPOff_R=" << avg_COnPOff_Recall
       << " " << "COnPOff_F=" << avg_COnPOff_Fmeasure

       << " " << "COnOff_P=" << avg_COnOff_Precision
       << " " << "COnOff_R=" << avg_COnOff_Recall
       << " " << "COnOff_F=" << avg_COnOff_Fmeasure

       << " " << "COnP_P=" << avg_COnP_Precision
       << " " << "COnP_R=" << avg_COnP_Recall
       << " " << "COnP_F=" << avg_COnP_Fmeasure

       << " " << "COn_P=" << avg_COn_Precision
       << " " << "COn_R=" << avg_COn_Recall
       << " " << "COn_F=" << avg_COn_Fmeasure

       << " " << "OBOn=" << avg_OBOn_rate
       << " " << "OBP=" << avg_OBP_rate
       << " " << "OBOff=" << avg_OBOff_rate

       << " " << "S=" << avg_S_rate
       << " " << "M=" << avg_M_rate
       << " " << "PU=" << avg_PU_rate
       << " " << "ND=" << avg_ND_rate

       << "\" >> " << run_dir << "/results";
  //cout << temp.str() << "\n"; cout.flush();
  my_system(temp.str().c_str());

  //if (mpi_rank == 0) {
    temp.str("");
    temp << gene_count << "(on thread #" << mpi_rank << ") end: " << currentDateTime() << "\n";
    log(temp.str());
    //}

  return avg_COnPOff_Fmeasure;
}




std::string make_proto(int nStates, int nMixes, const std::vector<int> msdInfo, const GA1DBinaryStringGenome &genome, int useHTKFlavor) {
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
      proto << " " << genome.gene(i);
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

std::string
genome2str(const GA1DBinaryStringGenome &genome) {
  std::stringstream gene_ss;
  for (int i=0; i<genome.length(); i++) {
    gene_ss << genome.gene(i);
  }
  return gene_ss.str();
}
