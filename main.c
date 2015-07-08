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

#define NFEAT    93
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


int mpi_tasks, mpi_rank;

std::string run_dir;
std::string run_number;
std::string config_file;
std::map<std::string, emxArray_real_T *> ground_truths;



float Objective(GAGenome &);    // This is the declaration of our obj function.
                                // The definition comes later in the file.
float Objective_exception_free(GAGenome &);

void read_proto(const std::string &, const std::string &);
void write_macros(const std::string &);
void write_hmmdefs(const std::string &, const std::vector<double> &, const std::vector<std::string> &);

void my_system(const char *cmd) {
  int retcode = system(cmd);
  if (retcode) {
    cout << "!!!!!!! ERROR IN EXECUTING " << cmd << "  RETCODE: " << retcode << "\n";
    throw std::exception();
  }
}

void load_mlf(const char *filename, std::map<std::string, emxArray_real_T *> &map) {
  std::ifstream f(filename);
  std::string line;
  std::string current = "";
  std::vector<std::array<double, 2> > starts_and_ends;
  while (std::getline(f, line)) {
    if (line == "#!MLF!#" || line == ".") {
      // store last file
      if (!current.empty()) {
        //cout << "Storing " << current << " from " << filename << '\n';
        emxArray_real_T *matrix;
        int size[2] = {static_cast<int>(starts_and_ends.size()), 3};
        matrix = emxCreateND_real_T(2, *(int (*)[2])&size[0]);

        cout << matrix->size[0] << " " << matrix->size[1] << "\n";

        for (int i = 0; i < matrix->size[0]; i++) {
          matrix->data[i + matrix->size[0]*0] = starts_and_ends[i][0];
          matrix->data[i + matrix->size[0]*1] = starts_and_ends[i][1];
          matrix->data[i + matrix->size[0]*2] = 1;  // unify pitch
          //cout << matrix->data[i + matrix->size[0]*0] << " " << matrix->data[i + matrix->size[0]*1] << " " << matrix->data[i + matrix->size[0]*2] << "\n";
        }
        map[current] = matrix;
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
        if (word == "VOICED") {
          std::array<double, 2> se = {start/10000000.0, end/10000000.0};
          starts_and_ends.push_back(se);
        }
      } else {
        //cout << "Unrecognized line, ignoring: " << line << "\n";
      }
    }
  }
}


