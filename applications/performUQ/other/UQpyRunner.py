# written: Michael Gardner @ UNR

import os
from uqRunner import UqRunnerFactory
from uqRunner import UqRunner

# THIS IS FOR WHEN MESSING AROUND WITH UQpy SOURCE
# import sys
# sys.path.append(os.path.abspath("/home/michael/UQpy/src"))

from UQpy.SampleMethods import LHS
from UQpy.RunModel import RunModel
from UQpy import Distributions
from createTemplate import createTemplate
import time
import csv
import json
import shutil

class UQpyRunner(UqRunner):
    def runUQ(self, uqData, simulationData, randomVarsData, demandParams,
              workingDir, runType, localAppDir, remoteAppDir):
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
        localAppDir:    Directory containing apps for local run
        remoteAppDir:   Directory containing apps for remote run
        """
    
        # There is still plenty of configuration that can and should be added here. This currently does LHS sampling with Uniform
        # distributions only, though this is easily expanded
        
        # Copy required python files to template directory
        shutil.copyfile(os.path.join(localAppDir, 'applications/performUQ/other/runWorkflowDriver.py'),
                                 os.path.join(workingDir, 'runWorkflowDriver.py'))
        shutil.copyfile(os.path.join(localAppDir, 'applications/performUQ/other/createTemplate.py'),
                                 os.path.join(workingDir, 'createTemplate.py'))
        shutil.copyfile(os.path.join(localAppDir, 'applications/performUQ/other/processUQpyOutput.py'),
                                 os.path.join(workingDir, 'processUQpyOutput.py'))

        # Parse configuration for UQ    
        distributionNames = []
        distributionParams = []
        variableNames = []
        distributionObjects = []
        samples = []
        samplingMethod = ""
        numberOfSamples = 0
        modelScript = 'runWorkflowDriver.py'
        inputTemplate = 'params.template'
        outputObjectName = 'OutputProcessor'
        outputScript = 'processUQpyOutput.py'
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
                distributionParams.append([val["lowerbound"], val["upperbound"]])
            else:
                raise IOError("ERROR: You'll need to update UQpyRunner.py to run your" +\
                              " specified RV distribution!")

        for val in uqData["Parameters"]:
            if val["name"] == "Sampling Method":
                samplingMethod = val["value"]
            
            if val["name"] == "Number of Samples":
                numberOfSamples = val["value"]

            if val["name"] == "Number of Concurrent Tasks":
                numberOfTasks = val["value"]

            if val["name"] == "Number of Nodes":
                numberOfNodes = val["value"]

            if val["name"] == "Cores per Task":
                coresPerTask = val["value"]


        # Create distribution objects
        for index, val in enumerate(distributionNames, 0):
            distributionObjects.append(Distributions.Uniform(distributionParams[index][0], distributionParams[index][1]))

        createTemplate(variableNames, inputTemplate)
            
        # Generate samples
        if samplingMethod == "LHS":
            samples = LHS(dist_object=distributionObjects, lhs_criterion='random',\
                          lhs_iter=None, nsamples=numberOfSamples, var_names=variableNames)        
            # samples = LHS(dist_name=distributionNames, dist_params=distributionParams, lhs_criterion='random',\
                #               lhs_iter=None, nsamples=numberOfSamples, var_names=variableNames)
        else:
            raise IOError("ERROR: You'll need to update UQpyRunner.py to run your specified" +\
                          " sampling method!")

        # Change workdir to the template directory
        os.chdir(workingDir)    
    
        # Run model based on input config
        startTime = time.time()
        model = RunModel(samples=samples.samples, model_script=modelScript,
                         input_template=inputTemplate, var_names=variableNames,
                         output_script=outputScript, output_object_name=outputObjectName,
                         verbose=True, ntasks=numberOfTasks,
                         nodes=numberOfNodes, cores_per_task=coresPerTask,
                         cluster=clusterRun, resume=resumeRun)
    
        runTime = time.time() - startTime
        print("\nTotal time for all experiments: ", runTime)

        with open(os.path.join(workingDir, '..', 'tabularResults.out'), 'w') as f:
            f.write("%eval_id\t interface\t")

            for val in variableNames:
                f.write("%s\t" % val)

            for val in demandParams:
                f.write("%s\t" % val["name"])

            f.write("\n")
            
            for index, experiment in enumerate(model.qoi_list, 0):
                if len(experiment) != 0:
                    for item in experiment:
                        f.write("%s\t custom\t" % (index + 1))
                        for sample in samples.samples[index]:
                            f.write("%s\t" % sample)

                        for result in item:
                            f.write("%s\t" % result)
                        
                            f.write("\n")
            f.close()
            
    # Factory for creating UQpy runner
    class Factory:
        def create(self):
            return UQpyRunner()
