import numpy as np
from scipy.stats import multivariate_normal


class CovError(Exception):
    """Raised when the number of covariance matrix terms are incorrect.

    Attributes:
        message -- explanation of the error
    """

    def __init__(self, message):
        self.message = message


def log_likelihood(calibrationData, numExperiments, covarianceMatrixList, edpNamesList, edpLengthsList,
                   covarianceMultiplierList):
    """ Compute the log-likelihood """
    # Read in the data
    data = calibrationData

    # Check if the correct number of covariance terms has been passed in
    numResponses = len(edpNamesList)
    if len(covarianceMatrixList) != numExperiments * numResponses:
        print("ERROR: The expected number of covariance matrices is {}, but only {} were passed "
              "in.".format(numExperiments * numResponses, len(covarianceMatrixList)))
        raise CovError("ERROR: The expected number of covariance matrices is {}, but only {} were passed "
                       "in.".format(numExperiments * numResponses, len(covarianceMatrixList)))

    # Read in the model prediction
    prediction = np.genfromtxt('results.out')

    # Compute the residuals
    allResiduals = data - prediction

    # Loop over the normalized residuals to compute the log-likelihood
    loglike = 0
    covListIndex = 0
    for i in range(numExperiments):
        currentPosition = 0
        for j in range(numResponses):
            # Get the residuals corresponding to this response variable
            residuals = allResiduals[i, currentPosition:currentPosition + edpLengthsList[j]]
            currentPosition += edpLengthsList[j]

            # Get the covariance matrix corresponding to this response variable
            cov = covarianceMatrixList[covListIndex]
            covListIndex += 1

            # Multiply the covariance matrix by the value of the covariance multiplier
            cov *= covarianceMultiplierList[j]

            # Check if there are any infs or nans
            infNanCheck = np.isinf(cov).any() | np.isnan(cov).any() | np.isinf(residuals).any() | np.isnan(
                residuals).any()

            # Compute the log-likelihood and add it to the total log-likelihood
            if infNanCheck:
                print("WARNING: There were infs or nans, ignoring this prediction of the response.\n"
                      "inf in cov: {}, nan in cov: {}, inf in residuals: {}, nan in residuals: {}".format(
                        np.isinf(cov).any(), np.isnan(cov).any(), np.isinf(residuals).any(),
                        np.isnan(residuals).any()))
                loglike += 0
            else:
                loglike += np.sum(multivariate_normal.logpdf(residuals, cov=cov))
    print('logLikelihood: ', loglike)
    return loglike
