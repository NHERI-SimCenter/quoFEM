# written: Michael Gardner @ UNR

# import functions for Python 2.X support
from __future__ import division, print_function
import sys
if sys.version.startswith('2'): 
    range=xrange
    string_types = basestring
else:
    string_types = str

import shutil
import json
import os
import stat
import sys
import platform
from subprocess import Popen, PIPE
import subprocess
import argparse
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

    print("WORKING DIR MAIN", workDirMain)
    print("WORKING DIR TEMP", workDirTemp)    
    
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

    # Run UQ based on data and selected UQ engine--if you need to preprocess files with custom delimiters, use preprocessUQ.py
    configureAndRunUQ(uqData, simulationData, randomVarsData, demandParams, workDirTemp, runType)
    
    
        
        # # NEED TO WRITE THIS FOR GENERIC RUN--PROBABLY BETTER AS A CLASS, SO NO NEED TO CALL SUBPROCESS
        # prepareUQ(inputFile, workflow_driver, runType, platform.system)
        
        # # preprocessorCommand = '"{}/preprocessDakota" {} {} {} {}'.format(myScriptDir, inputFile, workflow_driver, runType, osType)
        # # subprocess.Popen(preprocessorCommand, shell=True).wait()
        # print("DONE RUNNING PREPROCESSOR\n")
        
        
        # # edps = samplingData["edps"]
        # numResponses = 0
        # responseDescriptors=[]
        
        # for edp in myEDPs:
        #     responseDescriptors.append(edp["name"])
        #     numResponses += 1

        #     # DO NOT NEED FEM PROGRAM IN THIS CASE
        #     # femProgram = femData["program"]
        #     # print(femProgram)
            
        #     # CONTINUE HERE, PERHAPS YOU SHOULD WRITE PREPARE UQ FIRST?
        #     if runType in ['runningLocal']:
        #         os.chmod(workflow_driver, stat.S_IXUSR | stat.S_IRUSR | stat.S_IXOTH)
                
        #         command = Dakota + ' -input dakota.in -output dakota.out -error dakota.err'
                
        #         #Change permision of workflow driver
        #         st = os.stat(workflow_driver)
        #         os.chmod(workflow_driver, st.st_mode | stat.S_IEXEC)
                
        #         # copy the dakota input file to the main working dir for the structure
        #         shutil.move("dakota.in", "../")
                
        #         # change dir to the main working dir for the structure
        #         os.chdir("../")
                
        #         cwd = os.getcwd()
        #         print(cwd)
                
        #         if runType in ['runningLocal']:
                    
        #             #    p = Popen(command, stdout=PIPE, stderr=PIPE, shell=True)
        #             #    for line in p.stdout:
        #             #        print(str(line))
                    
        #             dakotaCommand = "dakota -input dakota.in -output dakota.out -error dakota.err"
        #             print('running Dakota: ', dakotaCommand)
        #             try:
        #                 result = subprocess.check_output(dakotaCommand, stderr=subprocess.STDOUT, shell=True)
        #                 returncode = 0
        #             except subprocess.CalledProcessError as e:
        #                 result = e.output
        #                 returncode = e.returncode

if __name__ == '__main__':
    main()
