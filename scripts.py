import glob
import os
from pylab import *


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
                        if start - prev < 0.20 and prev >= 0:  # <200ms SP
                            lines.append('{0} {1} SP'.format(toHTKTime(prev), toHTKTime(start)))
                            lines.append('{0} {1} NOTE'.format(toHTKTime(start), toHTKTime(end)))
                        else:    # >200ms SIL
                            if prev < 0:
                                prev = 0
                            lines.append('{0} {1} SIL'.format(toHTKTime(prev), toHTKTime(start)))
                            lines.append('{0} {1} NOTE'.format(toHTKTime(start), toHTKTime(end)))
                        prev = end
                lines.append("SIL")
                lines.append(".")
    with open(mlf, "w") as g:
        g.write("\n".join(lines)+"\n")


def groundtruths2labs(groundtruthsdir, outdir):

    def toHTKTime(second):
        ten_million = 10000000
        return int(second * ten_million)

    for filename in os.listdir(groundtruthsdir):
        if filename.endswith('.txt'):
            name = filename.split('.')[0]
            print name

            lines = []
            note_count = 0
            with open(groundtruthsdir + os.sep + filename, 'r') as f:
                prev = -1.0 # seconds

                for line in f:
                    things = line.split()
                    if len(things) == 3:
                        note_count += 1
                        start = float(things[0])
                        end = float(things[1])
                        if start - prev < 0.20 and prev >= 0:  # <200ms SP
                            lines.append('{0} {1} SP'.format(toHTKTime(prev), toHTKTime(start)))
                            lines.append('{0} {1} NOTE_{2}_{3}'.format(toHTKTime(start), toHTKTime(end), name, note_count))
                        else:    # >200ms SIL
                            if prev < 0:
                                prev = 0
                            lines.append('{0} {1} SIL'.format(toHTKTime(prev), toHTKTime(start)))
                            lines.append('{0} {1} NOTE_{2}_{3}'.format(toHTKTime(start), toHTKTime(end), name, note_count))
                        prev = end
                lines.append("SIL")
                #lines.append(".")
            with open(outdir + os.sep + name + ".lab", "w") as g:
                g.write("\n".join(lines)+"\n")
            with open(outdir + os.sep + name + ".monos", "w") as g:
                for i in range(note_count):
                    g.write("NOTE_{0}_{1}\n".format(name, i+1))


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


def split_test_mlf(mlf_file, test_dir, ext):
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
            if len(line.split()) >= 3:
                _, _, is_voiced = line.split()[:3]
                if is_voiced == 'NOTE':
                    contents.append(line)

        def transform(line):
            start, end = line.split()[:2]
            start = str(float(start) / 10000000.0)
            end = str(float(end) / 10000000.0)
            return start + ' ' + end + ' 1'
        contents = map(transform, contents)
        with open(test_dir + os.sep + basename + '.' + ext, 'w') as g:
            g.write('\n'.join(contents)+'\n')

def tony_format_to_evaluation_framework(tony_format_dir, output_dir, ext):
    for filename in os.listdir(tony_format_dir):
        basename = filename.split('.')[0]
        print basename
        with open(tony_format_dir + os.sep + filename, 'r') as f:
            with open(output_dir + os.sep + basename + "." + ext, 'w') as g:
                for line in f:
                    start, pit, dur = line.split()[:3]
                    g.write("{0} {1} {2}\n".format(start, float(start)+float(dur), 1))


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
