import sys
import os
from subprocess import Popen, PIPE
import subprocess

def main():
    
    paramsIn = sys.argv[1]
    paramsOut = sys.argv[2]
    
    #
    # first create paramsOut file
    #
    
    if not os.path.isfile(paramsIn):
        print("Input param file {} does not exist. Exiting...".format(paramsIn))
        sys.exit()

    outFILE = open(paramsOut, "w")
    
    with open(paramsIn) as inFILE:
        line = inFILE.readline()
        splitLine = line.split()
        numRV = int(splitLine[3])
        print(numRV, file=outFILE)
        
        # read numParameters
        for i in range(numRV):
            line = inFILE.readline()
            splitLine = line.split()
            nameRV = splitLine[1]                    
            valueRV = splitLine[3]        
            print('{} {}'.format(nameRV, valueRV), file=outFILE)

    outFILE.close
    inFILE.close

if __name__ == '__main__':
    main()
    
