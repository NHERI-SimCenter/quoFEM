"""
authors: Mukesh Kumar Ramancha, Maitreya Manoj Kurumbhati, and Prof. J.P. Conte 
affiliation: University of California, San Diego

"""

import os
from pathlib import Path
import re

def parseDataFunction(dakotaJsonLocation):

    # %% read form dakota.json location
    with open(dakotaJsonLocation, 'r') as file_object:
        string_to_parse = file_object.read()

    # %% Number of Samples

    numberOfSamples_pattern = re.compile(r'"numParticles": (?P<numParticles>\d+)')
    numberOfSamples_pattern_match = next(numberOfSamples_pattern.finditer(string_to_parse))
    numberOfSamples = int(numberOfSamples_pattern_match.group('numParticles'))

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

    workingDir_pattern = re.compile(r'"workingDir": "(?P<workingDir>.+)"')
    workingDir_pattern_match = next(workingDir_pattern.finditer(string_to_parse))
    workingDirLocation = (workingDir_pattern_match.group('workingDir'))

    workingDirPath = Path(workingDir_pattern_match.group('workingDir'))

    resultsLocation = Path.joinpath(workingDirPath, "tmp.SimCenter")
    resultsPath = resultsLocation

    localAppDir_pattern = re.compile(r'"localAppDir": "(?P<localAppDir>.+)"')
    localAppDir_pattern_match = next(localAppDir_pattern.finditer(string_to_parse))
    localAppDirLocation = (localAppDir_pattern_match.group('localAppDir'))
    localAppDirPath = Path(localAppDir_pattern_match.group('localAppDir'))

    # %% Python Directory

    PythonDir_pattern = re.compile(r'"python": "(?P<localAppDir>.+)"')
    PythonDir_pattern_match = next(PythonDir_pattern.finditer(string_to_parse))
    PythonDirLocation = (PythonDir_pattern_match.group('localAppDir'))
    PythonDirPath = Path(PythonDir_pattern_match.group('localAppDir'))

    # %% Variables
    variablesSection_pattern = re.compile(r'"randomVariables": \[(.*?)\]', re.DOTALL)
    variablesSection_match = next(variablesSection_pattern.finditer(string_to_parse))
    variablesSection_string = variablesSection_match.group(0)

    variablesInfo_pattern = re.compile(r'\{(?P<variablesInfo>.*?)\}', re.DOTALL)
    variablesInfo_match = variablesInfo_pattern.finditer(variablesSection_string)

    variablesInfo = []
    for match in variablesInfo_match:
        variablesInfo.append(match.group('variablesInfo'))

    variableName_pattern = re.compile(r'"name": "(?P<variableName>.*)"')
    variableDistType_pattern = re.compile(r'"distribution": "(?P<distType>.*)"')

    variables = {}
    variables['names'] = []
    variables['distributions'] = []
    variables['Par1'] = []
    variables['Par2'] = []
    variables['Par3'] = []
    variables['Par4'] = []

    for variable in range(len(variablesInfo)):

        variableName_match = next(variableName_pattern.finditer(variablesInfo[variable]))
        variableDistType_match = next(variableDistType_pattern.finditer(variablesInfo[variable]))

        (variables['names']).append(variableName_match.group('variableName'))
        (variables['distributions']).append(variableDistType_match.group('distType'))

        if variableDistType_match.group('distType') == 'Uniform':

            # variableDistPara1_pattern = re.compile(r'"Lower Bound": (?P<distPara1>\d*\.?\d*)')
            # variableDistPara2_pattern = re.compile(r'"Upper Bound": (?P<distPara2>\d*\.?\d*)')
            variableDistPara1_pattern = re.compile(r'"lowerbound": (?P<distPara1>\d*\.?\d*)')
            variableDistPara2_pattern = re.compile(r'"upperbound": (?P<distPara2>\d*\.?\d*)')

            variablePara1_match = next(variableDistPara1_pattern.finditer(variablesInfo[variable]))
            variablePara2_match = next(variableDistPara2_pattern.finditer(variablesInfo[variable]))

            (variables['Par1']).append(variablePara1_match.group('distPara1'))
            (variables['Par2']).append(variablePara2_match.group('distPara2'))
            (variables['Par3']).append(None)
            (variables['Par4']).append(None)

        elif variableDistType_match.group('distType') == 'Normal':

            variableDistPara1_pattern = re.compile(r'"mean": (?P<distPara1>\d*\.?\d*)')
            variableDistPara2_pattern = re.compile(r'"stdDev": (?P<distPara2>\d*\.?\d*)')

            variablePara1_match = next(variableDistPara1_pattern.finditer(variablesInfo[variable]))
            variablePara2_match = next(variableDistPara2_pattern.finditer(variablesInfo[variable]))

            (variables['Par1']).append(variablePara1_match.group('distPara1'))
            (variables['Par2']).append(variablePara2_match.group('distPara2'))
            (variables['Par3']).append(None)
            (variables['Par4']).append(None)

        elif variableDistType_match.group('distType') == 'Half-Normal':

            variableDistPara1_pattern = re.compile(r'"Standard Deviation": (?P<distPara1>\d*\.?\d*)')
            variableDistPara2_pattern = re.compile(r'"Upper Bound": (?P<distPara2>\d*\.?\d*)')

            variablePara1_match = next(variableDistPara1_pattern.finditer(variablesInfo[variable]))
            variablePara2_match = next(variableDistPara2_pattern.finditer(variablesInfo[variable]))

            (variables['Par1']).append(variablePara1_match.group('distPara1'))
            (variables['Par2']).append(None)
            (variables['Par3']).append(None)
            (variables['Par4']).append(None)

        elif variableDistType_match.group('distType') == 'Truncated-Normal':

            variableDistPara1_pattern = re.compile(r'"Mean": (?P<distPara1>\d*\.?\d*)')
            variableDistPara2_pattern = re.compile(r'"Standard Deviation": (?P<distPara2>\d*\.?\d*)')
            variableDistPara3_pattern = re.compile(r'"a": (?P<distPara3>\d*\.?\d*)')
            variableDistPara4_pattern = re.compile(r'"b": (?P<distPara4>\d*\.?\d*)')

            variablePara1_match = next(variableDistPara1_pattern.finditer(variablesInfo[variable]))
            variablePara2_match = next(variableDistPara2_pattern.finditer(variablesInfo[variable]))
            variablePara3_match = next(variableDistPara3_pattern.finditer(variablesInfo[variable]))
            variablePara4_match = next(variableDistPara4_pattern.finditer(variablesInfo[variable]))

            (variables['Par1']).append(variablePara1_match.group('distPara1'))
            (variables['Par2']).append(variablePara2_match.group('distPara2'))
            (variables['Par3']).append(variablePara3_match.group('distPara3'))
            (variables['Par4']).append(variablePara4_match.group('distPara4'))

    return variables, numberOfSamples, resultsLocation, resultsPath