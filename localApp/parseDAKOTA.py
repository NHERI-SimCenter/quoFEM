# written: fmk, adamzs

# import functions for Python 2.X support
from __future__ import division, print_function
import sys
if sys.version.startswith('2'): 
    range=xrange
    string_types = basestring
else:
    string_types = str

import json
import os
import sys
from subprocess import Popen, PIPE

inputArgs = sys.argv

workdir_main = inputArgs[1]
workdir_temp = inputArgs[2]
run_type = inputArgs[3]

# Replace the PATH TO strings with the path to the given executable in your 
# computer. The 'darwin' part corresponds to Mac, the 'else' clause corresponds 
# to Windows. You only need the path to either Feap or OpenSees depending on 
# which one you plan to use for the analysis. 

# run on local computer
if run_type in ['runningLocal',]:
    # MAC
    if (sys.platform == 'darwin'):
        OpenSees = 'OpenSees'
        Feap = 'feappv'
        Dakota = 'dakota'
        numCPUs = 1

    # Windows
    else:
        OpenSees = 'OpenSees'
        Feap = 'Feappv41.exe'
        Dakota = 'dakota'
        numCPUs = 1

# Stampede @ DesignSafe, DON'T EDIT
elif run_type in ['runningRemote',]:
    OpenSees = '/home1/00477/tg457427/bin/OpenSees'
    Feap = '/home1/00477/tg457427/bin/feappv'
    Dakota = 'dakota'

# change workdir to the templatedir
os.chdir(workdir_temp)
cwd = os.getcwd()

# open the dakota json file
with open('dakota.json') as data_file:    
    data = json.load(data_file)

uq_data = data["uqMethod"]
fem_data = data["fem"]
rnd_data = data["randomVariables"]

# parse the Random Variables

numUncertain = 0
uncertainName = []

numNormalUncertain = 0
normalUncertainName=[]
normalUncertainMean =[]
normalUncertainStdDev =[]

numLognormalUncertain = 0
lognormalUncertainName=[]
lognormalUncertainMean =[]
lognormalUncertainStdDev =[]

numUniformUncertain = 0
uniformUncertainName=[]
uniformUncertainLower =[]
uniformUncertainUpper =[]

numContinuousDesign = 0
continuousDesignName=[]
continuousDesignLower =[]
continuousDesignUpper =[]
continuousDesignInitialPoint =[]

numConstantState = 0
constantStateName=[]
constantStateValue =[]

numWeibullUncertain = 0
weibullUncertainName=[]
weibullUncertainAlphas =[]
weibullUncertainBetas =[]

numGammaUncertain = 0
gammaUncertainName=[]
gammaUncertainAlphas =[]
gammaUncertainBetas =[]

numGumbellUncertain = 0
gumbellUncertainName=[]
gumbellUncertainAlphas =[]
gumbellUncertainBetas =[]

numBetaUncertain = 0
betaUncertainName=[]
betaUncertainLower =[]
betaUncertainHigher =[]
betaUncertainAlphas =[]

