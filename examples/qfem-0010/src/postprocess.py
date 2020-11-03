#!/usr/bin/python

# written: fmk 01/18

import sys
import re

#print 'Number of arguments:', len(sys.argv), 'arguments.'
#print 'Argument List:', str(sys.argv)

inputArgs = sys.argv
#outFile = open('results.out','w')

#
# process output file "SimCenterOut.txt" for nodal displacements
#

ndm = 0

with open ('SimCenterOut.txt', 'rt') as inFile:
    line = inFile.readline()
    notFound = None
    while line and notFound == None:
        line = inFile.readline()
        notFound = re.search("Spatial Dimension of Mesh - - - - - :",line)
    words = line.split()
    ndm = int(words[len(words)-1])

    notFound = None
    while line and notFound == None:
        line = inFile.readline()
        notFound = re.search("N o d a l   D i s p l a c e m e n t s",line)
    line = inFile.readline()
    line = inFile.readline()
    line = inFile.readline()
    line = inFile.readline()
    displ = []
    displ.append(line.split())
    numNode = 1
    while  True:
        line2 = inFile.readline()
        words = line2.split()
        if (len(words) != 5):
            break;
        displ.append(words)
        numNode += 1

inFile.close

#
# now process the input args and write the results file
#

outFile = open('results.out','w')

#
# note for now assuming no ERROR in user data
#
for i in inputArgs[1:]:
    theList=i.split('_')

    if (theList[0] == 'Node'):
        nodeTag = int(theList[1])

        if (nodeTag > 0 and nodeTag <= numNode):
            if (theList[2] == 'Disp'):
                dof = int(theList[3])
                numNodalDOF = len(displ[nodeTag-1])-1-ndm;
                if (dof > 0 and dof <= numNodalDOF):
                    nodeDisp = displ[nodeTag-1]
                    nodeDisp = displ[nodeTag-1][ndm+dof]
                    outFile.write(nodeDisp)
                    outFile.write(' ')
                else:
                    outFile.write('0. ')
            else:
                outFile.write('0. ')
        else:
            outFile.write('0. ')
    else:
        outFile.write('0. ')

outFile.close
