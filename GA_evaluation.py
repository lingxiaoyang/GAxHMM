import sys
import os
from pylab import *

rcParams['figure.figsize'] = 15, 10

GENERATIONS = 150
POPULATION = 91

def read_gene_from_proto_30(proto_str):
    lines = proto_str.split('\n')
    line1 = lines[6]
    line2 = lines[98]
    line3 = lines[190]
    def extract_weights(line):
        start_pos = line.index('30')
        remaining = line[start_pos+2:]
        return map(int, remaining.strip().split())
    return extract_weights(line1) + extract_weights(line2) + extract_weights(line3)

if __name__ == '__main__':
    run_no = sys.argv[1]
    results = []
    corrs = {}
    genes = {}

    dirname = "./GArun_{0}/".format(run_no)

    # read genes
    with open(dirname + 'genes', 'r') as f:
        for line in f:
            timestamp, gene = line.split()
            genes[int(timestamp)] = map(int, list(gene.strip()))

    # read results
    with open(dirname + 'results', 'r') as f:
        for line in f:
            timestamp, corr = line.split()
            timestamp = int(timestamp)
            corr = float(corr)
            corrs[timestamp] = corr
            results.append([timestamp, corr, genes.get(timestamp)])

    results.sort(key=lambda t: t[1], reverse=True)
    print results[0:5]


    results.sort(key=lambda t: t[0])
    avgs = []
    maxs = []
    avg_genes = []
    for i in range(GENERATIONS):
        print "Generation {0}: ".format(i),
        s = 0
        m = 0
        genes = []
        for j in range(POPULATION*i, POPULATION*i+POPULATION):
            timestamp = results[j][0]
            gene = results[j][2]
            genes.append(gene)

            s += results[j][1]
            if results[j][1] > m:
                m = results[j][1]
        print 'avg =', s/float(POPULATION), ' ',
        print 'max =', m
        avgs.append(s/float(POPULATION))
        if maxs and  m < maxs[-1]:
            m = maxs[-1]
        maxs.append(m)
        avg_gene = map(average, zip(*genes))
        avg_genes.append(avg_gene)



    # plot
    if True:
        xs = arange(1, GENERATIONS+1)
        plot(xs, avgs, 'black', label="Average Corr%")
        plot(xs, maxs, 'r', label="Max Corr%")
        xlim((1, GENERATIONS+1))
        ylim((0, 0.3))
        legend(bbox_to_anchor=(0.8, 1), loc=2, borderaxespad=0.)
        show()

    # plot 2
    if True:
        marker_styles = [',', '.', 'o', 'v', '^', '*', '+', 'x', 'D', 'd']
        xs = arange(1, GENERATIONS+1)
        for i, avg_feats in enumerate(zip(*avg_genes)):
            #if not 0 <= i < 10:
            #    continue
            plot(xs, avg_feats, label="Feat {0}".format(i), marker=marker_styles[i % 10])
        xlim((1, GENERATIONS+1))
        ylim((0, 1))
        legend(bbox_to_anchor=(0, 0), loc=3, borderaxespad=0.)
        show()
