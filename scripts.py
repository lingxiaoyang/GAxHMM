import glob
import os
import lxml.etree
from scipy.signal import butter, lfilter, freqs
from pylab import *
import aubio


FEATURES = {
    0: ("Pitch", ),
    1: ("Energy", ),
    2: ("Voicing", ),
    3: ("Accent", ),
}



rcParams['figure.figsize'] = 8, 12

def groundtruths2mlf(groundtruthsdir, mlf):

    def toHTKTime(second):
        ten_million = 10000000
        return int(second * ten_million)

    lines = ["#!MLF!#"]
    for filename in os.listdir(groundtruthsdir):
        if filename.endswith('.txt'):
            name = filename.split('.')[0]
            print name
            lines.append('"*/{0}.lab"'.format(name))
            with open(groundtruthsdir + os.sep + filename, 'r') as f:
                prev = -1.0 # seconds
                for line in f:
                    things = line.split()
                    if len(things) == 3:
                        start = float(things[0])
                        end = float(things[1])
                        if start - prev > 0.1:  # 100ms
                            if prev < 0:
                                prev = 0
                            lines.append('{0} {1} UNVOICED'.format(toHTKTime(prev), toHTKTime(start)))
                        lines.append('{0} {1} VOICED'.format(toHTKTime(start), toHTKTime(end)))
                        prev = end
                #lines.append('{0} {1} UNVOICED'.format(toHTKTime(end), toHTKTime(end+2)))
                lines.append("UNVOICED")
                lines.append(".")
    with open(mlf, "w") as g:
        g.write("\n".join(lines)+"\n")




def groundtruths2mlf_onset(groundtruthsdir, mlf):

    def toHTKTime(second):
        ten_million = 10000000
        return int(second * ten_million)

    lines = ["#!MLF!#"]
    for filename in os.listdir(groundtruthsdir):
        if filename.endswith('.txt'):
            name = filename.split('.')[0]
            print name
            lines.append('"*/{0}.lab"'.format(name))

            starts = []
            with open(groundtruthsdir + os.sep + filename, 'r') as f:
                for line in f:
                    things = line.split()
                    if len(things) == 3:
                        start = float(things[0])
                        starts.append(start)

            prev = 0.0
            ONSET_LIM = 0.05 # sec
            for start in starts:
                s1 = start - ONSET_LIM
                s2 = start + ONSET_LIM
                if s1 > prev:
                    lines.append('{0} {1} NOT_ONSET'.format(toHTKTime(prev), toHTKTime(s1)))
                    lines.append('{0} {1} ONSET'.format(toHTKTime(s1), toHTKTime(s2)))
                    prev = s2
            lines.append("NOT_ONSET")
            lines.append(".")
    with open(mlf, "w") as g:
        g.write("\n".join(lines)+"\n")





def ACEsinglefeature2floats(ace_path, outdir, ext):
    tree = lxml.etree.parse(ace_path)
    data_sets = tree.xpath(r'//data_set')
    for ds in data_sets:
        dsid = ds.find(r'.//data_set_id')
        infile = os.path.basename(dsid.text)
        basename = infile.split('.')[0]
        outfile = outdir + os.sep + basename + '.' + ext
        print outfile

        vs = ds.xpath(r'.//v')
        with open(outfile, 'w') as g:
            for v in vs:
                g.write(v.text+'\n')

def ACEmfccs2accents(ace_path, outdir, ext):
    tree = lxml.etree.parse(ace_path)
    data_sets = tree.xpath(r'//data_set')
    for ds in data_sets:
        dsid = ds.find(r'.//data_set_id')
        infile = os.path.basename(dsid.text)
        basename = infile.split('.')[0]
        outfile = outdir + os.sep + basename + '.' + ext
        print outfile

        sections = ds.findall(".//section")
        mfccs = zeros((len(sections), 35), dtype=float32)

        for i, section in enumerate(sections):
            vs = section.findall(".//v")[1:]  # ignore 0-th MFCC
            for j, v in enumerate(vs):
                mfccs[i, j] = float32(v.text)

        for i in range(len(sections)):
            mfccs[i, :] = butter_lowpass_filter(mfccs[i, :], 20, 100)
        ad_mfccs = abs(diff(mfccs, axis=0))
        s = sum(ad_mfccs, axis=1)


        #plot(s)
        #show()

        with open(outfile, 'w') as g:
            for n in s:
                g.write(str(n)+'\n')

