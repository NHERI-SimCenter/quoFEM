### defines library of material parameters for cripple wall house model
import numpy as np
from math import pi, sqrt
from openseespy.opensees import *
import os



# FUNCTION: MaterialLibrary ----------------------------------------------------
# generates dictionary of 2-spring pinching material parameters
# ------------------------------------------------------------------------------

def _MaterialLibrary(Q):
	MaterialLib = {'CW_Stucco':{}, 'Stucco+GWB':{}, 'GWB':{}}

	# CRIPPLE WALL: Stucco
	# Wall Weights [psf]
	MaterialLib['CW_Stucco']['w_ext'] = 17/144. # psi
	MaterialLib['CW_Stucco']['w_int'] = 7/144.
	# MaterialLib['CW_Stucco']['w_cw'] = 14/144.

	# Force vs. Drift Backbone [%]
	# Quality score corresponds to: Poor (Q=0.0), Average (Q=0.5), Good (Q=1.0)
	# ordinates for 3 backbone points listed in order of poor, average, good
	d1 = [0.08, 0.100, 0.100]
	f1 = [123.0, 223.0, 307.0]
	d2 = [0.52, 0.772, 0.753]
	f2 = [315.0, 498.0, 612.0]
	d3 = [1.10, 1.60, 2.80]
	f3 = [366.0, 539.0, 732.0]
	d4 = [5.5, 6.7, 6.69]
	f4 = [110., 189., 315.]

	MaterialLib['CW_Stucco']['ePd1'] = Q*(d1[2]-d1[0])+d1[0]
	MaterialLib['CW_Stucco']['ePf1'] = ((f1[2]-f1[0])/(d1[2]-d1[0]))*(Q*(d1[2]-d1[0]))+f1[0]

	MaterialLib['CW_Stucco']['ePd2'] = Q*(d2[2]-d2[0])+d2[0]
	MaterialLib['CW_Stucco']['ePf2'] = ((f2[2]-f2[0])/(d2[2]-d2[0]))*(Q*(d2[2]-d2[0]))+f2[0]

	MaterialLib['CW_Stucco']['ePd3'] = Q*(d3[2]-d3[0])+d3[0]
	MaterialLib['CW_Stucco']['ePf3'] = ((f3[2]-f3[0])/(d3[2]-d3[0]))*(Q*(d3[2]-d3[0]))+f3[0]

	MaterialLib['CW_Stucco']['ePd4'] = Q*(d4[2]-d4[0])+d4[0]
	MaterialLib['CW_Stucco']['ePf4'] = ((f4[2]-f4[0])/(d4[2]-d4[0]))*(Q*(d4[2]-d4[0]))+f4[0]

	# Parallel Spring Factors (force only)
	MaterialLib['CW_Stucco']['a'] = 0.8
	MaterialLib['CW_Stucco']['b'] = .75
	MaterialLib['CW_Stucco']['c'] = 0.3
	MaterialLib['CW_Stucco']['d'] = 0.1

	# Cyclic Properties
	# Spring1 (low displacement)
	MaterialLib['CW_Stucco']['rDispP1'] = 0.05 # 0.06
	MaterialLib['CW_Stucco']['rForceP1'] = 0.2 # 0.18
	MaterialLib['CW_Stucco']['uForceP1'] = -0.1 # -0.07
	MaterialLib['CW_Stucco']['gD11'] = 0.3 # 0.15
	MaterialLib['CW_Stucco']['gDlim1'] = 3.0 # 2.0
	MaterialLib['CW_Stucco']['gK11'] = 0.0 # -0.25
	MaterialLib['CW_Stucco']['gKlim1'] = 0.0 # -1.0
	# Spring 2
	MaterialLib['CW_Stucco']['rDispP2'] = 0.25 # 0.37
	MaterialLib['CW_Stucco']['rForceP2'] = 0.16 # 0.20
	MaterialLib['CW_Stucco']['uForceP2'] = -0.1 # 0.08
	MaterialLib['CW_Stucco']['gD12'] = 0.2 # 0.12
	MaterialLib['CW_Stucco']['gDlim2'] = 0.2 # 0.40
	MaterialLib['CW_Stucco']['gK12'] = 0.0 # 0.12
	MaterialLib['CW_Stucco']['gKlim2'] = 0.0 # 0.50


	# SUPERSTRUCTURE: Avg Stucco + GWB
	# Wall Weights [psf]
	MaterialLib['Stucco+GWB']['w_ext'] = 17/144.
	MaterialLib['Stucco+GWB']['w_int'] = 7/144.
	MaterialLib['Stucco+GWB']['w_cw'] = 14/144.
	# Drift Backbone [%]
	MaterialLib['Stucco+GWB']['ePd1'] = 0.100
	MaterialLib['Stucco+GWB']['ePd2'] = 0.560
	MaterialLib['Stucco+GWB']['ePd3'] = 1.800
	MaterialLib['Stucco+GWB']['ePd4'] = 6.500
	# Force Backbone [plf]
	MaterialLib['Stucco+GWB']['ePf1'] = 350.
	MaterialLib['Stucco+GWB']['ePf2'] = 852.
	MaterialLib['Stucco+GWB']['ePf3'] = 956.
	MaterialLib['Stucco+GWB']['ePf4'] = 287.
	# Parallel Spring Factors (force only)
	MaterialLib['Stucco+GWB']['a'] = 0.8
	MaterialLib['Stucco+GWB']['b'] = 0.75
	MaterialLib['Stucco+GWB']['c'] = 0.3
	MaterialLib['Stucco+GWB']['d'] = 0.1
	# Cyclic Properties
	# Spring1 (low displacement)
	MaterialLib['Stucco+GWB']['rDispP1'] = 0.06
	MaterialLib['Stucco+GWB']['rForceP1'] = 0.26
	MaterialLib['Stucco+GWB']['uForceP1'] = -0.2
	MaterialLib['Stucco+GWB']['gD11'] = 0.13
	MaterialLib['Stucco+GWB']['gDlim1'] = 2.0
	MaterialLib['Stucco+GWB']['gK11'] = 0.0
	MaterialLib['Stucco+GWB']['gKlim1'] = 0.0
	# Spring 2
	MaterialLib['Stucco+GWB']['rDispP2'] = 0.06
	MaterialLib['Stucco+GWB']['rForceP2'] = 0.17
	MaterialLib['Stucco+GWB']['uForceP2'] = -0.20
	MaterialLib['Stucco+GWB']['gD12'] = 0.14
	MaterialLib['Stucco+GWB']['gDlim2'] = 2.0
	MaterialLib['Stucco+GWB']['gK12'] = 0.3
	MaterialLib['Stucco+GWB']['gKlim2'] = 2.0


	# INTERIOR: Gypsum Base Properties
	# Wall Weights [psf]
	MaterialLib['GWB']['w_ext'] = 7/144.
	MaterialLib['GWB']['w_int'] = 7/144.
	MaterialLib['GWB']['w_cw'] = 4/144.
	# Drift Backbone [%]
	MaterialLib['GWB']['ePd1'] = 0.1173
	MaterialLib['GWB']['ePd2'] = 0.3605
	MaterialLib['GWB']['ePd3'] = 0.8000
	MaterialLib['GWB']['ePd4'] = 5.6490
	# Force Backbone [plf]
	MaterialLib['GWB']['ePf1'] = 105.
	MaterialLib['GWB']['ePf2'] = 185.
	MaterialLib['GWB']['ePf3'] = 210.
	MaterialLib['GWB']['ePf4'] = 63.
	# Parallel Spring Factors (force only)
	MaterialLib['GWB']['a'] = 0.8
	MaterialLib['GWB']['b'] = 0.75
	MaterialLib['GWB']['c'] = 0.3
	MaterialLib['GWB']['d'] = 0.1
	# Cyclic Properties
	# Spring1 (low displacement)
	MaterialLib['GWB']['rDispP1'] = 0.15
	MaterialLib['GWB']['rForceP1'] = 0.22
	MaterialLib['GWB']['uForceP1'] = -0.21
	MaterialLib['GWB']['gD11'] = 0.10
	MaterialLib['GWB']['gDlim1'] = 2.0
	MaterialLib['GWB']['gK11'] = 0.0
	MaterialLib['GWB']['gKlim1'] = 0.0
	# Spring 2
	MaterialLib['GWB']['rDispP2'] = 0.40
	MaterialLib['GWB']['rForceP2'] = 0.12
	MaterialLib['GWB']['uForceP2'] = -0.19
	MaterialLib['GWB']['gD12'] = 0.12
	MaterialLib['GWB']['gDlim2'] = 2.0
	MaterialLib['GWB']['gK12'] = 0.2
	MaterialLib['GWB']['gKlim2'] = 2.0

	return MaterialLib



