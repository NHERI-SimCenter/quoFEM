#!/usr/bin/python

# written: adamzs 06/20
# The global minimum is at (a, a^2)

import sys

from RosenParams import *

def rosenbrock(x, y):
	a = 1.
	b = 100.
	return (a - x)**2. + b*(y - x**2.)**2.

if __name__ == "__main__":
	with open('results.out', 'w') as f:
		result = rosenbrock(X, Y)
		f.write('{:.60g}'.format(result))