def butter_lowpass(cutOff, fs, order=5):
    nyq = 0.5 * fs
    normalCutoff = cutOff / nyq
    b, a = butter(order, normalCutoff, btype='low', analog=False)
    return b, a

def butter_lowpass_filter(data, cutOff, fs, order=4):
    b, a = butter_lowpass(cutOff, fs, order=order)
    y = lfilter(b, a, data)
    return y

def compose_data(basename):
    # compose delta-pitch, energy, voicing, accent
    # delta-pitch: delta calculation: (use ratio)
    ## if 0.0 x.x 0.0   it's 0
    ## if 0.0 x.x x.x   it's the ratio
    ## if x.x x.x 0.0   same: the ratio
    ## if x.x x.x x.x   it's the square root of the ratio of left and right.

    print basename
    f_pit = basename+".pit"
    f_eng = basename+".eng"
    f_cfd = basename+".cfd"
    f_acc = basename+".acc"

    with open(f_pit) as f:
        l = f.read().split()
        pits = map(float32, l)
    with open(f_eng) as f:
        l = f.read().split()
        engs = map(float32, l)
    with open(f_cfd) as f:
        l = f.read().split()
        cfds = map(float32, l)
    with open(f_acc) as f:
        l = f.read().split()
        accs = map(float32, l)

    #clean_pits = []
    #for p in pits:
    #    if p < 70 or p > 880:
    #        clean_pits.append(0.0)
    #    else:
    #        clean_pits.append(p)
    clean_pits = pits

    dpits = []
    ZERO = -1e10
    for i in range(len(clean_pits)):
        if clean_pits[i] == 0:
            dpits.append(ZERO)
        elif (i == 0 or clean_pits[i-1] == 0) and (i == len(clean_pits)-1 or clean_pits[i+1] == 0):
            dpits.append(ZERO)
        elif (i == 0 or clean_pits[i-1] == 0):
            d = clean_pits[i+1] / clean_pits[i]
            dpits.append(d)
        elif (i == len(clean_pits)-1 or clean_pits[i+1] == 0):
            d = clean_pits[i] / clean_pits[i-1]
            dpits.append(d)
        else:
            d = sqrt(clean_pits[i+1] / clean_pits[i-1])
            dpits.append(d)

    data = zip(dpits, engs, cfds, accs)
    print len(data)

    f_out = basename+'.cmp'
    with open(f_out, 'w') as g:
        for frame in data:
            for ele in frame:
                g.write(str(ele) + '\n')

