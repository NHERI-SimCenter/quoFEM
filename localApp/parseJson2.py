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

os.chdir(path2)
cwd = os.getcwd()
#print cwd

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
uniformUncertainHigher =[];

numWeibullUncertain = 0;
weibullUncertainName=[];
weibullUncertainAlphas =[];
wuibullUncertainBetas =[];

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
    if (k["distribution"] == "Normal"):
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
    elif (k["distribution"] == "Uniform"):
        uncertainName.append(k["name"])
        numUncertain += 1
        uniformUncertainName.append(k["name"])
        uniformUncertainLower.append(k["upperBounds"])
        uniformUncertainUpper.append(k["lowerBounds"])
        numUniformUncertain += 1
    elif (k["distribution"] == "Weibull"):
        uncertainName.append(k["name"])
        numUncertain += 1
        weibullUncertainName.append(k["name"])
        weibullUncertainAlphas.append(k["alphas"])
        weibullUncertainBetas.append(k["betas"])
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

#
# write out the variable data
#

f.write('variables,\n')

if (numNormalUncertain > 0):
    f.write('normal_uncertain = ' '{}'.format(numNormalUncertain))
    f.write('\n')
    f.write('means = ')
    for i in xrange(numNormalUncertain):
        f.write('{}'.format(normalUncertainMean[i]))
        f.write(' ')
    f.write('\n')

    f.write('std_deviations = ')
    for i in xrange(numNormalUncertain):
        f.write('{}'.format(normalUncertainStdDev[i]))
        f.write(' ')
    f.write('\n')
    
    f.write('descriptors = ')    
    for i in xrange(numNormalUncertain):
        f.write('\'')
        f.write(normalUncertainName[i])
        f.write('\' ')
    f.write('\n')

if (numLognormalUncertain > 0):
    f.write('lognormal_uncertain = ' '{}'.format(numLognormalUncertain))
    f.write('\n')
    f.write('means = ')
    for i in xrange(numLognormalUncertain):
        f.write('{}'.format(lognormalUncertainMean[i]))
        f.write(' ')
    f.write('\n')

    f.write('std_deviations = ')
    for i in xrange(numLognormalUncertain):
        f.write('{}'.format(lognormalUncertainStdDev[i]))
        f.write(' ')
    f.write('\n')
    
    f.write('descriptors = ')    
    for i in xrange(numLognormalUncertain):
        f.write('\'')
        f.write(lognormalUncertainName[i])
        f.write('\' ')
    f.write('\n')

if (numUniformUncertain > 0):
    f.write('uniform_uncertain = ' '{}'.format(numUniformUncertain))
    f.write('\n')
    f.write('lower_bounds = ')
    for i in xrange(numUniformUncertain):
        f.write('{}'.format(uniformUncertainLower[i]))
        f.write(' ')
    f.write('\n')

    f.write('upper_bounds = ')
    for i in xrange(numUniformUncertain):
        f.write('{}'.format(uniformUncertainHigher[i]))
        f.write(' ')
    f.write('\n')
    
    f.write('descriptors = ')    
    for i in xrange(numUniformUncertain):
        f.write('\'')
        f.write(uniformUncertainName[i])
        f.write('\' ')
    f.write('\n')

if (numBetaUncertain > 0):
    f.write('beta_uncertain = ' '{}'.format(numBetaUncertain))
    f.write('\n')
    f.write('alphas = ')
    for i in xrange(numBetaUncertain):
        f.write('{}'.format(betaUncertainAlphas[i]))
        f.write(' ')
    f.write('\n')

    f.write('betas = ')
    for i in xrange(numBetaUncertain):
        f.write('{}'.format(betaUncertainBetas[i]))
        f.write(' ')
    f.write('\n')

    f.write('lower_bounds = ')
    for i in xrange(numBetaUncertain):
        f.write('{}'.format(betaUncertainLower[i]))
        f.write(' ')
    f.write('\n')

    f.write('upper_bounds = ')
    for i in xrange(numBetaUncertain):
        f.write('{}'.format(betaUncertainHigher[i]))
        f.write(' ')
    f.write('\n')
    
    f.write('descriptors = ')    
    for i in xrange(numBetaUncertain):
        f.write('\'')
        f.write(betaUncertainName[i])
        f.write('\' ')
    f.write('\n')

if (numGammaUncertain > 0):
    f.write('gamma_uncertain = ' '{}'.format(numGammaUncertain))
    f.write('\n')
    f.write('alphas = ')
    for i in xrange(numGammaUncertain):
        f.write('{}'.format(gammaUncertainAlphas[i]))
        f.write(' ')
    f.write('\n')

    f.write('betas = ')
    for i in xrange(numGammaUncertain):
        f.write('{}'.format(gammaUncertainBetas[i]))
        f.write(' ')
    f.write('\n')
    
    f.write('descriptors = ')    
    for i in xrange(numGammaUncertain):
        f.write('\'')
        f.write(gammaUncertainName[i])
        f.write('\' ')
    f.write('\n')

