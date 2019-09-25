# written: UQ team @ SimCenter

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
import numpy as np
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
my_edps = data["edps"]

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
betaUncertainBetas = []

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
        betaUncertainHigher.append(rnd_var["upperBounds"])
        betaUncertainLower.append(rnd_var["lowerBounds"])
        betaUncertainAlphas.append(rnd_var["alphas"])
        betaUncertainBetas.append(rnd_var["betas"])
        numBetaUncertain += 1

# Write the dakota input file: dakota.in 

# write out the method data

uq_method = uq_data["uqType"]

numResponses=0
responseDescriptors=[]

if uq_method == "Forward Propagation" or uq_method == "Sensitivity Analysis":
    
    samplingData = uq_data["samplingMethodData"]
    method = samplingData["method"]
    
    # write out the env data
    dakota_input = ""
    
    dakota_input += (
    """environment
    tabular_data
    tabular_data_file = 'dakotaTab.out'
    
    method,
    """)
    
    if method == "Importance Sampling":
        numSamples=samplingData["samples"]
        seed = samplingData["seed"]
        imp_sams_arg = samplingData["ismethod"]

        dakota_input += (
    """importance_sampling
    {ismethod}
    samples = {samples}
    seed = {seed}
    
    """.format(
        ismethod = imp_sams_arg,
        samples = numSamples,
        seed = seed))
    
        # edps = samplingData["edps"]
        for edp in my_edps:
            responseDescriptors.append(edp["name"])
            numResponses += 1

    elif method == "Monte Carlo":
        numSamples=samplingData["samples"]
        seed = samplingData["seed"]

        dakota_input += (
    """sampling
    sample_type = {sample_type}
    samples = {samples}
    seed = {seed}
    
    """.format(
        sample_type = 'random',
        samples = numSamples,
        seed = seed))
        
        if uq_method == "Sensitivity Analysis":
            dakota_input += (
            """variance_based_decomp
                
""")
    
        #edps = samplingData["edps"]
        for edp in my_edps:
            responseDescriptors.append(edp["name"])
            numResponses += 1

    elif method == "LHS":
        numSamples=samplingData["samples"]
        seed = samplingData["seed"]        

        dakota_input += (
    """sampling
    sample_type = {sample_type}
    samples = {samples}
    seed = {seed}
    
    """.format(
        sample_type = 'lhs' ,
        samples = numSamples,
        seed = seed))

        if uq_method == "Sensitivity Analysis":
            dakota_input += (
            """variance_based_decomp
                
""")
    
        #edps = samplingData["edps"]
        for edp in my_edps:
            responseDescriptors.append(edp["name"])
            numResponses += 1


    elif method == "Gaussian Process Regression":
        train_samples = samplingData["samples"]
        gpr_seed = samplingData["seed"]
        train_method = samplingData["dataMethod"]
        
        train_samples2 = samplingData["samples2"]
        gpr_seed2 = samplingData["seed2"]
        train_method2 = samplingData["dataMethod2"]
        
        # write out the env data
        dakota_input = ""
        
        dakota_input += (
        """environment
method_pointer = 'EvalSurrogate'
tabular_data
tabular_data_file = 'dakotaTab.out'
custom_annotated header eval_id
        
method
id_method = 'EvalSurrogate'
model_pointer = 'SurrogateModel'
        
sampling
samples = {no_surr_sams}
seed = {surr_seed}
sample_type {surr_sams_type}
        
model
id_model = 'SurrogateModel'
surrogate global
dace_method_pointer = 'DesignMethod'
gaussian_process surfpack
export_model
filename_prefix = 'dak_gp_model'
formats
text_archive
        
""").format(
        no_surr_sams = train_samples2,
        surr_seed = gpr_seed2,
        surr_sams_type = train_method2)

        #edps = samplingData["edps"]
        for edp in my_edps:
            responseDescriptors.append(edp["name"])
            numResponses += 1


    elif method == "Polynomial Chaos Expansion":
        quad_od = samplingData["order"]
        spg_level = samplingData["level"]
        train_method = samplingData["dataMethod"]
        myseed = samplingData["seed"]
        
        pce_method = ''
        if train_method == 'Quadrature':
            pce_method = 'quadrature_order'
            quad_method = quad_od
        elif train_method == 'Sparse Grid Quadrature':
            pce_method = 'sparse_grid_level'
            quad_method = spg_level
            
        #train_samples2 = samplingData["samples2"]
        #gpr_seed2 = samplingData["seed2"]
        #train_method2 = samplingData["dataMethod2"]
        
        # write out the env data
        dakota_input = ""
        
        dakota_input += (
        """environment
tabular_data
tabular_data_file = 'dakotaTab.out'

method        
polynomial_chaos
{set_method_name} = {integration_order}
samples_on_emulator = 1000
seed = {set_seed}
probability_levels = .1 .5 .9
variance_based_decomp
        
""").format(
        set_method_name = pce_method,
        integration_order = quad_method,
        set_seed = myseed)

        #edps = samplingData["edps"]
        for edp in my_edps:
            responseDescriptors.append(edp["name"])
            numResponses += 1


