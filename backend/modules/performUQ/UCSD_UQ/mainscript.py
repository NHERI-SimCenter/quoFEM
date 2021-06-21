"""
authors: Mukesh Kumar Ramancha, Maitreya Manoj Kurumbhati, Prof. J.P. Conte, Aakash Bangalore Satish*
affiliation: University of California, San Diego, *SimCenter, University of California, Berkeley

"""

# ======================================================================================================================
import os
import sys
import csv
from importlib import import_module
import numpy as np

from parseData import parseDataFunction
import pdfs
from runTMCMC import RunTMCMC


# ======================================================================================================================
class DataProcessingError(Exception):
    """Raised when errors found when processing user-supplied calibration and covariance data.

    Attributes:
        message -- explanation of the error
    """

    def __init__(self, message):
        self.message = message


# ======================================================================================================================
if __name__ == '__main__':

    inputArgs = sys.argv

    mainscript_path = inputArgs[0]
    mainscript_dir = os.path.split(mainscript_path)[0]
    workdir_main = inputArgs[1]
    workdir_template = inputArgs[2]
    run_type = inputArgs[3]  # either "runningLocal" or "runningRemote"

    # Create a logFile for recording the output
    logFile = open(os.path.join(workdir_main, "logFileTMCMC"), "w")

    # ======================================================================================================================
    logFile.write("\nRunning quoFEM's UCSD_UQ engine workflow")
    logFile.write('\nCWD: {}'.format(os.path.abspath('.')))
    dakotaJsonLocation = os.path.join(os.path.abspath(workdir_template), "dakota.json")
    logFile.write('\n\n==========================')
    logFile.write("\nParsing the json input file {}".format(dakotaJsonLocation))
    (variablesList, numberOfSamples, seedval, resultsLocation, resultsPath, logLikelihoodDirectoryPath,
     logLikelihoodFilename, calDataPath, calDataFileName, edpList, writeOutputs) = parseDataFunction(dakotaJsonLocation,
                                                                                                     logFile)
    logFile.flush()
    os.fsync(logFile.fileno())

    # ======================================================================================================================
    logFile.write('\n\n==========================')
    logFile.write("\nProcessing log-likelihood script options")
    # If loglikelihood script is provided, use that, otherwise, use default loglikelihood function
    if len(logLikelihoodDirectoryPath) > 0:  # if the path is not an empty string
        if os.path.exists(os.path.join(logLikelihoodDirectoryPath, logLikelihoodFilename)):
            sys.path.append(logLikelihoodDirectoryPath)
            logLikeModuleName = os.path.splitext(logLikelihoodFilename)[0]
            logFile.write("\nUsing the user-supplied log-likelihood script: {}".format(
                os.path.join(logLikelihoodDirectoryPath, logLikelihoodFilename)))
        else:
            logFile.write("\nERROR: The loglikelihood script {} cannot be found.".format(
                os.path.join(logLikelihoodDirectoryPath, logLikelihoodFilename)))
            raise FileNotFoundError("ERROR: The loglikelihood script {} cannot be found.".format(
                os.path.join(logLikelihoodDirectoryPath, logLikelihoodFilename)))
    else:
        defaultLogLikeFileName = "defaultLogLikeScript.py"
        defaultLogLikeDirectoryPath = mainscript_dir
        sys.path.append(defaultLogLikeDirectoryPath)
        logLikeModuleName = os.path.splitext(defaultLogLikeFileName)[0]
        logFile.write("\nUsing the default loglikelihood script: {}".format(
            os.path.join(defaultLogLikeDirectoryPath, defaultLogLikeFileName)))

    logLikeModule = import_module(logLikeModuleName)

    # ======================================================================================================================
    logFile.write('\n\n==========================')
    logFile.write('\nProcessing EDP definitions')
    lineLength = 0
    edpNamesList = []
    edpLengthsList = []
    # Get list of EDPs and their lengths
    for edp in edpList:
        lineLength += edp["length"]
        edpNamesList.append(edp["name"])
        edpLengthsList.append(edp["length"])
    logFile.write('\nThe EDPs defined are:')
    printString = ""
    for i in range(len(edpList)):
        printString += "Name: '{}', Length: {}\n".format(edpNamesList[i], edpLengthsList[i])
    logFile.write(printString)
    logFile.write("\nExpected length of each line in data file: {}".format(lineLength))

    # ======================================================================================================================
    # Process calibration data file
    logFile.write('\n\n==========================')
    logFile.write('\nProcessing calibration data file')
    calDataFile = os.path.join(calDataPath, calDataFileName)
    logFile.write("\nCalibration data file being processed: {}\n".format(calDataFile))
    tempCalDataFile = os.path.join(workdir_template, "quoFEMTempCalibrationDataFile.cal")
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
                    logFile.write("\nLine {}, length {}: {}".format(linenum, len(words), tempLine))
                    if numExperiments == 0:
                        f1.write(tempLine)
                    else:
                        f1.write("\n")
                        f1.write(tempLine)
                    numExperiments += 1
                else:
                    logFile.write("\nERROR: The number of entries ({}) in line num {} of the file '{}' "
                                  "does not match the expected length {}".format(len(words), linenum,
                                                                                 calDataFile, lineLength))
                    raise DataProcessingError("ERROR: The number of entries ({}) in line num {} of the file '{}' "
                                              "does not match the expected length {}".format(len(words), linenum,
                                                                                             calDataFile, lineLength))
    f1.close()

    # Read in the calibration data
    calibrationData = np.atleast_2d(np.genfromtxt(tempCalDataFile))
    logFile.write(
        "\nFinished reading in calibration data. Shape of calibration data: {}\n".format(np.shape(calibrationData)))
    logFile.write('\nThe number of experiments: {}'.format(np.shape(calibrationData)[0]))
    logFile.write('\nThe number of calibration terms per experiment: {}'.format(np.shape(calibrationData)[1]))

    # Compute the normalizing factors - absolute maximum of the data for each response variable
    logFile.write(
        "\nComputing normalizing factors. The normalizing factors used are the absolute maximum of the data for each "
        "response variable. The data and the prediction will be divided by the normalizing factors.")
    locShiftList = []
    normalizingFactors = []
    currentPosition = 0
    locShift = 0.0
    for j in range(len(edpList)):
        calibrationDataSlice = calibrationData[:, currentPosition:currentPosition + edpLengthsList[j]]
        absMax = np.absolute(np.max(calibrationDataSlice))
        if absMax == 0:  # This is to handle the case if abs max of data = 0.
            locShift = 1.0
            absMax = 1.0
        locShiftList.append(locShift)
        normalizingFactors.append(absMax)
        calibrationDataSlice += locShift
        calibrationData[:, currentPosition:currentPosition + edpLengthsList[j]] = calibrationDataSlice / absMax
        currentPosition += edpLengthsList[j]
    logFile.write("\nNormalized calibration data: \n{}".format(calibrationData))

    logFile.write("\nThe normalizing factors computed are: ")
    for j in range(len(edpList)):
        logFile.write("\nEDP: {}, normalizing factor: {}".format(edpNamesList[j], normalizingFactors[j]))

    logFile.write("\n\nThe locShift values are: ")
    for j in range(len(edpList)):
        logFile.write("\nEDP: {}, locShift: {}".format(edpNamesList[j], locShiftList[j]))

    # ======================================================================================================================
    # Processing covariance matrix options
    logFile.write('\n\n==========================')
    logFile.write('\nProcessing options for variance/covariance:')
    logFile.write('\nOne variance value or covariance matrix will be used per response quantity per experiment.')
    logFile.write('\nIf the user does not supply variance or covariance data, a default variance value will be\n'
                  'used per response quantity, which is constant across experiments. The default variance is\n'
                  'computed as the variance of the normalized data, if there is data from more than one experiment. \n'
                  'If there is data from only one experiment, then a default variance value is computed by \n'
                  'assuming that the standard deviation of the error is 5% of the absolute maximum value of \n'
                  'the corresponding normalized response data.')

    # For each response variable, compute the variance of the data. These will be the default error variance
    # values used in the calibration process. Values of the multiplier on these default error variance values will be
    # calibrated. There will be one such error variance value per response quantity. If there is only data from one
    # experiment,then the default error std.dev. value is assumed to be 5% of the absolute maximum value of the data
    # corresponding to that response quantity.
    scaleFactors = np.zeros_like(edpNamesList, dtype=float)
    if np.shape(calibrationData)[0] > 1:  # if there are more than 1 rows of data, i.e. data from multiple experiments
        currentIndex = 0
        for i in range(len(edpNamesList)):
            dataSlice = calibrationData[:, currentIndex:currentIndex + edpLengthsList[i]]
            scaleFactors[i] = np.nanvar(dataSlice)
            currentIndex += edpLengthsList[i]
    else:
        currentIndex = 0
        for i in range(len(edpNamesList)):
            dataSlice = calibrationData[:, currentIndex:currentIndex + edpLengthsList[i]]
            scaleFactors[i] = (0.05 * np.max(np.absolute(dataSlice))) ** 2
            currentIndex += edpLengthsList[i]

    # Create the covariance matrix
    covarianceMatrixList = []
    covarianceTypeList = []
    logFile.write("\n\nLooping over the experiments and EDPs")
    # First, check if the user has passed in any covariance matrix data
    for expNum in range(1, numExperiments + 1):
        logFile.write('\n\nExperiment number: {}'.format(expNum))
        for i, edpName in enumerate(edpNamesList):
            logFile.write('\n\tEDP: {}'.format(edpName))
            covarianceFileName = "{}.{}.sigma".format(edpName, expNum)
            covarianceFile = os.path.join(calDataPath, covarianceFileName)
            logFile.write(
                "\n\t\tChecking to see if user-supplied file '{}' exists in '{}'".format(covarianceFileName,
                                                                                         calDataPath))
            if os.path.isfile(covarianceFile):
                logFile.write("\n\t\tFound a user supplied file.")
                logFile.write("\n\t\tReading in user supplied covariance matrix from file: '{}'".format(covarianceFile))
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
                                # Check the length of the line
                                words = line.split()
                                if numRows == 0:
                                    numCols = len(words)
                                else:
                                    if numCols != len(words):
                                        logFile.write("\nERROR: The number of columns in line {} do not match the "
                                                      "number of columns in line {} of file {}.".format(numRows,
                                                                                                        numRows - 1,
                                                                                                        covarianceFile))
                                        raise DataProcessingError(
                                            "ERROR: The number of columns in line {} do not match the "
                                            "number of columns in line {} of file {}.".format(
                                                numRows, numRows - 1, covarianceFile))
                                tempLine = ""
                                for w in words:
                                    tempLine += "{} ".format(w)
                                # logFile.write("\ncovMatrixLine {}: ".format(linenum), tempLine)
                                if numRows == 0:
                                    f1.write(tempLine)
                                else:
                                    f1.write("\n")
                                    f1.write(tempLine)
                                numRows += 1
                covMatrix = np.genfromtxt(tmpCovFile)
                covarianceMatrixList.append(covMatrix)
                # os.remove(tmpCovFile)
                logFile.write("\n\t\tFinished reading the file. Checking the dimensions of the covariance data.")
                if numRows == 1:
                    if numCols == 1:
                        covarianceTypeList.append('scalar')
                        logFile.write(
                            "\n\t\tScalar variance value provided. The covariance matrix is an identity matrix "
                            "multiplied by this value.")
                    elif numCols == edpLengthsList[i]:
                        covarianceTypeList.append('diagonal')
                        logFile.write(
                            "\n\t\tA row vector provided. This will be treated as the diagonal entries of the "
                            "covariance matrix.")
                    else:
                        logFile.write("\nERROR: The number of columns of data in the covariance matrix file {}"
                                      " must be either 1 or {}. Found {} columns".format(covarianceFile,
                                                                                         edpLengthsList[i],
                                                                                         numCols))
                        raise DataProcessingError(
                            "ERROR: The number of columns of data in the covariance matrix file {}"
                            " must be either 1 or {}. Found {} columns".format(covarianceFile,
                                                                               edpLengthsList[i],
                                                                               numCols))
                elif numRows == edpLengthsList[i]:
                    if numCols == 1:
                        covarianceTypeList.append('diagonal')
                        logFile.write(
                            "\t\tA column vector provided. This will be treated as the diagonal entries of the "
                            "covariance matrix.")
                    elif numCols == edpLengthsList[i]:
                        covarianceTypeList.append('matrix')
                        logFile.write("\n\t\tA full covariance matrix provided.")
                    else:
                        logFile.write("\nERROR: The number of columns of data in the covariance matrix file {}"
                                      " must be either 1 or {}. Found {} columns".format(covarianceFile,
                                                                                         edpLengthsList[i],
                                                                                         numCols))
                        raise DataProcessingError(
                            "ERROR: The number of columns of data in the covariance matrix file {}"
                            " must be either 1 or {}. Found {} columns".format(covarianceFile,
                                                                               edpLengthsList[i],
                                                                               numCols))
                else:
                    logFile.write("\nERROR: The number of rows of data in the covariance matrix file {}"
                                  " must be either 1 or {}. Found {} rows".format(covarianceFile,
                                                                                  edpLengthsList[i],
                                                                                  numCols))
                    raise DataProcessingError("ERROR: The number of rows of data in the covariance matrix file {}"
                                              " must be either 1 or {}. Found {} rows".format(covarianceFile,
                                                                                              edpLengthsList[i],
                                                                                              numCols))
                logFile.write("\n\t\tCovariance matrix: {}".format(covMatrix))
            else:
                logFile.write("\n\t\tDid not find a user supplied file. Using the default variance value.")
                logFile.write("\n\t\tThe covariance matrix is an identity matrix multiplied by this value.")
                scalarVariance = np.array(scaleFactors[i])
                covarianceMatrixList.append(scalarVariance)
                covarianceTypeList.append('scalar')
                logFile.write("\n\t\tCovariance matrix: {}".format(scalarVariance))

    # ======================================================================================================================
    # Starting TMCMC workflow
    logFile.write('\n\n==========================')
    logFile.write('\nSetting up the TMCMC algorithm')

    sys.path.append(resultsPath)
    logFile.write("\n\tResults path: {}".format(resultsPath))

    # set the seed
    np.random.seed(seedval)
    logFile.write("\n\tSeed: {}".format(seedval))

    # number of particles: Np
    Np = numberOfSamples
    logFile.write("\n\tNumber of particles: {}".format(Np))

    # number of max MCMC steps
    Nm_steps_max = 2
    Nm_steps_maxmax = 5
    logFile.write("\n\tNumber of MCMC steps in first stage: {}".format(Nm_steps_max))
    logFile.write("\n\tMax. number of MCMC steps in any stage: {}".format(Nm_steps_maxmax))

    logFile.flush()
    os.fsync(logFile.fileno())

    # ======================================================================================================================
    logFile.write('\n\n==========================')
    logFile.write('\nLooping over each model')
    # %% For each model:
    for modelNum, variables in enumerate(variablesList):
        logFile.write('\n\n\t==========================')
        logFile.write("\n\tStarting analysis for model {}".format(modelNum))
        logFile.write('\n\t==========================')
        # Assign probability distributions to the parameters
        logFile.write('\n\t\tAssigning probability distributions to the parameters')
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

                AllPars.append(
                    pdfs.TrunNormal(mu=VariableMean, sig=VariableSD, a=VariableLowerLimit, b=VariableUpperLimit))

            if variables["distributions"][i] == 'InvGamma':
                VariableA = float(variables['Par1'][i])
                VariableB = float(variables['Par2'][i])

                AllPars.append(pdfs.InvGamma(a=VariableA, b=VariableB))

            if variables["distributions"][i] == "Beta":
                VariableAlpha = float(variables['Par1'][i])
                VariableBeta = float(variables['Par2'][i])
                VariableLowerLimit = float(variables['Par3'][i])
                VariableUpperLimit = float(variables['Par4'][i])

                AllPars.append(pdfs.BetaDist(alpha=VariableAlpha, beta=VariableBeta, lowerbound=VariableLowerLimit,
                                             upperbound=VariableUpperLimit))

            if variables["distributions"][i] == "Lognormal":
                VariableMu = float(variables['Par1'][i])
                VariableSigma = float(variables['Par2'][i])

                AllPars.append(pdfs.LogNormDist(mu=VariableMu, sigma=VariableSigma))

            if variables["distributions"][i] == "Gumbel":
                VariableAlphaParam = float(variables['Par1'][i])
                VariableBetaParam = float(variables['Par2'][i])

                AllPars.append(pdfs.GumbelDist(alpha=VariableAlphaParam, beta=VariableBetaParam))

            if variables["distributions"][i] == "Weibull":
                VariableShapeParam = float(variables['Par1'][i])
                VariableScaleParam = float(variables['Par2'][i])

                AllPars.append(pdfs.WeibullDist(shape=VariableShapeParam, scale=VariableScaleParam))

        # Run the Algorithm
        logFile.write('\n\n\t==========================')
        logFile.write('\n\tRunning the TMCMC algorithm')
        logFile.write('\n\t==========================')

        logFile.flush()
        os.fsync(logFile.fileno())

        # if __name__ == '__main__':
        mytrace = RunTMCMC(Np, AllPars, Nm_steps_max, Nm_steps_maxmax, logLikeModule.log_likelihood, variables,
                           resultsLocation, seedval, calibrationData, numExperiments, covarianceMatrixList,
                           edpNamesList, edpLengthsList, normalizingFactors, locShiftList, run_type, logFile)
        logFile.write('\n\n\t==========================')
        logFile.write('\n\tTMCMC algorithm finished running')
        logFile.write('\n\t==========================')

        logFile.write('\n\n\t==========================')
        logFile.write("\n\tStarting post-processing")

        # Compute model evidence
        logFile.write('\n\n\t\tComputing the model evidence')
        evidence = 1
        for i in range(len(mytrace)):
            Wm = mytrace[i][2]
            evidence = evidence * (sum(Wm) / len(Wm))
        logFile.write("\n\t\t\tModel evidence: {:e}".format(evidence))

        # # Write Data to '.csv' files
        # logFile.write("\n\n\t\tWriting samples from each stage to csv files")
        # for i in range(len(mytrace)):
        #     dataToWrite = mytrace[i][0]
        #
        #     stringToAppend = 'resultsStage{}.csv'.format(i)
        #     resultsFilePath = os.path.join(os.path.abspath(resultsLocation), stringToAppend)
        #
        #     with open(resultsFilePath, 'w', newline='') as csvfile:
        #         csvWriter = csv.writer(csvfile)
        #         csvWriter.writerows(dataToWrite)
        #     logFile.write("\n\t\t\tWrote to file {}".format(resultsFilePath))

        # Write the results of the last stage to a file named dakotaTab.out for quoFEM to be able to read the results
        logFile.write("\n\n\t\tWriting posterior samples to 'dakotaTab.out' for quoFEM to read the results")
        tabFilePath = os.path.join(resultsLocation, "dakotaTab.out")

        # Create the headings, which will be the first line of the file
        logFile.write("\n\t\t\tCreating headings")
        headings = 'eval_id\tinterface\t'
        for v in variables['names']:
            headings += '{}\t'.format(v)
        if writeOutputs:  # create headings for outputs
            for i, edp in enumerate(edpNamesList):
                if edpLengthsList[i] == 1:
                    headings += '{}\t'.format(edp)
                else:
                    for comp in range(edpLengthsList[i]):
                        headings += '{}_{}\t'.format(edp, comp + 1)
        headings += '\n'

        # Get the data from the last stage
        logFile.write("\n\t\t\tGetting data from last stage")
        dataToWrite = mytrace[-1][0]

        logFile.write("\n\t\t\tWriting to file {}".format(tabFilePath))
        with open(tabFilePath, "w") as f:
            f.write(headings)
            for i in range(Np):
                string = "{}\t{}\t".format(i + 1, 1)
                for j in range(len(variables['names'])):
                    string += "{}\t".format(dataToWrite[i, j])
                if writeOutputs:  # write the output data
                    analysisNumString = ("analysis" + str(i))
                    prediction = np.atleast_2d(np.genfromtxt(os.path.join(workdir_main, analysisNumString,
                                                                          'results.out'))).reshape((1, -1))
                    for predNum in range(np.shape(prediction)[1]):
                        string += "{}\t".format(prediction[0, predNum])
                string += "\n"
                f.write(string)

        logFile.write('\n\n\t==========================')
        logFile.write("\n\tPost processing finished")
        logFile.write('\n\t==========================')

        # Delete Analysis Folders

        # for analysisNumber in range(0, Np):
        #     stringToAppend = ("analysis" + str(analysisNumber))
        #     analysisLocation = os.path.join(resultsLocation, stringToAppend)
        #     # analysisPath = Path(analysisLocation)
        #     analysisPath = os.path.abspath(analysisLocation)
        #     shutil.rmtree(analysisPath)

        logFile.write('\n\n\t==========================')
        logFile.write("\n\tCompleted analysis for model {}".format(modelNum))
        logFile.write('\n\t==========================')

    logFile.write('\n\n==========================')
    logFile.write('\nFinished looping over each model')
    logFile.write('\n==========================\n')

    # ======================================================================================================================
    logFile.write("\nUCSD_UQ engine workflow complete!\n")
    logFile.close()

    # ======================================================================================================================