for rnd_var in rnd_data:
    if (rnd_var["distribution"] == 'Normal'):
        uncertainName.append(rnd_var["name"])
        numUncertain += 1
        normalUncertainName.append(rnd_var["name"])
        normalUncertainMean.append(rnd_var["mean"])
        normalUncertainStdDev.append(rnd_var["stdDev"])
        numNormalUncertain += 1
    elif (rnd_var["distribution"] == "Lognormal"):
        uncertainName.append(rnd_var["name"])
        numUncertain += 1
        lognormalUncertainName.append(rnd_var["name"])
        lognormalUncertainMean.append(rnd_var["mean"])
        lognormalUncertainStdDev.append(rnd_var["stdDev"])
        numLognormalUncertain += 1
    elif (rnd_var["distribution"] == "Constant"):
        uncertainName.append(rnd_var["name"])
        numUncertain += 1
        constantStateName.append(rnd_var["name"])
        constantStateValue.append(rnd_var["value"])
        numConstantState += 1
    elif (rnd_var["distribution"] == "Uniform"):
        uncertainName.append(rnd_var["name"])
        numUncertain += 1
        print("Hellooo,, Setting lower upper bounds...")
        uniformUncertainName.append(rnd_var["name"])
        uniformUncertainLower.append(rnd_var["lowerbound"])
        uniformUncertainUpper.append(rnd_var["upperbound"])
        numUniformUncertain += 1
    elif (rnd_var["distribution"] == "ContinuousDesign"):
        uncertainName.append(rnd_var["name"])
        numUncertain += 1
        continuousDesignName.append(rnd_var["name"])
        continuousDesignLower.append(rnd_var["lowerbound"])
        continuousDesignUpper.append(rnd_var["upperbound"])
        continuousDesignInitialPoint.append(rnd_var["initialpoint"])
        numContinuousDesign += 1
    elif (rnd_var["distribution"] == "Weibull"):
        uncertainName.append(rnd_var["name"])
        numUncertain += 1
        weibullUncertainName.append(rnd_var["name"])
        weibullUncertainAlphas.append(rnd_var["scaleparam"])
        weibullUncertainBetas.append(rnd_var["shapeparam"])
        numWeibullUncertain += 1
    elif (rnd_var["distribution"] == "Gamma"):
        uncertainName.append(rnd_var["name"])
        numUncertain += 1
        gammaUncertainName.append(rnd_var["name"])
        gammaUncertainAlphas.append(rnd_var["alphas"])
        gammaUncertainBetas.append(rnd_var["betas"])
        numGammaUncertain += 1
    elif (rnd_var["distribution"] == "Gumbell"):
        uncertainName.append(rnd_var["name"])
        numUncertain += 1
        gumbellUncertainName.append(rnd_var["name"])
        gumbellUncertainAlphas.append(rnd_var["alphas"])
        gumbellUncertainBetas.append(rnd_var["betas"])
        numGumbellUncertain += 1
    elif (rnd_var["distribution"] == "Beta"):
        uncertainName.append(rnd_var["name"])
        numUncertain += 1
        betaUncertainName.append(rnd_var["name"])
        betaUncertainLower.append(rnd_var["upperBounds"])
        betaUncertainUpper.append(rnd_var["lowerBounds"])
        betaUncertainAlphas.append(rnd_var["alphas"])
        betaUncertainBetas.append(rnd_var["betas"])
        numBetaUncertain += 1

# Write the dakota input file: dakota.in 

dakota_input = ""

# write out the env data

#f.write("environment\n")
#f.write("tabular_data\n")
#f.write("tabular_data_file = \'dakotaTab.out\'\n\n")

dakota_input += (
"""environment
tabular_data
tabular_data_file = 'dakotaTab.out'

method,
""")

# write out the method data

uq_method = uq_data["uqType"]

numResponses=0
responseDescriptors=[]

if uq_method == "Sampling":
    samplingData = uq_data["samplingMethodData"]
    method = samplingData["method"]
    numSamples=samplingData["samples"]
    seed = samplingData["seed"]

    dakota_input += (
"""sampling
sample_type = {sample_type}
samples = {samples}
seed = {seed}

""".format(
    sample_type = 'random' if method == 'Monte Carlo' else 'lhs',
    samples = numSamples,
    seed = seed))

    edps = samplingData["edps"]
    for edp in edps:
        responseDescriptors.append(edp["name"])
        numResponses += 1

elif uq_method == 'Calibration':
    calibrationData = uq_data["calibrationMethodData"]
    convergenceTol=calibrationData["convergenceTol"]
    maxIter = calibrationData["maxIterations"]
    method = calibrationData["method"]

    dakota_input += (
"""{method_type}
convergence_tolerance = {convTol}
max_iterations = {maxIter}

""".format(
    method_type = 'optpp_g_newton' if method == 'OPT++GaussNewton' else 'nl2sol',
    convTol = convergenceTol,
    maxIter = maxIter))

    edps = calibrationData["edps"]
    for edp in edps:
        responseDescriptors.append(edp["name"])
        numResponses += 1

