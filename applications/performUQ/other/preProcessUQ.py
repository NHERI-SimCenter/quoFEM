# written: Michael Gardner @ UNR

import argparse
# import sys
from rvDelimiter import RvDelimiterFactory

# main function
def main():

    parser = argparse.ArgumentParser(description='Preprocess simulation configuration files to replace parameter realizations')
    parser.add_argument('--params', '-p', required=True, help="Parameter realizations")
    parser.add_argument('--inputFile', '-i', required=True, help="Input template")
    parser.add_argument('--outputFile', '-o', required=True, help="Output file")
    parser.add_argument('--rvSpecifier', '-s', required=True, help="Symbol identifying value as random variable",
                        choices=["SimCenterDelimiter", "UQpyDelimiter"])
    parser.add_argument('--fixedFormat', '-f', required=False, help="Flag for identifying whether fixed format output is required",
                        action='store_true')

    args = parser.parse_args()
    
    paramsFile = args.params
    inputFile = args.inputFile
    outputFile = args.outputFile
    fixedFormat = args.fixedFormat
    rvSpecifier = RvDelimiterFactory.createRvDelimiter(args.rvSpecifier)
    
    # Open parameters file and read parameter settings
    numRVs = 0
    lineCount = 0
    rvNames = []
    rvSettings = []
    
    try:
        with open(paramsFile, 'r') as params:
            for line in params:
                if lineCount == 0:
                    rvNames = [i.strip() for i in line.split(',')]
                    numRVs = len(rvNames)
                    # Replace RV names based on delimiter
                    for i, rv in enumerate(rvNames):
                        rvNames[i] = rvSpecifier.replaceRV(rv)

                else:
                    rvSettings = [i.strip() for i in line.split(',')]

                lineCount = lineCount + 1
                    
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
        for i, rv in enumerate(rvNames):
            try:
                line = line.replace(rv, rvSettings[i])
            except:
                pass

        realizationOutput.write(line)

    realizationOutput.close()

 
if __name__ == '__main__':
    main()
