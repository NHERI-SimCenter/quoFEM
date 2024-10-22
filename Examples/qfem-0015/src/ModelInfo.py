import numpy as np
from math import pi, sqrt
from openseespy.opensees import *
import os
from CWHMaterials import material_weight, create_wall_material, create_truss_material



class ModelInfo:
	def __init__(self, g, num_stories, story_heights, plan_x_length, plan_z_length, floor_weights, e_x, e_z, walls, damping):
		self.g = g # acceleration of gravity
		self.num_stories = num_stories
		self.story_heights = story_heights # H
		self.plan_x_length = plan_x_length # L
		self.plan_z_length = plan_z_length # W
		self.floor_weights = floor_weights
		self.e_x = e_x # eccentricity in interior walls in x direction
		self.e_z = e_z # eccentricity in interior walls in z direction
		self.walls = walls # dictionary
		self.damping = damping

		self._damped_nodes = []
		self._damped_elems = []
        # cripple wall toggle - include thirdway nodes



# FUNCTION: define_nodes -------------------------------------------------------
# defines nodal coordinates for each floor
# ------------------------------------------------------------------------------

	def define_nodes(self):

		L = self.plan_x_length
		H = self.story_heights
		W = self.plan_z_length

		# main structure nodes
		for i in range(0,self.num_stories+1):
			# exterior walls
			node(12000+i, L/2, H[i], 0.0)
			node(14000+i, 0.0, H[i], W/2)
			node(16000+i, L, H[i], W/2)
			node(18000+i, L/2, H[i], W)
			# center of diaphragm
			node(15000+i, L/2, H[i], W/2)
			# p-delta nodes at corners
			node(11000+i, 0.0, H[i], 0.0)
			node(13000+i, L, H[i], 0.0)
			node(17000+i, 0.0, H[i], W)
			node(19000+i, L, H[i], W)
			# interior walls
			node(21000+i, (L/2)+self.e_x, H[i], W/2)
			node(22000+i, L/2, H[i], (W/2)+self.e_z)
			print("Node:")
			print(i)
		# cripple wall nodes
		# if self.CW == 1:
		# 	for j in [0,1]:
		# 		# cripple walls
		# 		node(31000+i, L/3, H[j], 0.0)
		# 		node(32000+i, 2*L/3, H[j], 0.0)
		# 		node(33000+i, 0.0, H[j], W/3)
		# 		node(34000+i, 0.0, H[j], 2*W/3)
		# 		node(35000+i, L, H[j], W/3)
		# 		node(36000+i, L, H[j], 2*W/3)
		# 		node(37000+i, L/3, H[j], W)
		# 		node(38000+i, 2*L/3, H[j], W)


		# nodes with damping
		for i in range(1,self.num_stories+1):
			self._damped_nodes.append(11000+i)
			self._damped_nodes.append(12000+i)
			self._damped_nodes.append(13000+i)
			self._damped_nodes.append(14000+i)
			self._damped_nodes.append(15000+i)
			self._damped_nodes.append(16000+i)
			self._damped_nodes.append(17000+i)
			self._damped_nodes.append(18000+i)
			self._damped_nodes.append(19000+i)

		# print('Damped nodes: {}'.format(self._damped_nodes))
		# print('Nodes defined.')
		# print("TEST: coordinates of 15001: {}".format(nodeCoord(15001)))



# FUNCTION: define_fixities ----------------------------------------------------
# define fixed constraints at base nodes
# ------------------------------------------------------------------------------

	def define_fixities(self):
		# fix all base nodes at exterior
		base_ext_IDs = np.arange(11000, 20000, 1000)
		for node_id in base_ext_IDs:
			fix(int(node_id), 1, 1, 1, 1, 1, 1)

		# fix interior base nodes
		# fix(21000, 1, 1, 1, 1, 1, 1)
		# fix(22000, 1, 1, 1, 1, 1, 1)


		# print('Fixities defined.')