elif uq_method == 'Bayesian_Calibration':
    samplingData = uq_data["bayesian_calibration_method_data"]
    chainSamples=samplingData["chain_samples"]
    seed = samplingData["seed"]

    dakota_input += (
"""bayes_calibration dream
chain_samples = {chainSamples}
seed = {seed}

""".format(
    chainSamples = chainSamples,
    seed = seed))

    edps = samplingData["edps"]
    for edp in edps:
        responseDescriptors.append(edp["name"])
        numResponses += 1

# write out the variable data // shall be replaced to make human-readable

dakota_input += ('variables,\n')

if (numNormalUncertain > 0):
    dakota_input += ('normal_uncertain = ' '{}'.format(numNormalUncertain))
    dakota_input += ('\n')
    dakota_input += ('means = ')
    for i in range(numNormalUncertain):
        dakota_input += ('{}'.format(normalUncertainMean[i]))
        dakota_input += (' ')
    dakota_input += ('\n')

    dakota_input += ('std_deviations = ')
    for i in range(numNormalUncertain):
        dakota_input += ('{}'.format(normalUncertainStdDev[i]))
        dakota_input += (' ')
    dakota_input += ('\n')
    
    dakota_input += ('descriptors = ')    
    for i in range(numNormalUncertain):
        dakota_input += ('\'')
        dakota_input += (normalUncertainName[i])
        dakota_input += ('\' ')
    dakota_input += ('\n')

if (numLognormalUncertain > 0):
    dakota_input += ('lognormal_uncertain = ' '{}'.format(numLognormalUncertain))
    dakota_input += ('\n')
    dakota_input += ('means = ')
    for i in range(numLognormalUncertain):
        dakota_input += ('{}'.format(lognormalUncertainMean[i]))
        dakota_input += (' ')
    dakota_input += ('\n')

    dakota_input += ('std_deviations = ')
    for i in range(numLognormalUncertain):
        dakota_input += ('{}'.format(lognormalUncertainStdDev[i]))
        dakota_input += (' ')
    dakota_input += ('\n')
    
    dakota_input += ('descriptors = ')    
    for i in range(numLognormalUncertain):
        dakota_input += ('\'')
        dakota_input += (lognormalUncertainName[i])
        dakota_input += ('\' ')
    dakota_input += ('\n')

if (numUniformUncertain > 0):
    dakota_input += ('uniform_uncertain = ' '{}'.format(numUniformUncertain))
    dakota_input += ('\n')
    dakota_input += ('lower_bounds = ')
    for i in range(numUniformUncertain):
        dakota_input += ('{}'.format(uniformUncertainLower[i]))
        dakota_input += (' ')
    dakota_input += ('\n')

    dakota_input += ('upper_bounds = ')
    for i in range(numUniformUncertain):
        dakota_input += ('{}'.format(uniformUncertainUpper[i]))
        dakota_input += (' ')
    dakota_input += ('\n')
    
    dakota_input += ('descriptors = ')    
    for i in range(numUniformUncertain):
        dakota_input += ('\'')
        dakota_input += (uniformUncertainName[i])
        dakota_input += ('\' ')
    dakota_input += ('\n')


if (numContinuousDesign > 0):
    dakota_input += ('continuous_design = ' '{}'.format(numContinuousDesign))
    dakota_input += ('\n')

    dakota_input += ('initial_point = ')
    for i in range(numContinuousDesign):
        dakota_input += ('{}'.format(continuousDesignInitialPoint[i]))
        dakota_input += (' ')
    dakota_input += ('\n')

    dakota_input += ('lower_bounds = ')
    for i in range(numContinuousDesign):
        dakota_input += ('{}'.format(continuousDesignLower[i]))
        dakota_input += (' ')
    dakota_input += ('\n')

    dakota_input += ('upper_bounds = ')
    for i in range(numContinuousDesign):
        dakota_input += ('{}'.format(continuousDesignUpper[i]))
        dakota_input += (' ')
    dakota_input += ('\n')
    
    dakota_input += ('descriptors = ')    
    for i in range(numContinuousDesign):
        dakota_input += ('\'')
        dakota_input += (continuousDesignName[i])
        dakota_input += ('\' ')
    dakota_input += ('\n')


