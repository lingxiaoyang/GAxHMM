NFOLD = 3

from random import shuffle
import sys
import os

if __name__ == "__main__":
    name = sys.argv[1]
    if len(sys.argv) > 2:
        evaluate_only = True
    else:
        evaluate_only = False
    print "name:", name

    with open("all.scp") as f:
        lines = filter(lambda x: x, f.readlines())
    print lines
    shuffle(lines)

    fold_size = len(lines) // NFOLD
    n_folds_with_one_more = len(lines) % NFOLD
    fold_pointer = 0

    if not evaluate_only:
      for i in range(NFOLD):
        this_fold_size = fold_size+1 if i<n_folds_with_one_more else fold_size

        train_scp = "TEMP_{0}_fold{1}_train.scp".format(name, i+1)
        test_scp = "TEMP_{0}_fold{1}_test.scp".format(name, i+1)

        with open(train_scp, 'w') as ftrain:
            with open(test_scp, 'w') as ftest:
                for j, line in enumerate(lines):
                    if j < fold_pointer:
                        ftrain.write(line)
                    elif j < fold_pointer + this_fold_size:
                        ftest.write(line)
                    else:
                        ftrain.write(line)

        fold_pointer += this_fold_size

    if not evaluate_only:
      for i in range(NFOLD):
        train_scp = "TEMP_{0}_fold{1}_train.scp".format(name, i+1)
        test_scp = "TEMP_{0}_fold{1}_test.scp".format(name, i+1)
        os.system("mpirun -np 8 a.out {0}_fold{1} {2} {3}".format(name, i+1, i*1000, train_scp))

    for i in range(NFOLD):
        train_scp = "TEMP_{0}_fold{1}_train.scp".format(name, i+1)
        test_scp = "TEMP_{0}_fold{1}_test.scp".format(name, i+1)
        GArun_folder = "GArun_{0}_fold{1}".format(name, i+1)
        with open(GArun_folder+"/best_genome") as f:
            best = f.read().strip()

        os.system("./main_evaluation {0} {1} {2}".format(best, train_scp, test_scp))
