# written: UQ team @ SimCenter

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


inputArgs = sys.argv

workdir_main = inputArgs[1]
workdir_temp = inputArgs[2]
run_type = inputArgs[3]

# Replace the PATH TO strings with the path to the given executable in your 
# computer. The 'darwin' part corresponds to Mac, the 'else' clause corresponds 
# to Windows. You only need the path to either Feap or OpenSees depending on 
# which one you plan to use for the analysis. 

# run on local computer
if run_type in ['runningLocal',]:
    # MAC
    if (sys.platform == 'darwin'):
        OpenSees = 'OpenSees'
        Feap = 'feappv'
        Dakota = 'dakota'
        fem_driver = 'workflow_driver'

    # Windows
    else:
        OpenSees = 'OpenSees'
        Feap = 'Feappv41.exe'
        Dakota = 'dakota'
        fem_driver = 'workflow_driver.bat'

# Stampede @ DesignSafe, DON'T EDIT
elif run_type in ['runningRemote',]:
    OpenSees = '/home1/00477/tg457427/bin/OpenSees'
    Feap = '/home1/00477/tg457427/bin/feappv'
    Dakota = 'dakota'
    fem_driver = 'workflow_driver'

# change workdir to the templatedir
os.chdir(workdir_temp)
cwd = os.getcwd()

# open the dakota json file
with open('dakota.json') as data_file:    
    data = json.load(data_file)

uq_data = data["UQ_Method"]
fem_data = data["fem"]
rnd_data = data["randomVariables"]
my_edps = data["EDP"]

myScriptDir = os.path.dirname(os.path.realpath(__file__))
inputFile = "dakota.json"

osType = platform.system()
preprocessorCommand = '"{}/preprocessDakota" {} {} {} {}'.format(myScriptDir, inputFile, fem_driver, run_type, osType)
subprocess.Popen(preprocessorCommand, shell=True).wait()
print("DONE RUNNING PREPROCESSOR\n")


# edps = samplingData["edps"]
numResponses = 0
responseDescriptors=[]

for edp in my_edps:
    responseDescriptors.append(edp["name"])
    numResponses += 1

femProgram = fem_data["program"]
print(femProgram)


if run_type in ['runningLocal']:
    os.chmod(fem_driver, stat.S_IXUSR | stat.S_IRUSR | stat.S_IXOTH)

command = Dakota + ' -input dakota.in -output dakota.out -error dakota.err'

#Change permision of workflow driver
st = os.stat(fem_driver)
os.chmod(fem_driver, st.st_mode | stat.S_IEXEC)

# copy the dakota input file to the main working dir for the structure
shutil.move("dakota.in", "../")

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



