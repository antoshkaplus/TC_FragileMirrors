
import sys
import os
import subprocess as sp
from multiprocessing import cpu_count
from multiprocessing import Pool
from optparse import OptionParser


SCORE_START = "Score  = "
TEST_COUNT = 100
REPEAT_COUNT = 2
VERSION = None


parser = OptionParser()
parser.add_option("--test-count", dest="test_count")
parser.add_option("--repeat-count", dest="repeat_count")
parser.add_option("--version", dest="version")

(options, args) = parser.parse_args()

if options.test_count:
    TEST_COUNT = int(options.test_count)
if options.repeat_count:
    REPEAT_COUNT = int(options.repeat_count)
if options.version:
    VERSION = options.version
else:
    parser.error("version not specified")

def worker(i):
    command = ["java", "-jar", "FragileMirrorsVis.jar", "-exec", "../bin/" + VERSION + " -d", "-novis", "-seed", str(i)]
    s = sp.check_output(command)
    lines = s.split("\n")
    for ln in lines:
        if ln.startswith(SCORE_START):
            return (i, ln[len(SCORE_START):])

pool = Pool(cpu_count())
result = pool.map(worker, (i for i in range(TEST_COUNT) for _ in range(REPEAT_COUNT)))
result = [max(result[i*REPEAT_COUNT:(i+1)*REPEAT_COUNT], key=lambda x: x[1]) for i in range(TEST_COUNT)]

result.sort(key=lambda x: x[0])
with open("../scores/" + VERSION + ".txt", "w") as out:
    out.write(str(len(result)) + "\n")
    for index,score in result:
        out.write(str(index) + "," + score + "\n")
