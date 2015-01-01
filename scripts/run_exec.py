#!python

input = open("input.txt", "w")

n = int(raw_input())
input.write(str(n) + "\n");
for i in range(n):
    s = raw_input()
    input.write(s + "\n")

input.close()

executable = "./../build-xcode/Release/FragileMirrors"
import os
os.system(executable + " -solve -i input.txt -o output.txt")

output = open("output.txt", "r")
s = output.read()
print s
output.close()


