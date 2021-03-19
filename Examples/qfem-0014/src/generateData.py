"""
@author: Mukesh, Maitreya, and Conte 

"""

import numpy as np
from scipy.linalg import eigh

#masses
m1 = 0.52 #kips-s2/in
m2 = 0.26 #kips-s2/in

# story stiffness
k1_true = 958.611 #kips/in
k2_true = 958.611 #kips/in

# mass matrix
M = np.array([[m1, 0], [0, m2]])
# stiffness matrix
K = np.array([[k1_true+k2_true, -k2_true], [-k2_true, k2_true]])

# solve generalized eigenvalue problem
eigenValues , eigenVectors = eigh(K, M, eigvals_only=False)
for i in range(len(eigenValues)):
    eigenVectors[:,i] = eigenVectors[:,i] / eigenVectors[0,i]

# true values
TrueValue1 = eigenValues[0]
TrueValue2 = eigenVectors[1,0]

# simulate data using true values
np.random.seed(123)
data1 = TrueValue1 + 0.05*TrueValue1*np.random.randn(5) #5% gaussian noise
data2 = TrueValue2 + 0.05*TrueValue2*np.random.randn(5) #5% gaussian noise

data = np.vstack((data1,data2))