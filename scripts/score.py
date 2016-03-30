
import sys
import os
import subprocess as sp
from multiprocessing import cpu_count
from multiprocessing import Pool

SCORE_START = "Score  = "
TEST_COUNT = 100


def worker(i):
    command = ["java", "-jar", "FragileMirrorsVis.jar", "-exec", "../bin/psyho -d", "-novis"]
    s = sp.check_output(command)
    lines = s.split("\n")
    for ln in lines:
        print ln
        if ln.startswith(SCORE_START):
            result.append((i, ln[len(SCORE_START):]))

pool = Pool(cpu_count())
result = pool.map(worker, (i for i in range(TEST_COUNT)))

result.sort(key=lambda x: x[0])
with open("../scores/psyho.txt", "w") as out:
    out.write(str(len(result)) + "\n")
    for index,score in result:
        out.write(str(index) + "," + score + "\n")