# FUNCTION: UniaxialPinching ---------------------------------------------------
# defines material parameters for pinched force-deformation response with cyclic degradation, same behavior in positive/negative envelopes
# ------------------------------------------------------------------------------

def UniaxialPinching(matTag, ePf1, ePd1, ePf2, ePd2, ePf3, ePd3, ePf4, ePd4, rDispP, rForceP, uForceP, gK1, gKlim, gD1, gDlim, wallLength, wallHeight):
    # inputs:
    # ePf1, ePf2, ePf3, ePf4 = floating point values defining force points on the positive response envelope [lb]
    # ePd1, ePd2, ePd3, ePd4 = floating point values defining deformation points on the positive response envelope [ft?]
    # rDispP = floating point value defining ratio of the deformation at which reloading occurs to the maximum historic deformation demand
    # rForceP = floating point value defining ratio of the force at which reloading begins to force corresponding to the maximum historic deformation demand
    # uForceP = floating point value defining the ratio of strength developed upon unloading from negative load to the maximum strength developed under monotonic loading
    # gK1, gKlim = floating point values controlling cyclic degradation model for unloading stiffness degradation
    # gD1, gDlim = floating point values controlling cyclic degradation model for reloading stiffness degradation
    # wallLength = length of wall [in]
    # wallHeight = height of wall [in]

    wallLength = wallLength/12.  # convert to ft.
    drift = wallHeight/100.      # equivalent to 1% drift displacement
    F_factor = wallLength;       # factor accounting for wall length
    D_factor = drift;            # factor to change % drift inputs to inches based on wall height

    # backbone forces [kip]
    ePf1 = ePf1*F_factor/1000.0
    ePf2 = ePf2*F_factor/1000.0
    ePf3 = ePf3*F_factor/1000.0
    ePf4 = ePf4*F_factor/1000.0

    # backbone displacements [in]
    ePd1 = ePd1*D_factor
    ePd2 = ePd2*D_factor
    ePd3 = ePd3*D_factor
    ePd4 = ePd4*D_factor

    gK = 0.
    gD = 0.
    gF = 0.
    gFlim = 0.
    gE = 100.0
    dmgType = 'energy' # damage type


    uniaxialMaterial('Pinching4',matTag, ePf1, ePd1, ePf2, ePd2, ePf3, ePd3, ePf4, ePd4, -ePf1, -ePd1, -ePf2, -ePd2, -ePf3, -ePd3, -ePf4, -ePd4,
    rDispP, rForceP, uForceP, rDispP, rForceP, uForceP, gK1, gK, gK, gK, gKlim, gD1, gD, gD, gD, gDlim, gF, gF, gF, gF, gFlim, gE, dmgType)

    print('Uniaxial Pinching material #' + str(matTag) + ' defined')



