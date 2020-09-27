# written: Michael Gardner @ UNR

from UQpy.SampleMethods import LHS
from UQpy.RunModel import RunModel
import numpy as np
import matplotlib.pyplot as plt
import time
import csv
import json
import importlib.util
from pathlib import Path  

def runUQpy(uqData, simulationData, randomVarsData, demandParams, workingDir, runType):
    """
    This function configures and runs a UQ simulation using UQpy based on the 
    input UQ configuration, simulation configuration, random variables,
    and requested demand parameters
    
    Input:
    uqData:         JsonObject that UQ options as input into the quoFEM GUI
    simulationData: JsonObject that contains information on the analysis package to run and its
                    configuration as input in the quoFEM GUI
    randomVarsData: JsonObject that specifies the input random variables, their distributions,
                    and associated parameters as input in the quoFEM GUI
    demandParams:   JsonObject that specifies the demand parameters as input in the quoFEM GUI
    workingDir:     Directory in which to run simulations and store temporary results
    runType:        Specifies whether computations are being run locally or on an HPC cluster
    """
    
    # There is still plenty of configuration that can and should be added here. This currently does LHS sampling with Uniform
    # distributions only, though this is easily expanded

    # Parse configuration for UQ    
    distributionNames = []
    distributionParams = []
    variableNames = []
    samples = []
    samplingMethod = ""
    numberOfSamples = 0
    modelScript = ""
    inputTemplate = ""
    outputObjectName = ""
    outputScript = ""
    numberOfTasks = 1
    numberOfNodes = 1
    coresPerTask = 1
    clusterRun = False
    resumeRun = False

    # If computations are being executed on HPC, enable UQpy to start computations using srun
    if runType == "runningRemote":
        clusterRun = True

    for val in randomVarsData:
        if val["distribution"] == "Uniform":
            distributionNames.append('Uniform')
            variableNames.append(val["name"])
            distributionParams.append([val["lowerbound"], (val["upperbound"] - val["lowerbound"])])
        else:
            raise IOError("ERROR: You'll need to update runUQpy.py to run your specified RV distribution!")

    for val in uqData["Parameters"]:
        if val["name"] == "Sampling Method":
            samplingMethod = val["value"]
            
        if val["name"] == "Number of Samples":
            numberOfSamples = val["value"]

        if val["name"] == "Model Script":
            modelScript = val["value"]

        if val["name"] == "Input Template":
            inputTemplate = val["value"]

        if val["name"] == "Output Object":
            outputObjectName = val["value"]

        if val["name"] == "Output Script":
            outputScript = val["value"]
            spec = importlib.util.spec_from_file_location(Path(outputScript).stem, outputScript)

        if val["name"] == "Number of Concurrent Tasks":
            numberOfTasks = val["value"]

        if val["name"] == "Number of Nodes":
            numberOfNodes = val["value"]

        if val["name"] == "Cores per Task":
            coresPerTask = val["value"]

    # Generate samples
    if samplingMethod == "LHS":
        samples = LHS(dist_name=distributionNames, dist_params=distributionParams, lhs_criterion='random',\
                      lhs_iter=None, nsamples=numberOfSamples, var_names=variableNames)
    else:
        raise IOError("ERROR: You'll need to update runUQpy.py to run your specified sampling method!")

    # Run model based on input config
    startTime = time.time()
    model = RunModel(samples=samples.samples, model_script=modelScript,
                     input_template=inputTemplate, var_names=samples.var_names,
                     output_script=outputScript, output_object_name=outputObjectName,
                     model_dir=workingDir, verbose=True, ntasks=numberOfTasks,
                     nodes=numberOfNodes, cores_per_task=coresPerTask,
                     cluster=clusterRun, resume=resumeRun)
    
    runTime = time.time() - startTime
    print("\nTotal time for all experiments: ", runTime)