# FUNCTION: define_floor_diaphragms --------------------------------------------
# defines floor diaphragms built of beam-column elements
# ------------------------------------------------------------------------------

	def define_floor_diaphragms(self):
		# Define transformation for sections
		BeamTransf = 3
		geomTransf('Linear', BeamTransf, 0., 1., 0.)
		LargeNum = 1.e9

		for i in range(0, self.num_stories+1):
			# perimeter ties
			element('elasticBeamColumn', 912000+i, 11000+i, 12000+i, LargeNum, 100.0, 100.0, LargeNum, LargeNum, LargeNum, BeamTransf)
			element('elasticBeamColumn', 923000+i, 12000+i, 13000+i, LargeNum, 100.0, 100.0, LargeNum, LargeNum, LargeNum, BeamTransf)
			element('elasticBeamColumn', 936000+i, 13000+i, 16000+i, LargeNum, 100.0, 100.0, LargeNum, LargeNum, LargeNum, BeamTransf)
			element('elasticBeamColumn', 969000+i, 16000+i, 19000+i, LargeNum, 100.0, 100.0, LargeNum, LargeNum, LargeNum, BeamTransf)
			element('elasticBeamColumn', 998000+i, 19000+i, 18000+i, LargeNum, 100.0, 100.0, LargeNum, LargeNum, LargeNum, BeamTransf)
			element('elasticBeamColumn', 987000+i, 18000+i, 17000+i, LargeNum, 100.0, 100.0, LargeNum, LargeNum, LargeNum, BeamTransf)
			element('elasticBeamColumn', 974000+i, 17000+i, 14000+i, LargeNum, 100.0, 100.0, LargeNum, LargeNum, LargeNum, BeamTransf)
			element('elasticBeamColumn', 941000+i, 14000+i, 11000+i, LargeNum, 100.0, 100.0, LargeNum, LargeNum, LargeNum, BeamTransf)
			# interior ties
			element('elasticBeamColumn', 925000+i, 12000+i, 15000+i, LargeNum, 100.0, 100.0, LargeNum, LargeNum, LargeNum, BeamTransf)
			element('elasticBeamColumn', 945000+i, 14000+i, 15000+i, LargeNum, 100.0, 100.0, LargeNum, LargeNum, LargeNum, BeamTransf)
			element('elasticBeamColumn', 956000+i, 15000+i, 16000+i, LargeNum, 100.0, 100.0, LargeNum, LargeNum, LargeNum, BeamTransf)
			element('elasticBeamColumn', 958000+i, 15000+i, 18000+i, LargeNum, 100.0, 100.0, LargeNum, LargeNum, LargeNum, BeamTransf)
			element('elasticBeamColumn', 924000+i, 12000+i, 14000+i, LargeNum, 100.0, 100.0, LargeNum, LargeNum, LargeNum, BeamTransf)
			element('elasticBeamColumn', 948000+i, 14000+i, 18000+i, LargeNum, 100.0, 100.0, LargeNum, LargeNum, LargeNum, BeamTransf)
			element('elasticBeamColumn', 986000+i, 18000+i, 16000+i, LargeNum, 100.0, 100.0, LargeNum, LargeNum, LargeNum, BeamTransf)
			element('elasticBeamColumn', 962000+i, 16000+i, 12000+i, LargeNum, 100.0, 100.0, LargeNum, LargeNum, LargeNum, BeamTransf)
			element('elasticBeamColumn', 957000+i, 15000+i, 17000+i, LargeNum, 100.0, 100.0, LargeNum, LargeNum, LargeNum, BeamTransf)
			element('elasticBeamColumn', 951000+i, 15000+i, 11000+i, LargeNum, 100.0, 100.0, LargeNum, LargeNum, LargeNum, BeamTransf)
			element('elasticBeamColumn', 953000+i, 15000+i, 13000+i, LargeNum, 100.0, 100.0, LargeNum, LargeNum, LargeNum, BeamTransf)
			element('elasticBeamColumn', 959000+i, 15000+i, 19000+i, LargeNum, 100.0, 100.0, LargeNum, LargeNum, LargeNum, BeamTransf)

			# ties to interior walls
			element('elasticBeamColumn', 921700+i, 21000+i, 17000+i, LargeNum, 100.0, 100.0, LargeNum, LargeNum, LargeNum, BeamTransf)
			element('elasticBeamColumn', 921100+i, 21000+i, 11000+i, LargeNum, 100.0, 100.0, LargeNum, LargeNum, LargeNum, BeamTransf)
			element('elasticBeamColumn', 921300+i, 21000+i, 13000+i, LargeNum, 100.0, 100.0, LargeNum, LargeNum, LargeNum, BeamTransf)
			element('elasticBeamColumn', 921900+i, 21000+i, 19000+i, LargeNum, 100.0, 100.0, LargeNum, LargeNum, LargeNum, BeamTransf)

			element('elasticBeamColumn', 922700+i, 22000+i, 17000+i, LargeNum, 100.0, 100.0, LargeNum, LargeNum, LargeNum, BeamTransf)
			element('elasticBeamColumn', 922100+i, 22000+i, 11000+i, LargeNum, 100.0, 100.0, LargeNum, LargeNum, LargeNum, BeamTransf)
			element('elasticBeamColumn', 922300+i, 22000+i, 13000+i, LargeNum, 100.0, 100.0, LargeNum, LargeNum, LargeNum, BeamTransf)
			element('elasticBeamColumn', 922900+i, 22000+i, 19000+i, LargeNum, 100.0, 100.0, LargeNum, LargeNum, LargeNum, BeamTransf)

			if not (self.e_x == 0. and self.e_z == 0.):
				element('elasticBeamColumn', 920000+i, 21000+i, 22000+i, LargeNum, 100.0, 100.0, LargeNum, LargeNum, LargeNum, BeamTransf)

			# cripple wall diagonals

		# print('Floor diaphragm defined.')