elif uq_method == "Reliability Analysis":

    sampling_data = uq_data["samplingMethodData"]
    rel_method = sampling_data["method"]       # [FORM, SORM]
    mpp_method = sampling_data["mpp_Method"]      # [no_approx, ...]
    rel_scheme = sampling_data["reliability_Scheme"]      # [local, global]
    active_level = sampling_data["activeLevel"]     # [ProbabilityLevel, ResponseLevel]

    #edps = sampling_data["edps"]
    for edp in my_edps:
        responseDescriptors.append(edp["name"])
        numResponses += 1
    
    write_order = ""    
    if rel_method == "FORM":
        write_order = "first_order"
    elif rel_method == "SORM":
        write_order = "second_order"

    write_scheme = ""
    if rel_scheme == "Local":
        write_scheme = "local_reliability"
    elif rel_scheme == "Global":
        write_scheme = "global_reliability"

    set_probability_level = "0.0 "
    set_response_level = "0.0"
    if active_level == "ProbabilityLevel":
        set_probability_level = ""
        for l in range(len(sampling_data["probabilityLevel"])):
            set_probability_level += str(sampling_data["probabilityLevel"][l]) + '  '
    elif active_level == "ResponseLevel":
        set_response_level = ""
        for m in range(len(sampling_data["responseLevel"])):
            set_response_level += str(sampling_data["responseLevel"][m]) + '  '

    # write out the env data
    dakota_input = ""    
        
    dakota_input += (
    """environment
output_file 'dakotaTab.out'

method
{set_reliability_scheme}
mpp_search {mpp_search_method}
integration {set_reliability_order}
probability_levels = {set_my_prob}
response_levels = {set_my_resp}

""").format(
    mpp_search_method = mpp_method,
    set_reliability_order = write_order,
    set_reliability_scheme = write_scheme,
    set_my_prob = set_probability_level,
    set_my_resp = set_response_level)


elif uq_method == 'Calibration':
    calibrationData = uq_data["calibrationMethodData"]
    convergenceTol=calibrationData["convergenceTol"]
    maxIter = calibrationData["maxIterations"]
    method = calibrationData["method"]

    # write out the env data
    dakota_input = ""
    
    dakota_input += (
    """environment
    tabular_data
    tabular_data_file = 'dakotaTab.out'
    
    method,
    """)

    dakota_input += (
"""{method_type}
convergence_tolerance = {convTol}
max_iterations = {maxIter}

""".format(
    method_type = 'optpp_g_newton' if method == 'OPT++GaussNewton' else 'nl2sol',
    convTol = convergenceTol,
    maxIter = maxIter))

    #edps = calibrationData["edps"]
    for edp in my_edps:
        responseDescriptors.append(edp["name"])
        numResponses += 1

elif uq_method == 'Bayesian_Calibration':
    samplingData = uq_data["bayesian_calibration_method_data"]
    chainSamples=samplingData["chain_samples"]
    seed = samplingData["seed"]

    # write out the env data
    dakota_input = ""
    
    dakota_input += (
    """environment
    tabular_data
    tabular_data_file = 'dakotaTab.out'
    
    method,
    """)

    dakota_input += (
"""bayes_calibration dream
chain_samples = {chainSamples}
seed = {seed}

""".format(
    chainSamples = chainSamples,
    seed = seed))

    #edps = samplingData["edps"]
    for edp in my_edps:
        responseDescriptors.append(edp["name"])
        numResponses += 1

# write out the variable data // shall be replaced to make human-readable

dakota_input += ('variables,\n')
dakota_input += ('active uncertain\n')

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


