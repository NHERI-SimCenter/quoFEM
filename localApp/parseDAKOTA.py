import io
import json
import os
import sys
import stat
import shutil
from pprint import pprint

inputArgs = sys.argv
#print inputArgs

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
        OpenSees = 'PATH TO OpenSees'
        Feap = 'PATH TO feappv'
        Dakota = 'PATH TO dakota'
        DakotaR = 'PATH TO dprepro'
        fem_driver = 'fem_driver'
        numCPUs = 4

    # Windows
    else:
        OpenSees = 'OpenSees'
        Feap = 'C:\\Users\\SimCenter\\feap\\Feappv41.exe'
        Dakota = 'dakota'
        DakotaR = 'perl "C:/Adam/Dakota test/bin/dprepro"'
        fem_driver = 'fem_driver.bat'
        numCPUs = 4

# Stampede @ DesignSafe, DON'T EDIT
elif run_type in ['runningRemote',]:
    OpenSees = '/home1/00477/tg457427/bin/OpenSees'
    Feap = '/home1/00477/tg457427/bin/feappv'
    Dakota = 'dakota'
    DakotaR = 'dprepro'
    fem_driver = 'fem_driver'

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

f = open('../dakota.in', 'w')

# write out the env data

#f.write("environment\n")
#f.write("tabular_data\n")
#f.write("tabular_data_file = \'dakotaTab.out\'\n\n")

