# written: Michael Gardner @ UNR

from runUQpy import runUQpy
# Currently, the only UQ driver that is implemented is UQpy. Use this as a starting point if you want
# to add other UQ capabilities

def configureAndRunUQ(uqData, simulationData, randomVarsData, demandParams, workingDir, runType):
    """
    This function configures and runs a UQ simulation based on the input
    UQ driver and its associated inputs, simulation configuration, random
    variables, and requested demand parameters
    
    Input:
    uqData:         JsonObject that specifies the UQ driver and other options as input into the quoFEM GUI
    simulationData: JsonObject that contains information on the analysis package to run and its
                    configuration as input in the quoFEM GUI
    randomVarsData: JsonObject that specifies the input random variables, their distributions,
                    and associated parameters as input in the quoFEM GUI
    demandParams:   JsonObject that specifies the demand parameters as input in the quoFEM GUI
    workingDir:     Directory in which to run simulations and store temporary results
    runType:        Specifies whether computations are being run locally or on an HPC cluster
    """
    uqDriver = ""
    
    for val in uqData["Parameters"]:
        if val["name"] == "UQ Driver":
            uqDriver = val["value"]
    
    if uqDriver == "UQpy":
        runUQpy(uqData, simulationData, randomVarsData, demandParams, workingDir, runType)
    else:
        raise ValueError("ERROR: Input UQ driver application not recognized: ", uqDriver)            