# FUNCTION: define_leaningcolumns ----------------------------------------------
# defines geometry and material properties of leaning columns
# ------------------------------------------------------------------------------

	def define_leaning_columns(self):
		# create material - unaxial truss
		StiffMat, SoftMat = create_truss_material()
		trussArea = 1000. # large truss cross sectional area
		TrussRayOn = 0 # 1=use for Rayleigh damping,  0=do not use for Rayleigh damping

		for i in range(0,self.num_stories):
			# assign leaning columns on each story i
			element('corotTruss', 811000+i, 11000+i, 11000+i+1, trussArea, StiffMat) # '-doRayleigh', TrussRayOn)
			element('corotTruss', 812000+i, 12000+i, 12000+i+1, trussArea, StiffMat) # '-doRayleigh', TrussRayOn)
			element('corotTruss', 813000+i, 13000+i, 13000+i+1, trussArea, StiffMat) # '-doRayleigh', TrussRayOn)
			element('corotTruss', 814000+i, 14000+i, 14000+i+1, trussArea, StiffMat) # '-doRayleigh', TrussRayOn)
			element('corotTruss', 815000+i, 15000+i, 15000+i+1, trussArea, StiffMat) # '-doRayleigh', TrussRayOn)
			element('corotTruss', 816000+i, 16000+i, 16000+i+1, trussArea, StiffMat) # '-doRayleigh', TrussRayOn)
			element('corotTruss', 817000+i, 17000+i, 17000+i+1, trussArea, StiffMat) # '-doRayleigh', TrussRayOn)
			element('corotTruss', 818000+i, 18000+i, 18000+i+1, trussArea, StiffMat) # '-doRayleigh', TrussRayOn)
			element('corotTruss', 819000+i, 19000+i, 19000+i+1, trussArea, StiffMat) # '-doRayleigh', TrussRayOn)


		#  print('Leaning columns defined.')

