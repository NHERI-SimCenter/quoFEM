import numpy as np
from math import pi, sqrt
from sys import platform
if platform == "darwin": # MACOS
    from openseespymac.opensees import *
else:
    from openseespy.opensees import *
import os


''' FUNCTION: build_model ------------------------------------------------------
Generates OpenSeesPy model of an elastic-perfectly plastic SDOF system and runs
gravity analysis.
Inputs: in model_params
	W - weight of structure
	f_yield - yield stiffness
	T1 - fundamental period
Outputs:

-----------------------------------------------------------------------------'''

def build_model(model_params):

	G = 386.1
	W = model_params["W"]
	f_yield = model_params["f_yield"]
	T1 = model_params["T1"]
	m = W / G
	print("m: " + str(m))

	# set model dimensions and deg of freedom
	model('basic', '-ndm', 3, '-ndf', 6)

	# define nodes
	base_node_tag = 10000
	top_node_tag = 10001
	height = 240. # in
	node(base_node_tag, 0., 0., 0.)
	node(top_node_tag, 0., 0., height)

	# define fixities
	fix(base_node_tag, 1, 1, 1, 1, 1, 1)
	fix(top_node_tag, 0, 0, 0, 1, 1, 1)

	# define bilinear (elastic-perfectly plastic) material
	material_tag = 100
	stiffmat = 110
	K = m / (T1/(2*pi))**2
	print("K: " + str(K))
	uniaxialMaterial('Steel01', material_tag, f_yield, K, 0.0001)
	uniaxialMaterial('Elastic', stiffmat, 1.e9)

	# define element
	element_tag = 1000
	element('twoNodeLink', element_tag, base_node_tag, top_node_tag, '-mat', stiffmat, material_tag, material_tag, '-dir', 1, 2, 3, '-orient', 0., 0., 1., 0., 1., 0., '-doRayleigh')

	# define mass
	mass(top_node_tag, m, m, m, 0., 0., 0.)

	# define gravity loads
	# W = m * 386.01 # g
	timeSeries('Linear', 1)
	pattern('Plain', 101, 1)
	load(top_node_tag, 0., 0., -W, 0., 0., 0.)

	# define damping based on first eigenmode
	damp_ratio = 0.05
	angular_freq = eigen(1)[0]**0.5
	beta_k = 2 * damp_ratio / angular_freq
	rayleigh(0., beta_k, 0., 0.)

	# run gravity analysis
	tol = 1e-8                          # convergence tolerance for test
	iter = 100                          # max number of iterations
	nstep = 100               		    # apply gravity loads in 10 steps
	incr = 1./nstep          			# first load increment

	# analysis settings
	constraints('Transformation')       # enforce boundary conditions using transformation constraint handler
	numberer('RCM')			            # renumbers dof's to minimize band-width (optimization)
	system('BandGeneral')		        # stores system of equations as 1D array of size bandwidth x number of unknowns
	test('EnergyIncr', tol, iter, 0)    # tests for convergence using dot product of solution vector and norm of right-hand side of matrix equation
	algorithm('Newton')                 # use Newton's solution algorithm: updates tangent stiffness at every iteration
	integrator('LoadControl', incr)     # determine the next time step for an analysis # apply gravity in 10 steps
	analysis('Static')		            # define type of analysis, static or transient
	analyze(nstep)	          			# perform gravity analysis

	# after gravity analysis, change time and tolerance for the dynamic analysis
	loadConst('-time', 0.0)


# FUNCTION: PeakDriftRecorder --------------------------------------------------
# saves envelope of interstory drift ratio for each story at one analysis step
# ------------------------------------------------------------------------------

def PeakDriftRecorder(EDP_specs, envDict):
    # inputs:
    # EDP_specs = dictionary of EDP type, location, direction
    # envDict = dictionary of envelope values

    for loc in EDP_specs['PID']:
        pos = 0
        for dof in EDP_specs['PID'][loc]:
            storynodes = [int(x) for x in EDP_specs['PID'][loc][dof]]
            # print("computing drifts for nodes: {}".format(storynodes))
            story_height = nodeCoord(storynodes[1],3) - nodeCoord(storynodes[0],3)
            # compute drift
            topDisp = nodeDisp(storynodes[1],dof)
            botDisp = nodeDisp(storynodes[0],dof)
            new_drift = abs((topDisp-botDisp)/story_height)
            # update dictionary
            curr_drift = envDict['PID'][loc][pos]
            new_max = max(new_drift, curr_drift)
            envDict['PID'][loc][pos] = new_max
            pos += 1

    return envDict



