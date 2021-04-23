### runs gravity and dynamic analysis on structural model
import numpy as np
from math import pi, sqrt
from openseespy.opensees import *
import os
import json
import pandas as pd
from BuildModel import build_model

wipe()



# FUNCTION: PeakDriftRecorder --------------------------------------------------
# saves envelope of interstory drift ratio for each story at one analysis step
# ------------------------------------------------------------------------------

def PeakDriftRecorder(envDict, recorderNodes, recorderID):
    # inputs:
    # envDict = dictionary of envelope values
    # recorderNodes = list of nodes where EDP is recorded

    # obtain story heights
    levels, num_nodes = recorderNodes.shape
    story_heights = []
    for i in range(levels):
        story_heights.append(nodeCoord(recorderNodes[i,0].tolist(),2))

    # compute and save drifts
    for level in range(1,levels):
            story_height = story_heights[level]-story_heights[level-1]
            for n in range(num_nodes):
                # interstory drift ratio in the X direction
                topDisp = nodeDisp(int(recorderNodes[level,n]),1)
                botDisp = nodeDisp(int(recorderNodes[level-1,n]),1)
                new_drift = abs((topDisp-botDisp)/story_height)
                curr_drift = envDict['PID'][recorderID[level,n]][0]
                new_max = max(new_drift, curr_drift)
                envDict['PID'][recorderID[level,n]][0] = new_max

                # interstory drift ratio in the Z direction
                topDisp = nodeDisp(int(recorderNodes[level,n]),3)
                botDisp = nodeDisp(int(recorderNodes[level-1,n]),3)
                new_drift = abs((topDisp-botDisp)/story_height)
                curr_drift = envDict['PID'][recorderID[level,n]][1]
                new_max = max(new_drift, curr_drift)
                envDict['PID'][recorderID[level,n]][1] = new_max

    return envDict


# FUNCTION: PeakRotRecorder --------------------------------------------------
# saves envelope of center rotation for each story at one analysis step
# ------------------------------------------------------------------------------

def PeakRotRecorder(envDict, recorderNodes):
    # inputs:
    # envDict = dictionary of envelope values
    # recorderNodes = list of nodes where EDP is recorded


    levels = len(recorderNodes)
    for level in range(1,levels):
        new_rot = nodeDisp(recorderNodes[level],5)
        # print("new_rot: {}".format(new_rot))
        # record peak value
        curr_rot = envDict['PFD'][level][0]
        new_max = max(abs(new_rot), curr_rot)
        envDict['PFD'][level][0] = new_max

    return envDict




# FUNCTION: AccelHistoryRecorder -----------------------------------------------
# saves time history of relative floor acceleration for each story at one analysis step
# ------------------------------------------------------------------------------

def AccelHistoryRecorder(histDict, recorderNodes, count):
    # inputs:
    # histDict = dictionary of time histories
    # recorderNodes = list of nodes where EDP is recorded
    # count = current count in the time history

    stories = len(recorderNodes)-1
    for level in range(0,stories+1):
        # acceleration in X direction
        new_acc = nodeAccel(recorderNodes[level], 1)
        histDict['accel'][level][1][count] = new_acc
        # acceleration in Z direction
        new_acc = nodeAccel(recorderNodes[level], 3)
        histDict['accel'][level][3][count] = new_acc

    return histDict



# FUNCTION: WriteEDPToFile -----------------------------------------------------
# writes EDP time history records from DynamicAnalysis to an outfile matching DPW
# ------------------------------------------------------------------------------

def WriteEDPToFile(outfile, caseNum, envDict, recorderNodes):
    # inputs:
    # outfile = file object for exporting values
    # caseNum = record number in the stripe list
    # Dict = dictionary of envelope values
    # recorderNodes = list of nodes where PFA is recorded

    # print EDPs per GM  in each line
    stories = len(recorderNodes)-1
    line = str(caseNum)
    # append PID values for both horizontal directions at each level
    for level in range(1,stories+1):
        line = line + '\t\t' + '%.6f' %envDict['PID'][level][0] + '\t\t' + '%.6f' %envDict['PID'][level][1]
    # append PFA values for both horizontal directions at each level
    for level in range(0,stories+1):
        line = line + '\t\t' + '%.6f' %envDict['PFA'][level][0] + '\t\t' + '%.6f' %envDict['PFA'][level][1]

    line += '\n'
    outfile.write(line)


# FUNCTION: RunDynamicAnalysis -------------------------------------------------
# performs dynamic analysis and records EDPs in dictionary
# ------------------------------------------------------------------------------

