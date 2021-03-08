# written: Michael Gardner @ UNR

import sys
from rvDelimiter import *

def main():
    # These are the delimiter choices, which can expanded as more UQ programs are added. Remember to also
    # extend the factory in rvDelimiter to handle addtional cases
    rvDelimiterChoices=["SimCenterDelimiter", "UQpyDelimiter"]

    inputArgs = sys.argv    
    paramsFile = inputArgs[1]
    inputFile = inputArgs[2]
    outputFile = inputArgs[3]
    rvSpecifier = inputArgs[4]    
    
    if rvSpecifier not in rvDelimiterChoices:
        raise IOError("ERROR: preProcessUQ.py: Symbol identifying value as random variable not recognized : ", rvSpecifier)            
    
    # Open parameters file and read parameter settings
    numRVs = 0
    rvSettings = []

    rvSub = globals()[rvSpecifier]    
    try:
        with open(paramsFile, 'r') as params:
            numRVs = params.readline().strip()
            
            for line in params:
                rvSettings.append([i.strip() for i in line.split()])

    except IOError:
        print("ERROR: preProcessUQ.py could not open parameters file: " + paramsFile)

    # Next, open input file and search for random variables that need to be replaced by parameter realizations
    inputTemplate = "inputTemplate"
    realizationOutput = "outputFile"
    try:
        inputTemplate = open(inputFile, 'r')
    except IOError:
        print("ERROR: preProcessUQ.py could not open input template file: " + inputFile)

    try:
        realizationOutput = open(outputFile, 'w')
    except IOError:
        print("ERROR: preProcessUQ.py could not open output file: " + outputFile)        

    # Iterate over all lines in input template
    for line in inputTemplate:
        # Iterate over all RVs to check they need to be replaced
        for i, rv in enumerate(rvSettings):
            try:
                line = line.replace(rvSub().replaceRV(rv[0]), rv[1])                
            except:
                pass

        realizationOutput.write(line)

    realizationOutput.close()

if __name__ == '__main__':
    main()