numCState = 0
if (numCState > 0):
    dakota_input += ('discrete_state_range = ' '{}'.format(numConstantState))
    dakota_input += ('\n')

    dakota_input += ('initial_state = ')
    for i in range(numConstantState):
        dakota_input += ('{}'.format(constantStateValue[i]))
        dakota_input += (' ')
    dakota_input += ('\n')

    dakota_input += ('descriptors = ')    
    for i in range(numConstantState):
        dakota_input += ('\'')
        dakota_input += (constantStateName[i])
        dakota_input += ('\' ')
    dakota_input += ('\n')

if (numConstantState > 0):
    dakota_input += ('discrete_design_set\nreal = ' '{}'.format(numConstantState))
    dakota_input += ('\n')

    dakota_input += ('num_set_values = ')
    for i in range(numConstantState):
        dakota_input += ('{}'.format(1))
        dakota_input += (' ')
    dakota_input += ('\n')

    dakota_input += ('set_values = ')
    for i in range(numConstantState):
        dakota_input += ('{}'.format(constantStateValue[i]))
        dakota_input += (' ')
    dakota_input += ('\n')

    dakota_input += ('descriptors = ')    
    for i in range(numConstantState):
        dakota_input += ('\'')
        dakota_input += (constantStateName[i])
        dakota_input += ('\' ')
    dakota_input += ('\n')

if (numBetaUncertain > 0):
    dakota_input += ('beta_uncertain = ' '{}'.format(numBetaUncertain))
    dakota_input += ('\n')
    dakota_input += ('alphas = ')
    for i in range(numBetaUncertain):
        dakota_input += ('{}'.format(betaUncertainAlphas[i]))
        dakota_input += (' ')
    dakota_input += ('\n')

    dakota_input += ('betas = ')
    for i in range(numBetaUncertain):
        dakota_input += ('{}'.format(betaUncertainBetas[i]))
        dakota_input += (' ')
    dakota_input += ('\n')

    dakota_input += ('lower_bounds = ')
    for i in range(numBetaUncertain):
        dakota_input += ('{}'.format(betaUncertainLower[i]))
        dakota_input += (' ')
    dakota_input += ('\n')

    dakota_input += ('upper_bounds = ')
    for i in range(numBetaUncertain):
        dakota_input += ('{}'.format(betaUncertainHigher[i]))
        dakota_input += (' ')
    dakota_input += ('\n')
    
    dakota_input += ('descriptors = ')    
    for i in range(numBetaUncertain):
        dakota_input += ('\'')
        dakota_input += (betaUncertainName[i])
        dakota_input += ('\' ')
    dakota_input += ('\n')

if (numGammaUncertain > 0):
    dakota_input += ('gamma_uncertain = ' '{}'.format(numGammaUncertain))
    dakota_input += ('\n')
    dakota_input += ('alphas = ')
    for i in range(numGammaUncertain):
        dakota_input += ('{}'.format(gammaUncertainAlphas[i]))
        dakota_input += (' ')
    dakota_input += ('\n')

    dakota_input += ('betas = ')
    for i in range(numGammaUncertain):
        dakota_input += ('{}'.format(gammaUncertainBetas[i]))
        dakota_input += (' ')
    dakota_input += ('\n')
    
    dakota_input += ('descriptors = ')    
    for i in range(numGammaUncertain):
        dakota_input += ('\'')
        dakota_input += (gammaUncertainName[i])
        dakota_input += ('\' ')
    dakota_input += ('\n')

if (numGumbellUncertain > 0):
    dakota_input += ('gamma_uncertain = ' '{}'.format(numGumbellUncertain))
    dakota_input += ('\n')
    dakota_input += ('alphas = ')
    for i in range(numGumbellUncertain):
        dakota_input += ('{}'.format(gumbellUncertainAlphas[i]))
        dakota_input += (' ')
    dakota_input += ('\n')

    dakota_input += ('betas = ')
    for i in range(numGumbellUncertain):
        dakota_input += ('{}'.format(gumbellUncertainBetas[i]))
        dakota_input += (' ')
    dakota_input += ('\n')
    
    dakota_input += ('descriptors = ')    
    for i in range(numGumbellUncertain):
        dakota_input += ('\'')
        dakota_input += (gumbellUncertainName[i])
        dakota_input += ('\' ')
    dakota_input += ('\n')

