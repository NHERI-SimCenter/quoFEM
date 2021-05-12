import numpy as np
from scipy.stats import multivariate_normal


class CovError(Exception):
    """Raised when the number of covariance matrix terms are incorrect.

    Attributes:
        message -- explanation of the error
    """

    def __init__(self, message):
        self.message = message


def log_likelihood(dataFile, numExperiments, covarianceMatrixList, edpNamesList, edpLengthsList, covarianceTypeList,
                   covarianceMultiplierList):
    """ Compute the log-likelihood """
    # Read in the data
    data = np.genfromtxt(dataFile)

    # Check if the correct number of covariance terms has been passed in
    numResponses = len(edpNamesList)
    if len(covarianceMatrixList) != numExperiments * numResponses:
        raise CovError("ERROR: The expected number of covariance matrices is {}, but only {} were passed "
                       "in.".format(numExperiments * numResponses, len(covarianceMatrixList)))

    # Read in the model prediction
    prediction = np.genfromtxt('results.out')

    # Loop over the calibration data to compute the log-likelihood
    loglike = 0
    covListIndex = 0
    for i in range(numExperiments):
        currentPosition = 0
        for j in range(numResponses):
            # Get the data corresponding to this response variable
            currentData = data[i, currentPosition:currentPosition + edpLengthsList[j]]
            currentPrediction = prediction[currentPosition:currentPosition + edpLengthsList[j]]
            # Compute the residuals
            residuals = currentData - currentPrediction
            # Get the covariance matrix corresponding to this response variable
            cov = covarianceMatrixList[covListIndex]
            covListIndex += 1
            # Multiply the covariance matrix by the value of the covariance multiplier
            cov *= covarianceMultiplierList[j]
            # Compute the log-likelihood and add it to the total log-likelihood
            loglike += multivariate_normal.logpdf(residuals, cov=cov)

    return loglike