# FUNCTION: define_mass_and_gravity_loads --------------------------------------
# define nodal masses at leaning column nodes
# ------------------------------------------------------------------------------

	def define_mass_and_gravity_loads(self):


		# for gravity loads
		timeSeries('Linear', 1)
		pattern('Plain', 101, 1)

		L = self.plan_x_length
		W = self.plan_z_length

		# tributary lengths of: (0) x dimension of diaphragm tributary area,
		# (1) z dimension of diaphragm tributary area,
		# (2) total tributary length of exterior wall
		# (3) total tributary length of interior wall
		TL = np.array([[0.25*L, 0.25*W, 0.25*L+0.25*W],	# 11000 nodes
					   [0.50*L, 0.25*W, 0.50*L],		# 12000 nodes
					   [0.25*L, 0.25*W, 0.25*L+0.25*W],	# 13000 nodes
					   [0.25*L, 0.50*W, 0.50*W],		# 14000 nodes
					   [0.50*L, 0.50*W, 0.],			# 15000 nodes
					   [0.25*L, 0.50*W, 0.50*W],		# 16000 nodes
					   [0.25*L, 0.25*W, 0.25*L+0.25*W],	# 17000 nodes
					   [0.50*L, 0.25*W, 0.50*L],		# 18000 nodes
					   [0.25*L, 0.25*W, 0.25*L+0.25*W]])# 19000 nodes

		# base node IDs
		nodes_base = np.arange(11000, 20000, 1000)
		Q = self.walls[1]['qualityscore']

		# story weights
		if self.num_stories != 1:
			for i in range(1,self.num_stories):
				### weight of interior walls
				w_int_x = (self.walls[i]['height']/2) * self.walls[i]['int_length'][0] * material_weight(self.walls[i]['int_material'], 'w_int', Q) / 1000
				w_int_x += (self.walls[i+1]['height']/2) * self.walls[i+1]['int_length'][0] * material_weight(self.walls[i+1]['int_material'], 'w_int', Q) / 1000
				w_int_z = (self.walls[i]['height']/2) * self.walls[i]['int_length'][1] * material_weight(self.walls[i]['int_material'], 'w_int', Q) / 1000
				w_int_z += (self.walls[i+1]['height']/2) * self.walls[i+1]['int_length'][1] * material_weight(self.walls[i+1]['int_material'], 'w_int', Q) / 1000

				m_int_x = w_int_x/self.g
				m_int_z = w_int_z/self.g

				# interior wall weights
				load(int(21000+i), 0., -w_int_x, 0., 0., 0., 0.)
				load(int(22000+i), 0., -w_int_z, 0., 0., 0., 0.)
				mass(int(21000+i), m_int_x, m_int_x, m_int_x, 0., 0., 0.)
				mass(int(22000+i), m_int_x, m_int_x, m_int_x, 0., 0., 0.)

				print('weight at {}: {}'.format(21000+i, w_int_x))
				print('weight at {}: {}'.format(22000+i, w_int_z))

				### weight of floor diaphragm and exterior walls
				for n in range(len(nodes_base)):
					# floor diaphragm weight
					w_floor = self.floor_weights[i-1] * (TL[n,0]) * (TL[n,1]) / 1000
					# exterior wall weight from below
					w_ext = (self.walls[i]['height']/2) * (TL[n,2]) * material_weight(self.walls[i]['ext_material'], 'w_ext', Q) / 1000
					# exterior wall weight from above
					w_ext += (self.walls[i+1]['height']/2) * (TL[n,2]) * material_weight(self.walls[i+1]['ext_material'], 'w_ext', Q) / 1000

					# total weight/mass
					w_i = w_floor + w_ext
					m_i = w_i/self.g

					# diaphragm weights and exterior wall weights
					load(int(nodes_base[n]+i), 0., -w_i, 0., 0., 0., 0.)
					mass(int(nodes_base[n]+i), m_i, m_i, m_i, 0., 0., 0.)

					# check
					print('weight at {}: {}'.format(nodes_base[n]+i, w_i))

		# roof weight
		i = self.num_stories
		### weight of interior walls
		w_int_x = (self.walls[i]['height']/2) * self.walls[i]['int_length'][0] * material_weight(self.walls[i]['int_material'], 'w_int', Q) / 1000
		w_int_z = (self.walls[i]['height']/2) * self.walls[i]['int_length'][1] * material_weight(self.walls[i]['int_material'], 'w_int', Q) / 1000

		m_int_x = w_int_x/self.g
		m_int_z = w_int_z/self.g

		# interior wall weights
		load(int(21000+i), 0., -w_int_x, 0., 0., 0., 0.)
		load(int(22000+i), 0., -w_int_z, 0., 0., 0., 0.)
		mass(int(21000+i), m_int_x, m_int_x, m_int_x, 0., 0., 0.)
		mass(int(22000+i), m_int_x, m_int_x, m_int_x, 0., 0., 0.)

		print('weight at {}: {}'.format(21000+i, w_int_x))
		print('weight at {}: {}'.format(22000+i, w_int_z))

		for n in range(len(nodes_base)):

			# floor diaphragm weight
			w_floor = self.floor_weights[-1] * (TL[n,0]) * (TL[n,1]) / 1000
			# exterior wall weight from below (or roof weight at top level)
			w_ext = (self.walls[i]['height']/2) * (TL[n,2]) * material_weight(self.walls[i]['ext_material'], 'w_ext', Q) / 1000

			# total weight/mass
			w_i = w_floor + w_ext
			m_i = w_i/self.g

			# diaphragm weights and exterior wall weights
			load(int(nodes_base[n]+i), 0., -w_i, 0., 0., 0., 0.)
			mass(int(nodes_base[n]+i), m_i, m_i, m_i, 0., 0., 0.)

			# check
			print('weight at {}: {}'.format(nodes_base[n]+i, w_i))


		# print('Mass and gravity loads defined.')



