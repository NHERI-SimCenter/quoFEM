import sys
import os
import json
import stat
import subprocess
import platform

if __name__ == '__main__':

    inputArgs = sys.argv

    mainScriptPath = inputArgs[0]
    tmpSimCenterDir = inputArgs[1]
    templateDir = inputArgs[2]
    runType = inputArgs[3]  # either "runningLocal" or "runningRemote"

    # Change permission of workflow driver
    if platform.system() != "Windows":
        workflowDriverFile = os.path.join(templateDir, "workflow_driver")
        if runType in ['runningLocal']:
            os.chmod(workflowDriverFile, stat.S_IXUSR | stat.S_IRUSR | stat.S_IXOTH)
        st = os.stat(workflowDriverFile)
        os.chmod(workflowDriverFile, st.st_mode | stat.S_IEXEC)

    if runType in ["runningLocal"]:
        # Get path to python from dakota.json file
        dakotaJsonFile = os.path.join(os.path.abspath(templateDir), "dakota.json")
        with open(dakotaJsonFile, 'r') as f:
            jsonInputs = json.load(f)
        pythonCommand = jsonInputs["python"]

        # Get the path to the mainscript.py of TMCMC
        mainScriptDir = os.path.split(mainScriptPath)[0]
        mainScript = os.path.join(mainScriptDir, "mainscript.py")
        command = "{} {} {} {} {}".format(pythonCommand, mainScript, tmpSimCenterDir, templateDir, runType)
        try:
            result = subprocess.check_output(command, stderr=subprocess.STDOUT, shell=True)
            returnCode = 0
        except subprocess.CalledProcessError as e:
            result = e.output
            returnCode = e.returncode

