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
#include <sys/stat.h>
#include "rt_nonfinite.h"
#include "classifyNotes.h"
#include "classifyNotes_terminate.h"
#include "classifyNotes_emxAPI.h"
#include "classifyNotes_initialize.h"
#include "mpi.h"

#define DEBUG

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

#define BEAM               "250.0 200.0 20500.0"

#define NFOLD_GA        3         // n-fold cross-validation for GA training (upper level)
#define NFOLD_HMM       3         // n-fold cross-validation for HMM training(lower level)

#define FILE_MSDINFO         "train/msdInfo"
#define FILE_GROUNDTRUTH     "groundtruth.mlf"
#define FILE_GRAMMAR         "grammar"
#define FILE_WDNET           "wdnet"
#define FILE_TRAIN           "train.scp"
#define FILE_TEST            "test.scp"
#define SIGNAL_READY         "ready"

#define NOTE_NSTATES       3
#define NOTE_MIN_CLUSTER_RATIO     0.10
#define SIL_NSTATES        2
#define SP_NSTATES         1

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

std::string make_proto(int nStates, int nMixes, const std::vector<int> msdInfo, const int []);
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

inline bool file_exists (const std::string& name) {
  struct stat buffer;
  return (stat (name.c_str(), &buffer) == 0);
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
        if (word.find("NOTE") == 0) {  // starts with NOTE
          std::array<double, 2> se = {start/10000000.0, end/10000000.0};
          starts_and_ends.push_back(se);
        }
      } else {
        //cout << "Unrecognized line, ignoring: " << line << "\n";
      }
    }
  }
}