if (uq_method == "Reliability"): 
    # set correlation matrix of random input
    
    if "correlationMatrix" in data:
        
        data["correlationMatrix"]
        correlationMatrix=np.reshape(data["correlationMatrix"],(numUncertain,numUncertain))
        
        dakota_input += ("uncertain_correlation_matrix = ")
        rows,cols = correlationMatrix.shape
        
        for i in range(0,rows):
            if (i==0):
                row_string = ""
            else:
                row_string = "                               "
            for j in range(0,cols):
                row_string = row_string + "{0:.5f}".format(correlationMatrix[i,j]) + " "
            row_string = row_string + "\n"
            dakota_input += (row_string)
            
# if (uq_method == "Forward Propagation") -> use rank matrix instead of correlation

dakota_input += ('\n')


if uq_method == "Forward Propagation" or uq_method == "Sensitivity Analysis":
    
    samplingData = uq_data["samplingMethodData"]
    method = samplingData["method"]
    
    if method == "Gaussian Process Regression":
        
        train_samples = samplingData["samples"]
        gpr_seed = samplingData["seed"]
        train_method = samplingData["dataMethod"]

        dakota_input += (
        """method
id_method = 'DesignMethod'
model_pointer = 'SimulationModel'
sampling
seed = {setseed}
sample_type {settype}
samples = {setsamples}
        
model
id_model = 'SimulationModel'
single
interface_pointer = 'SimulationInterface'
        
""").format(
    setseed = gpr_seed,
    settype = train_method,
    setsamples = train_samples
    )
        

# write out the interface data

femProgram = fem_data["program"]

if femProgram in ['OpenSees', 'OpenSees-2', 'FEAPpv']:
    dakota_input += ('interface,\n')
    if uq_method == "Forward Propagation" or uq_method == "Sensitivity Analysis":
        samplingData = uq_data["samplingMethodData"]
        method = samplingData["method"]
        if method == "Gaussian Process Regression":
            dakota_input += ('id_interface = \'SimulationInterface\',\n')
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

if uq_method == "Forward Propagation" or uq_method == "Sensitivity Analysis":
    
    samplingData = uq_data["samplingMethodData"]
    method = samplingData["method"]
    
    if method == "Monte Carlo":       
        dakota_input += (
    """responses,
    response_functions = {numResponses}
    response_descriptors = {responseDescriptors}
    no_gradients
    no_hessians
    
    """.format(
        numResponses = numResponses,
        responseDescriptors = '\n'.join(["'{}'".format(r) for r in responseDescriptors])))
    
    elif method == "LHS":
        dakota_input += (
    """responses,
    response_functions = {numResponses}
    response_descriptors = {responseDescriptors}
    no_gradients
    no_hessians
    
    """.format(
        numResponses = numResponses,
        responseDescriptors = '\n'.join(["'{}'".format(r) for r in responseDescriptors])))

    elif method == "Importance Sampling":
        dakota_input += (
    """responses,
    response_functions = {numResponses}
    response_descriptors = {responseDescriptors}
    no_gradients
    no_hessians
    
    """.format(
        numResponses = numResponses,
        responseDescriptors = '\n'.join(["'{}'".format(r) for r in responseDescriptors])))

    elif method == "Gaussian Process Regression":
        dakota_input += (
    """responses,
    response_functions = {numResponses}
    response_descriptors = {responseDescriptors}
    no_gradients
    no_hessians
    
    """.format(
        numResponses = numResponses,
        responseDescriptors = '\n'.join(["'{}'".format(r) for r in responseDescriptors])))

    elif method == "Polynomial Chaos Expansion":
        dakota_input += (
    """responses,
    response_functions = {numResponses}
    response_descriptors = {responseDescriptors}
    no_gradients
    no_hessians
    
    """.format(
        numResponses = numResponses,
        responseDescriptors = '\n'.join(["'{}'".format(r) for r in responseDescriptors])))


elif uq_method == "Reliability Analysis":
       
    # write out the env data
    dakota_input += (
    """responses,
response_functions = {numResponses}
response_descriptors = {responseDescriptors}
numerical_gradients
method_source dakota
interval_type central
fd_step_size = 1.e-4
no_hessians
    
    """.format(
        numResponses = numResponses,
        responseDescriptors = '\n'.join(["'{}'".format(r) for r in responseDescriptors])))  
        

elif uq_method == "Parameter Estimation":
    dakota_input += (
"""responses,
calibration_terms = {numResponses}
response_descriptors = {responseDescriptors}
numerical_gradients
no_hessians

""".format(
    numResponses = numResponses,
    responseDescriptors = '\n'.join(["'{}'".format(r) for r in responseDescriptors])))

elif uq_method == "Inverse Problem":
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



