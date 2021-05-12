"""
authors: Mukesh Kumar Ramancha, Maitreya Manoj Kurumbhati, Prof. J.P. Conte, Aakash Bangalore Satish (*)
affiliation: University of California, San Diego, (*) SimCenter, University of California, Berkeley

"""

import os
import sys
import shutil
# import pickle
from pathlib import Path
import csv
from importlib import import_module
import numpy as np

from parseData import parseDataFunction
import pdfs
from runTMCMC import RunTMCMC

inputArgs = sys.argv

workdir_main = inputArgs[1]
workdir_temp = inputArgs[2]
run_type = inputArgs[3]


class DataProcessingError(Exception):
    """Raised when errors found when processing user-supplied calibration and covariance data.

    Attributes:
        message -- explanation of the error
    """

    def __init__(self, message):
        self.message = message


# dakotaJsonLocation = Path.joinpath(Path(workdir_temp), "dakota.json")
dakotaJsonLocation = os.path.join(os.path.abspath(workdir_temp), "dakota.json")
variablesList, numberOfSamples, seedval, resultsLocation, resultsPath, logLikelihoodDirectoryPath, \
logLikelihoodFilename, calDataPath, calDataFileName, edpList = parseDataFunction(dakotaJsonLocation)

# If loglikelihood script is provided, use that, otherwise, use default loglikelihood function
if len(logLikelihoodDirectoryPath) > 0:  # if the path is not an empty string
    if os.path.exists(os.path.join(logLikelihoodDirectoryPath, logLikelihoodFilename)):
        sys.path.append(logLikelihoodDirectoryPath)
        logLikeModuleName = os.path.splitext(logLikelihoodFilename)[0]
    else:
        raise DataProcessingError("ERROR: The loglikelihood script cannot be found. Using the default loglikelihood.")
        # logLikeModuleName = "defaultLogLikeScript"
else:
    print("Using the default loglikelihood")
    logLikeModuleName = "defaultLogLikeScript"

logLikeModule = import_module(logLikeModuleName)

lineLength = 0
edpNamesList = []
edpLengthsList = []
# Get list of EDPs and their lengths
for edp in edpList:
    lineLength += edp["length"]
    edpNamesList.append(edp["name"])
    edpLengthsList.append(edp["length"])

# Process calibration data file
calDataFile = os.path.join(calDataPath, calDataFileName)
tempCalDataFile = os.path.join(workdir_temp, "quoFEMTempCalibrationDataFile.cal")
f1 = open(tempCalDataFile, "w")
numExperiments = 0
linenum = 0
with open(calDataFile, "r") as f:
    for line in f:
        linenum += 1
        if len(line.strip()) == 0:
            continue
        else:
            line = line.replace(',', ' ')
            # Check length of each line
            words = line.split()
            tempLine = ""
            if len(words) == lineLength:
                for w in words:
                    tempLine += "{} ".format(w)
                print("Line {}: ".format(linenum), tempLine)
                if numExperiments == 0:
                    f1.write(tempLine)
                else:
                    f1.write("\n")
                    f1.write(tempLine)
                numExperiments += 1
            else:
                raise DataProcessingError("ERROR: The number of entries ({}) in line num {} of the file '{}' "
                                          "does not match the expected length {}".format(len(words), linenum,
                                                                                         calDataFile, lineLength))
f1.close()

