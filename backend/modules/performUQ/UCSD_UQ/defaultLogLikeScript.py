import numpy as np


class CovError(Exception):
    """Raised when the number of covariance matrix terms are incorrect.

    Attributes:
        message -- explanation of the error
    """

    def __init__(self, message):
        self.message = message


def log_likelihood(calibrationData, numExperiments, covarianceMatrixList, edpNamesList, edpLengthsList,
                   covarianceMultiplierList, normalizingFactors):
    """ Compute the log-likelihood """
    # Read in the normalized data
    data = calibrationData

    # Check if the correct number of covariance terms has been passed in
    numResponses = len(edpLengthsList)
    if len(covarianceMatrixList) != numExperiments * numResponses:
        print("ERROR: The expected number of covariance matrices is {}, but only {} were passed "
              "in.".format(numExperiments * numResponses, len(covarianceMatrixList)))
        raise CovError("ERROR: The expected number of covariance matrices is {}, but only {} were passed "
                       "in.".format(numExperiments * numResponses, len(covarianceMatrixList)))

    # Read in the model prediction
    prediction = np.genfromtxt('results.out').reshape((1, -1))

    # Normalize the prediction
    currentPosition = 0
    for j in range(len(edpLengthsList)):
        prediction[:, currentPosition:currentPosition + edpLengthsList[j]] /= normalizingFactors[j]
        # normalizedPredictionSlice = prediction[:, currentPosition:currentPosition + edpLengthsList[j]] / \
        #                             normalizingFactors[j]
        # prediction[:, currentPosition:currentPosition + edpLengthsList[j]] = normalizedPredictionSlice
        currentPosition += edpLengthsList[j]

    # Compute the normalized residuals
    allResiduals = data - prediction

    # Loop over the normalized residuals to compute the log-likelihood
    loglike = 0
    covListIndex = 0
    for i in range(numExperiments):
        currentPosition = 0
        for j in range(numResponses):
            # Get the residuals corresponding to this response variable
            # residuals = allResiduals[i, currentPosition:currentPosition + edpLengthsList[j]]
            residuals = allResiduals[i, currentPosition:currentPosition + edpLengthsList[j]].reshape((1, -1))
            currentPosition += edpLengthsList[j]

            # Get the covariance matrix corresponding to this response variable
            cov = covarianceMatrixList[covListIndex]
            covListIndex += 1

            # Multiply the covariance matrix by the value of the covariance multiplier
            cov *= covarianceMultiplierList[j]

            # CASE I & II: Known/Estimate Noise
            # noise covariance matrix
            sig = [np.sqrt(cov)]
            E = np.diag(np.array(sig) ** 2)
            logdetE = np.log(np.linalg.det(E))
            Einv = np.linalg.inv(E)
            N = len(residuals)
            Ny = 1
            LL = -0.5 * N * Ny * np.log(2 * np.pi) - 0.5 * N * logdetE + sum(
                np.diag(-0.5 * residuals.T @ Einv @ residuals))

            if not np.isnan(LL):
                loglike += LL

    return loglike
