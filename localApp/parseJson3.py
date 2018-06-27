import io
import json
import os
import sys
import stat
import shutil
from pprint import pprint

inputArgs = sys.argv
#print inputArgs

path1 = inputArgs[1]
path2 = inputArgs[2]
exeDakota = inputArgs[3]

if (sys.platform == 'darwin'):
    OpenSees = '/Users/fmckenna/bin/OpenSees'
    Feap = '/Users/fmckenna/bin/feappv'
    Dakota = '/Users/fmckenna/dakota-6.7.0/bin/dakota'
    DakotaR = '/Users/fmckenna/dakota-6.7.0/bin/dprepro'
    fem_driver = 'fem_driver'
    numCPUs = 8

else:
    OpenSees = 'C:\\Users\\SimCenter\\OpenSees\\Win64\\bin\\OpenSees.exe'
    Feap = 'C:\\Users\\SimCenter\\feap\\Feappv41.exe'
    Dakota = 'C:\\Users\\SimCenter\\dakota-6.7\\bin\\dakota.bat'
    DakotaR = 'python C:\\Users\\SimCenter\\dakota-6.7\\bin\\dprepro'
    fem_driver = 'fem_driver.bat'
    numCPUs = 8

if exeDakota in ['runningRemote']:
    OpenSees = '/home1/00477/tg457427/bin/OpenSees'
    Feap = '/home1/00477/tg457427/bin/feappv'
    Dakota = 'dakota'
    DakotaR = 'dprepro'
    fem_driver = 'fem_driver'

os.chdir(path2)
cwd = os.getcwd()

#
# open file
#

with open('dakota.json') as data_file:    
    data = json.load(data_file)

uqData = data["uqMethod"];
femData = data["fem"];

#with open('data.txt', 'w') as outfile:  
#    json.dump(data, outfile)
#print data["method"]

# 
# parse the Random Variables
#
numUncertain = 0;
uncertainName = [];

numNormalUncertain = 0;
normalUncertainName=[];
normalUncertainMean =[];
normalUncertainStdDev =[];

numLognormalUncertain = 0;
lognormalUncertainName=[];
lognormalUncertainMean =[];
lognormalUncertainStdDev =[];

numUniformUncertain = 0;
uniformUncertainName=[];
uniformUncertainLower =[];
uniformUncertainUpper =[];

numContinuousDesign = 0;
continuousDesignName=[];
continuousDesignLower =[];
continuousDesignUpper =[];
continuousDesignInitialPoint =[];

numConstantState = 0;
constantStateName=[];
constantStateValue =[];

numWeibullUncertain = 0;
weibullUncertainName=[];
weibullUncertainAlphas =[];
weibullUncertainBetas =[];

numGammaUncertain = 0;
gammaUncertainName=[];
gammaUncertainAlphas =[];
gammaUncertainBetas =[];

numGumbellUncertain = 0;
gumbellUncertainName=[];
gumbellUncertainAlphas =[];
gumbellUncertainBetas =[];

numBetaUncertain = 0;
betaUncertainName=[];
betaUncertainLower =[];
betaUncertainHigher =[];
betaUncertainAlphas =[];

