"""
@author: Mukesh, Maitreya, and Conte 

"""

import os
import sys
import shutil
import pickle
from pathlib import Path

from parseData import variables
from parseData import numberOfSamples 
from parseData import resultsLocation
from parseData import resultsPath

print('numVariables {}',format(variables))
print('numSamples {}',format(numberOfSamples))

import pdfs
from runTMCMC import RunTMCMC
from postProcessing import log_likelihood


#%%

sys.path.append(resultsPath)

# number of particles: Np
Np = numberOfSamples

# number of max MCMC steps
Nm_steps_max = 2
Nm_steps_maxmax = 5

#%%

''' Assign probability distributions to the parameters  '''

AllPars = []

for i in range(len(variables["names"])):
    
    if variables["distributions"][i] == 'Uniform':
        
        VariableLowerLimit = float(variables['Par1'][i])
        VariableUpperLimit = float(variables['Par2'][i])
        
        AllPars.append(pdfs.Uniform(lower = VariableLowerLimit , upper = VariableUpperLimit))

    if variables["distributions"][i] == 'Normal':
        
        VariableMean = float(variables['Par1'][i])
        VariableSD = float(variables['Par2'][i])

        AllPars.append(pdfs.Normal(mu = VariableMean , sig = VariableSD ))

    if variables["distributions"][i] == 'Half-Normal':
        
        VariableSD = float(variables['Par1'][i])

        AllPars.append(pdfs.Halfnormal(sig = VariableSD))
      
    if variables["distributions"][i] == 'Truncated-Normal':
        
        VariableMean = float(variables['Par1'][i])
        VariableSD = float(variables['Par2'][i])
        VariableLowerLimit = float(variables['Par3'][i])
        VariableUpperLimit = float(variables['Par4'][i])
    
        AllPars.append(pdfs.TrunNormal( mu = VariableMean , sig = VariableSD , a = VariableLowerLimit , b = VariableUpperLimit))

#%%

''' Run the Algorithm '''

if __name__ == '__main__':
    mytrace = RunTMCMC(Np,AllPars,Nm_steps_max,Nm_steps_maxmax,log_likelihood)

    with open('mytrace.pickle', 'wb') as handle1:
        pickle.dump(mytrace, handle1, protocol=pickle.HIGHEST_PROTOCOL)
    
    for analysisNumber in range(0, Np):
        stringtoappend = ("analysis" + str(analysisNumber))
        analysisLocation = os.path.join(resultsLocation, stringtoappend)
        analysisPath = Path(analysisLocation)       
        shutil.rmtree(analysisPath)

    exec(open("createResult.py").read())