# FUNCTION: CreateWallMaterial -------------------------------------------------
# creates 2-spring pinching material for single wall element
# ------------------------------------------------------------------------------

def create_wall_material(matName, matTag, Leff, h, Q):
	# inputs:
	# matNames = string of name of material in dictionary
	# baseID = starting number for material tag
	# Leff = effective lengths of wall
	# h = height of wall

	# call on material library of 2-spring pinching materials
	ML = _MaterialLibrary(Q)
	# extract parameter values from library
	ePd1 = ML[matName]['ePd1']
	ePd2 = ML[matName]['ePd2']
	ePd3 = ML[matName]['ePd3']
	ePd4 = ML[matName]['ePd4']

	ePf1 = ML[matName]['ePf1']
	ePf2 = ML[matName]['ePf2']
	ePf3 = ML[matName]['ePf3']
	ePf4 = ML[matName]['ePf4']

	a = ML[matName]['a']
	b = ML[matName]['b']
	c = ML[matName]['c']
	d = ML[matName]['d']

	rDispP1 = ML[matName]['rDispP1']
	rForceP1 = ML[matName]['rForceP1']
	uForceP1 = ML[matName]['uForceP1']
	gD11 = ML[matName]['gD11']
	gDlim1 = ML[matName]['gDlim1']
	gK11 = ML[matName]['gK11']
	gKlim1 = ML[matName]['gKlim1']

	rDispP2 = ML[matName]['rDispP2']
	rForceP2 = ML[matName]['rForceP2']
	uForceP2 = ML[matName]['uForceP2']
	gD12 = ML[matName]['gD12']
	gDlim2 = ML[matName]['gDlim2']
	gK12 = ML[matName]['gK12']
	gKlim2 = ML[matName]['gKlim2']

	spring1ID = matTag + 10000
	spring2ID = matTag + 20000

	# create Spring 1
	UniaxialPinching(spring1ID, ePf1*a, ePd1, ePf2*b, ePd2, ePf3*c, ePd3, ePf4*d, ePd4,
	rDispP1, rForceP1, uForceP1, gK11, gKlim1, gD11, gDlim1, Leff, h)

	# check inputs
	spring1_inputs = [spring1ID, ePf1*a, ePd1, ePf2*b, ePd2, ePf3*c, ePd3, ePf4*d, ePd4,
	rDispP1, rForceP1, uForceP1, gK11, gKlim1, gD11, gDlim1, Leff, h]
	print(spring1_inputs)

	# create Spring 2
	UniaxialPinching(spring2ID, ePf1*(1-a), ePd1, ePf2*(1-b), ePd2, ePf3*(1-c), ePd3, ePf4*(1-d), ePd4,
	rDispP2, rForceP2, uForceP2, gK12, gKlim2, gD12, gDlim2, Leff, h)

	# check inputs
	spring2_inputs = [spring2ID, ePf1*(1-a), ePd1, ePf2*(1-b), ePd2, ePf3*(1-c), ePd3, ePf4*(1-d), ePd4,
	rDispP2, rForceP2, uForceP2, gK12, gKlim2, gD12, gDlim2, Leff, h]
	print(spring2_inputs)

	# Combined Parallel Material
	uniaxialMaterial('Parallel', matTag, spring1ID, spring2ID)
	# uniaxialMaterial('Elastic', matTag, 341.11)
	# uniaxialMaterial('ElasticBilin', matTag, 341.11, 0., 0.01)

	print(str(matName) + ' walls created (material ID #' + str(matTag) + ')')



# FUNCTION: CreateTrussMaterial ------------------------------------------------
# define stiff/soft uniaxial materials for the truss material
#-------------------------------------------------------------------------------

def create_truss_material():
	LargeNumber = 1.e9
	SmallNumber = 1.e-12
	StiffMat = 1200
	uniaxialMaterial('Elastic', StiffMat, LargeNumber)
	SoftMat = 1300
	uniaxialMaterial('Elastic', SoftMat, SmallNumber)

	return StiffMat, SoftMat



# FUNCTION: material_weight ----------------------------------------------------
# getter function for dictionary of material weights
#-------------------------------------------------------------------------------

def material_weight(matName, walltype, Q):
	ML = _MaterialLibrary(Q)
	return ML[matName][walltype]
