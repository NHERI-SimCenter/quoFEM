#!/usr/bin/python

# written: adamzs

# import functions for Python 2.X support
from __future__ import division, print_function
import sys
if sys.version.startswith('2'): 
    range=xrange
    string_types = basestring
else:
    string_types = str

import numpy as np

def process_results(response):
	target_data = (np.loadtxt("target.txt")).T
	current_data = (np.loadtxt("node.txt")).T
	
	diff = (current_data - target_data)**2.

	__, val1, val2 = [np.sqrt(np.sum(v)) for v in diff]

	print(val1, val2)

	with open('results.out', 'wb') as f:
		f.write("{:.6f} {:.6f}".format(val1, val2).encode('utf8'))
