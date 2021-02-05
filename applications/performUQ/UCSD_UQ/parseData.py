"""
authors: Mukesh Kumar Ramancha, Maitreya Manoj Kurumbhati, and Prof. J.P. Conte 
affiliation: University of California, San Diego

"""

# import os
from pathlib import Path
import re
import json


def parseDataFunction(dakotaJsonLocation):
    # %% read form dakota.json location
    with open(dakotaJsonLocation, 'r') as file_object:
        string_to_parse = file_object.read()

    # Read in the json object
    with open(dakotaJsonLocation, 'r') as f:
        jsonInputs = json.load(f)

    # Read in the data of the objects within the json file
    applications = jsonInputs['Applications']
    edp = jsonInputs['EDP']
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

    seedval = uqInputs['seed']
    numberOfSamples = uqInputs['numParticles']
    logLikelihoodPath = uqInputs['logLikelihoodPath']
    logLikelihoodFile = uqInputs['logLikelihoodFile']

    workingDirPath = Path(workingDirInputs)
    resultsLocation = Path.joinpath(workingDirPath, "tmp.SimCenter")
    resultsPath = resultsLocation

    localAppDirPath = Path(localAppDirInputs)
    PythonDirPath = Path(pythonInputs)

    print('\n\nThe seed is: {:d}\n\n'.format(seedval))
    print('\n\nThe number of samples is: {:d}\n\n'.format(numberOfSamples), type(numberOfSamples))

    inputFileList = []
    nModels = femInputs['numInputs']
    if nModels > 1:
        fileInfo = femInputs['fileInfo']
        for m in range(nModels):
            inputFileList.append(fileInfo[m]['inputFile'])
    else:
        inputFileList.append(femInputs['inputFile'])

    print('\n\ninputFileList: ', inputFileList)

    print('\n\nThe number of models is: {:d}\n\n'.format(nModels))

    # %% Number of Samples

    # numberOfSamples_pattern = re.compile(r'"numParticles": (?P<numParticles>\d+)')
    # numberOfSamples_pattern_match = next(numberOfSamples_pattern.finditer(string_to_parse))
    # numberOfSamples = int(numberOfSamples_pattern_match.group('numParticles'))

    # %% Seed

    # seed_pattern = re.compile(r'"seed": (?P<seed>\d+)')
    # seed_pattern_match = next(seed_pattern.finditer(string_to_parse))
    # seedval = int(seed_pattern_match.group('seed'))

    # Loglikelihood path and filename

    # logLikelihoodPathPattern = re.compile(r'"logLikelihoodPath": "(?P<logLikelihoodPath>.+)"')
    # logLikelihoodPathPatternMatch = next(logLikelihoodPathPattern.finditer(string_to_parse))
    # logLikelihoodPath = logLikelihoodPathPatternMatch.group('logLikelihoodPath')

    # logLikelihoodFilePattern = re.compile(r'"logLikelihoodFile": "(?P<logLikelihoodFile>.+)"')
    # logLikelihoodFilePatternMatch = next(logLikelihoodFilePattern.finditer(string_to_parse))
    # logLikelihoodFile = logLikelihoodFilePatternMatch.group('logLikelihoodFile')

    # %% FEM Input file

    # femInputFile_pattern = re.compile(r'"inputFile": "(?P<femInputFile>.+)"')
    # femInputFile_pattern_match = next(femInputFile_pattern.finditer(string_to_parse))
    # femInputFileLocation = (femInputFile_pattern_match.group('femInputFile'))
    # femInputFilePath = Path(femInputFile_pattern_match.group('femInputFile'))

    # %% Local Results Directory

    # temp = femInputFileLocation.split("/")
    # resultsLocation = "/".join(temp[:-1])
    # resultsPath = Path(resultsLocation)

    # %% Post Processing Directory

    # postProcessFile_pattern = re.compile(r'"post processor": "(?P<postProcessor>.+)"')
    # postProcessFile_pattern_match = next(postProcessFile_pattern.finditer(string_to_parse))
    # postProcessFileLocation = (postProcessFile_pattern_match.group('postProcessor'))
    # postProcessFilePath = Path(postProcessFile_pattern_match.group('postProcessor'))

    # %% Local App Directory

    # workingDir_pattern = re.compile(r'"workingDir": "(?P<workingDir>.+)"')
    # workingDir_pattern_match = next(workingDir_pattern.finditer(string_to_parse))
    # workingDirLocation = (workingDir_pattern_match.group('workingDir'))
    #
    # workingDirPath = Path(workingDir_pattern_match.group('workingDir'))

    # resultsLocation = Path.joinpath(workingDirPath, "tmp.SimCenter")
    # resultsPath = resultsLocation

    # localAppDir_pattern = re.compile(r'"localAppDir": "(?P<localAppDir>.+)"')
    # localAppDir_pattern_match = next(localAppDir_pattern.finditer(string_to_parse))
    # localAppDirLocation = (localAppDir_pattern_match.group('localAppDir'))
    # localAppDirPath = Path(localAppDir_pattern_match.group('localAppDir'))

    # %% Python Directory

    # PythonDir_pattern = re.compile(r'"python": "(?P<localAppDir>.+)"')
    # PythonDir_pattern_match = next(PythonDir_pattern.finditer(string_to_parse))
    # PythonDirLocation = (PythonDir_pattern_match.group('localAppDir'))
    # PythonDirPath = Path(PythonDir_pattern_match.group('localAppDir'))

    # %% Variables
    variablesList = []
    variables = {'names': [], 'distributions': [], 'Par1': [], 'Par2': [], 'Par3': [], 'Par4': []}
    for m in range(nModels):
        variablesList.append(variables)

    # if nModels > 1:
    ind = -1
    for rv in rvInputs:
        if nModels == 1:
            ind = 0
        else:
            ind += 1
            # ind = rv['model']
        variablesList[ind]['names'].append(rv['name'])
        variablesList[ind]['distributions'].append(rv['distribution'])
        if rv['distribution'] == 'Uniform':
            variablesList[ind]['Par1'].append(rv['lowerbound'])
            variablesList[ind]['Par2'].append(rv['upperbound'])
            variablesList[ind]['Par3'].append(None)
            variablesList[ind]['Par4'].append(None)
        elif rv['distribution'] == 'Normal':
            variablesList[ind]['Par1'].append(rv['mean'])
            variablesList[ind]['Par2'].append(rv['stdDev'])
            variablesList[ind]['Par3'].append(None)
            variablesList[ind]['Par4'].append(None)
        elif rv['distribution'] == 'Half-Normal':
            variablesList[ind]['Par1'].append(rv['Standard Deviation'])
            variablesList[ind]['Par2'].append(rv['Upper Bound'])
            variablesList[ind]['Par3'].append(None)
            variablesList[ind]['Par4'].append(None)
        elif rv['distribution'] == 'Truncated-Normal':
            variablesList[ind]['Par1'].append(rv['Mean'])
            variablesList[ind]['Par2'].append(rv['Standard Deviation'])
            variablesList[ind]['Par3'].append(rv['a'])
            variablesList[ind]['Par4'].append(rv['b'])

    # variablesSection_pattern = re.compile(r'"randomVariables": \[(.*?)\]', re.DOTALL)
    # variablesSection_match = next(variablesSection_pattern.finditer(string_to_parse))
    # variablesSection_string = variablesSection_match.group(0)
    #
    # variablesInfo_pattern = re.compile(r'\{(?P<variablesInfo>.*?)\}', re.DOTALL)
    # variablesInfo_match = variablesInfo_pattern.finditer(variablesSection_string)
    #
    # variablesInfo = []
    # for match in variablesInfo_match:
    #     variablesInfo.append(match.group('variablesInfo'))
    #
    # variableName_pattern = re.compile(r'"name": "(?P<variableName>.*)"')
    # variableDistType_pattern = re.compile(r'"distribution": "(?P<distType>.*)"')
    #
    # for variable in range(len(variablesInfo)):
    #
    #     variableName_match = next(variableName_pattern.finditer(variablesInfo[variable]))
    #     variableDistType_match = next(variableDistType_pattern.finditer(variablesInfo[variable]))
    #
    #     (variables['names']).append(variableName_match.group('variableName'))
    #     (variables['distributions']).append(variableDistType_match.group('distType'))
    #
    #     if variableDistType_match.group('distType') == 'Uniform':
    #
    #         # variableDistPara1_pattern = re.compile(r'"Lower Bound": (?P<distPara1>\d*\.?\d*)')
    #         # variableDistPara2_pattern = re.compile(r'"Upper Bound": (?P<distPara2>\d*\.?\d*)')
    #         variableDistPara1_pattern = re.compile(r'"lowerbound": (?P<distPara1>-?\d*\.?\d*)')
    #         variableDistPara2_pattern = re.compile(r'"upperbound": (?P<distPara2>-?\d*\.?\d*)')
    #
    #         variablePara1_match = next(variableDistPara1_pattern.finditer(variablesInfo[variable]))
    #         variablePara2_match = next(variableDistPara2_pattern.finditer(variablesInfo[variable]))
    #
    #         (variables['Par1']).append(variablePara1_match.group('distPara1'))
    #         (variables['Par2']).append(variablePara2_match.group('distPara2'))
    #         (variables['Par3']).append(None)
    #         (variables['Par4']).append(None)
    #
    #     elif variableDistType_match.group('distType') == 'Normal':
    #
    #         variableDistPara1_pattern = re.compile(r'"mean": (?P<distPara1>-?\d*\.?\d*)')
    #         variableDistPara2_pattern = re.compile(r'"stdDev": (?P<distPara2>\d*\.?\d*)')
    #
    #         variablePara1_match = next(variableDistPara1_pattern.finditer(variablesInfo[variable]))
    #         variablePara2_match = next(variableDistPara2_pattern.finditer(variablesInfo[variable]))
    #
    #         (variables['Par1']).append(variablePara1_match.group('distPara1'))
    #         (variables['Par2']).append(variablePara2_match.group('distPara2'))
    #         (variables['Par3']).append(None)
    #         (variables['Par4']).append(None)
    #
    #     elif variableDistType_match.group('distType') == 'Half-Normal':
    #
    #         variableDistPara1_pattern = re.compile(r'"Standard Deviation": (?P<distPara1>\d*\.?\d*)')
    #         variableDistPara2_pattern = re.compile(r'"Upper Bound": (?P<distPara2>\d*\.?\d*)')
    #
    #         variablePara1_match = next(variableDistPara1_pattern.finditer(variablesInfo[variable]))
    #         variablePara2_match = next(variableDistPara2_pattern.finditer(variablesInfo[variable]))
    #
    #         (variables['Par1']).append(variablePara1_match.group('distPara1'))
    #         (variables['Par2']).append(None)
    #         (variables['Par3']).append(None)
    #         (variables['Par4']).append(None)
    #
    #     elif variableDistType_match.group('distType') == 'Truncated-Normal':
    #
    #         variableDistPara1_pattern = re.compile(r'"Mean": (?P<distPara1>-?\d*\.?\d*)')
    #         variableDistPara2_pattern = re.compile(r'"Standard Deviation": (?P<distPara2>\d*\.?\d*)')
    #         variableDistPara3_pattern = re.compile(r'"a": (?P<distPara3>-?\d*\.?\d*)')
    #         variableDistPara4_pattern = re.compile(r'"b": (?P<distPara4>-?\d*\.?\d*)')
    #
    #         variablePara1_match = next(variableDistPara1_pattern.finditer(variablesInfo[variable]))
    #         variablePara2_match = next(variableDistPara2_pattern.finditer(variablesInfo[variable]))
    #         variablePara3_match = next(variableDistPara3_pattern.finditer(variablesInfo[variable]))
    #         variablePara4_match = next(variableDistPara4_pattern.finditer(variablesInfo[variable]))
    #
    #         (variables['Par1']).append(variablePara1_match.group('distPara1'))
    #         (variables['Par2']).append(variablePara2_match.group('distPara2'))
    #         (variables['Par3']).append(variablePara3_match.group('distPara3'))
    #         (variables['Par4']).append(variablePara4_match.group('distPara4'))

    # return variables, numberOfSamples, seedval, resultsLocation, resultsPath, logLikelihoodPath, logLikelihoodFile
    return variablesList, numberOfSamples, seedval, resultsLocation, resultsPath, logLikelihoodPath, logLikelihoodFile
