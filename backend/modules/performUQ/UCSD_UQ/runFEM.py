"""
authors: Dr. Frank McKenna*, Mukesh Kumar Ramancha, Maitreya Manoj Kurumbhati, and Prof. J.P. Conte 
affiliation: SimCenter*; University of California, San Diego

"""

import os
import platform
import subprocess
from pathlib import Path
import shutil


def copytree(src, dst, symlinks=False, ignore=None):
    if not os.path.exists(dst):
        os.makedirs(dst)
    for item in os.listdir(src):
        s = os.path.join(src, item)
        d = os.path.join(dst, item)
        if os.path.isdir(s):
            copytree(s, d, symlinks, ignore)
        else:
            if not os.path.exists(d) or os.stat(s).st_mtime - os.stat(d).st_mtime > 1:
                shutil.copy2(s, d)


def runFEM(ParticleNum, par, variables, resultsLocation, log_likelihood):
    """ 
    this function runs FE model (model.tcl) for each parameter value (par)
    model.tcl should take parameter input
    model.tcl should output 'output$PN.txt' -> column vector of size 'Ny'
    """

    stringtoappend = ("analysis" + str(ParticleNum))
    analysisPath = Path.joinpath(resultsLocation, stringtoappend)

    if os.path.isdir(analysisPath):
        pass
    else:
        os.mkdir(analysisPath)

    # copy templatefiles
    templateDir = Path.joinpath(resultsLocation, "templatedir")

    print('src: {}'.format(templateDir.as_posix()))
    print('dst: {}'.format(analysisPath))

    copytree(templateDir.as_posix(), analysisPath)

    os.chdir(analysisPath)

    # write input file
    ParameterName = variables["names"]
    f = open("params.in", "w")
    f.write('{}\n'.format(len(par)))
    for i in range(0, len(par)):
        name = str(ParameterName[i])
        value = str(par[i])

        f.write('{} {}\n'.format(name, value))
    f.close()

    # run FE model for the written input file
    # FNULL = open(os.devnull, 'w')
    #    call("OpenSees parinput" + str(ParticleNum) +".tcl", stdout=FNULL, stderr=subprocess.STDOUT)

    # env = os.environ

    if platform.system() == 'Windows':
        script = "workflow_driver.bat"
    else:
        script = "./workflow_driver"

    p = subprocess.Popen(script, stderr=subprocess.PIPE, shell=True)
    (output, err) = p.communicate()
    p_status = p.wait()

    # load output file (output file should be a column with Ny outputs)

    files = [f for f in os.listdir('.') if os.path.isfile(f)]
    for f in files:
        print(f)

    return log_likelihood()