def RunDynamicAnalysis(tol,iter,dt,driftLimit,recorderNodes,subSteps,GMX,GMZ):
    # inputs:
    # tol = tolerance criteria to check for convergence
    # iter = max number of iterations to check
    # dt = time increment for analysis
    # driftLimit = percent interstory drift limit indicating collapse
    # recorderNodes = vector of node labels used to check global drifts and record EDPs
    # subSteps = number of subdivisions in cases of ill convergence
    # GMX = list of GM acceleration ordinates in X direction
    # GMZ = list of GM acceleration ordinates in Z direction

    # pad shorter record with zeros (free vibration) such that two horizontal records are the same length
    nsteps = max(len(GMX),len(GMZ))
    if len(GMX) < nsteps:
        diff = nsteps - len(GMX)
        GMX.extend(np.zeros(diff))
    if len(GMZ) < nsteps:
        diff = nsteps - len(GMZ)
        GMZ.extend(np.zeros(diff))

    # create labels for each recorder node
    CODnodes = recorderNodes[:,0].astype(int).tolist()  # center of diaphragm nodes
    levels, num_nodes = recorderNodes.shape
    recorderID = np.zeros(recorderNodes.shape)
    for level in range(levels):
        base = 10*(level)
        for node in range(num_nodes):
            recorderID[level, node] = base + node
    recorderID = recorderID.astype(int)

    # generate time array from recording
    time_record = np.linspace(0,nsteps*dt,num=nsteps,endpoint=False)

    # intitialize dictionary of envelope and time histories of EDPs
    time_analysis = np.zeros(nsteps*5)
    envelopeDict = {'PFD':{}, 'PID':{}, 'PFA':{}}
    historyDict = {'accel':{}}

    # set up dictionary for recording peak interstory drift (PID)
    for level in range(1,levels):
        for node in range(num_nodes):
            envelopeDict['PID'][recorderID[level,node]] = [0.,0.] # [peak X value, peak Z value]
    # set up dictionary for recording peak floor acceleration (PFA) and acceleration time history (accel)
    for level in range(0,levels):
        envelopeDict['PFD'][level] = [0.]
        envelopeDict['PFA'][level] = [0.,0.] # save to fill later
        historyDict['accel'][level] = {}
        for dof in [1,3]:
            historyDict['accel'][level][dof] = np.zeros(nsteps*5)

    constraints('Transformation')            # handles boundary conditions based on transformation equation method
    numberer('RCM')                          # renumber dof's to minimize band-width (optimization)
    system('UmfPack')                        # constructs sparse system of equations using UmfPack solver
    test('NormDispIncr',tol,iter)            # tests for convergence using norm of left-hand side of matrix equation
    algorithm('NewtonLineSearch')            # use Newton's solution algorithm: updates tangent stiffness at every iteration
    integrator('Newmark', 0.5, 0.25)         # Newmark average acceleration method for numerical integration
    analysis('Transient')                    # define type of analysis: time-dependent

    # initialize variables
    maxDiv = 1024
    minDiv = subSteps
    step = 0
    ok = 0
    breaker = 0
    maxDrift = 0
    count = 0

    while step<nsteps and ok==0 and breaker==0:
        step = step + 1  # take 1 step
        ok = 2
        div = minDiv
        length = maxDiv
        while div<=maxDiv and length>0 and breaker==0:
            stepSize = dt/div
            ok = analyze(1,stepSize)               # perform analysis for one increment; will return 0 if no convergence issues
            if ok==0:
                count = count + 1
                length = length - maxDiv/div
                # check if drift limits are satisfied
                level = 1
                while level < levels:

                    story_height = nodeCoord(CODnodes[level],2)-nodeCoord(CODnodes[level-1],2)
                    # check X direction drifts (direction 1)
                    topDisp = nodeDisp(CODnodes[level],1)
                    botDisp = nodeDisp(CODnodes[level-1],1)
                    deltaDisp = abs(topDisp-botDisp)
                    drift = deltaDisp/story_height
                    if drift >= driftLimit:
                        breaker = 1
                    # check Z direction drifts (direction 3)
                    topDisp = nodeDisp(CODnodes[level],3)
                    botDisp = nodeDisp(CODnodes[level-1],3)
                    deltaDisp = abs(topDisp-botDisp)
                    drift = deltaDisp/story_height
                    if drift >= driftLimit:
                        breaker = 1
                    # move on to check next level
                    level = level + 1
                # save parameter values in recording dictionaries at every step
                time_analysis[count] = time_analysis[count-1]+stepSize
                envelopeDict = PeakDriftRecorder(envelopeDict, recorderNodes, recorderID)
                envelopeDict = PeakRotRecorder(envelopeDict, CODnodes)
                historyDict = AccelHistoryRecorder(historyDict, CODnodes, count)
            else: # if ok != 0
                div = div*2
                print("Number of increments increased to ",str(div))
    # end analysis once drift limit has been reached
    if breaker == 1:
        ok = 1
        print("Collapse drift has been reached")

    print("Number of analysis steps completed: {}".format(count))

    # remove extra zeros from time history
    time_analysis = time_analysis[1:count+1]
    historyDict['time'] = time_analysis

    # remove extra zeros from accel time history, add GM to obtain absolute acceleration, and record envelope value
    GMX_interp = np.interp(time_analysis, time_record, GMX)
    GMZ_interp = np.interp(time_analysis, time_record, GMZ)
    for level in range(0,levels):
        # X direction
        historyDict['accel'][level][1] = historyDict['accel'][level][1][1:count+1]
        historyDict['accel'][level][1] = np.asarray(historyDict['accel'][level][1]) + GMX_interp
        envelopeDict['PFA'][level][0] = max(abs(historyDict['accel'][level][1]))
        # Z direction
        historyDict['accel'][level][3] = historyDict['accel'][level][3][1:count+1]
        historyDict['accel'][level][3] = np.asarray(historyDict['accel'][level][3]) + GMZ_interp
        envelopeDict['PFA'][level][1] = max(abs(historyDict['accel'][level][3]))


    return envelopeDict



