#!python


temp_dir = "./../temp/"
executable = "./../bin/solver"

input_name = temp_dir + "input.txt"
output_name = temp_dir + "output.txt"


input = open(input_name, "w")

n = int(raw_input())
input.write(str(n) + "\n");
for i in range(n):
    s = raw_input()
    input.write(s + "\n")

input.close()

import os
import sys
os.system(executable + " -i " + input_name + " -o " + output_name)

output = open(output_name, "r")
s = output.read()
print s
sys.stdout.flush()
output.close()


