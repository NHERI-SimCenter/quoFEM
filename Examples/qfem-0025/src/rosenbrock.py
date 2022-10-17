#!/usr/bin/python

# written: adamzs 06/20
# The global minimum is at (a, a^2)

import sys
import numpy as np
from params import *
import os
def rosenbrock(x, y):
	
	np.random.seed(int(os.getcwd().split("workdir.",1)[1])) # to make this example reroducable. 
	# different working directories should take DIFFERENT random seed.

	a = 1.
	b = 100.
	eps = 50*np.random.normal(loc=0.0, scale=1.0, size=None)
	print(eps)
	return (a - x)**2. + b*(y - x**2.)**2 + eps*(x+2.0)

if __name__ == "__main__":

	Y = 0.0 # deterministic variable
	with open('results.out', 'w') as f:
		result = rosenbrock(X, Y)
		f.write('{:.60g}'.format(result))