# MAIN: run_analysis -----------------------------------------------------------
# runs dynamic analysis for single event and returns dictionary of envelope EDPs
# ------------------------------------------------------------------------------

def run_analysis(GM_dt, GM_npts, TS_List, EDP_specs):
    # inputs:
    # GM_dt = time step of GM record
    # GM_npts = number of steps in GM record
    # TS_List = 1x2 list where first component is a list of GMX acceleration points, second component is a list of GMZ acceleration points (scaled and multipled by G)
    GMX_points = TS_List[0]
    GMZ_points = TS_List[1]

    #recorderNodes = np.array([[15000, 11000, 13000, 17000, 19000],
    #                           [15001, 11001, 13001, 17001, 19001],
    #                           [15002, 11002, 13002, 17002, 19002]]).astype(int)
    #       node(12000+i, L/2, H[i], 0.0)
    #       node(14000+i, 0.0, H[i], W/2)
    #       node(16000+i, L, H[i], W/2)
    #        node(18000+i, L/2, H[i], W)
    #        # center of diaphragm
    #        node(15000+i, L/2, H[i], W/2)
    #        # p-delta nodes at corners
    #        node(11000+i, 0.0, H[i], 0.0)
    #        node(13000+i, L, H[i], 0.0)
    #        node(17000+i, 0.0, H[i], W)
    #        node(19000+i, L, H[i], W)
    #        # interior walls
    #        node(21000+i, (L/2)+self.e_x, H[i], W/2)
    #        node(22000+i, L/2, H[i], (W/2)+self.e_z)

                                # center of diaphragm, p-delta nodes at a corner1 ~ 4
    recorderNodes = np.array([[15000, 11000, 13000, 17000, 19000],
                               [15001, 11001, 13001, 17001, 19001]]).astype(int)


    wipeAnalysis()
    # wipe()
    # build_model()
    # define recorders for hysteresis
    recorder('Element', '-file', '1_4032477_EDY.txt', '-time', '-ele', 718001, 'deformation')
    recorder('Element', '-file', '1_4032477_FY.txt', '-time', '-ele', 718001, 'force')


    # define parameters for dynamic analysis
    driftLimit = 0.20   # %
    toler = 1.e-08
    maxiter = 30
    subSteps = 2

    envdata = RunDynamicAnalysis(toler,maxiter,GM_dt,driftLimit,recorderNodes,subSteps,GMX_points,GMZ_points)
    # cwdata = pd.DataFrame(data=histdata['accel'][1])
    # timedata = pd.DataFrame(data=histdata['time'])
    # cwdata = pd.concat([timedata, cwdata], axis=1)
    # print(cwdata)
    # cwdata.to_csv("AccelHist.csv", header=False, index=False)

     # print(envdata)

    return envdata
