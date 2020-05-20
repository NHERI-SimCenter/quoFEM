#!/usr/bin/python

# written: fmk, adamzs 01/18

# import functions for Python 2.X support
from __future__ import division, print_function
import sys
if sys.version.startswith('2'): 
    range=xrange
    string_types = basestring
else:
    string_types = str

import sys

def process_results(inputArgs):

    #
    # process output file "node.out" for nodal displacements
    #

    with open ('node.out', 'rt') as inFile:
        line = inFile.readline()
        displ = line.split()
        numNode = len(displ)

    inFile.close

    # now process the input args and write the results file

    outFile = open('results.out','w')

    # note for now assuming no ERROR in user data
    for i in inputArgs:

        theList=i.split('_')

        if (len(theList) == 4):
            dof = int(theList[3])
        else:
            dof = 1
                      
        if (theList[0] == "Node"):
            nodeTag = int(theList[1])

            if (nodeTag > 0 and nodeTag <= numNode):
                if (theList[2] == "Disp"):
                    nodeDisp = abs(float(displ[((nodeTag-1)*2)+dof-1]))
                    outFile.write(str(nodeDisp))
                    outFile.write(' ')
                else:
                    outFile.write('0. ')
            else:
                outFile.write('0. ')
        else:
            outFile.write('0. ')

    outFile.close


if __name__ == "__main__":
    n = len(sys.argv)
    responses = []
    for i in range(1,n):
        responses.append(sys.argv[i])

    process_results(responses)