//////
struct MatchPathSeparator
{
    bool operator()( char ch ) const
    {
        return ch == '/';
    }
};
std::string basename( std::string const& pathname )
{
    return std::string(
        std::find_if( pathname.rbegin(), pathname.rend(),
                      MatchPathSeparator() ).base(),
        pathname.end() );
}
std::string removeExtension( std::string const& filename )
{
    std::string::const_reverse_iterator
                        pivot
            = std::find( filename.rbegin(), filename.rend(), '.' );
    return pivot == filename.rend()
        ? filename
        : std::string( filename.begin(), pivot.base() - 1 );
}
//////

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


  if (mpi_rank != 0) {   // other threads: wait for ready signal
    while (!file_exists(SIGNAL_READY)) {
      std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    }
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


    // make_proto NOTEs
    std::ofstream fproto_note((run_dir+"/proto_note").c_str());
    int all1weights[NFEAT];
    std::fill_n(all1weights, NFEAT, 1);
    fproto_note << make_proto(NOTE_NSTATES, 1, msdInfo, all1weights);
    fproto_note.close();

    // HCompV
    std::stringstream temp;
    temp.str("");
    temp << "HCompV"
         << " -C " << config_file
         << " -f 0.01"
         << " -m"
         << " -S " << FILE_TRAIN
         << " -M " << run_dir
         << " -o proto_note.average"
         << " " << run_dir << "/proto_note";
    my_system(temp);

    // Build init mmf file
    std::ifstream ffeat_proto((run_dir+"/proto_note").c_str());
    std::string line;
    std::getline(ffeat_proto, line);
    ffeat_proto.close();
    std::ofstream ffeat_init((run_dir+"/proto_note.macro").c_str());
    ffeat_init << line << "\n";
    std::ifstream vFloors((run_dir+"/vFloors").c_str());
    while (std::getline(vFloors, line)) {
      ffeat_init << line << "\n";
    }
    ffeat_init.close();

    // find all training excerpts
    std::vector<std::string> training_excerpts;
    std::ifstream ftrain_scp(FILE_TRAIN);
    while (std::getline(ftrain_scp, line)) {
      std::string base = removeExtension(basename(line));
      training_excerpts.push_back(base);
      std::ofstream fthis_scp((run_dir+"/"+base+".scp").c_str());
      fthis_scp << line << "\n";
      fthis_scp.close();
    }
    ftrain_scp.close();

    // HInit for all notes
    int note_count = 0;
    std::string train_dir = "train";
    std::ofstream fnote_monos((run_dir+"/note.monos").c_str());
    std::ofstream fall_monos((run_dir+"/all.monos").c_str());
    std::ofstream fall_dict((run_dir+"/all.dict").c_str());
    for (std::vector<std::string>::iterator it = training_excerpts.begin(); it != training_excerpts.end(); ++it) {
      std::ifstream ftraining_mono((train_dir+"/"+*it+".monos").c_str());
      //cout << *it << "\n";
      while (std::getline(ftraining_mono, line)) {
        note_count += 1;
        fnote_monos << line << "\n";
        fall_monos << line << "\n";
        fall_dict << "NOTE " << line << "\n";
        temp.str("");
        temp << "HInit"
             << " -C " << config_file
             << " -H " << run_dir << "/proto_note.macro"
             << " -M " << run_dir
             << " -l " << line
             << " -o " << line
             << " -w 0"
             << " -m 1"
             << " -S " << run_dir << "/" << *it << ".scp"
             << " -L train"
          //<< " -u mvw"   // don't update trans matrix
             << " " << run_dir << "/proto_note";
        my_system(temp);
      }
    }
    fnote_monos.close();
    fall_monos << "SP\n" << "SIL\n";
    fall_monos.close();
    fall_dict << "SP SP\n" << "SIL SIL\n";
    fall_dict.close();


    // HInit for SP and SIL
    std::ofstream fproto_sil((run_dir+"/proto_sil").c_str());
    fproto_sil << make_proto(SIL_NSTATES, 1, msdInfo, all1weights);
    fproto_sil.close();

    // HCompV
    temp.str("");
    temp << "HCompV"
         << " -C " << config_file
         << " -f 0.01"
         << " -m"
         << " -S " << FILE_TRAIN
         << " -M " << run_dir
         << " -o proto_sil.average"
         << " " << run_dir << "/proto_sil";
    my_system(temp);

    // Build init mmf file
    {
      std::ifstream ffeat_proto((run_dir+"/proto_sil").c_str());
      std::string line;
      std::getline(ffeat_proto, line);
      ffeat_proto.close();
      std::ofstream ffeat_init((run_dir+"/proto_sil.macro").c_str());
      ffeat_init << line << "\n";
      std::ifstream vFloors((run_dir+"/vFloors").c_str());
      while (std::getline(vFloors, line)) {
        ffeat_init << line << "\n";
      }
      ffeat_init.close();
    }

    // make proto
    std::ofstream fproto_sp((run_dir+"/proto_sp").c_str());
    fproto_sp << make_proto(SP_NSTATES, 1, msdInfo, all1weights);
    fproto_sp.close();

    // HCompV
    temp.str("");
    temp << "HCompV"
         << " -C " << config_file
         << " -f 0.01"
         << " -m"
         << " -S " << FILE_TRAIN
         << " -M " << run_dir
         << " -o proto_sp.average"
         << " " << run_dir << "/proto_sp";
    my_system(temp);

    // Build init mmf file
    {
      std::ifstream ffeat_proto((run_dir+"/proto_sp").c_str());
      std::string line;
      std::getline(ffeat_proto, line);
      ffeat_proto.close();
      std::ofstream ffeat_init((run_dir+"/proto_sp.macro").c_str());
      ffeat_init << line << "\n";
      std::ifstream vFloors((run_dir+"/vFloors").c_str());
      while (std::getline(vFloors, line)) {
        ffeat_init << line << "\n";
      }
      ffeat_init.close();
    }

    // HInit
    temp.str("");
    temp << "HInit"
         << " -C " << config_file
         << " -H " << run_dir << "/proto_sil.macro"
         << " -M " << run_dir
         << " -I " << FILE_GROUNDTRUTH
         << " -l SIL"
         << " -o SIL"
         << " -S " << FILE_TRAIN
         << " -w 5000"
         << " " << run_dir << "/proto_sil";
    my_system(temp);

    temp.str("");
    temp << "HInit"
         << " -C " << config_file
         << " -H " << run_dir << "/proto_sp.macro"
         << " -M " << run_dir
         << " -I " << FILE_GROUNDTRUTH
         << " -l SP"
         << " -o SP"
         << " -S " << FILE_TRAIN
         << " -w 5000"
         << " " << run_dir << "/proto_sp";
    my_system(temp);


    // HERest for generating stats
    temp.str("");
    temp << "mkdir " << run_dir << "/HERest";
    my_system(temp);

    temp.str("");
    temp << "HERest"
#ifdef DEBUG
         << " -T 1"
#endif
         << " -s " << run_dir << "/all.stats"
         << " -C " << config_file
         << " -L train"
         << " -t " << BEAM
         << " -S " << FILE_TRAIN
         << " -d " << run_dir
         << " -M " << run_dir << "/HERest"
         << " " << run_dir << "/all.monos";
    my_system(temp);

    // hack stats (remove SIL and SP)
    std::ifstream fall_stats((run_dir+"/all.stats").c_str());
    std::ofstream fnote_stats((run_dir+"/note.stats").c_str());
    int stat_line_number = 0;
    std::string ta, tb, tc, td, te, tf;

    while (std::getline(fall_stats, line)) {
      if ((line.find("SP") == std::string::npos) && (line.find("SIL") == std::string::npos)) {  // no SP and no SIL
        std::istringstream iss(line);
        if (iss >> ta >> tb >> tc >> td >> te >> tf) {
          stat_line_number += 1;
          fnote_stats << "\t" << stat_line_number;
          fnote_stats << "\t" << tb;
          fnote_stats << "\t" << tc;
          fnote_stats << "\t" << td;
          fnote_stats << "\t" << te;
          fnote_stats << "\t" << tf;
          fnote_stats << "\n";
        }
      }
    }
    fall_stats.close();
    fnote_stats.close();



    // HHEd  data-driven clustering
    std::ofstream fhed((run_dir+"/cluster.hed").c_str());
    fhed << "FV GArun_test/vFloors" << "\n"
      //<< "TI T_NOTE {NOTE_*.transP}" << "\n"
         << "RO " << (int)((float)note_count*NOTE_MIN_CLUSTER_RATIO)+1 << " \"" << run_dir << "/note.stats\"\n"
         << "NC " << (int)(1.0/NOTE_MIN_CLUSTER_RATIO)+1 << " \"NOTEc_\" {NOTE_*}" << "\n";
    /*float NClusters_reduction[] = {1, 1, 1, 0.3, 0.3, 0.3, 0.3};
    for (int i=2; i<NOTE_NSTATES+2; i++) {
      fhed << "NC " << (int)(1.0/NOTE_MIN_CLUSTER_RATIO*NClusters_reduction[i])+1 << " \"NOTEs" << i << "_\" {NOTE_*.state[" << i << "]}" << "\n";
      }*/
    fhed << "CO " << run_dir << "/note.tied\n";
    fhed.close();

    temp.str("");
    temp << "HHEd"
#ifdef DEBUG
         << " -T 1"
#endif
         << " -C " << config_file
         << " -p"
         << " -i"
         << " -d " << run_dir
         << " -w " << run_dir << "/NOTEs"
         << " -m"
         << " " << run_dir << "/cluster.hed"
         << " " << run_dir << "/note.monos";
    my_system(temp);

    // Generate all monos
    temp.str("");
    temp << "cat"
         << " " << run_dir << "/note.tied"
         << " > " << run_dir << "/all.monos"
         << ";"
         << "echo 'SP' >> " << run_dir << "/all.monos"
         << ";"
         << "echo 'SIL' >> " << run_dir << "/all.monos";
    my_system(temp);


    // Prepare tie all hed
    std::ofstream ftie_all_hed((run_dir+"/tie_all.hed").c_str());
    ftie_all_hed << "FV " << run_dir << "/vFloors" << "\n"
                 << "TI \"weights\" {NOTE*.state[2-" << NOTE_NSTATES+1 << "].weights,SP.state[2-" << SP_NSTATES+1 << "].weights,SIL.state[2-" << SIL_NSTATES+1 << "].weights}";
    ftie_all_hed.close();


    // touch ready signal
    temp.str("");
    temp << "touch"
         << " " << run_dir << "/" << SIGNAL_READY;
    my_system(temp);
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
  int weights[NFEAT];
  {
    std::stringstream gene_ss;
    for (int i=0; i<NFEAT; i++) {
      gene_ss << genome.gene(i);
      weights[i] = genome.gene(i);
    }
    temp.str("");
    temp << "echo " << "\"" << id << " " << gene_ss.str() << "\" >> " << run_dir << "/genes";
    cout << temp.str() << "\n"; cout.flush();
    my_system(temp);
  }


  /////////////////////////////////////////
  // 0. prepare starting HMMs for SIL, SP
  /////////////////////////////////////////
  temp.str("");
  temp << "mkdir " << working_dir << "/hmm0";
  my_system(temp);


  // make proto  sil
  std::ofstream fproto_sil((working_dir+"/hmm0/proto_sil").c_str());
  fproto_sil << make_proto(3, 2, msdInfo, weights);
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
  fproto_sp << make_proto(1, 1, msdInfo, weights);
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


  // HInit for SIL, SP
  temp.str("");
  temp << "HInit"
#ifdef DEBUG
         << " -T 1"
#endif
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
#ifdef DEBUG
         << " -T 1"
#endif
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


  // HHEd   make a monophone mmf   tie the weights
  temp.str("");
  temp << "HHEd"
#ifdef DEBUG
         << " -T 1"
#endif
       << " -C " << config_file
       << " -p"
       << " -i"
       << " -H " << run_dir << "/NOTEs"
       << " -d " << working_dir << "/hmm0"
       << " -w " << working_dir << "/hmm0/feat.monophones"
       << " " << run_dir << "/tie_all.hed"
       << " " << run_dir << "/all.monos";
  my_system(temp);


  std::string cmd_mkdir = "mkdir ";

  // HERest (0->1) ////////////////////////
  my_system(cmd_mkdir + working_dir + "/hmm1");
  temp.str("");
  temp << "HERest"
#ifdef DEBUG
         << " -T 1"
#endif
       << " -C " << config_file
       << " -L train"
       << " -t " << BEAM
       << " -S " << FILE_TRAIN
       << " -H " << working_dir << "/hmm0/feat.monophones"
       << " -M " << working_dir << "/hmm1"
       << " " << run_dir << "/all.monos";
  my_system(temp);

  // HERest (1->2) ////////////////////////
  my_system(cmd_mkdir + working_dir + "/hmm2");
  temp.str("");
  temp << "HERest"
#ifdef DEBUG
         << " -T 1"
#endif
       << " -C " << config_file
       << " -L train"
       << " -t " << BEAM
       << " -S " << FILE_TRAIN
       << " -H " << working_dir << "/hmm1/feat.monophones"
       << " -M " << working_dir << "/hmm2"
       << " " << run_dir << "/all.monos";
  my_system(temp);

  // HERest (2->3) ////////////////////////
  my_system(cmd_mkdir + working_dir + "/hmm3");
  temp.str("");
  temp << "HERest"
#ifdef DEBUG
         << " -T 1"
#endif
       << " -C " << config_file
       << " -L train"
       << " -t " << BEAM
       << " -S " << FILE_TRAIN
       << " -H " << working_dir << "/hmm2/feat.monophones"
       << " -M " << working_dir << "/hmm3"
       << " " << run_dir << "/all.monos";
  my_system(temp);


  // HVite generate test set ////////////////////
  temp.str("");
  temp << "HVite"
#ifdef DEBUG
         << " -T 1"
#endif
       << " -C " << config_file
       << " -H " << working_dir << "/hmm3/feat.monophones"
       << " -S " << FILE_TEST
       << " -l '*' "
       << " -i " << working_dir << "/test.mlf"
       << " -w " << FILE_WDNET
       << " -p 0.0"
       << " -s 5.0"
       << " " << run_dir << "/all.dict"
       << " " << run_dir << "/all.monos";
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




std::string make_proto(int nStates, int nMixes, const std::vector<int> msdInfo, const int weights[]) {
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
      proto << " " << weights[i];
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