def plot_hmmdefs(hmmdef_file):
    with open(hmmdef_file) as f:
        lines = f.read().split('\n')
    print lines[3]
    assert lines[3].strip() == '~h "UNVOICED"'
    print lines[31]
    assert lines[31].strip() == '~h "VOICED"'

    u2ms = map(float32, lines[8].strip().split())
    u2vs = map(float32, lines[10].strip().split())
    u3ms = map(float32, lines[14].strip().split())
    u3vs = map(float32, lines[16].strip().split())
    u4ms = map(float32, lines[20].strip().split())
    u4vs = map(float32, lines[22].strip().split())
    v2ms = map(float32, lines[36].strip().split())
    v2vs = map(float32, lines[38].strip().split())
    v3ms = map(float32, lines[42].strip().split())
    v3vs = map(float32, lines[44].strip().split())
    v4ms = map(float32, lines[48].strip().split())
    v4vs = map(float32, lines[50].strip().split())

    print u2ms, u2vs, v2ms, v2vs

    titles = ["Pitch Difference", "Energy", "Voicing", "Accent"]
    for i in range(4):
        subplot(4, 1, i+1)
        xlpos = [1, 2, 3, 5, 6, 7]
        xlabels = ["voiced-1", "voiced-2", "voiced-3", "unvoiced-1", "unvoiced-2", "unvoiced-3"]
        errorbar(xlpos,
                 [v2ms[i], v3ms[i], v4ms[i], u2ms[i], u3ms[i], u4ms[i]],
                 [v2vs[i], v3vs[i], v4vs[i], u2vs[i], u3vs[i], u4vs[i]],
                 linestyle="None", marker="^")
        xticks(xlpos, xlabels, size="small")
        xlim((xlpos[0]-1, xlpos[-1]+1))
        if i == 2: # voicing
            ylim((0, 1))
        elif i == 3: # accent
            ylim((0, 80))
        elif i == 0: # pitch diff
            ylim((-1e10, 1e9))
        title(titles[i])
        grid(axis="y")

    show()

    if False:
        i = 0
        xlpos = [1, 2, 3, 5, 6, 7]
        xlabels = ["voiced-1", "voiced-2", "voiced-3", "unvoiced-1", "unvoiced-2", "unvoiced-3"]
        errorbar(xlpos,
                 [v2ms[i], v3ms[i], v4ms[i], u2ms[i], u3ms[i], u4ms[i]],
                 [v2vs[i], v3vs[i], v4vs[i], u2vs[i], u3vs[i], u4vs[i]],
                 linestyle="None", marker="^")
        xticks(xlpos, xlabels, size="small")
        xlim((xlpos[0]-1, xlpos[-1]+1))
        ylim((0, 15))
        title(titles[i])
        grid(axis="y")
        show()


def split_test_mlf(mlf_file, test_dir, no_pitch=False):
    with open(mlf_file) as f:
        content = f.read()
        files = content.split('.\n')

    for f in files:
        f = f.strip()
        lines = f.split('\n')
        if lines[0] == "#!MLF!#":
            lines = lines[1:]
        basename = lines[0][3:-5]
        print basename

        contents = []
        for line in lines[1:]:
            _, _, is_voiced = line.split()[:3]
            if is_voiced == 'VOICED':
                contents.append(line)
        if no_pitch:
            def transform(line):
                start, end = line.split()[:2]
                start = str(float(start) / 10000000.0)
                end = str(float(end) / 10000000.0)
                return start + ' ' + end + ' 1'
            contents = map(transform, contents)
        with open(test_dir + os.sep + basename + '.lab', 'w') as g:
            g.write('\n'.join(contents)+'\n')

def extract_notes_from_onsets(labfile, pitfile, outfile):
    print labfile
    notes = []
    with open(labfile) as f:
        for line in f:
            notes.append(line.strip().split())

    fs = 44100
    hopsize = 512
    pits = []  # hopsize = 512
    with open(pitfile) as f:
        pits = map(float32, f.read().split())

    ten_million = 10000000

    extracted_notes = []
    for note in notes:
        if note[2] == "VOICED":
            start = float32(note[0]) / ten_million
            end = float32(note[1]) / ten_million
            start_b = floor(start*fs/hopsize)
            end_b = floor(end*fs/hopsize)

            candidates = []
            for i in range(int(start_b), int(end_b)+1):
                p = pits[i]
                if p > 70 and p < 880:
                    candidates.append(p)

            if not candidates:
                print "WARNING:: ignoring this note"
                continue
            out_p = median(candidates)
            print out_p
            extracted_notes.append("{0} {1} {2}".format(start, end, aubio.freqtomidi(out_p)))

    with open(outfile, 'w') as g:
        g.write('\n'.join(extracted_notes)+'\n')


def pitch_to_1_ground_truths(src, target):
    with open(src) as f:
        with open(target, 'w') as g:
            for line in f:
                start, end = line.split()[:2]
                g.write(start+' '+end+' 1\n')