void load_mlf_onset_only(const char *filename, std::map<std::string, emxArray_real_T *> &map) {
  const double DUR = 1.0; // 1.0sec assumed max duration
  std::ifstream f(filename);
  std::string line;
  std::string current = "";
  std::vector<std::array<double, 2> > starts_and_ends;
  while (std::getline(f, line)) {
    if (line == "#!MLF!#" || line == ".") {
      // store last file
      if (!current.empty()) {
        //cout << "Storing " << current << " from " << filename << '\n';
        emxArray_real_T *matrix;
        int size[2] = {static_cast<int>(starts_and_ends.size()), 3};
        matrix = emxCreateND_real_T(2, *(int (*)[2])&size[0]);

        cout << matrix->size[0] << " " << matrix->size[1] << "\n";

        for (int i = 0; i < matrix->size[0]; i++) {
          matrix->data[i + matrix->size[0]*0] = starts_and_ends[i][0];
          matrix->data[i + matrix->size[0]*1] = starts_and_ends[i][1];
          matrix->data[i + matrix->size[0]*2] = 1;  // unify pitch
          //cout << matrix->data[i + matrix->size[0]*0] << " " << matrix->data[i + matrix->size[0]*1] << " " << matrix->data[i + matrix->size[0]*2] << "\n";
        }
        map[current] = matrix;
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
        if (word == "ONSET") {
          double s = start/10000000.0;
          double e = end/10000000.0;
          //cout << "Onset length: " << e-s << "\n";  // in case of very long onsets
          double onset = (s + e) / 2.0;
          int sz = starts_and_ends.size();
          if (sz > 0) {
            // fix last end point.
            double last_end = starts_and_ends[sz-1][1];
            if (last_end >= onset) {
              starts_and_ends[sz-1][1] = onset-0.01;
            }
          }

          std::array<double, 2> se = {onset, onset+DUR};
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

  classifyNotes_initialize();
  load_mlf("groundtruth_on_and_off.mlf", ground_truths);


  run_number = argv[1];
  cout << "Run #: " << run_number << "\n";

  run_dir = "GArun_";run_dir += run_number;
  config_file = run_dir+"/config";


  if (mpi_rank != 0) {   // other threads: wait a bit for main thread
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  } else {  // main thread

    std::string cmd = "mkdir -p ";
    cmd += run_dir;
    my_system(cmd.c_str());

    my_system((cmd+"/hmm0").c_str());
    my_system((cmd+"/hmm1").c_str());
    my_system((cmd+"/hmm2").c_str());
    my_system((cmd+"/hmm3").c_str());
    //my_system((cmd+"/hmm4").c_str());
    //my_system((cmd+"/hmm5").c_str());
    //my_system((cmd+"/hmm6").c_str());

    cmd = "touch ";
    cmd += run_dir;
    my_system((cmd+"/genes").c_str());
    my_system((cmd+"/results").c_str());


    // write HTK config file
    std::ofstream htk_config(config_file.c_str());
    htk_config << "APPLYVFLOOR = T" << "\n"
               << "NATURALREADORDER = T" << "\n"
               << "NATURALWRITEORDER = T" << "\n"
               << "VFLOORSCALESTR = \"Vector " << NFEAT << "\"" << "\n";
    htk_config.close();


    // prepare proto and compute variance floor
    // make proto //////////////////////////////////
    std::stringstream proto;
    proto << "~o <VecSize> " << NFEAT << " <USER>\n"
          << "   <StreamInfo> " << NFEAT;
    for (int i=0; i<NFEAT; i++) {
      proto << " " << "1";
    }
    proto << "\n"
          << "~h \"proto\"\n"
          << " <BeginHMM>\n"
          << "   <NumStates> 5\n";

    // State 2
    std::stringstream gene_ss;
    proto << "   <State> 2\n"
          << "     <SWeights> " << NFEAT;
    for (int i=0; i<NFEAT; i++) {
      proto << " " << "1";
    }
    proto << "\n";

    //// make one stream for all states
    std::stringstream one_stream;
    for (int i=0; i<NFEAT; i++) {
      one_stream << "      <Stream> " << i+1 << "\n"
                 << "        <Mean> 1 0.0\n"
                 << "        <Variance> 1 1.0\n";
    }

    proto << one_stream.str();

    // State 3
    proto << "   <State> 3\n"
          << "     <SWeights> " << NFEAT;
    for (int i=0; i<NFEAT; i++) {
      proto << " " << "1";
    }
    proto << "\n";
    proto << one_stream.str();

    // State 4
    proto << "   <State> 4\n"
          << "     <SWeights> " << NFEAT;
    for (int i=0; i<NFEAT; i++) {
      proto << " " << "1";
    }
    proto << "\n";
    proto << one_stream.str();

    // TransP
    proto << "   <TransP> 5\n"
          << "     0.0 1.0 0.0 0.0 0.0\n"
          << "     0.0 0.5 0.5 0.0 0.0\n"
          << "     0.0 0.0 0.5 0.5 0.0\n"
          << "     0.0 0.0 0.0 0.5 0.5\n"
          << "     0.0 0.0 0.0 0.0 0.0\n"
          << " <EndHMM>\n";

    std::stringstream temp;
    temp.str("");
    temp << run_dir << "/proto";
    std::string fnproto_0 = temp.str();
    std::ofstream fproto_0(fnproto_0.c_str());
    fproto_0 << proto.str();
    fproto_0.close();

    // HCompV ///////////////////
    temp.str("");
    temp << "HCompV -C " << config_file << " -f 0.01 -m -S train.scp -M " << run_dir << "/hmm0" << " " << fnproto_0;
    cout << temp.str() << "\n"; cout.flush();
    my_system(temp.str().c_str());
  }

  //// read proto HMM and vFloors
  read_proto(run_dir+"/hmm0/proto", run_dir+"/hmm0/vFloors");

  if (mpi_rank == 0) {
    write_macros(run_dir+"/hmm0/macros");
  }


// Now create the GA and run it.  First we create a genome of the type that
// we want to use in the GA.  The ga doesn't operate on this genome in the
// optimization - it just uses it to clone a population of genomes.

  GA1DBinaryStringGenome genome(NFEAT, Objective_exception_free);

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


float Objective_exception_free(GAGenome& g) {
  try {
    return Objective(g);
  } catch (std::exception &e) {
    cout << e.what() << "\n";
    return 0.0;
  }
}


float
Objective(GAGenome& g) {
  GA1DBinaryStringGenome & genome = (GA1DBinaryStringGenome &)g;
  std::stringstream temp;

  clock_t t;
  t = clock();
  uintmax_t id_thread = (uintmax_t)t;
  temp << mpi_rank << "_" << id_thread;
  std::string id = temp.str();
  temp.str("");

  cout << run_number << " " << id << " " << genome << "\n";

  //////// write genes
  std::vector<double> weights;
  std::stringstream gene_ss;
  for (int i=0; i<NFEAT; i++) {
    gene_ss << genome.gene(i);
    weights.push_back(genome.gene(i));
  }
  temp.str("");
  temp << "echo " << "\"" << id << " " << gene_ss.str() << "\" >> " << run_dir << "/genes";
  cout << temp.str() << "\n"; cout.flush();
  my_system(temp.str().c_str());


  std::vector<std::string> words;
  words.push_back("ONSET");
  words.push_back("NOT_ONSET");
  write_hmmdefs(run_dir+"/hmm0/"+id+"_hmmdefs", weights, words);


  // HERest (0->1) ////////////////////////
  temp.str("");
  temp << "HERest -C " << config_file << " -I groundtruth.mlf -t 250.0 200.0 10500.0 -S train.scp "
       << "-H " << run_dir << "/hmm0/macros "
       << "-H " << run_dir << "/hmm0/" << id << "_hmmdefs "
       << "-M " << run_dir << "/hmm1 "
       << "monophones";
  cout << temp.str() << "\n"; cout.flush();
  my_system(temp.str().c_str());


  // HERest (1->2) ////////////////////////
  temp.str("");
  temp << "HERest -C " << config_file << " -I groundtruth.mlf -t 250.0 200.0 10500.0 -S train.scp "
       << "-H " << run_dir << "/hmm1/macros "
       << "-H " << run_dir << "/hmm1/" << id << "_hmmdefs "
       << "-M " << run_dir << "/hmm2 "
       << "monophones";
  cout << temp.str() << "\n"; cout.flush();
  my_system(temp.str().c_str());


  // HERest (2->3) ////////////////////////
  temp.str("");
  temp << "HERest -C " << config_file << " -I groundtruth.mlf -t 250.0 200.0 10500.0 -S train.scp "
       << "-H " << run_dir << "/hmm2/macros "
       << "-H " << run_dir << "/hmm2/" << id << "_hmmdefs "
       << "-M " << run_dir << "/hmm3 "
       << "monophones";
  cout << temp.str() << "\n"; cout.flush();
  my_system(temp.str().c_str());

  // HVite generate test set ////////////////////
  temp.str("");
  temp << run_dir << "/" << id << "_eval.mlf";
  std::string fneval = temp.str();

  temp.str("");
  temp << "HVite -C " << config_file << " "
       << "-H " << run_dir << "/hmm3/macros "
       << "-H " << run_dir << "/hmm3/" << id << "_hmmdefs "
       << "-S test.scp "
       << "-l '*' "
       << "-i " << fneval << " "
       << "-w wdnet "
       << "-p 0.0 -s 5.0 dict monophones";
  cout << temp.str() << "\n"; cout.flush();
  my_system(temp.str().c_str());


  // Evaluation Framework ////////////////
  temp.str("");
  temp << run_dir << "/" << id << "_results";
  std::string fnresults = temp.str();

  std::map<std::string, emxArray_real_T *> results;
  load_mlf_onset_only(fneval.c_str(), results);


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


std::string _macros_content;
std::vector<std::string> _hmmdefs_body_parts;

void read_proto(const std::string &fn_proto, const std::string &fn_vFloors) {
  std::ifstream fproto(fn_proto.c_str());
  std::string line;
  int lcount = 0;  // line count
  std::string section_o;
  std::string temp;
  while (std::getline(fproto, line)) {
    if (lcount < 3) {
      section_o += line;
      section_o.push_back('\n');
    } else if (lcount >= 4) {
      temp += line;
      temp.push_back('\n');
      if (line.compare(0, 10, "<SWEIGHTS>") == 0) {
        _hmmdefs_body_parts.push_back(temp);
        temp = "";
        std::getline(fproto, line);   // jump next line
      }
    }
    lcount++;
  }
  _hmmdefs_body_parts.push_back(temp);

  std::ifstream fvFloors(fn_vFloors.c_str());
  std::string section_v;
  while (std::getline(fvFloors, line)) {
    section_v += line;
    section_v.push_back('\n');
  }

  _macros_content = section_o + section_v;
}

void write_macros(const std::string &fn_macros) {
  std::ofstream fmacros(fn_macros.c_str());
  fmacros << _macros_content;
  fmacros.close();
}

void write_hmmdefs(const std::string &fn_hmmdefs, const std::vector<double> &weights, const std::vector<std::string> &words) {
  std::ofstream fhmmdefs(fn_hmmdefs.c_str());
  for (int i=0; i<words.size(); i++) {
    fhmmdefs << "~h " << "\"" << words[i] << "\"" << "\n";
    fhmmdefs << _hmmdefs_body_parts[0];
    for (int j=1; j<_hmmdefs_body_parts.size(); j++) {
      for (int k=0; k<weights.size(); k++) {
        fhmmdefs << " " << weights[k];
      }
      fhmmdefs << "\n";
      fhmmdefs << _hmmdefs_body_parts[j];
    }
  }
  fhmmdefs.close();
}
