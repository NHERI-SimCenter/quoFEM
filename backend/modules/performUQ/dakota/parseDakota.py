# written: UQ team @ SimCenter

# import functions for Python 2.X support
# from __future__ import division, print_function
# import sys
# if sys.version.startswith('2'): 
#     range=xrange
#     string_types = basestring
# else:
#     string_types = str

import shutil
import json
import os
import stat
import sys
import platform
from subprocess import Popen, PIPE
import subprocess
import glob


inputArgs = sys.argv

workdir_main = inputArgs[1]
workdir_temp = inputArgs[2]
run_type = inputArgs[3]

workflow_driver = 'blank'
workflow_driver1 = 'blank'

#with open('PATH.err', 'w') as f:
#    print(os.environ, file=f)

# Replace the PATH TO strings with the path to the given executable in your 
# computer. The 'darwin' part corresponds to Mac, the 'else' clause corresponds 
# to Windows. You only need the path to either Feap or OpenSees depending on 
# which one you plan to use for the analysis. 

# run on local computer
if run_type in ['runningLocal',]:

    if (sys.platform == 'darwin' or sys.platform == "linux" or sys.platform == "linux2"):
        Dakota = 'dakota'
        workflow_driver = 'workflow_driver'
        workflow_driver1 = 'workflow_driver1'        
    else:
        Dakota = 'dakota'
        workflow_driver = 'workflow_driver.bat'
        workflow_driver1 = 'workflow_driver1.bat'        

elif run_type in ['runningRemote',]:
    Dakota = 'dakota'
    workflow_driver = 'workflow_driver'
    workflow_driver1 = 'workflow_driver1'

# change workdir to the templatedir
os.chdir(workdir_temp)
cwd = os.getcwd()

myScriptDir = os.path.dirname(os.path.realpath(__file__))
inputFile = "dakota.json"

osType = platform.system()
preprocessorCommand = '"{}/preprocessDakota" {} {} {} {} {}'.format(myScriptDir, inputFile, workflow_driver, workflow_driver1, run_type, osType)
subprocess.Popen(preprocessorCommand, shell=True).wait()

print("DONE RUNNING PREPROCESSOR\n")

#femProgram = fem_data["program"]
#print(femProgram)

if run_type in ['runningLocal']:
    os.chmod(workflow_driver, stat.S_IXUSR | stat.S_IRUSR | stat.S_IXOTH)
    os.chmod(workflow_driver1, stat.S_IXUSR | stat.S_IRUSR | stat.S_IXOTH)

command = Dakota + ' -input dakota.in -output dakota.out -error dakota.err'

#Change permision of workflow driver
st = os.stat(workflow_driver)
os.chmod(workflow_driver, st.st_mode | stat.S_IEXEC)
os.chmod(workflow_driver1, st.st_mode | stat.S_IEXEC)

# copy the dakota input file to the main working dir for the structure
shutil.move("dakota.in", "../")

# If calibration data files exist, copy to the main working directory
if os.path.isfile("readCalibrationData.cal"):
    with open(inputFile, 'r') as f:
        jsonInputs = json.load(f)
    edpArray = jsonInputs['EDP']
    edpNames = [edp["name"] for edp in edpArray]

    for edpName in edpNames:
        datFileList = glob.glob("{}.*.dat".format(edpName))
        for datFile in datFileList:
            if datFile.split(".")[1].isdigit():
                shutil.move(datFile, "../")

    os.remove("readCalibrationData.cal")

# change dir to the main working dir for the structure
os.chdir("../")

cwd = os.getcwd()
print(cwd)

if run_type in ['runningLocal']:

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