for k in data["randomVariables"]:
    if (k["distribution"] == 'Normal'):
        uncertainName.append(k["name"])
        numUncertain += 1
        normalUncertainName.append(k["name"])
        normalUncertainMean.append(k["mean"])
        normalUncertainStdDev.append(k["stdDev"])
        numNormalUncertain += 1
    elif (k["distribution"] == "Lognormal"):
        uncertainName.append(k["name"])
        numUncertain += 1
        lognormalUncertainName.append(k["name"])
        lognormalUncertainMean.append(k["mean"])
        lognormalUncertainStdDev.append(k["stdDev"])
        numLognormalUncertain += 1
    elif (k["distribution"] == "Constant"):
        uncertainName.append(k["name"])
        numUncertain += 1
        constantStateName.append(k["name"])
        constantStateValue.append(k["value"])
        numConstantState += 1
    elif (k["distribution"] == "Uniform"):
        uncertainName.append(k["name"])
        numUncertain += 1
        print("Hellooo,, Setting lower upper bounds...")
        uniformUncertainName.append(k["name"])
        uniformUncertainLower.append(k["lowerbound"])
        uniformUncertainUpper.append(k["upperbound"])
        numUniformUncertain += 1
    elif (k["distribution"] == "ContinuousDesign"):
        uncertainName.append(k["name"])
        numUncertain += 1
        continuousDesignName.append(k["name"])
        continuousDesignLower.append(k["lowerbound"])
        continuousDesignUpper.append(k["upperbound"])
        continuousDesignInitialPoint.append(k["initialpoint"])
        numContinuousDesign += 1
    elif (k["distribution"] == "Weibull"):
        uncertainName.append(k["name"])
        numUncertain += 1
        weibullUncertainName.append(k["name"])
        weibullUncertainAlphas.append(k["scaleparam"])
        weibullUncertainBetas.append(k["shapeparam"])
        numWeibullUncertain += 1
    elif (k["distribution"] == "Gamma"):
        uncertainName.append(k["name"])
        numUncertain += 1
        gammaUncertainName.append(k["name"])
        gammaUncertainAlphas.append(k["alphas"])
        gammaUncertainBetas.append(k["betas"])
        numGammaUncertain += 1
    elif (k["distribution"] == "Gumbell"):
        uncertainName.append(k["name"])
        numUncertain += 1
        gumbellUncertainName.append(k["name"])
        gumbellUncertainAlphas.append(k["alphas"])
        gumbellUncertainBetas.append(k["betas"])
        numGumbellUncertain += 1
    elif (k["distribution"] == "Beta"):
        uncertainName.append(k["name"])
        numUncertain += 1
        betaUncertainName.append(k["name"])
        betaUncertainLower.append(k["upperBounds"])
        betaUncertainUpper.append(k["lowerBounds"])
        betaUncertainAlphas.append(k["alphas"])
        betaUncertainBetas.append(k["betas"])
        numBetaUncertain += 1

#print("-----------------")
#print(data)
#print("-----------------")

#
# Write the dakota input file: dakota.in 
#

f = open('../dakota.in', 'w')

#
# write out the env data
#

f.write("environment\n")
f.write("tabular_data\n")
f.write("tabular_data_file = \'dakotaTab.out\'\n\n")

#
# write out the methods data
#

f.write('method,\n')

uqData = data["uqMethod"];
type = uqData["uqType"];

numResponses=0;
responseDescriptors=[];

if (type == "Sampling"):
    samplingData = uqData["samplingMethodData"];
    numSamples = 0;
    seed = 0;    
    f.write('sampling\n');
    method = samplingData["method"];
    if (method == "Monte Carlo"):
        method = 'random'
    else:
        method = 'lhs'
    numSamples=samplingData["samples"];
    seed = samplingData["seed"];
    f.write('sample_type = ' '{}'.format(method))
    f.write('\n');
    f.write('samples = ' '{}'.format(numSamples))
    f.write('\n');
    f.write('seed = ' '{}'.format(seed))

    edps = samplingData["edps"];
    for edp in edps:
        responseDescriptors.append(edp["name"]);
        numResponses += 1;


    f.write('\n\n')

elif (type == "Calibration"):
    calibrationData = uqData["calibrationMethodData"];
    maxIterations = 10;
    convergenceTol = 1.0e-4
    method = calibrationData["method"];
    if (method == "OPT++GaussNewton"):
        method = 'optpp_g_newton'
    else:
        method = 'nl2sol'
    f.write(method);
    f.write('\n');
    convergenceTol=calibrationData["convergenceTol"];
    maxIter = calibrationData["maxIterations"];
    f.write('convergence_tolerance = ' '{}'.format(convergenceTol))
    f.write('\n');
    f.write('max_iterations = ' '{}'.format(maxIter))

    edps = calibrationData["edps"];
    for edp in edps:
        responseDescriptors.append(edp["name"]);
        numResponses += 1;


    f.write('\n\n')

elif (type == "Bayesian Calibration"):
    samplingData = uqData["bayesian_calibration_method_data"];
    chainSamples = 0;
    seed = 0;    

    method = samplingData["method"];
    if (method == "DREAM"):
        method = 'dream'
    chainSamples=samplingData["chain_samples"];
    seed = samplingData["seed"];

    f.write('bayes_calibration ' '{}'.format(method))
    f.write('\n');
    f.write('chain_samples = ' '{}'.format(chainSamples))
    f.write('\n');
    f.write('seed = ' '{}'.format(seed))

    edps = samplingData["edps"];
    for edp in edps:
        responseDescriptors.append(edp["name"]);
        numResponses += 1;


    f.write('\n\n')


#
# write out the variable data
#

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

f.write('\n\n')

#
# write out the interface data
#


femProgram = femData["program"];

