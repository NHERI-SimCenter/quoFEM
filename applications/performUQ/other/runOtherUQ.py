# written: Michael Gardner @ UNR
# based on: parseDAKOTA.py

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
# Import custom preprocessor for dealint with different engine RV delimiters
import preProcessUQ

def main()
    parser = argparse.ArgumentParser(description='Generate workflow driver based on input configuration')
    parser.add_argument('--mainWorkDir', '-m', required=True, help="Main work directory")
    parser.add_argument('--tempWorkDir', '-t', required=True, help="Temporary work directory")
    parser.add_argument('--runType', '-r', required=True, help="Type of run")

    args = parser.parse_args()

    workDirMain = args.mainWorkDir
    workDirTemp = args.tempWorkDir
    runType = args.runType

    # CURRENTLY ONLY WORKS FOR LOCAL, STILL NEED TO EXPAND TO REMOTE
    # run on local computer
    if run_type in ['runningLocal',]:
        # MAC
        if (sys.platform == 'darwin'):
            # OpenSees = 'OpenSees'
            # Feap = 'feappv'
            # Dakota = 'dakota'
            workflow_driver = 'workflow_driver'
            
            # Windows
        elif:
            workflow_driver = 'workflow_driver'
        else:
            # OpenSees = 'OpenSees'
            # Feap = 'Feappv41.exe'
            # Dakota = 'dakota'
            workflow_driver = 'workflow_driver.bat'

            # Stampede @ DesignSafe, DON'T EDIT (NEED TO TEST THIS ON STAMPEDE--SHOULD BE CUSTOMIZED TO USER?)
    elif run_type in ['runningRemote',]:
        # OpenSees = '/home1/00477/tg457427/bin/OpenSees'
        # Feap = '/home1/00477/tg457427/bin/feappv'
        # Dakota = 'dakota'
        workflow_driver = 'workflow_driver'
        
    # change workdir to the templatedir
    os.chdir(workDirTemp)
    cwd = os.getcwd()
        
    # open the dakota json file (We'll leave this as dakota.json since Frank likes to hardcode things
    # into C++... After MainWindow.cpp get updated this could perhaps be changed to something more generic)
    with open('dakota.json') as data_file:    
        data = json.load(data_file)
        
        uqData = data["UQ_Method"]
        femData = data["fem"]
        rndData = data["randomVariables"]
        myEDPs = data["EDP"]
        
        # myScriptDir = os.path.dirname(os.path.realpath(__file__))
        inputFile = "dakota.json" # Another hardcoded name... why would that be here?
        
        # NEED TO WRITE THIS FOR GENERIC RUN--PROBABLY BETTER AS A CLASS, SO NO NEED TO CALL SUBPROCESS
        prepareUQ(inputFile, workflow_driver, runType, platform.system)
        
        # preprocessorCommand = '"{}/preprocessDakota" {} {} {} {}'.format(myScriptDir, inputFile, workflow_driver, runType, osType)
        # subprocess.Popen(preprocessorCommand, shell=True).wait()
        print("DONE RUNNING PREPROCESSOR\n")
        
        
        # edps = samplingData["edps"]
        numResponses = 0
        responseDescriptors=[]
        
        for edp in myEDPs:
            responseDescriptors.append(edp["name"])
            numResponses += 1

            # DO NOT NEED FEM PROGRAM IN THIS CASE
            # femProgram = femData["program"]
            # print(femProgram)
            
            # CONTINUE HERE, PERHAPS YOU SHOULD WRITE PREPARE UQ FIRST?
            if runType in ['runningLocal']:
                os.chmod(workflow_driver, stat.S_IXUSR | stat.S_IRUSR | stat.S_IXOTH)
                
                command = Dakota + ' -input dakota.in -output dakota.out -error dakota.err'
                
                #Change permision of workflow driver
                st = os.stat(workflow_driver)
                os.chmod(workflow_driver, st.st_mode | stat.S_IEXEC)
                
                # copy the dakota input file to the main working dir for the structure
                shutil.move("dakota.in", "../")
                
                # change dir to the main working dir for the structure
                os.chdir("../")
                
                cwd = os.getcwd()
                print(cwd)
                
                if runType in ['runningLocal']:
                    
                    #    p = Popen(command, stdout=PIPE, stderr=PIPE, shell=True)
                    #    for line in p.stdout:
                    #        print(str(line))
                    
                    dakotaCommand = "dakota -input dakota.in -output dakota.out -error dakota.err"
                    print('running Dakota: ', dakotaCommand)
                    try:
                        result = subprocess.check_output(dakotaCommand, stderr=subprocess.STDOUT, shell=True)
                        returncode = 0
                    except subprocess.CalledProcessError as e:
                        result = e.output
                        returncode = e.returncode

if __name__ == '__main__':
    main()