f.write(
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

    f.write(
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

    f.write(
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

    f.write(
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

f.write('variables,\n')

if (numNormalUncertain > 0):
    f.write('normal_uncertain = ' '{}'.format(numNormalUncertain))
    f.write('\n')
    f.write('means = ')
    for i in range(numNormalUncertain):
        f.write('{}'.format(normalUncertainMean[i]))
        f.write(' ')
    f.write('\n')

    f.write('std_deviations = ')
    for i in range(numNormalUncertain):
        f.write('{}'.format(normalUncertainStdDev[i]))
        f.write(' ')
    f.write('\n')
    
    f.write('descriptors = ')    
    for i in range(numNormalUncertain):
        f.write('\'')
        f.write(normalUncertainName[i])
        f.write('\' ')
    f.write('\n')

if (numLognormalUncertain > 0):
    f.write('lognormal_uncertain = ' '{}'.format(numLognormalUncertain))
    f.write('\n')
    f.write('means = ')
    for i in range(numLognormalUncertain):
        f.write('{}'.format(lognormalUncertainMean[i]))
        f.write(' ')
    f.write('\n')

    f.write('std_deviations = ')
    for i in range(numLognormalUncertain):
        f.write('{}'.format(lognormalUncertainStdDev[i]))
        f.write(' ')
    f.write('\n')
    
    f.write('descriptors = ')    
    for i in range(numLognormalUncertain):
        f.write('\'')
        f.write(lognormalUncertainName[i])
        f.write('\' ')
    f.write('\n')

if (numUniformUncertain > 0):
    f.write('uniform_uncertain = ' '{}'.format(numUniformUncertain))
    f.write('\n')
    f.write('lower_bounds = ')
    for i in range(numUniformUncertain):
        f.write('{}'.format(uniformUncertainLower[i]))
        f.write(' ')
    f.write('\n')

    f.write('upper_bounds = ')
    for i in range(numUniformUncertain):
        f.write('{}'.format(uniformUncertainUpper[i]))
        f.write(' ')
    f.write('\n')
    
    f.write('descriptors = ')    
    for i in range(numUniformUncertain):
        f.write('\'')
        f.write(uniformUncertainName[i])
        f.write('\' ')
    f.write('\n')


if (numContinuousDesign > 0):
    f.write('continuous_design = ' '{}'.format(numContinuousDesign))
    f.write('\n')

    f.write('initial_point = ')
    for i in range(numContinuousDesign):
        f.write('{}'.format(continuousDesignInitialPoint[i]))
        f.write(' ')
    f.write('\n')

    f.write('lower_bounds = ')
    for i in range(numContinuousDesign):
        f.write('{}'.format(continuousDesignLower[i]))
        f.write(' ')
    f.write('\n')

    f.write('upper_bounds = ')
    for i in range(numContinuousDesign):
        f.write('{}'.format(continuousDesignUpper[i]))
        f.write(' ')
    f.write('\n')
    
    f.write('descriptors = ')    
    for i in range(numContinuousDesign):
        f.write('\'')
        f.write(continuousDesignName[i])
        f.write('\' ')
    f.write('\n')


numCState = 0
if (numCState > 0):
    f.write('discrete_state_range = ' '{}'.format(numConstantState))
    f.write('\n')

    f.write('initial_state = ')
    for i in range(numConstantState):
        f.write('{}'.format(constantStateValue[i]))
        f.write(' ')
    f.write('\n')

    f.write('descriptors = ')    
    for i in range(numConstantState):
        f.write('\'')
        f.write(constantStateName[i])
        f.write('\' ')
    f.write('\n')

if (numConstantState > 0):
    f.write('discrete_design_set\nreal = ' '{}'.format(numConstantState))
    f.write('\n')

    f.write('num_set_values = ')
    for i in range(numConstantState):
        f.write('{}'.format(1))
        f.write(' ')
    f.write('\n')

    f.write('set_values = ')
    for i in range(numConstantState):
        f.write('{}'.format(constantStateValue[i]))
        f.write(' ')
    f.write('\n')

    f.write('descriptors = ')    
    for i in range(numConstantState):
        f.write('\'')
        f.write(constantStateName[i])
        f.write('\' ')
    f.write('\n')

if (numBetaUncertain > 0):
    f.write('beta_uncertain = ' '{}'.format(numBetaUncertain))
    f.write('\n')
    f.write('alphas = ')
    for i in range(numBetaUncertain):
        f.write('{}'.format(betaUncertainAlphas[i]))
        f.write(' ')
    f.write('\n')

    f.write('betas = ')
    for i in range(numBetaUncertain):
        f.write('{}'.format(betaUncertainBetas[i]))
        f.write(' ')
    f.write('\n')

    f.write('lower_bounds = ')
    for i in range(numBetaUncertain):
        f.write('{}'.format(betaUncertainLower[i]))
        f.write(' ')
    f.write('\n')

    f.write('upper_bounds = ')
    for i in range(numBetaUncertain):
        f.write('{}'.format(betaUncertainHigher[i]))
        f.write(' ')
    f.write('\n')
    
    f.write('descriptors = ')    
    for i in range(numBetaUncertain):
        f.write('\'')
        f.write(betaUncertainName[i])
        f.write('\' ')
    f.write('\n')

if (numGammaUncertain > 0):
    f.write('gamma_uncertain = ' '{}'.format(numGammaUncertain))
    f.write('\n')
    f.write('alphas = ')
    for i in range(numGammaUncertain):
        f.write('{}'.format(gammaUncertainAlphas[i]))
        f.write(' ')
    f.write('\n')

    f.write('betas = ')
    for i in range(numGammaUncertain):
        f.write('{}'.format(gammaUncertainBetas[i]))
        f.write(' ')
    f.write('\n')
    
    f.write('descriptors = ')    
    for i in range(numGammaUncertain):
        f.write('\'')
        f.write(gammaUncertainName[i])
        f.write('\' ')
    f.write('\n')

if (numGumbellUncertain > 0):
    f.write('gamma_uncertain = ' '{}'.format(numGumbellUncertain))
    f.write('\n')
    f.write('alphas = ')
    for i in range(numGumbellUncertain):
        f.write('{}'.format(gumbellUncertainAlphas[i]))
        f.write(' ')
    f.write('\n')

    f.write('betas = ')
    for i in range(numGumbellUncertain):
        f.write('{}'.format(gumbellUncertainBetas[i]))
        f.write(' ')
    f.write('\n')
    
    f.write('descriptors = ')    
    for i in range(numGumbellUncertain):
        f.write('\'')
        f.write(gumbellUncertainName[i])
        f.write('\' ')
    f.write('\n')

if (numWeibullUncertain > 0):
    f.write('weibull_uncertain = ' '{}'.format(numWeibullUncertain))
    f.write('\n')
    f.write('alphas = ')
    for i in range(numWeibullUncertain):
        f.write('{}'.format(weibullUncertainAlphas[i]))
        f.write(' ')
    f.write('\n')

    f.write('betas = ')
    for i in range(numWeibullUncertain):
        f.write('{}'.format(weibullUncertainBetas[i]))
        f.write(' ')
    f.write('\n')
    
    f.write('descriptors = ')    
    for i in range(numWeibullUncertain):
        f.write('\'')
        f.write(weibullUncertainName[i])
        f.write('\' ')
    f.write('\n')

f.write('\n')

# write out the interface data

femProgram = fem_data["program"]

if femProgram in ['OpenSees', 'OpenSees-2', 'FEAPpv']:
    f.write('interface,\n')
    #f.write('system # asynch evaluation_concurrency = 8')
    #f.write('fork asynchronous evaluation_concurrency = ' '{}'.format(numCPUs))
    if run_type in ['runningLocal',]:
        f.write("fork asynchronous evaluation_concurrency = %d" % numCPUs)
    else:
        f.write('fork asynchronous')
    f.write('\nanalysis_driver = \'fem_driver\' \n')
    f.write('parameters_file = \'params.in\' \n')
    f.write('results_file = \'results.out\' \n')
    f.write('work_directory directory_tag \n')
    f.write('copy_files = \'templatedir/*\' \n')
#    f.write('named \'workdir\' file_save  directory_save \n')
    f.write('named \'workdir\' \n')
    f.write('aprepro \n')
    f.write('\n')
    
# write out the responses

if uq_method == "Sampling":
    f.write(
"""responses,
response_functions = {numResponses}
response_descriptors = {responseDescriptors}
no_gradients
no_hessians

""".format(
    numResponses = numResponses,
    responseDescriptors = '\n'.join(["'{}'".format(r) for r in responseDescriptors])))

elif uq_method == "Calibration":
    f.write(
"""responses,
calibration_terms = {numResponses}
response_descriptors = {responseDescriptors}
numerical_gradients
no_hessians

""".format(
    numResponses = numResponses,
    responseDescriptors = '\n'.join(["'{}'".format(r) for r in responseDescriptors])))

elif uq_method == "Bayesian Calibration":
    f.write(
"""responses,
calibration_terms = {numResponses}
response_descriptors = {responseDescriptors}
numerical_gradients
no_hessians

""".format(
    numResponses = numResponses,
    responseDescriptors = '\n'.join(["'{}'".format(r) for r in responseDescriptors])))

f.close()  # you can omit in most cases as the destructor will call it

# prepare the workflow driver file for dakota

# if OpenSees, Write the OpenSees file for dprepo
if (femProgram == "OpenSees-SingleScript"):

    model_file = fem_data["mainInput"]    

    os.chdir(workdir_main)

    f = open(fem_driver, 'w')
    f.write(DakotaR)
    f.write(' params.in ')
    f.write(model_file)
    f.write(' SimCenterInput.tcl\n')
    f.write(OpenSees)
    f.write(' SimCenterInput.tcl >> ops.out\n')
    f.close()

if (femProgram == "OpenSees"):

    model_file = fem_data["mainInput"]    
    postprocessScript = fem_data["mainPostprocessScript"]    

    with open('SimCenterParams.template', 'w') as f:
        for rnd_name in uncertainName:
            f.write('set {rnd_name} {{{rnd_name}}}\n'.format(rnd_name=rnd_name))

    with open('SimCenterInput.ops', 'w') as f:
        f.write(
"""source SimCenterParamIN.ops
source {model_file}
""".format(model_file=model_file))

    with open(fem_driver, 'w') as f:
        f.write(
"""{DakotaR} params.in SimCenterParams.template SimCenterParamIN.ops
{OpenSees} SimCenterInput.ops >> ops.out
python {postprocessScript} {responseDescriptors}
""".format(
    DakotaR = DakotaR,
    OpenSees = OpenSees,
    postprocessScript = postprocessScript,
    responseDescriptors = ' '.join(responseDescriptors)))
	
    os.chdir(workdir_main)
    
    # this file seems identical to the previous fem_driver, so it would be easier to just copy it
    with open(fem_driver, 'w') as f:
        f.write(
"""{DakotaR} params.in SimCenterParams.template SimCenterParamIN.ops
{OpenSees} SimCenterInput.ops >> ops.out
python {postprocessScript} {responseDescriptors}
""".format(
    DakotaR = DakotaR,
    OpenSees = OpenSees,
    postprocessScript = postprocessScript,
    responseDescriptors = ' '.join(responseDescriptors)))

if (femProgram == "FEAPpv"):

    inputFile = fem_data["mainInput"]    
    postprocessScript = fem_data["mainPostprocessScript"]
    
    if (sys.version_info > (3, 0)):
        f = open('feapname', 'w')
        f.write('SimCenterIn.txt   \n')
        f.write('SimCenterOut.txt   \n')
        f.write('SimCenterR.txt   \n')
        f.write('SimCenterR.txt   \n')
        f.write('NONE   \n')
        f.write('\n')
        f.close()
        
        os.chdir(workdir_main)
        f = open(fem_driver, 'w', newline='\n')
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
        f = io.open('feapname', 'w', newline='\n')
        f.write(unicode('SimCenterIn.txt   \n'))
        f.write(unicode('SimCenterOut.txt   \n'))
        f.write(unicode('SimCenterR.txt   \n'))
        f.write(unicode('SimCenterR.txt   \n'))
        f.write(unicode('NONE   \n'))
        f.write(unicode('\n'))
        f.close()
        
        os.chdir(workdir_main)
        f = io.open(fem_driver, 'w', newline='\n')
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


os.chmod(fem_driver, stat.S_IXUSR | stat.S_IRUSR | stat.S_IXOTH)

command = Dakota + ' -input dakota.in -output dakota.out -error dakota.err'
print(command)
#os.popen("/Users/fmckenna/dakota-6.7.0/bin/dakota -input dakota.in -output dakota.out -error dakota.err").read()

if run_type in ['runningLocal']:
    os.popen(command).read()