if (femProgram == "OpenSees" or femProgram == "OpenSees-2" or femProgram == "FEAPpv"):
    f.write('interface,\n')
    #f.write('system # asynch evaluation_concurrency = 8')
    #f.write('fork asynchronous evaluation_concurrency = ' '{}'.format(numCPUs))
    if exeDakota in ['runningLocal']:
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
# print(numResponses)

if (type == "Sampling"):
    f.write('responses, \n')
    f.write('response_functions = ' '{}'.format(numResponses))
    f.write('\n')
    f.write('response_descriptors = ')    
    for i in range(numResponses):
        f.write('\'')
        f.write(responseDescriptors[i])
        f.write('\' ')
        f.write('\n')
    f.write('no_gradients\n')
    f.write('no_hessians\n\n')

elif (type == "Calibration"):
    f.write('responses, \n')
    f.write('calibration_terms = ' '{}'.format(numResponses))
    f.write('\n')
    f.write('response_descriptors = ')    
    for i in range(numResponses):
        f.write('\'')
        f.write(responseDescriptors[i])
        f.write('\' ')
        f.write('\n')
    f.write('numerical_gradients\n')
    f.write('no_hessians\n\n')

elif (type == "Bayesian Calibration"):
    f.write('responses, \n')
    f.write('calibration_terms = ' '{}'.format(numResponses))
    f.write('\n')
    f.write('response_descriptors = ')    
    for i in range(numResponses):
        f.write('\'')
        f.write(responseDescriptors[i])
        f.write('\' ')
        f.write('\n')
    f.write('numerical_gradients\n')
    f.write('no_hessians\n\n')

f.close()  # you can omit in most cases as the destructor will call it


#print('params.in')

#
# if OpenSees, Write the OpenSees file for dprepo
#

femInputFile = "";
femPostprocessFile = "";

if (femProgram == "OpenSees-SingleScript"):

    inputFile = femData["mainInput"];    

    os.chdir(path1)

    f = open(fem_driver, 'w')
    f.write(DakotaR)
    f.write(' params.in ')
    f.write(inputFile)
    f.write(' SimCenterInput.tcl\n')
    f.write(OpenSees)
    f.write(' SimCenterInput.tcl >> ops.out\n')
    f.close()

if (femProgram == "OpenSees"):

    inputFile = femData["mainInput"];    
    postprocessScript = femData["mainPostprocessScript"];    

    f = open('SimCenterParams.template', 'w')
    for i in range(numUncertain):
        f.write('set ')
        f.write(uncertainName[i])
        f.write(' {')
        f.write(uncertainName[i])
        f.write('}\n')
    f.close()

    f = open('SimCenterInput.ops', 'w')
    f.write('source SimCenterParamIN.ops \n')
    f.write('source ')
    f.write(inputFile)
    f.write(' \n')
    f.close()

    f = open(fem_driver, 'w')
    f.write(DakotaR)
    f.write(' params.in SimCenterParams.template SimCenterParamIN.ops\n')
    f.write(OpenSees)
    f.write(' SimCenterInput.ops >> ops.out\n')
    #    f.write('dprepro params.in %s SimCenterInput.tcl\n' %inputFile)
    #    f.write(OpenSeesPath)
    #    f.write('OpenSees SimCenterInput.tcl >> ops.out\n')
    f.write('python ')
    f.write("%s " % postprocessScript)
    #    f.write(postprocessScript)
    for i in range(numResponses):
        #        f.write(' ')
        #        f.write(responseDescriptors[i])    
        f.write("%s " %responseDescriptors[i])
    f.write('\n')
    f.close()
	
    os.chdir(path1)
    f = open(fem_driver, 'w')
    f.write(DakotaR)
    f.write(' params.in SimCenterParams.template SimCenterParamIN.ops\n')
    f.write(OpenSees)
    f.write(' SimCenterInput.ops >> ops.out\n')
    f.write('python ')
    f.write("%s " % postprocessScript)
    for i in range(numResponses):
        f.write("%s " %responseDescriptors[i])    

    f.close()

if (femProgram == "FEAPpv"):

    inputFile = femData["mainInput"];    
    postprocessScript = femData["mainPostprocessScript"];    

    f = io.open('feapname', 'w', newline='\n')
    f.write(unicode('SimCenterIn.txt   \n'))
    f.write(unicode('SimCenterOut.txt   \n'))
    f.write(unicode('SimCenterR.txt   \n'))
    f.write(unicode('SimCenterR.txt   \n'))
    f.write(unicode('NONE   \n'))
    f.write(unicode('\n'))
    f.close()
    
    os.chdir(path1)
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

if exeDakota in ['runningLocal']:
    os.popen(command).read()