# Create the covariance matrix
covarianceMatrixList = []
covarianceTypeList = []
# First, check if the user has passed in any covariance matrix data
for expNum in range(1, numExperiments + 1):
    for i, edpName in enumerate(edpNamesList):
        covarianceFileName = "{}.{}.sigma".format(edpName, expNum)
        covarianceFile = os.path.join(calDataPath, covarianceFileName)
        if os.path.isfile(covarianceFile):
            # Check the data in the covariance matrix file
            tmpCovFile = os.path.join(calDataPath, "quoFEMTempCovMatrixFile.sigma")
            numRows = 0
            numCols = 0
            linenum = 0
            with open(tmpCovFile, "w") as f1:
                with open(covarianceFile, "r") as f:
                    for line in f:
                        linenum += 1
                        if len(line.strip()) == 0:
                            continue
                        else:
                            line = line.replace(',', ' ')
                            # Check length of each line
                            words = line.split()
                            if numRows == 0:
                                numCols = len(words)
                            else:
                                if numCols != len(words):
                                    raise DataProcessingError(
                                        "ERROR: The number of columns in line {} do not match the "
                                        "number of columns in line {} of file {}.".format(
                                            numRows, numRows - 1, covarianceFile))
                            tempLine = ""
                            for w in words:
                                tempLine += "{} ".format(w)
                            print("covMatrixLine {}: ".format(linenum), tempLine)
                            if numRows == 0:
                                f1.write(tempLine)
                            else:
                                f1.write("\n")
                                f1.write(tempLine)
                            numRows += 1
            covMatrix = np.genfromtxt(tmpCovFile)
            os.remove(tmpCovFile)

            if numRows == 1:
                if numCols == 1:
                    covarianceTypeList.append('scalar')
                elif numCols == edpLengthsList[i]:
                    covarianceTypeList.append('diagonal')
                else:
                    raise DataProcessingError("ERROR: The number of columns of data in the covariance matrix file {}"
                                              " must be either 1 or {}. Found {} columns".format(covarianceFile,
                                                                                                 edpLengthsList[i],
                                                                                                 numCols))
            elif numRows == edpLengthsList[i]:
                if numCols == 1:
                    covarianceTypeList.append('diagonal')
                elif numCols == edpLengthsList[i]:
                    covarianceTypeList.append('matrix')
                else:
                    raise DataProcessingError("ERROR: The number of columns of data in the covariance matrix file {}"
                                              " must be either 1 or {}. Found {} columns".format(covarianceFile,
                                                                                                 edpLengthsList[i],
                                                                                                 numCols))
            else:
                raise DataProcessingError("ERROR: The number of rows of data in the covariance matrix file {}"
                                          " must be either 1 or {}. Found {} rows".format(covarianceFile,
                                                                                          edpLengthsList[i],
                                                                                          numCols))
        else:
            scalarVariance = np.ones((1,))
            covarianceMatrixList.append(scalarVariance)
            covarianceTypeList.append('scalar')
print(covarianceMatrixList, covarianceTypeList)

# %%

sys.path.append(resultsPath)

# set the seed
np.random.seed(seedval)

# number of particles: Np
Np = numberOfSamples

# number of max MCMC steps
Nm_steps_max = 2
Nm_steps_maxmax = 5

# %% For each model:
for variables in variablesList:
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

        if variables["distributions"][i] == 'InvGamma':
            a = float(variables['Par1'][i])
            b = float(variables['Par2'][i])

            AllPars.append(pdfs.InvGamma(a=a, b=b))
    # %%

    ''' Run the Algorithm '''

    if __name__ == '__main__':
        mytrace = RunTMCMC(Np, AllPars, Nm_steps_max, Nm_steps_maxmax, logLikeModule.log_likelihood, variables,
                           resultsLocation, seedval, tempCalDataFile, numExperiments, covarianceMatrixList,
                           edpNamesList, edpLengthsList, covarianceTypeList)

        ''' Compute model evidence '''
        evidence = 1
        for i in range(len(mytrace)):
            Wm = mytrace[i][2]
            evidence = evidence * (sum(Wm) / len(Wm))
        print("\n\nModel evidence: {:e}\n\n".format(evidence))

        ''' Write Data to '.csv' file '''

        dataToWrite = mytrace[-1][0]

        stringtoappend = 'results.csv'
        # resultsFilePath = Path.joinpath(resultsLocation, stringtoappend)
        resultsFilePath = os.path.join(os.path.abspath(resultsLocation), stringtoappend)

        with open(resultsFilePath, 'w', newline='') as csvfile:

            csvwriter = csv.writer(csvfile)
            csvwriter.writerows(dataToWrite)

        ''' Write the results to a file named dakotaTab.out for quoFEM to be able to read the results '''

        # tabFilePath = Path.joinpath(resultsLocation, "dakotaTab.out")
        tabFilePath = os.path.join(resultsLocation, "dakotaTab.out")

        ''' Create the headings, which will be the first line of the file '''
        headings = 'eval_id\tinterface\t'
        for v in variables['names']:
            headings += '{}\t'.format(v)
        headings += '\n'

        with open(tabFilePath, "w") as f:
            f.write(headings)
            for i in range(Np):
                string = "{}\t{}\t".format(i + 1, 1)
                for j in range(len(variables['names'])):
                    string += "{}\t".format(dataToWrite[i, j])
                string += "\n"
                f.write(string)

        ''' Delete Analysis Folders '''

        for analysisNumber in range(0, Np):
            stringtoappend = ("analysis" + str(analysisNumber))
            analysisLocation = os.path.join(resultsLocation, stringtoappend)
            # analysisPath = Path(analysisLocation)
            analysisPath = os.path.abspath(analysisLocation)
            shutil.rmtree(analysisPath)
