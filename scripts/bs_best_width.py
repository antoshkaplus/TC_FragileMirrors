
import sys
import os
import subprocess as sp
from multiprocessing import cpu_count
from multiprocessing import Pool
from optparse import OptionParser


LINE_MIN_WIDTH = "Min width: "
LINE_MAX_WIDTH = "Max width: "
LINE_AVE_WIDTH = "Ave width: "


class Stats:
    def __init__(self, min_w=-1, max_w=-1, ave_w=-1):
        self.min_w = min_w
        self.max_w = max_w
        self.ave_w = ave_w

class WorkArgs:
    def __init__(self, sz, min_w, max_w, n=1, s=10):
        self.sz = sz
        self.min_w = min_w
        self.max_w = max_w
        self.n = n
        self.s = s

def bounds(sz):
    if sz < 60:
        return (1500, 5000)
    if sz < 75:
        return (1000, 2500)
    return (500, 1500)


CPU_COUNT = cpu_count()
if CPU_COUNT > 2: CPU_COUNT-= 2

parser = OptionParser(usage="usage: %prog [--cpu=%cpu_count] [--n=%n]")
parser.add_option("--cpu", dest="cpu_count")
parser.add_option("--n", dest="n")

(options, args) = parser.parse_args()

if options.cpu_count:
    CPU_COUNT = int(options.cpu_count)
if options.n:
    N = int(options.n)


def worker(args):
    try:
        command = ("../bin/bs_best_width -sz %d -min_w %d -max_w %d -n %d -s %d" %
                   (args.sz, args.min_w, args.max_w, args.n, args.s)).split()
        s = sp.check_output(command)
        lines = s.split("\n")
        out = lambda ind: int(lines[ind][len(LINE_MIN_WIDTH):])
        return Stats(*(out(i) for i in range(3)))
    except Exception as ex:
        print ex
        return Stats()


pool = Pool(CPU_COUNT)
r_sz = range(50, 100+1, 5)

result = pool.map(worker, (WorkArgs(sz, bounds(sz)[0], bounds(sz)[1], N) for sz in r_sz))
with open("../data/bs_best_width.csv", "w") as out:
    out.write("sz;min_w;max_w;ave_w" + "\n")
    for index, st in enumerate(result):
        out.write("%d;%d;%d;%d\n" % (r_sz[index], st.min_w, st.max_w, st.ave_w))
