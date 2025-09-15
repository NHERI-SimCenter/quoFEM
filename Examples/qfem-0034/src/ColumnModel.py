#!/usr/bin/python

# written: galvisf 02/22
# units kN & m

import sys
import openseespy.opensees as ops

ops.wipe()

from ColumnParams import *

def run_analysis():
	# build the model
	ops.wipe()
	ops.model('basic', '-ndm', 2)

	# Geometry
	I = 0.4 ** 4 / 12
	Icr  = crack_factor*I
	A = 0.4 * 0.4
	L = 1.60

	# Contant model parameters
	Ec = 30000000
	du = 0.10
	dr = 0.20
	MrMp = 0.1
	P = 744

	# Plastic hinge backbones
	matTagHinge = 1
	n = 10
	Kspr = n * (3 * Ec * Icr / L)
	theta_y = Mp / Kspr
	pinchX = 1
	pinchY = 1
	damage1 = 0
	damage2 = 0
	ops.uniaxialMaterial('Hysteretic', matTagHinge, Mp, theta_y, Mc, du, Mp*MrMp, dr, -Mp, -theta_y, -Mc, -du, -Mp*MrMp,
					 -dr, pinchX, pinchY, damage1, damage2)
	matTagRig = 2
	ops.uniaxialMaterial('Elastic', matTagRig, 1000 * Ec)

	# Create nodes
	nodeTag1 = 1
	nodeTag2 = 2
	nodeTag3 = 3
	ops.node(nodeTag1, 0, 0)
	ops.node(nodeTag2, 0, 0)
	ops.node(nodeTag3, 0, L)

	# Add boundary conditions
	ops.fix(nodeTag1, 1, 1, 1)

	# Secondary effects
	transfTag = 1
	coordTransf = "PDelta"  # Linear, PDelta, Corotational
	ops.geomTransf(coordTransf, transfTag)

	# Create spring elements (only rotational)
	sprTag1 = 1
	ops.element('zeroLength', sprTag1, nodeTag1, nodeTag2, '-mat', matTagRig, matTagRig, matTagHinge, '-dir', 1, 2, 3)

	# Create elastic element
	eleTag = 2
	ops.element('elasticBeamColumn', eleTag, nodeTag2, nodeTag3, A, 1.1 * Ec, Icr, transfTag)

	# Apply gravity load
	ops.timeSeries('Linear', 1)
	ops.pattern('Plain', 1, 1)
	# Create nodal loads at nodes 2
	#    nd  FX,  FY, MZ
	ops.load(nodeTag3, 0.0, -P, 0.0)

	ops.system("BandGeneral")
	ops.test("RelativeEnergyIncr", 1.0e-5, 20)
	ops.numberer("Plain")
	ops.constraints("Plain")
	ops.algorithm("Newton")
	ops.integrator("LoadControl", 0.1)
	ops.analysis("Static")
	ops.analyze(10)
	ops.loadConst("-time", 0.0)

	# Define lateral load
	F = 1
	ops.timeSeries('Linear', 2)
	ops.pattern('Plain', 100, 2)
	# Create nodal loads at nodes 2
	#    nd  FX,  FY, MZ
	ops.load(nodeTag3, F, 0.0, 0.0)

	# Define displacement time history
	DisplacementStep = []
	with open('experimentDisp.csv') as f:
		for line in f:
			DisplacementStep.append(float(line))  # m

	# Specify control nodes
	IDctrlNode = nodeTag3
	IDctrlDOF = 1
	n_steps = 20

	# ------------------------------------------------------------------------------
	# Main analysis - performed as a number of analyses equal to the number of steps
	# ------------------------------------------------------------------------------
	# system("SparseGeneral","-piv")
	# test("NormUnbalance", 1.0e-6, 1000, 4)
	ops.system("UmfPack")
	ops.test("EnergyIncr", 1.0e-6, 50, 0)
	ops.numberer("RCM")
	ops.constraints("Plain")
	ops.algorithm("NewtonLineSearch")

	Vcyc = []
	D0 = 0.0
	for Dstep in DisplacementStep:
		D1 = Dstep
		Dincr = (D1 - D0) / n_steps
		ops.integrator("DisplacementControl", IDctrlNode, IDctrlDOF, Dincr)
		ops.analysis("Static")
		ok = ops.analyze(n_steps)
		# ----------------------------------------------if convergence failure-------------------------
		D0 = D1  # move to next step
		if ok != 0:
			print("Analysis failed at {} step.".format(Dstep))
			break
		else:
			Vcyc.append(ops.eleForce(eleTag, 4))

	return Vcyc

if __name__ == "__main__":
	force = run_analysis()

	# save the results
	results = []
	for i in range(len(force)):
		results.append(str(force[i]))

	with open('results.out', 'w') as f:
		for i in range(len(force)):
			f.write(results[i])
			if i < len(force):
				f.write('\n')