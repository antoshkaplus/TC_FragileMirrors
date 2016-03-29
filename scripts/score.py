
import sys
import os
import subprocess as sp

n_tests = 1
for i in range(n_tests):
    command = "java -jar FragileMirrorsVis.jar -exec \"../bin/colun -d\" -novis"
    #s = sp.check_output(["echo", "Hello world"])
    #s = sp.check_output(["echo", "Hello world"])
    #s = sp.check_output(command)
    command = ["java", "-jar", "FragileMirrorsVis.jar", "-exec", "../bin/psyho -d", "-novis"]
    s = sp.check_output(command)
    print "fuck all"
    print s
