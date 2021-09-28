"""
authors: Mukesh Kumar Ramancha, Maitreya Manoj Kurumbhati, Prof. J.P. Conte, and Aakash Bangalore Satish*
affiliation: University of California, San Diego, *SimCenter, University of California, Berkeley

"""

import os
import json
import numpy as np
from shutil import copyfile
import sys
from importlib import import_module


class DataProcessingError(Exception):
    """Raised when errors found when processing user-supplied calibration and covariance data.

    Attributes:
        message -- explanation of the error
    """

    def __init__(self, message):
        self.message = message


def parseDataFunction(dakotaJsonLocation, logFile, workdirMain, mainscriptDir):
    # Read in the json object
    logFile.write("\n\n\tReading the json file")
    with open(dakotaJsonLocation, 'r') as f:
        jsonInputs = json.load(f)
    logFile.write(" ... Done")

    # Read in the data of the objects within the json file
    logFile.write('\n\n\tParsing the inputs read in from json file')
    applications = jsonInputs['Applications']
    edpInputs = jsonInputs['EDP']
    uqInputs = jsonInputs['UQ_Method']
    femInputs = jsonInputs['fem']
    localAppDirInputs = jsonInputs['localAppDir']
    pythonInputs = jsonInputs['python']
    rvInputs = jsonInputs['randomVariables']
    remoteAppDirInputs = jsonInputs['remoteAppDir']
    uqResultsInputs = jsonInputs['uqResults']
    if uqResultsInputs:
        resultType = uqResultsInputs['resultType']
        if resultType == 'UCSD_Results':
            spreadsheet = uqResultsInputs['spreadsheet']
            dataValues = spreadsheet['data']
            headings = spreadsheet['headings']
            numCol = spreadsheet['numCol']
            numRow = spreadsheet['numRow']
            summary = uqResultsInputs['summary']
    workingDir = jsonInputs['workingDir']

    # # Get total length of outputs
    # # logFile.write('\n\n\t\t==========================')
    # logFile.write('\n\n\t\tProcessing EDP definitions to get the names and lengths')
    # lineLength = 0
    # edpNamesList = []
    # edpLengthsList = []
    # # Get list of EDPs and their lengths
    # for edp in edpInputs:
    #     lineLength += edp["length"]
    #     edpNamesList.append(edp["name"])
    #     edpLengthsList.append(edp["length"])
    # logFile.write('\n\t\t\tThe EDPs defined are:')
    # printString = ""
    # for i in range(len(edpInputs)):
    #     printString += "\n\t\t\t\tName: '{}', Length: {}".format(edpNamesList[i], edpLengthsList[i])
    # logFile.write(printString)
    # logFile.write("\n\n\t\t\tExpected length of each line in data file: {}".format(lineLength))

    # Processing UQ inputs
    logFile.write("\n\n\t\tProcessing UQ inputs")
    seedval = uqInputs['seed']
    numberOfSamples = uqInputs['numParticles']
    logLikelihoodPath = uqInputs['logLikelihoodPath']
    logLikelihoodFile = uqInputs['logLikelihoodFile']
    calDataPath = uqInputs['calDataFilePath']
    calDataFile = uqInputs['calDataFile']
    readUserDefinedCovarianceData = uqInputs['readUserDefinedCovarianceData']
    numExperiments = uqInputs["numExperiments"]
    writeOutputs = True

    # # Check if calibration data files exist workdirMain, else raise error
    # logFile.write("\n\t\t\tSearching for the calibration data file")
    # if os.path.isfile(os.path.join(workdirMain, calDataFile)):
    #     logFile.write("\n\t\t\t\tFound calibration data file '{}' in \n\t\t\t\t\t{}.".format(calDataFile, workdirMain))
    # # elif os.path.isfile(os.path.join(workdirMain, calDataFile)):
    # #     logFile.write(
    # #         "\n\t\t\tDid not find calibration data file {} in {},\n\t\t\tbut found it in {}."
    # #         "\n\t\t\tCopying it to {}".format(calDataFile, workdirMain, workdirMain, workdirMain))
    # #     src = os.path.join(workdirMain, calDataFile)
    # #     dst = os.path.join(workdirMain, calDataFile)
    # #     copyfile(src, dst)
    # else:
    #     logFile.write(
    #         "\n\n\t\t\t\tERROR: Did not find calibration data file '{}' in \n\t\t\t\t\t{}".format(calDataFile,
    #                                                                                               workdirMain))
    #     raise DataProcessingError(
    #         "ERROR: Did not find calibration data file '{}' in {}".format(calDataFile, workdirMain))

    # # Check the line length for each experiment data
    # logFile.write('\n\n\t\t\tGetting the shape of the calibration data')
    # numExperimentsChecked = 0
    # linenum = 0
    # with open(os.path.join(workdirMain, calDataFile), "r") as f:
    #     for line in f:
    #         linenum += 1
    #         if len(line.strip()) == 0:
    #             continue
    #         else:
    #             line = line.replace(',', ' ')
    #             # Check length of each line
    #             words = line.split()
    #             if len(words) == lineLength:
    #                 numExperimentsChecked += 1
    #             else:
    #                 logFile.write(
    #                     "\n\t\t\t\tERROR: The number of calibration data terms in line {} of \n\t\t\t\t\t{} is {}."
    #                     "\n\t\t\t\t\tExpected number of data terms is equal to the sum of the length of the QoIs, "
    #                     "which is {}.".format(linenum, calDataFile, len(words), lineLength))
    #                 raise DataProcessingError(
    #                     "ERROR: The number of calibration data terms in line {} of {} is {}. "
    #                     "Expected number of data terms is equal to the sum of the length of the QoIs, "
    #                     "which is {}.".format(linenum, calDataFile, len(words), lineLength))
    #
    # logFile.write("\n\t\t\t\tThe shape of calibration data is: [{}, {}]".format(numExperiments, len(words)))
    # logFile.write("\n\t\t\t\tThe number of experiments is: {}".format(numExperiments))
    #
    # if numExperimentsChecked == numExperiments:
    #     logFile.write("\n\t\t\t\tThe number of calibration data terms for each experiment is: {}".format(lineLength))

    # # Check if any user defined error covariance files exist in workdirMain
    # logFile.write("\n\n\t\t\tSearching for any user-defined error covariance files")
    # for expNum in range(1, numExperiments + 1):
    #     logFile.write('\n\t\t\t\tExperiment number: {}'.format(expNum))
    #     for i, edpName in enumerate(edpNamesList):
    #         logFile.write('\n\t\t\t\t\tEDP: {}'.format(edpName))
    #         covarianceFileName = "{}.{}.sigma".format(edpName, expNum)
    #         covarianceFile = os.path.join(workdirMain, covarianceFileName)
    #
    #         if os.path.isfile(covarianceFile):
    #             logFile.write(
    #                 "\n\t\t\t\t\t\tFound covariance data file {} in {}.".format(covarianceFileName, workdirMain))
    #             # elif os.path.isfile(os.path.join(workdirMain, covarianceFileName)):
    #             #     logFile.write(
    #             #         "\n\t\t\tChecking to see if user-supplied file '{}' exists in '{}'".format(covarianceFileName,
    #             #                                                                                  workdirMain))
    #             #     logFile.write("\n\t\t\tFound a user supplied file.")
    #             #
    #             #     src = os.path.join(workdirMain, covarianceFileName)
    #             #     dst = os.path.join(workdirMain, covarianceFileName)
    #             #     logFile.write("\n\t\t\tCopying user-supplied covariance file from {} to {}".format(src, dst))
    #             #     copyfile(src, dst)
    #             #     covarianceFile = dst
    #
    #             logFile.write(
    #                 "\n\t\t\t\t\t\t\tReading user supplied covariance matrix from file: '{}'".format(covarianceFile))
    #             # Check the data in the covariance matrix file
    #             tmpCovFile = os.path.join(workdirMain, "quoFEMTempCovMatrixFile.sigma")
    #             numRows = 0
    #             numCols = 0
    #             linenum = 0
    #             with open(tmpCovFile, "w") as f1:
    #                 with open(covarianceFile, "r") as f:
    #                     for line in f:
    #                         linenum += 1
    #                         if len(line.strip()) == 0:
    #                             continue
    #                         else:
    #                             line = line.replace(',', ' ')
    #                             # Check the length of the line
    #                             words = line.split()
    #                             if numRows == 0:
    #                                 numCols = len(words)
    #                             else:
    #                                 if numCols != len(words):
    #                                     logFile.write("\n\t\t\t\t\t\t\tERROR: The number of columns in line {} do not "
    #                                                   "match the number of columns in the first line of "
    #                                                   "file {}.".format(numRows, covarianceFile))
    #                                     raise DataProcessingError(
    #                                         "ERROR: The number of columns in line {} do not match the "
    #                                         "number of columns in the first line of file {}.".format(
    #                                             numRows, covarianceFile))
    #                             tempLine = ""
    #                             for w in words:
    #                                 tempLine += "{} ".format(w)
    #                             if numRows == 0:
    #                                 f1.write(tempLine)
    #                             else:
    #                                 f1.write("\n")
    #                                 f1.write(tempLine)
    #                             numRows += 1
    #             covMatrix = np.genfromtxt(tmpCovFile)
    #
    #             logFile.write("\n\t\t\t\t\t\t\tFinished reading the file. Checking the dimensions of the covariance "
    #                           "data.")
    #             if numRows == 1:
    #                 if numCols == 1:
    #                     logFile.write(
    #                         "\n\t\t\t\t\t\t\t\tScalar variance value provided. The covariance matrix is an identity "
    #                         "matrix of size {}x{} multiplied by this scalar variance value.".format(edpLengthsList[i],
    #                                                                                                 edpLengthsList[i]))
    #                 elif numCols == edpLengthsList[i]:
    #                     logFile.write(
    #                         "\n\t\t\t\t\t\t\t\tA row vector provided. This will be treated as the diagonal entries of "
    #                         "the covariance matrix.")
    #                 else:
    #                     logFile.write("\n\t\t\t\t\t\t\t\tERROR: The number of columns of data in the covariance matrix "
    #                                   "file {} must be either 1 or {}. Found {} columns".format(covarianceFile,
    #                                                                                             edpLengthsList[i],
    #                                                                                             numCols))
    #                     raise DataProcessingError(
    #                         "ERROR: The number of columns of data in the covariance matrix file {}"
    #                         " must be either 1 or {}. Found {} columns".format(covarianceFile,
    #                                                                            edpLengthsList[i],
    #                                                                            numCols))
    #             elif numRows == edpLengthsList[i]:
    #                 if numCols == 1:
    #                     logFile.write(
    #                         "\n\t\t\t\t\t\t\t\tA column vector provided. This will be treated as the diagonal entries "
    #                         "of the covariance matrix.")
    #                 elif numCols == edpLengthsList[i]:
    #                     logFile.write("\n\t\t\t\t\t\t\t\tA full covariance matrix provided.")
    #                 else:
    #                     logFile.write("\n\t\t\t\t\t\t\t\tERROR: The number of columns of data in the covariance matrix "
    #                                   "file {} must be either 1 or {}. Found {} columns".format(covarianceFile,
    #                                                                                             edpLengthsList[i],
    #                                                                                             numCols))
    #                     raise DataProcessingError(
    #                         "ERROR: The number of columns of data in the covariance matrix file {}"
    #                         " must be either 1 or {}. Found {} columns".format(covarianceFile,
    #                                                                            edpLengthsList[i],
    #                                                                            numCols))
    #             else:
    #                 logFile.write("\n\t\t\t\t\t\t\t\tERROR: The number of rows of data in the covariance matrix file {}"
    #                               " must be either 1 or {}. Found {} rows".format(covarianceFile,
    #                                                                               edpLengthsList[i],
    #                                                                               numCols))
    #                 raise DataProcessingError("ERROR: The number of rows of data in the covariance matrix file {}"
    #                                           " must be either 1 or {}. Found {} rows".format(covarianceFile,
    #                                                                                           edpLengthsList[i],
    #                                                                                           numCols))
    #             logFile.write("\n\t\t\t\t\t\t\tCovariance matrix: {}".format(covMatrix))
    #         else:
    #             logFile.write("\n\t\t\t\t\t\t\tDid not find a user supplied file. Using the default variance value.")
    #             logFile.write("\n\t\t\t\t\t\t\tThe covariance matrix is an identity matrix multiplied by this value.")

    logFile.write("\n\n\t\t\tProcessing the log-likelihood script options")
    # If log-likelihood script is provided, use that, otherwise, use default log-likelihood function
    if len(logLikelihoodFile) > 0:  # if the log-likelihood file is not an empty string
        logFile.write("\n\t\t\t\tSearching for a user-defined log-likelihood script '{}'".format(logLikelihoodFile))
        if os.path.exists(os.path.join(workdirMain, logLikelihoodFile)):
            logFile.write("\n\t\t\t\tFound log-likelihood file '{}' in {}.".format(logLikelihoodFile, workdirMain))
            logLikeModuleName = os.path.splitext(logLikelihoodFile)[0]
            try:
                import_module(logLikeModuleName)
            except:
                logFile.write("\n\t\t\t\tERROR: The log-likelihood script '{}' cannot be imported.".format(
                    os.path.join(workdirMain, logLikelihoodFile)))
                # raise DataProcessingError("\nERROR: The log-likelihood script {} cannot be imported.".format(
                #     os.path.join(workdirMain, logLikelihoodFile)))
                raise
        # elif os.path.exists(os.path.join(logLikelihoodPath, logLikelihoodFile)):
        #     sys.path.append(logLikelihoodPath)
        #     logLikeModuleName = os.path.splitext(logLikelihoodFile)[0]
        #     logFile.write("\nUsing the user-supplied log-likelihood script: {}".format(
        #         os.path.join(logLikelihoodPath, logLikelihoodFile)))
        else:
            logFile.write(
                "\n\t\t\t\tERROR: The log-likelihood script '{}' cannot be found in {}.".format(logLikelihoodFile,
                                                                                                workdirMain))
            raise FileNotFoundError(
                "ERROR: The log-likelihood script '{}' cannot be found in {}.".format(logLikelihoodFile, workdirMain))
    else:
        defaultLogLikeFileName = "defaultLogLikeScript.py"
        defaultLogLikeDirectoryPath = mainscriptDir
        sys.path.append(defaultLogLikeDirectoryPath)
        logLikeModuleName = os.path.splitext(defaultLogLikeFileName)[0]
        logFile.write("\n\t\t\t\tUsing the default log-likelihood script: {}".format(
            os.path.join(defaultLogLikeDirectoryPath, defaultLogLikeFileName)))

    logLikeModule = import_module(logLikeModuleName)

    # Processing number of models
    logFile.write("\n\n\t\t\tGetting the number of models")
    inputFileList = []
    nModels = femInputs['numInputs']
    if nModels > 1:
        fileInfo = femInputs['fileInfo']
        for m in range(nModels):
            inputFileList.append(fileInfo[m]['inputFile'])
    else:
        inputFileList.append(femInputs['inputFile'])
    logFile.write('\n\t\t\t\tThe number of models is: {}'.format(nModels))

    # Variables
    variablesList = []
    variables = {'names': [], 'distributions': [], 'Par1': [], 'Par2': [], 'Par3': [], 'Par4': []}
    for m in range(nModels):
        variablesList.append(variables)

    logFile.write('\n\n\t\t\tLooping over the models')
    for ind in range(nModels):
        logFile.write('\n\t\t\t\tModel number: {}'.format(ind))
        # Processing RV inputs
        logFile.write('\n\t\t\t\t\tProcessing priors from RV inputs')
        for i, rv in enumerate(rvInputs):
            variablesList[ind]['names'].append(rv['name'])
            variablesList[ind]['distributions'].append(rv['distribution'])
            paramString = ""
            if rv['distribution'] == 'Uniform':
                variablesList[ind]['Par1'].append(rv['lowerbound'])
                variablesList[ind]['Par2'].append(rv['upperbound'])
                variablesList[ind]['Par3'].append(None)
                variablesList[ind]['Par4'].append(None)
                paramString = "params: {}, {}".format(rv['lowerbound'], rv['upperbound'])
            elif rv['distribution'] == 'Normal':
                variablesList[ind]['Par1'].append(rv['mean'])
                variablesList[ind]['Par2'].append(rv['stdDev'])
                variablesList[ind]['Par3'].append(None)
                variablesList[ind]['Par4'].append(None)
                paramString = "params: {}, {}".format(rv['mean'], rv['stdDev'])
            elif rv['distribution'] == 'Half-Normal':
                variablesList[ind]['Par1'].append(rv['Standard Deviation'])
                variablesList[ind]['Par2'].append(rv['Upper Bound'])
                variablesList[ind]['Par3'].append(None)
                variablesList[ind]['Par4'].append(None)
                paramString = "params: {}, {}".format(rv['Standard Deviation'], rv['Upper Bound'])
            elif rv['distribution'] == 'Truncated-Normal':
                variablesList[ind]['Par1'].append(rv['Mean'])
                variablesList[ind]['Par2'].append(rv['Standard Deviation'])
                variablesList[ind]['Par3'].append(rv['a'])
                variablesList[ind]['Par4'].append(rv['b'])
                paramString = "params: {}, {}, {}, {}".format(rv['Mean'], rv['Standard Deviation'], rv['a'], rv['b'])
            elif rv['distribution'] == 'Beta':
                variablesList[ind]['Par1'].append(rv['alphas'])
                variablesList[ind]['Par2'].append(rv['betas'])
                variablesList[ind]['Par3'].append(rv['lowerbound'])
                variablesList[ind]['Par4'].append(rv['upperbound'])
                paramString = "params: {}, {}, {}, {}".format(rv['alphas'], rv['betas'], rv['lowerbound'],
                                                              rv['upperbound'])
            elif rv['distribution'] == "Lognormal":
                meanValue = rv['mean']
                stdevValue = rv['stdDev']
                mu = np.log(pow(meanValue, 2) / np.sqrt(pow(stdevValue, 2) + pow(meanValue, 2)))
                sig = np.sqrt(np.log(pow(stdevValue / meanValue, 2) + 1))
                variablesList[ind]['Par1'].append(mu)
                variablesList[ind]['Par2'].append(sig)
                variablesList[ind]['Par3'].append(None)
                variablesList[ind]['Par4'].append(None)
                paramString = "params: {}, {}".format(mu, sig)
            elif rv['distribution'] == "Gumbel":
                variablesList[ind]['Par1'].append(rv['alphaparam'])
                variablesList[ind]['Par2'].append(rv['betaparam'])
                variablesList[ind]['Par3'].append(None)
                variablesList[ind]['Par4'].append(None)
                paramString = "params: {}, {}".format(rv['alphaparam'], rv['betaparam'])
            elif rv['distribution'] == "Weibull":
                variablesList[ind]['Par1'].append(rv['shapeparam'])
                variablesList[ind]['Par2'].append(rv['scaleparam'])
                variablesList[ind]['Par3'].append(None)
                variablesList[ind]['Par4'].append(None)
                paramString = "params: {}, {}".format(rv['shapeparam'], rv['scaleparam'])

            logFile.write(
                "\n\t\t\t\t\t\tRV number: {}, name: {}, dist: {}, {}".format(i, rv['name'], rv['distribution'],
                                                                             paramString))

        # Adding one prior distribution per EDP for the error covariance multiplier term
        logFile.write("\n\t\t\t\t\tAdding one prior distribution per EDP for the error covariance multiplier term")
        # logFile.write("\n\t\t\tThe prior on the error covariance multipliers is an inverse gamma distribution \n"
        #       "\t\twith parameters a and b set to 100. This corresponds to a variable whose mean \n"
        #       "\t\tand mode are approximately 1.0 and whose standard deviation is approximately 0.1.")
        # a = 100
        # b = 100
        # The prior with parameters = 100 is too narrow. Using these values instead:
        a = 3
        b = 2
        for i, edp in enumerate(edpInputs):
            name = edp['name'] + '.CovMultiplier'
            variablesList[ind]['names'].append(name)
            variablesList[ind]['distributions'].append('InvGamma')
            variablesList[ind]['Par1'].append(a)
            variablesList[ind]['Par2'].append(b)
            variablesList[ind]['Par3'].append(None)
            variablesList[ind]['Par4'].append(None)
            paramString = "params: {}, {}".format(a, b)
            logFile.write(
                "\n\t\t\t\t\t\tEDP number: {}, name: {}, dist: {}, {}".format(i, name, 'InvGamma', paramString))

    logFile.write('\n\n\tCompleted parsing the inputs')
    logFile.write('\n\n==========================')
    logFile.flush()
    os.fsync(logFile.fileno())
    # return (variablesList, numberOfSamples, seedval, logLikelihoodFile,
    #         calDataFile, edpInputs, writeOutputs)
    return (variablesList, numberOfSamples, seedval, logLikeModule,
            calDataFile, edpInputs, writeOutputs)
