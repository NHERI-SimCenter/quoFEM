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
import re

def process_results(response):

    #print 'Number of arguments:', len(sys.argv), 'arguments.'
    #print 'Argument List:', str(sys.argv)

    inputArgs = response
    #outFile = open('results.out','w')

    #
    # process output file "SimCenterOut.txt" for nodal displacements
    #

    with open ('node.out', 'rt') as inFile:
        line = inFile.readline()
        print(line)
        print(line.split())
        displ = line.split()
        numNode = len(displ)

    print(numNode)

    inFile.close

    # now process the input args and write the results file

    outFile = open('results.out','w')

    # note for now assuming no ERROR in user data
    for i in inputArgs:
        print(i)
        theList=i.split('_')
        print(theList)
        if (theList[0] == "Node"):
            nodeTag = int(theList[1])
            print(nodeTag)

            if (nodeTag > 0 and nodeTag <= numNode):
                print(theList[2])
                if (theList[2] == "Disp"):
                    nodeDisp = displ[nodeTag-1]
                    print(nodeDisp)
                    outFile.write(nodeDisp)
                    outFile.write(' ')
                else:
                    print('NOTDISP')
                    outFile.write('0. ')
            else:
                outFile.write('0. ')
        else:
            print(theList[0])
            outFile.write('0. ')

    outFile.close
