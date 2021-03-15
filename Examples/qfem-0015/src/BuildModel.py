import numpy as np
import os
from ModelInfo import ModelInfo
from math import sqrt
from openseespy.opensees import *



def build_model(model_params):
	G = 386.1;
	# load parameters
	num_diaphragms = model_params["numStories"]		# 1 or 2
	cwTag = model_params["crippleWallTag"]			# O (no) or 1 (yes)
	constrEra = model_params["constructionEra"]		# "pre-1950" or "post-1950"
	extMat = model_params["exteriorMaterial"]		# "Wood Siding" or "Stucco"
	Q = model_params['materialQuality']				# float, [0,1], no units
	planArea = model_params['planArea']*144			# float, [800., 2400.] inch
	aspectRatio = model_params['aspectRatio']		# float, [0.4, 0.8], no units
	damping = model_params['damping']				# float, [0.02, 0.1] percent
	extDensity_x = model_params['extDensity_x']		# float, [0.5, 0.8], no units (where extDensity_x*Perimeter = length of exterior wall on first floor in x direction)
	extDensity_z = model_params['extDensity_z']		# float, [0.5, 0.8], no units (where extDensity_z*Perimeter = length of exterior wall on first floor in z direction)
	rho_int = model_params['intDensity']			# float, [0.06, 0.08], no units (where rho_int*Area =  total length of interior wall on first floor)
	extEcc_x = model_params['extEcc_x']				# float, [0.0, 0.5], no units (offset from CoD to x-direction exterior wall)
	extEcc_z = model_params['extEcc_z']				# float, [0.0, 0.5], no units (offset from CoD to z-direction exterior wall)
	intEcc_x = model_params['intEcc_x']				# float, [0.0, 0.5], no units (offset from CoD to x-direction exterior wall)
	intEcc_z = model_params['intEcc_z']				# float, [0.0, 0.5], no units (offset from CoD to z-direction exterior wall)
	floor1Weight = model_params['floor1Weight']		# float, [10.0, 15.5] psf
	floor2Weight = model_params['floor2Weight']		# float, psf (range dependent on construction era)
	roofWeight = model_params['roofWeight']			# float, psf (range dependent on construction era)


	# floor weights
	floor_weights = np.array([floor1Weight, floor2Weight, roofWeight])/144.

	# plan dimensions
	plan_x_length = sqrt(planArea/aspectRatio) # L
	plan_z_length = aspectRatio*sqrt(planArea/aspectRatio) # W

	# interior wall material (determined by construction era)
	if constrEra == "pre-1950":
		int_mat = 'Plaster'
	elif constrEra == "post-1950":
		int_mat = 'GWB'

	# exterior wall material
	if extMat == "Wood Siding":
		cw_mat = 'CW_Siding'
		ext_mat = 'SS_Siding'
	elif extMat == "Stucco":
		cw_mat = 'CW_Stucco'
		ext_mat = 'Stucco+GWB'

	# wall lengths
	ext_L_x = extDensity_x*plan_x_length
	ext_L_z = extDensity_z*plan_z_length
	ratio_x = 1-extEcc_x
	ratio_z = 1-extEcc_z

	a = 2/(ratio_x + 1) * extDensity_x
	b = 2*ratio_x/(ratio_x + 1) * extDensity_x
	c = 2*ratio_z/(ratio_z + 1) * extDensity_z
	d = 2/(ratio_z + 1) * extDensity_z

	int_L_tot = 2*(rho_int/12)*planArea
	# divide by 12 to keep in units of inches
	# multiply by 2 for double-sided wall


	# create walls
	s = 1	# story index
	story_heights = [0.]
	walls = {}

	# add cripple wall level
	if cwTag == 1:
		story_heights.append(24.)
		s = 2

		# CW level
		walls[1] = {}
		walls[1]['qualityscore'] = Q
		walls[1]['ext_length'] = [plan_x_length, plan_x_length, plan_z_length, plan_z_length]
		walls[1]['ext_material'] = cw_mat
		walls[1]['int_length'] = [1.e-5, 1.e-5]
		walls[1]['int_material'] = int_mat
		walls[1]['height'] = 24. # story_heights[1]-story_heights[0]


	# story 1
	new_height = story_heights[-1] + 108.
	story_heights.append(new_height)

	walls[s] = {}
	walls[s]['qualityscore'] = Q
	walls[s]['ext_length'] = [a*plan_x_length, b*plan_x_length, c*plan_z_length, d*plan_z_length] # [0.625*plan_x_length, 0.625*plan_x_length, 0.5889*plan_z_length, 0.7111*plan_z_length]
	walls[s]['ext_material'] = ext_mat
	walls[s]['int_length'] = [(int_L_tot/(1+1.25)), int_L_tot - (int_L_tot/(1+1.25))] # 2.7*plan_x_length, 2.9*plan_z_length
	walls[s]['int_material'] = int_mat
	walls[s]['height'] = 108. # story_heights[story]-story_heights[story-1]

	# story 2
	if num_diaphragms - cwTag == 2:
		new_height = story_heights[-1] + 108.
		story_heights.append(new_height)

		walls[s+1] = {}
		walls[s+1]['qualityscore'] = Q
		walls[s+1]['ext_length'] = [extDensity_x*plan_x_length, extDensity_x*plan_x_length, extDensity_z*plan_z_length, extDensity_z*plan_z_length]
		walls[s+1]['ext_material'] = ext_mat
		walls[s+1]['int_length'] = [(int_L_tot/(1+1.24)), int_L_tot - (int_L_tot/(1+1.24))]
		walls[s+1]['int_material'] = int_mat
		walls[s+1]['height'] = 108.


	# CHECK
	print("MODEL 7 (New Workflow)")
	print("number of diaphragms: " + str(num_diaphragms))
	print("story heights: " + str(story_heights))
	print("floor_weights: " + str(floor_weights))
	print("plan length: " + str(plan_x_length))
	print("plan width: " + str(plan_z_length))
	print("EXTERIOR WALL COEFFICIENTS: ")
	print(a,b,c,d)
	print("INTERIOR WALL LENGTHS: ")
	print((int_L_tot/(1+1.25)), int_L_tot - (int_L_tot/(1+1.25)))


	# CREATE MODEL
	model('basic', '-ndm', 3, '-ndf', 6)
	geomTransf('PDelta', 1, 0., 0., -1.)


	CWModel = ModelInfo(G, num_diaphragms, story_heights, plan_x_length, plan_z_length, floor_weights, intEcc_x, intEcc_z, walls, damping)
	CWModel.define_nodes()
	CWModel.define_fixities()
	CWModel.define_floor_diaphragms()
	CWModel.define_walls()
	CWModel.define_leaning_columns()
	CWModel.define_mass_and_gravity_loads()
	CWModel.assign_damping()

	# Tlist = CWModel.run_eigenvalue_analysis(3)
	# print("periods: " + str(Tlist))

	CWModel.run_gravity_analysis()
	loadConst('-time', 0.0)             # maintain constant gravity loads and reset time to zero

	# recorddir = "C:/Users/Joanna Zou/Documents/Stanford/Research/Model_EEUQ"
	# CWModel.run_static_pushover()
	#
	# wipeAnalysis()
