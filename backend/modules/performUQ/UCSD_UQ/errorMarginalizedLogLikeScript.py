import numpy as np


def log_likelihood(calibrationData, prediction, parameters, numExperiments, covarianceMatrixList, edpNamesList,
                   edpLengthsList, covarianceMultiplierList, scaleFactors, shiftFactors):
    """ Compute the log-likelihood

    :param calibrationData: Calibration data consisting of the measured values of response. Each row contains the data
    from one experiment. The length of each row equals the sum of the lengths of all response quantities.
    :type calibrationData: numpy ndarray (atleast_2d)

    :param prediction: Prediction of the response from the model, evaluated using the parameter values for
    which the log-likelihood function needs to be calculated.
    :type prediction: numpy ndarray (atleast_2d)

    :param parameters: A sample value of the model parameter vector.
    :type parameters: numpy ndarray

    :param numExperiments: Number of experiments from which data is available, this is equal to the number of rows
    (i.e., the first index) of the calibration data array
    :type numExperiments: int

    :param covarianceMatrixList: A list of length numExperiments * numResponses, where each item in the list contains
    the covariance matrix or variance value corresponding to that experiment and response quantity, i.e., each item in
    the list is a block on the diagonal of the error covariance matrix
    :type covarianceMatrixList: list of numpy ndarrays

    :param edpNamesList: A list containing the names of the response quantities
    :type edpNamesList: list of strings

    :param edpLengthsList: A list containing the length of each response quantity
    :type edpLengthsList: list of ints

    :param covarianceMultiplierList: A list containing multipliers on the default covariance matrices or variance
    values. The length of this list is equal to the number of response quantities. These additional variables, one
    per response quantity, are parameters of the likelihood model whose values are also calibrated.
    :type covarianceMultiplierList: list of floats

    :param scaleFactors: A list containing the normalizing factors used to scale (i.e. divide) the model
    prediction values. The length of this list is equal to the number of response quantities.
    :type scaleFactors: list of ints

    :param shiftFactors: A list containing the values used to shift (i.e. added to) the prediction values. The length of
    this list is equal to the number of response quantities.
    :type shiftFactors: list of ints

    :return: loglikelihood. This is a scalar value, which is equal to the logpdf of a zero-mean multivariate normal
    distribution and a user-supplied covariance structure. Block-diagonal covariance structures are supported. The value
    of multipliers on the covariance block corresponding to each response quantity is also calibrated.
    :rtype: float
    """
    numResponses = len(edpLengthsList)
    # Shift and normalize the prediction
    currentPosition = 0
    for j in range(numResponses):
        prediction[:, currentPosition:currentPosition + edpLengthsList[j]] += shiftFactors[j]
        prediction[:, currentPosition:currentPosition + edpLengthsList[j]] /= scaleFactors[j]
        currentPosition += edpLengthsList[j]

    # Compute the normalized residuals
    allResiduals = prediction - calibrationData

    # Loop over the normalized residuals to compute the log-likelihood
    loglike = 0
    for i in range(numExperiments):
        currentPosition = 0
        for j in range(numResponses):
            # Get the residuals corresponding to this response variable
            length = edpLengthsList[j]
            residuals = allResiduals[i, currentPosition:currentPosition + length]
            currentPosition += length
            ll = -length * np.log(np.sum(residuals ** 2))
            if not np.isnan(ll):
                loglike += ll
            else:
                loglike += -np.inf
    return loglike
