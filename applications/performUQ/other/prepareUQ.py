# written: Michael Gardner @ UNR

# import sys
import preProcessUQ

def prepareUQ(paramsFile, inputFile, outputFile, rvSpecifier):
    # These are the delimiter choices, which can expanded as more UQ programs are added. Remember to also
    # extend the factory in rvDelimiter to handle addtional cases
    rvDelimiterChoices=["SimCenterDelimiter", "UQpyDelimiter"]
    
    if rvSpecifier not in rvDelimiterChoices:
        except IOError:
            print("ERROR: preProcessUQ.py: Symbol identifying value as random variable not recognized : ", rvSpecifier)            
    
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