if (numWeibullUncertain > 0):
    dakota_input += ('weibull_uncertain = ' '{}'.format(numWeibullUncertain))
    dakota_input += ('\n')
    dakota_input += ('alphas = ')
    for i in range(numWeibullUncertain):
        dakota_input += ('{}'.format(weibullUncertainAlphas[i]))
        dakota_input += (' ')
    dakota_input += ('\n')

    dakota_input += ('betas = ')
    for i in range(numWeibullUncertain):
        dakota_input += ('{}'.format(weibullUncertainBetas[i]))
        dakota_input += (' ')
    dakota_input += ('\n')
    
    dakota_input += ('descriptors = ')    
    for i in range(numWeibullUncertain):
        dakota_input += ('\'')
        dakota_input += (weibullUncertainName[i])
        dakota_input += ('\' ')
    dakota_input += ('\n')

dakota_input += ('\n')

# write out the interface data

femProgram = fem_data["program"]

if femProgram in ['OpenSees', 'OpenSees-2', 'FEAPpv']:
    dakota_input += ('interface,\n')
    #dakota_input += ('system # asynch evaluation_concurrency = 8')
    #dakota_input += ('fork asynchronous evaluation_concurrency = ' '{}'.format(numCPUs))
    if run_type in ['runningLocal',]:
        dakota_input += ("fork asynchronous evaluation_concurrency = %d" % numCPUs)
    else:
        dakota_input += ('fork asynchronous')
    dakota_input += ('\nanalysis_driver = \'python analysis_driver.py\' \n')
    dakota_input += ('parameters_file = \'params.in\' \n')
    dakota_input += ('results_file = \'results.out\' \n')
    dakota_input += ('work_directory directory_tag directory_save\n')
    dakota_input += ('copy_files = \'templatedir/*\' \n')
#    dakota_input += ('named \'workdir\' file_save  directory_save \n')
    dakota_input += ('named \'workdir\' \n')
    dakota_input += ('aprepro \n')
    dakota_input += ('\n')
    
# write out the responses

if uq_method == "Sampling":
    dakota_input += (
"""responses,
response_functions = {numResponses}
response_descriptors = {responseDescriptors}
no_gradients
no_hessians

""".format(
    numResponses = numResponses,
    responseDescriptors = '\n'.join(["'{}'".format(r) for r in responseDescriptors])))

elif uq_method == "Calibration":
    dakota_input += (
"""responses,
calibration_terms = {numResponses}
response_descriptors = {responseDescriptors}
numerical_gradients
no_hessians

""".format(
    numResponses = numResponses,
    responseDescriptors = '\n'.join(["'{}'".format(r) for r in responseDescriptors])))

elif uq_method == "Bayesian Calibration":
    dakota_input += (
"""responses,
calibration_terms = {numResponses}
response_descriptors = {responseDescriptors}
numerical_gradients
no_hessians

""".format(
    numResponses = numResponses,
    responseDescriptors = '\n'.join(["'{}'".format(r) for r in responseDescriptors])))

with open('../dakota.in', 'wb') as f:
    f.write(dakota_input.encode('utf8'))

# prepare the workflow driver file for dakota

# if OpenSees, Write the OpenSees file for dprepo
if (femProgram == "OpenSees-SingleScript"):

    model_file = fem_data["mainInput"]    

    os.chdir(workdir_main)

    fem_driver_script = ""
    fem_driver_script += (DakotaR)
    fem_driver_script += (' params.in ')
    fem_driver_script += (model_file)
    fem_driver_script += (' SimCenterInput.tcl\n')
    fem_driver_script += (OpenSees)
    fem_driver_script += (' SimCenterInput.tcl >> ops.out\n')

    with open(fem_driver, 'wb') as f: 
        f.write(fem_driver_script.encode('utf8'))

