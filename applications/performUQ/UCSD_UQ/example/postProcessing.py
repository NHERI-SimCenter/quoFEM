"""
authors: Mukesh Kumar Ramancha, Maitreya Manoj Kurumbhati, and Prof. J.P. Conte 
affiliation: University of California, San Diego

"""

#import standard python modules
import numpy as np
from generateData import data

#%%

# normalize data (IMPORTANT for TMCMC)
normalizeData = np.mean(data,axis=1)
data = data/normalizeData[:,np.newaxis]

# noise in each channel
sig_channels = np.array([0.05,0.05]) #My x 1,  Ny = number of measurement channels 
sig = np.array([sig_channels[0]*1079.88,sig_channels[1]*1.414]) # array of size: ny
sig = sig/normalizeData

def log_likelihood():
    """ this function computes the log-likelihood for parameter value: par """
     
    # num of data point: N, num of measurement channels: Ny
    N = data.shape[1]
    Ny = data.shape[0]
    
    FEpred = np.loadtxt('results.out')/normalizeData
    
    FEpred = FEpred[:,np.newaxis]

    E = np.diag(sig**2)
    logdetE = np.log(np.linalg.det(E))
    Einv = np.linalg.inv(E)
    # log-likelihood
    LL = -0.5*N*Ny*np.log(2*np.pi) -0.5*N*logdetE +sum(np.diag(-0.5* (data-FEpred).T @ Einv @ (data-FEpred)))
    
    return LL