# FUNCTION: AccelHistoryRecorder -----------------------------------------------
# saves time history of relative floor acceleration for each story at one analysis step
# ------------------------------------------------------------------------------

def AccelHistoryRecorder(EDP_specs, histDict, count):
    # inputs:
    # histDict = dictionary of time histories
    # recorderNodes = list of nodes where EDP is recorded
    # count = current count in the time history

    for loc in EDP_specs['PFA']:
        for dof in EDP_specs['PFA'][loc]:
            storynode = int(EDP_specs['PFA'][loc][dof][0])
            # obtain acceleration
            new_acc = nodeAccel(storynode, dof)
            histDict['accel'][loc][dof][count] = new_acc


    return histDict



# FUNCTION: RunDynamicAnalysis -------------------------------------------------
# performs dynamic analysis and records EDPs in dictionary
# ------------------------------------------------------------------------------

def RunDynamicAnalysis(tol,iter,dt,driftLimit,EDP_specs,subSteps,GMX,GMZ):
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


    # generate time array from recording
    time_record = np.linspace(0,nsteps*dt,num=nsteps,endpoint=False)

    # initialize dictionary of envelope EDPs
    envelopeDict = {}
    for edp in EDP_specs:
        envelopeDict[edp] = {}
        for loc in EDP_specs[edp]:
            numdof = len(EDP_specs[edp][loc])
            envelopeDict[edp][loc] = np.zeros(numdof).tolist()

    print(envelopeDict)

    # initialize dictionary of time history EDPs
    historyDict = {'accel':{}}
    time_analysis = np.zeros(nsteps*5)
    for loc in EDP_specs['PFA']:
        historyDict['accel'][loc] = {}
        for dof in EDP_specs['PFA'][loc]:
            historyDict['accel'][loc][dof] = np.zeros(nsteps*5)

    # number of diaphragm levels
    levels = len(EDP_specs['PFA'])
    CODnodes = []
    for loc in EDP_specs['PFA']:
        CODnodes.append(int(EDP_specs['PFA'][loc][1][0]))

    print(CODnodes)


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
                    story_height = nodeCoord(CODnodes[level],3)-nodeCoord(CODnodes[level-1],3)
                    # check X direction drifts (direction 1)
                    topDisp = nodeDisp(CODnodes[level],1)
                    botDisp = nodeDisp(CODnodes[level-1],1)
                    deltaDisp = abs(topDisp-botDisp)
                    drift = deltaDisp/story_height
                    if drift >= driftLimit:
                        breaker = 1
                    # check Y direction drifts (direction 2)
                    topDisp = nodeDisp(CODnodes[level],2)
                    botDisp = nodeDisp(CODnodes[level-1],2)
                    deltaDisp = abs(topDisp-botDisp)
                    drift = deltaDisp/story_height
                    if drift >= driftLimit:
                        breaker = 1
                    # move on to check next level
                    level = level + 1
                # save parameter values in recording dictionaries at every step
                time_analysis[count] = time_analysis[count-1]+stepSize
                envelopeDict = PeakDriftRecorder(EDP_specs, envelopeDict)
                historyDict = AccelHistoryRecorder(EDP_specs, historyDict, count)
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
    historyDict['time'] = time_analysis.tolist()

    # remove extra zeros from accel time history, add GM to obtain absolute acceleration, and record envelope value
    GMX_interp = np.interp(time_analysis, time_record, GMX)
    GMZ_interp = np.interp(time_analysis, time_record, GMZ)
    for level in range(0,levels):
        # X direction
        historyDict['accel'][level][1] = historyDict['accel'][level][1][1:count+1]
        historyDict['accel'][level][1] = np.asarray(historyDict['accel'][level][1]) + GMX_interp
        envelopeDict['PFA'][level][0] = max(abs(historyDict['accel'][level][1]))
        # Z direction
        historyDict['accel'][level][2] = historyDict['accel'][level][2][1:count+1]
        historyDict['accel'][level][2] = np.asarray(historyDict['accel'][level][2]) + GMZ_interp
        envelopeDict['PFA'][level][1] = max(abs(historyDict['accel'][level][2]))


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

    # print(EDP_specs)

    wipeAnalysis()


    # define parameters for dynamic analysis
    driftLimit = 0.20   # %
    toler = 1.e-08
    maxiter = 30
    subSteps = 2

    envdata = RunDynamicAnalysis(toler,maxiter,GM_dt,driftLimit,EDP_specs,subSteps,GMX_points,GMZ_points)
    print(envdata)

    return envdata
