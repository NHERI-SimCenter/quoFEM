from scipy import special
import scipy as sp
from scipy.stats import norm, beta
import numpy as np
import shutil 
import json

###############################userdefined transformation example 1###############
##################################################################################
def transformsamples_from_Alex(x):
# this takes a vector of four rows, 1 column and then tranforms it into two gaussian and 2 others  
    nargout = 2 # Returns Param, cgroup
    # Characteristics for the probability models
    CovAP = np.array([[4, 0.5*2*10], [0.5*2*10, 100]])
    meanAP = np.array([[20],[100]])
    medianE = 20.0
    covE = 0.3
    alphaL = 3.0
    betaL = 2.0
    rangeL = 4.0
    shiftL = 2.0
    # Perform transformation 
    [nx, ns] = x.shape
    # initialize parameter vector
    Param = np.zeros((nx, ns), dtype=np.float32)
    # Initialize vector of correlation groupings
    cgroup = np.zeros((nx,1), dtype=np.uint8)
    # Transformation
    cholesky_decomposition = np.linalg.cholesky(CovAP)
    repmat = np.zeros((2,ns))
    repmat[0,:] = meanAP[0]
    repmat[1,:] = meanAP[1]
    # print repmat
    result = np.matmul(cholesky_decomposition, x[0:2,:])+repmat
    # print result
    Param[0:2,:] = result
    Param[2,:] = medianE*np.exp(x[2,:]*covE)
    Param[3,:] = shiftL+(rangeL*beta.ppf(norm.cdf(x[3,:]), alphaL, betaL))#special.betainc(alphaL, betaL, norm.cdf(x[3,:])))
    # Assuming two arguments to be returned
    if(nargout==2):
        cgroup[0] = 1
        cgroup[1] = 1
        cgroup[2] = 2
        cgroup[3] = 3
        # print cgroup
    return [Param, cgroup]
#####################################################################################

###The following function takes 4 userdefined variables in UI and transforms them to four normal distributed with desired 
### mean and standard deviation
def transformsamples_four_variables_gaussian(x):

    print("\n\n The value of x before Transformation   \n\n")

    print (x);

    # now conducting the transformation

    for k in range(len(x)):
        print(x[k])

        if(x[k][0]=="E" and x[k][2]=="UserDef"):
            x[k][1]= 205000.0 + x[k][1]*25000.0
        if(x[k][0]=="P" and x[k][2]=="UserDef"):
            x[k][1]= 25000.0 + x[k][1]*2500.0
        if(x[k][0]=="A1" and x[k][2]=="UserDef"):
            x[k][1]= 250.0 + x[k][1]*25.0
        if(x[k][0]=="A2" and x[k][2]=="UserDef"):
            x[k][1]= 500.0 + x[k][1]*50.0

    return x 

if __name__ == '__main__':

    variable_file = "params.in"
    shutil.copyfile(variable_file,"params.bak")
    with open(variable_file) as f:
        content = f.readlines()
    inp_file = [x.strip() for x in content]

    numberofparameters = int(inp_file[0].split("=")[1].replace(" ", "").replace("}", ""))

    # print("Number of variables are: "+str(numberofparameters))
    #for k in range(len(inp_file)):
    #   print (inp_file[k])
    # print "--------------------------------------"

    numberofsamples = 1 #since params.in contains only one "x-vector"

    with open('dakota.json') as data_file:
        data = json.load(data_file)

    variable_type=[]

    x = []
    replace_with = []
    for k in range(1, numberofparameters+1):
        variable_value = float(inp_file[k].split("=")[1].replace(" ", "").replace("}", ""))
        variable_name  = inp_file[k].split("=")[0].replace(" ", "").replace("{", "")

        for k in data["randomVariables"]:
            if(k["name"]==variable_name):
                variable_type=k["distribution"];
        
        x.append([variable_name,variable_value,variable_type])

    #    replace_with.append(inp_file[k].split("="))

    #print(x)

    transformed_x = transformsamples_four_variables_gaussian(x)
    
    for k in range(0, numberofparameters):
        inp_file[k+1] = "{ " + transformed_x[k][0] + "  =  " + str(transformed_x[k][1]) + " }"

    # print "---------------- Overwrite params file ----------------------"
    file = open("params.in", "w")
    for k in range(len(inp_file)):
        #print (inp_file[k])
        file.write("                    "+inp_file[k]+"\n")
    #print "-------------------------------------------------------------"
    file.close()