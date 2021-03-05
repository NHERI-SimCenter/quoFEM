#!/usr/bin/python

# written: fmk, adamzs 06/20
# units kN & mm

import sys
import openseespy.opensees as ops

ops.wipe()

from params import *

def run_analysis():

	# build the model

	ops.model('basic', '-ndm', 2, '-ndf', 2)

	ops.node(1,     0,    0)
	ops.node(2,  4000,    0)
	ops.node(3,  8000,    0)
	ops.node(4, 12000,    0)
	ops.node(5,  4000, 4000)
	ops.node(6,  8000, 4000)

	ops.fix(1, 1, 1)
	ops.fix(4, 0, 1)

	ops.uniaxialMaterial('Elastic', 1, E)

	ops.element('truss', 1, 1, 2, Ao, 1)
	ops.element('truss', 2, 2, 3, Ao, 1)
	ops.element('truss', 3, 3, 4, Ao, 1)
	ops.element('truss', 4, 1, 5, Au, 1)
	ops.element('truss', 5, 5, 6, Au, 1)
	ops.element('truss', 6, 6, 4, Au, 1)
	ops.element('truss', 7, 2, 5, Ao, 1)
	ops.element('truss', 8, 3, 6, Ao, 1)
	ops.element('truss', 9, 5, 3, Ao, 1)

	ops.timeSeries('Linear', 1)
	ops.pattern('Plain', 1, 1)

	ops.load(2, 0, -P)
	ops.load(3, 0, -P)

	# build and perform the analysis

	ops.algorithm('Linear')
	ops.integrator('LoadControl', 1.0)
	ops.system('ProfileSPD')
	ops.numberer('RCM')
	ops.constraints('Plain')
	ops.analysis('Static')
	ops.analyze(1)

	node_disp = [[ops.nodeDisp(node_i, dof_j) 
	             for dof_j in [1,2]] for node_i in range(1, 7)]

	return node_disp

def process_results(responses, node_disp):

	# identify the responses of interest
	nodes = [int(r.split('_')[1]) for r in responses]
	dofs = [int(r.split('_')[3]) if len(r.split('_'))>2 else 1 
	        for r in responses]

	# get the results
	results = []
	for n_i, d_i in zip(nodes, dofs):
		try:
			results.append(str(node_disp[n_i-1][d_i-1]))
		except:
			results.append('0.0')

	# save the results
	with open('results.out', 'w') as f:
		f.write(' '.join(results))

if __name__ == "__main__":

    node_disp = run_analysis()

    process_results(sys.argv[1:], node_disp)