# FUNCTION: define_walls -------------------------------------------------------
# saves geometric and material properties of wall element
# ------------------------------------------------------------------------------

	def define_walls(self):
		base_ID = [12000, 18000, 14000, 16000]
		wall_dir = [2, 2, 3, 3]

		# create walls on each story
		for storynum in range(1,self.num_stories+1):
			Q = self.walls[storynum]['qualityscore']
			h = self.walls[storynum]['height']

			# exterior walls - S, N, W, E faces
			for n in range(0,4):
				wall_ID = 700000 + base_ID[n] + storynum
				self._damped_elems.append(wall_ID)

				ext_L = self.walls[storynum]['ext_length'][n]
				ext_mat = self.walls[storynum]['ext_material']

				create_wall_material(ext_mat, wall_ID, ext_L, h, Q)

				element('twoNodeLink', wall_ID, base_ID[n]+storynum-1, base_ID[n]+storynum, '-mat', wall_ID, '-dir', wall_dir[n], '-orient', 0., 1., 0., 1., 0., 0., '-doRayleigh')

			# interior walls
			wall_ID_x = 621000 + storynum
			wall_ID_z = 622000 + storynum
			self._damped_elems.append(wall_ID_x)
			self._damped_elems.append(wall_ID_z)

			int_L = self.walls[storynum]['int_length']
			int_mat = self.walls[storynum]['int_material']

			create_wall_material(int_mat, wall_ID_x, int_L[0], h, Q)
			create_wall_material(int_mat, wall_ID_z, int_L[1], h, Q)

			element('twoNodeLink', wall_ID_x, 21000+storynum-1, 21000+storynum, '-mat', wall_ID_x, '-dir', 2, '-orient', 0., 1., 0., 1., 0., 0., '-doRayleigh')
			element('twoNodeLink', wall_ID_z, 22000+storynum-1, 22000+storynum, '-mat', wall_ID_z, '-dir', 3, '-orient', 0., 1., 0., 1., 0., 0., '-doRayleigh')

		# print(self._damped_elems)
		# print('Walls defined.')



# FUNCTION: assign_damping -----------------------------------------------------
# defines Rayleigh damping parameters to assign at given nodes and elements
# ------------------------------------------------------------------------------

	def assign_damping(self):
		# calculate modal periods from eigenvalues
		nEigen = 3
		nLambda = eigen('-fullGenLapack', nEigen)

		Tlist = np.array([])
		for lamb in nLambda:
			Ti = 2.*pi/sqrt(lamb)
			Tlist = np.append(Tlist, Ti)

		print("Modal Periods: " + str(Tlist))

		# define damping parameters
		omegaI = 2.0*pi / Tlist[0] # T1_elastic
		omegaJ = 2.0*pi / Tlist[nEigen-1] #
		alpha1Coeff = (2.0*omegaI*omegaJ) / (omegaI + omegaJ)
		alpha2Coeff = 2.0 / (omegaI + omegaJ)
		alpha1 = alpha1Coeff * self.damping # 0.025
		alpha2 = alpha2Coeff * self.damping # 0.025

		# assign damping  to elements
		region(1,'-ele', *self._damped_elems, '-rayleigh', 0., alpha2, 0., 0.)

		# Assign damping to nodes
		region(2,'-node', *self._damped_nodes, '-rayleigh', alpha1, 0., 0., 0.)

		# print('Damping assigned.')



# FUNCTION: run_gravity_analysis -----------------------------------------------
# uses Newton's solution algorithm to perform load-controlled static analysis, BandGeneral SOE
# ------------------------------------------------------------------------------

	def run_gravity_analysis(self):

		# set up parameters for gravity analysis
		tol = 1.e-8                           # convergence tolerance for test
		iter = 100                            # max number of iterations
		nstep = 100                           # apply gravity loads in 100 steps
		incr = 1./nstep                 	  # first load increment

		# set up analysis case
		constraints('Transformation')       # enforce boundary conditions using transformation constraint handler
		numberer('RCM')			            # renumbers dof's to minimize band-width (optimization)
		system('BandGeneral')		        # stores system of equations as 1D array of size bandwidth x number of unknowns
		test('EnergyIncr',tol,iter)   		# tests for convergence using dot product of solution vector and norm of right-hand side of matrix equation
		algorithm('Newton')                 # use Newton's solution algorithm: updates tangent stiffness at every iteration
		integrator('LoadControl', incr)     # determine the next time step for an analysis # apply gravity in 10 steps
		analysis('Static')		            # define type of analysis, static or transient
		# run analysis
		return analyze(nstep)	          	# perform gravity analysis


