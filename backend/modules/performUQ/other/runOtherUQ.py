# written: Michael Gardner @ UNR

import json
import os
import sys
import platform
# import argparse
from configureAndRunUQ import configureAndRunUQ

def main():
    # KEEP THIS FOR NOW--MAYBE BACKEND WILL BE UPDATED ACCEPT DIFFERENT ARGUMENTS...
    # parser = argparse.ArgumentParser(description='Generate workflow driver based on input configuration')
    # parser.add_argument('--mainWorkDir', '-m', required=True, help="Main work directory")
    # parser.add_argument('--tempWorkDir', '-t', required=True, help="Temporary work directory")
    # parser.add_argument('--runType', '-r', required=True, help="Type of run")
    # parser.add_argument('--inputFile', '-i', required=True, help="Input JSON file with configuration from UI")
    # Options for run type
    runTypeOptions=["runningLocal", "runningRemote"]
    
    # args = parser.parse_args()

    # workDirMain = args.mainWorkDir
    # workDirTemp = args.tempWorkDir
    # runType = args.runType
    # inputFile = args.inputFile

    inputArgs = sys.argv    
    workDirMain = inputArgs[1]
    workDirTemp = inputArgs[2]
    runType = inputArgs[3]
    inputFile = "dakota.json" # Why is this hardcoded, you might ask? Check with Frank...
    
    if runType not in runTypeOptions:
        raise ValueError("ERROR: Input run type has to be either local or remote")
    
    # change workdir to the templatedir
    os.chdir(workDirTemp)
    cwd = os.getcwd()
    
    # Open input file
    inputdata = {}
    with open(inputFile) as data_file:
        inputData = json.load(data_file)

    # Get data to pass to UQ driver
    uqData = inputData["UQ_Method"]
    simulationData = inputData["fem"]
    randomVarsData = inputData["randomVariables"]
    demandParams = inputData["EDP"]
    localAppDir = inputData["localAppDir"]
    remoteAppDir = inputData["remoteAppDir"]

    # Run UQ based on data and selected UQ engine--if you need to preprocess files with custom delimiters, use preprocessUQ.py
    configureAndRunUQ(uqData, simulationData, randomVarsData, demandParams, workDirTemp, runType, localAppDir, remoteAppDir)

if __name__ == '__main__':
    main()