if (numGumbellUncertain > 0):
    f.write('gamma_uncertain = ' '{}'.format(numGumbellUncertain))
    f.write('\n')
    f.write('alphas = ')
    for i in xrange(numGumbellUncertain):
        f.write('{}'.format(gumbellUncertainAlphas[i]))
        f.write(' ')
    f.write('\n')

    f.write('betas = ')
    for i in xrange(numGumbellUncertain):
        f.write('{}'.format(gumbellUncertainBetas[i]))
        f.write(' ')
    f.write('\n')
    
    f.write('descriptors = ')    
    for i in xrange(numGumbellUncertain):
        f.write('\'')
        f.write(gumbellUncertainName[i])
        f.write('\' ')
    f.write('\n')

if (numWeibullUncertain > 0):
    f.write('gamma_uncertain = ' '{}'.format(numWeibullUncertain))
    f.write('\n')
    f.write('alphas = ')
    for i in xrange(numWeibullUncertain):
        f.write('{}'.format(weibullUncertainAlphas[i]))
        f.write(' ')
    f.write('\n')

    f.write('betas = ')
    for i in xrange(numWeibullUncertain):
        f.write('{}'.format(weibullUncertainBetas[i]))
        f.write(' ')
    f.write('\n')
    
    f.write('descriptors = ')    
    for i in xrange(numWeibullUncertain):
        f.write('\'')
        f.write(weibullUncertainName[i])
        f.write('\' ')
    f.write('\n')

f.write('\n\n')

#
# write out the interface data
#


femProgram = femData["program"];

if (femProgram == "OpenSees" or femProgram == "OpenSees-2"):
    f.write('interface,\n')
    f.write('system # asynch evaluation_concurrency = 4\n')
    f.write('analysis_driver = \'fem_driver\' \n')
    f.write('parameters_file = \'params.in\' \n')
    f.write('results_file = \'results.out\' \n')
    f.write('work_directory directory_tag \n')
    f.write('copy_files = \'templatedir/*\' \n')
    f.write('named \'workdir\' file_save  directory_save \n')
    f.write('aprepro \n')
    f.write('\n')
    

# write out the responses
print(numResponses)

f.write('responses, \n')
f.write('response_functions = ' '{}'.format(numResponses))
f.write('\n')
f.write('response_descriptors = ')    
for i in xrange(numResponses):
    f.write('\'')
    f.write(responseDescriptors[i])
    f.write('\' ')
f.write('\n')
f.write('no_gradients\n')
f.write('no_hessians\n\n')

f.close()  # you can omit in most cases as the destructor will call it


#print('params.in')

#
# if OpenSees, Write the OpenSees file for dprepo
#

femInputFile = "";
femPostprocessFile = "";

if (femProgram == "OpenSees"):

    f = open('params.template', 'w')
    inputFile = femData["mainInput"];    
    f.write('pwd\n')
    
    for i in xrange(numUncertain):
        f.write('set ')
        f.write(uncertainName[i])
        f.write(' {')
        f.write(uncertainName[i])
        f.write('}\n')

    f.close()

    f = open('paramOUT.ops', 'w')
    f.write('set outFile [open results.out w]\n')
    
    for i in xrange(numResponses):
        f.write('puts $outFile $')
        f.write(responseDescriptors[i])
        f.write('\n')
        
    f.write('close $outFile\n\n')
    f.close()

    f = open('main.ops', 'w')
    f.write('source paramIN.ops \n')
    f.write('source ')
    f.write(inputFile)
    f.write(' \n')
    f.write('source paramOUT.ops \n')
    f.close()

    f = open('fem_driver', 'w')
    f.write('dprepro $1 params.template paramIN.ops\n')
    f.write('/Users/fmckenna/bin/OpenSees main.ops >> ops.out\n')
    f.close()

    os.chmod('fem_driver', stat.S_IXUSR | stat.S_IRUSR | stat.S_IXOTH)

if (femProgram == "OpenSees-2"):

    f = open('params.template', 'w')
    inputFile = femData["mainInput"];    
    postprocessScript = femData["mainPostprocessScript"];    
    f.write('pwd\n')
    
    for i in xrange(numUncertain):
        f.write('set ')
        f.write(uncertainName[i])
        f.write(' {')
        f.write(uncertainName[i])
        f.write('}\n')

    f.close()

    f = open('main.ops', 'w')
    f.write('source paramIN.ops \n')
    f.write('source ')
    f.write(inputFile)
    f.write(' \n')
    f.close()

    f = open('../fem_driver', 'w')
    f.write('/Users/fmckenna/dakota-6.7.0/bin/dprepro $1 params.template paramIN.ops\n')
    f.write('/Users/fmckenna/bin/OpenSees main.ops >> ops.out\n')
    f.write('python ')
    f.write(postprocessScript)
    for i in xrange(numResponses):
        f.write(' ')
        f.write(responseDescriptors[i])    
                                   
    f.write('\n')
    f.close()

    os.chmod('../fem_driver', stat.S_IXUSR | stat.S_IRUSR | stat.S_IXOTH)

    
os.chdir(path1)
os.popen("/Users/fmckenna/dakota-6.7.0/bin/dakota -input dakota.in -output dakota.out -error dakota.err").read()