# FUNCTION: run_eigenvalue_analysis --------------------------------------------
# get building periods from eigenvalue analysis
# ------------------------------------------------------------------------------

	def run_eigenvalue_analysis(self, nEigen):
	    nLambda = eigen('-fullGenLapack', nEigen)

	    Tlist = np.array([])
	    for lamb in nLambda:
	        Ti = 2.*pi/sqrt(lamb)
	        Tlist = np.append(Tlist, Ti)

	    return Tlist



# FUNCTION: run_static_pushover --------------------------------------------
# get building periods from eigenvalue analysis
# ------------------------------------------------------------------------------

	# def run_static_pushover(self):
	# 	print('Starting Static Analysis:')
	# 	timeSeries('Linear', 400)
	# 	pattern('Plain', 444, 400)
	# 	load(15001, 0.184, 0., 0., 0., 0., 0.)
	# 	load(15002, 0.816, 0., 0., 0., 0., 0.)
	# 	tol = 1.e-6
	# 	maxiter = 400
	#
	# 	IDCtrlNode = 15002
	# 	IDCtrlDOF = 1
	# 	Dincr = 0.001
	# 	Dmax = 10.0
	#
	# 	RecorderNode = 15002
	# 	BaseNodes = np.arange(11000, 20000, 1000)
	# 	# recorder('Node', '-file', os.path.join(recorddir,'Pushover_DisplX_Story1.out'), '-time', '-node', 15001,'-dof', 1, 'disp')
	# 	# recorder('Node', '-file', os.path.join(recorddir,'Pushover_DisplX_Story2.out'), '-time', '-node', 15002,'-dof', 1, 'disp')
	# 	#
	# 	# for node in nodes_base:
	# 	# recorder('Node', '-file', os.path.join(recorddir,'Pushover_ReactX_{}.out'.format(node)), '-node', node,'-dof', 1, 'reaction')
	#
	# 	constraints('Transformation')
	# 	numberer('RCM')
	# 	system('UmfPack')
	# 	test('EnergyIncr',tol,maxiter)
	# 	algorithm('KrylovNewton')
	# 	integrator('DisplacementControl', IDCtrlNode, IDCtrlDOF, Dincr)
	# 	analysis('Static')                                                  # define type of analysis: time-independent
	#
	# 	nsteps = int(Dmax/Dincr)
	# 	# ok = analyze(nsteps)                                              # this will return 0 if no convergence issues are encountered
	#
	# 	# record node displacement append base reaction
	# 	dataPush = np.zeros((nsteps,2))
	# 	for i in range(nsteps):
	# 	    analyze(1)                                                      # runs analysis one step at a time
	# 	    dataPush[i,0] = nodeDisp(RecorderNode,1)
	# 	    reactions()
	# 	    for node in BaseNodes:
	# 	        dataPush[i,1] += nodeReaction(int(node),1)                     # sum base node reactions to obtain total base shear
	#
	# 	# flip sign of base reactions to be positive
	# 	dataPush[:,1] = [-a for a in dataPush[:,1]]
	#
	# 	print("Pushover done")
	#
	# 	with open('PushoverXData_{}.txt'.format(RecorderNode),'w') as f:
	# 		for i in range(len(dataPush[:,0])):
	# 			f.write("%.8f %.8f\n" % (dataPush[i,0], dataPush[i,1]))
	#
	# 	# plot pushover curve
	# 	plt.figure()
	# 	plt.plot(dataPush[:,0], dataPush[:,1])
	# 	plt.xlabel('Roof Displacement (in)')
	# 	plt.ylabel('Base Shear (k)')
	# 	plt.title('Static Pushover Curve (Model_EEUQ)')
	# 	# plt.savefig('Figures/{}_Pushover_{}_X.png'.format(ModelNo,recorder_node), bbox_inches='tight')
	# 	plt.show()
	#
	#
	# 	return dataPush
