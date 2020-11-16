"""
authors: Mukesh Kumar Ramancha, Maitreya Manoj Kurumbhati, and Prof. J.P. Conte 
affiliation: University of California, San Diego

"""

import os
import sys
import shutil
import pickle
from pathlib import Path
import csv

from parseData import parseDataFunction

inputArgs = sys.argv

workdir_main = inputArgs[1]
workdir_temp = inputArgs[2]
run_type = inputArgs[3]

dakotaJsonLocation = Path.joinpath(Path(workdir_temp), "dakota.json")
variables, numberOfSamples, resultsLocation, resultsPath, logLikelihoodDirectoryPath, logLikelihoodFilename = parseDataFunction(dakotaJsonLocation)

print('numVariables {}', format(variables))
print('numSamples {}', format(numberOfSamples))

import pdfs
from runTMCMC import RunTMCMC
# from postProcessing import log_likelihood

sys.path.append(logLikelihoodDirectoryPath)
logLikeModuleName = os.path.splitext(logLikelihoodFilename)[0]
from importlib import import_module
logLikeModule = import_module(logLikeModuleName)

# %%

sys.path.append(resultsPath)

# number of particles: Np
Np = numberOfSamples

# number of max MCMC steps
Nm_steps_max = 2
Nm_steps_maxmax = 5

# %%

''' Assign probability distributions to the parameters  '''

AllPars = []

for i in range(len(variables["names"])):

    if variables["distributions"][i] == 'Uniform':
        VariableLowerLimit = float(variables['Par1'][i])
        VariableUpperLimit = float(variables['Par2'][i])

        AllPars.append(pdfs.Uniform(lower=VariableLowerLimit, upper=VariableUpperLimit))

    if variables["distributions"][i] == 'Normal':
        VariableMean = float(variables['Par1'][i])
        VariableSD = float(variables['Par2'][i])

        AllPars.append(pdfs.Normal(mu=VariableMean, sig=VariableSD))

    if variables["distributions"][i] == 'Half-Normal':
        VariableSD = float(variables['Par1'][i])

        AllPars.append(pdfs.Halfnormal(sig=VariableSD))

    if variables["distributions"][i] == 'Truncated-Normal':
        VariableMean = float(variables['Par1'][i])
        VariableSD = float(variables['Par2'][i])
        VariableLowerLimit = float(variables['Par3'][i])
        VariableUpperLimit = float(variables['Par4'][i])

        AllPars.append(pdfs.TrunNormal(mu=VariableMean, sig=VariableSD, a=VariableLowerLimit, b=VariableUpperLimit))

# %%

''' Run the Algorithm '''

if __name__ == '__main__':
    mytrace = RunTMCMC(Np, AllPars, Nm_steps_max, Nm_steps_maxmax, logLikeModule.log_likelihood, variables, resultsLocation)

    ''' Write Data to '.csv' file '''

    dataToWrite = mytrace[-1][0]

    stringtoappend = 'results.csv'
    resultsFilePath = Path.joinpath(resultsLocation, stringtoappend)

    with open(resultsFilePath, 'w', newline='') as csvfile:

        csvwriter = csv.writer(csvfile)
        csvwriter.writerows(dataToWrite)

    ''' Write the results to a file named dakotaTab.out for quoFEM to be able to read the results '''

    tabFilePath = Path.joinpath(resultsLocation, "dakotaTab.out")

    ''' Create the headings, which will be the first line of the file '''
    headings = 'eval_id\tinterface\t'
    for v in variables['names']:
        headings += '{}\t'.format(v)
    headings += '\n'

    with open(tabFilePath, "w") as f:
        f.write(headings)
        for i in range(Np):
            string = "{}\t{}\t".format(i+1, 1)
            for j in range(len(variables['names'])):
                string += "{}\t".format(dataToWrite[i, j])
            string += "\n"
            f.write(string)

    ''' Delete Analysis Folders '''

    for analysisNumber in range(0, Np):
        stringtoappend = ("analysis" + str(analysisNumber))
        analysisLocation = os.path.join(resultsLocation, stringtoappend)
        analysisPath = Path(analysisLocation)
        shutil.rmtree(analysisPath)
