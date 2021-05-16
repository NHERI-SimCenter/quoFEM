"""
authors: Mukesh Kumar Ramancha, Maitreya Manoj Kurumbhati, Prof. J.P. Conte, and Aakash Bangalore Satish*
affiliation: University of California, San Diego, *SimCenter, University of California, Berkeley

"""

import os
import json


def parseDataFunction(dakotaJsonLocation):
    # Read in the json object
    print("\nReading the json file")
    with open(dakotaJsonLocation, 'r') as f:
        jsonInputs = json.load(f)

    # Read in the data of the objects within the json file
    print('\nParsing the inputs read in from json file')
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
    workingDirInputs = jsonInputs['workingDir']

    # Processing UQ inputs
    print("\tProcessing UQ inputs")
    seedval = uqInputs['seed']
    numberOfSamples = uqInputs['numParticles']
    logLikelihoodPath = uqInputs['logLikelihoodPath']
    logLikelihoodFile = uqInputs['logLikelihoodFile']
    calDataPath = uqInputs['calDataFilePath']
    calDataFile = uqInputs['calDataFile']

    # Processing path data
    print("\tProcessing path data")
    workingDirPath = os.path.abspath(workingDirInputs)
    resultsLocation = os.path.join(workingDirPath, "tmp.SimCenter")
    resultsPath = resultsLocation

    # Processing number of models
    print("\tGetting the number of models")
    inputFileList = []
    nModels = femInputs['numInputs']
    if nModels > 1:
        fileInfo = femInputs['fileInfo']
        for m in range(nModels):
            inputFileList.append(fileInfo[m]['inputFile'])
    else:
        inputFileList.append(femInputs['inputFile'])
    print('\tThe number of models is: {}'.format(nModels))

    # %% Variables
    variablesList = []
    variables = {'names': [], 'distributions': [], 'Par1': [], 'Par2': [], 'Par3': [], 'Par4': []}
    for m in range(nModels):
        variablesList.append(variables)

    print('\tLooping over the models')
    for ind in range(nModels):
        print('\tModel number: {}'.format(ind))
        # Processing RV inputs
        print('\t\tProcessing priors from RV inputs')
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
            print("\t\t\tRV number: {}, name: {}, dist: {}, {}".format(i, rv['name'], rv['distribution'], paramString))

        # Adding one prior distribution per EDP for the error covariance multiplier term
        print("\t\tAdding one prior distribution per EDP for the error covariance multiplier term")
        # print("\t\tThe prior on the error covariance multipliers is an inverse gamma distribution \n"
        #       "\t\twith parameters a and b set to 100. This corresponds to a variable whose mean \n"
        #       "\t\tand mode are approximately 1.0 and whose standard deviation is approximately 0.1.")
        # a = 100
        # b = 100
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
            print("\t\t\tEDP number: {}, name: {}, dist: {}, {}".format(i, name, 'InvGamma', paramString))

    print('Completed parsing the inputs')
    return (variablesList, numberOfSamples, seedval, resultsLocation, resultsPath, logLikelihoodPath, logLikelihoodFile,
            calDataPath, calDataFile, edpInputs)
