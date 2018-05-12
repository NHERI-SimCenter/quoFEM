#!/usr/bin/python

# written: fmk 01/18

import numpy
import math

desiredData = numpy.loadtxt("wanted.txt")
currentData = numpy.loadtxt("node.txt")
diff = currentData - desiredData;
numRows = diff.shape[0]
print(diff[0,1])

val1 = 0
val2 = 0
for j in range(0, numRows-1):
    val1 = val1 + diff[j,1]*diff[j,1]
    val2 = val2 + diff[j,2]*diff[j,2]

val1 = math.sqrt(val1);
val2 = math.sqrt(val2);

#result = math.sqrt(val1) + math.sqrt(val2)
#print("%.3f %.3f" % (val1,val2))

outFile = open('results.out','w')
outFile.write("%.3f %.3f" % (val1,val2))
outFile.close