if (femProgram == "OpenSees"):       

    SCInput_ops_script = (
'''source SimCenterParamIN.ops
source {model_file}
'''.format(model_file=fem_data["mainInput"]))

    with open('SimCenterInput.ops', 'wb') as f:
        f.write(SCInput_ops_script.encode('utf8'))
        
    analysis_driver_script =(
'''
import os, sys

if 'TACC_DAKOTA_DIR' in os.environ:
    dakota_dir = os.environ['TACC_DAKOTA_DIR']
    dakota_py_dir = dakota_dir+'/share/dakota/Python'
    sys.path.append(dakota_py_dir)

import dakota.interfacing as di 
import subprocess  
from {postprocess} import process_results

# parse the Dakota parameters file
params, results = di.read_parameters_file()

# prepare the inputs for OpenSEES
params_in = ""
'''.format(postprocess = fem_data["mainPostprocessScript"][:-3]))

    for rnd_name in uncertainName:
        analysis_driver_script += (
            """params_in += "set {rnd_name} {{}}\\n".format(params['{rnd_name}'])\n""".format(rnd_name = rnd_name))

    analysis_driver_script += (
'''
with open('SimCenterParamIN.ops','wb') as f:
    f.write(params_in.encode('utf8'))

# run the simulation
OPS_command = "{OpenSees} SimCenterInput.ops >> ops.out"
result = subprocess.check_output(OPS_command, stderr=subprocess.STDOUT, 
    shell=True)

# process the results
process_results(response = [{responseDescriptors}])
'''.format(OpenSees = OpenSees,
           responseDescriptors = ', '.join(['"{}"'.format(rd) for rd in responseDescriptors])))

    with open('analysis_driver.py','wb') as f:
        f.write(analysis_driver_script.encode('utf8'))

    os.chdir(workdir_main)
    
    with open('analysis_driver.py', 'wb') as f:
        f.write(analysis_driver_script.encode('utf8'))

if (femProgram == "FEAPpv"):

    inputFile = fem_data["mainInput"]    
    postprocessScript = fem_data["mainPostprocessScript"]
    
    if (sys.version_info > (3, 0)):
        f = open('feapname', 'wb')
        f.write('SimCenterIn.txt   \n')
        f.write('SimCenterOut.txt   \n')
        f.write('SimCenterR.txt   \n')
        f.write('SimCenterR.txt   \n')
        f.write('NONE   \n')
        f.write('\n')
        f.close()
        
        os.chdir(workdir_main)
        f = open(fem_driver, 'wb')
        f.write(DakotaR)
        f.write(' params.in ')
        f.write(inputFile)
        f.write(' SimCenterIn.txt --output-format=\'\%10.5f\'\n')
        f.write('echo y|')
        f.write(Feap)
        f.write(' \n')
        f.write('python ')
        f.write("%s " % postprocessScript)
        for i in range(numResponses):
            f.write("%s " %responseDescriptors[i])                                   
        f.write('\n')
        f.close()
        
    else:     
        f = open('feapname', 'wb')
        f.write(unicode('SimCenterIn.txt   \n'))
        f.write(unicode('SimCenterOut.txt   \n'))
        f.write(unicode('SimCenterR.txt   \n'))
        f.write(unicode('SimCenterR.txt   \n'))
        f.write(unicode('NONE   \n'))
        f.write(unicode('\n'))
        f.close()
        
        os.chdir(workdir_main)
        f = open(fem_driver, 'wb')
        f.write(unicode(DakotaR))
        f.write(unicode(' params.in '))
        f.write(unicode(inputFile))
        f.write(unicode(' SimCenterIn.txt --output-format=\'\%10.5f\'\n'))
        f.write(unicode('echo y|'))
        f.write(unicode(Feap))
        f.write(unicode(' \n'))
        f.write(unicode('python '))
        f.write(unicode(postprocessScript))
        for i in range(numResponses):
            f.write(unicode(' '))
            f.write(unicode(responseDescriptors[i]))
                                       
        f.write(unicode('\n'))
        f.close()

command = Dakota + ' -input dakota.in -output dakota.out -error dakota.err'
print(command)

if run_type in ['runningLocal']:
    p = Popen(command, stdout=PIPE, stderr=PIPE, shell=True)
    for line in p.stdout:
        print(str(line))



