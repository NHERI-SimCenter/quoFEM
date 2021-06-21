"""
authors: Mukesh Kumar Ramancha, Maitreya Manoj Kurumbhati, and Prof. J.P. Conte 
affiliation: University of California, San Diego

"""

import numpy as np
import tmcmcFunctions
import multiprocessing as mp
from multiprocessing import Pool
from runFEM import runFEM
from numpy.random import SeedSequence, default_rng
import os
import csv


def RunTMCMC(N, AllPars, Nm_steps_max, Nm_steps_maxmax, log_likelihood, variables, resultsLocation, seed,
             calibrationData, numExperiments, covarianceMatrixList, edpNamesList, edpLengthsList, normalizingFactors,
             locShiftList, run_type, logFile):
    """ Runs TMCMC Algorithm """

    # Initialize (beta, effective sample size)
    beta = 0
    ESS = N
    mytrace = []

    totalNumberOfModelEvaluations = N

    # Initialize other TMCMC variables
    Nm_steps = Nm_steps_max
    parallelize_MCMC = 'yes'  # yes or no
    # parallelize_MCMC = 'no'  # yes or no
    Adap_calc_Nsteps = 'yes'  # yes or no
    Adap_scale_cov = 'yes'  # yes or no
    scalem = 1  # cov scale factor
    evidence = 1  # model evidence
    stageNum = 0  # stage number of TMCMC

    logFile.write('\n\n\t\t==========================')
    logFile.write("\n\t\tStage number: {}".format(stageNum))
    logFile.write("\n\t\tSampling from prior")
    logFile.write("\n\t\tbeta = 0")
    logFile.write("\n\t\tESS = %d" % ESS)
    logFile.write("\n\t\tscalem = %.2f" % scalem)
    logFile.write("\n\n\t\tNumber of model evaluations in this stage: {}".format(N))
    logFile.write("\n\t\tTotal number of model evaluations so far: {}".format(totalNumberOfModelEvaluations))
    logFile.write('\n\t\t==========================')
    logFile.flush()
    os.fsync(logFile.fileno())

    # initial samples
    Sm = tmcmcFunctions.initial_population(N, AllPars)

    # Evaluate posterior at Sm
    Priorm = np.array([tmcmcFunctions.log_prior(s, AllPars) for s in Sm]).squeeze()
    Postm = Priorm  # prior = post for beta = 0

    # Evaluate log-likelihood at current samples Sm
    if parallelize_MCMC == 'yes':
        if run_type == "runningLocal":
            pool = Pool(processes=mp.cpu_count())
            Lmt = pool.starmap(runFEM, [(ind, Sm[ind], variables, resultsLocation, log_likelihood, calibrationData,
                                         numExperiments, covarianceMatrixList, edpNamesList, edpLengthsList,
                                         normalizingFactors, locShiftList) for ind in range(N)], )
            # pool.close()
        else:
            logFile.write("\n\n\nRunning remote")
            logFile.write("\nmax_workers: {}".format(os.cpu_count() - 1))
            from mpi4py.futures import MPIPoolExecutor
            executor = MPIPoolExecutor(max_workers=os.cpu_count() - 1)
            iterables = [(ind, Sm[ind], variables, resultsLocation, log_likelihood, calibrationData,
                          numExperiments, covarianceMatrixList, edpNamesList, edpLengthsList,
                          normalizingFactors, locShiftList) for ind in range(N)]
            Lmt = list(executor.starmap(runFEM, iterables))
            executor.shutdown()
        Lm = np.array(Lmt).squeeze()
    else:
        Lm = np.array([runFEM(ind, Sm[ind], variables, resultsLocation, log_likelihood, calibrationData,
                              numExperiments, covarianceMatrixList, edpNamesList, edpLengthsList, normalizingFactors,
                              locShiftList)
                       for ind in range(N)]).squeeze()

    while beta < 1:
        # adaptively compute beta s.t. ESS = N/2 or ESS = 0.95*prev_ESS
        # plausible weights of Sm corresponding to new beta
        beta, Wm, ESS = tmcmcFunctions.compute_beta(beta, Lm, ESS, threshold=0.95)
        # beta, Wm, ESS = tmcmcFunctions.compute_beta(beta, Lm, ESS, threshold=0.5)

        stageNum += 1

        # seed to reproduce results
        ss = SeedSequence(seed)
        child_seeds = ss.spawn(N + 1)

        # update model evidence
        evidence = evidence * (sum(Wm) / N)

        # Calculate covariance matrix using Wm_n
        Cm = np.cov(Sm, aweights=Wm / sum(Wm), rowvar=False)
        # logFile.write("\nCovariance matrix: {}".format(Cm))

        # Resample ###################################################
        # Resampling using plausible weights
        # SmcapIDs = np.random.choice(range(N), N, p=Wm / sum(Wm))
        rng = default_rng(child_seeds[-1])
        SmcapIDs = rng.choice(range(N), N, p=Wm / sum(Wm))
        # SmcapIDs = resampling.stratified_resample(Wm_n)
        Smcap = Sm[SmcapIDs]
        Lmcap = Lm[SmcapIDs]
        Postmcap = Postm[SmcapIDs]

        # save to trace
        # stage m: samples, likelihood, weights, next stage ESS, next stage beta, resampled samples
        mytrace.append([Sm, Lm, Wm, ESS, beta, Smcap])

        # Write Data to '.csv' files
        dataToWrite = mytrace[stageNum - 1][0]
        logFile.write("\n\n\t\tWriting samples from stage {} to csv file".format(stageNum - 1))

        stringToAppend = 'resultsStage{}.csv'.format(stageNum - 1)
        resultsFilePath = os.path.join(os.path.abspath(resultsLocation), stringToAppend)

        with open(resultsFilePath, 'w', newline='') as csvfile:
            csvWriter = csv.writer(csvfile)
            csvWriter.writerows(dataToWrite)
        logFile.write("\n\t\t\tWrote to file {}".format(resultsFilePath))
        # Finished writing data

        logFile.write('\n\n\t\t==========================')
        logFile.write("\n\t\tStage number: {}".format(stageNum))
        if beta < 1e-7:
            logFile.write("\n\t\tbeta = %9.6e" % beta)
        else:
            logFile.write("\n\t\tbeta = %9.8f" % beta)
        logFile.write("\n\t\tESS = %d" % ESS)
        logFile.write("\n\t\tscalem = %.2f" % scalem)

        logFile.flush()
        os.fsync(logFile.fileno())

        # Perturb ###################################################
        # perform MCMC starting at each Smcap (total: N) for Nm_steps
        Em = (scalem ** 2) * Cm  # Proposal dist covariance matrix

        numProposals = N * Nm_steps
        numAccepts = 0

        totalNumberOfModelEvaluations += numProposals

        if parallelize_MCMC == 'yes':
            if run_type == "runningLocal":
                # pool = Pool(processes=mp.cpu_count())
                results = pool.starmap(tmcmcFunctions.MCMC_MH,
                                       [(j1, Em, Nm_steps, Smcap[j1], Lmcap[j1], Postmcap[j1], beta,
                                         numAccepts, AllPars, log_likelihood, variables,
                                         resultsLocation, default_rng(child_seeds[j1]),
                                         calibrationData, numExperiments, covarianceMatrixList,
                                         edpNamesList, edpLengthsList, normalizingFactors,
                                         locShiftList)
                                        for j1 in range(N)], )
                # pool.close()
            else:
                logFile.write("\n\n\nRunning remote - MCMC steps")
                logFile.write("\nmax_workers: {}".format(os.cpu_count() - 1))
                from mpi4py.futures import MPIPoolExecutor
                executor = MPIPoolExecutor(max_workers=os.cpu_count() - 1)
                iterables = [(j1, Em, Nm_steps, Smcap[j1], Lmcap[j1], Postmcap[j1], beta,
                              numAccepts, AllPars, log_likelihood, variables,
                              resultsLocation, default_rng(child_seeds[j1]),
                              calibrationData, numExperiments, covarianceMatrixList,
                              edpNamesList, edpLengthsList, normalizingFactors,
                              locShiftList)
                             for j1 in range(N)]
                results = list(executor.starmap(tmcmcFunctions.MCMC_MH, iterables))
                executor.shutdown()
        else:
            results = [
                tmcmcFunctions.MCMC_MH(j1, Em, Nm_steps, Smcap[j1], Lmcap[j1], Postmcap[j1], beta, numAccepts, AllPars,
                                       log_likelihood, variables, resultsLocation, default_rng(child_seeds[j1]),
                                       calibrationData, numExperiments, covarianceMatrixList,
                                       edpNamesList, edpLengthsList, normalizingFactors, locShiftList) for j1 in
                range(N)]

        Sm1, Lm1, Postm1, numAcceptsS, all_proposals, all_PLP = zip(*results)
        Sm1 = np.asarray(Sm1)
        Lm1 = np.asarray(Lm1)
        Postm1 = np.asarray(Postm1)
        numAcceptsS = np.asarray(numAcceptsS)
        numAccepts = sum(numAcceptsS)
        all_proposals = np.asarray(all_proposals)
        all_PLP = np.asarray(all_PLP)

        logFile.write("\n\n\t\tNumber of model evaluations in this stage: {}".format(numProposals))
        logFile.write("\n\t\tTotal number of model evaluations so far: {}".format(totalNumberOfModelEvaluations))

        # total observed acceptance rate
        R = numAccepts / numProposals
        if R < 1e-5:
            logFile.write("\n\n\t\tacceptance rate = %9.5e" % R)
        else:
            logFile.write("\n\n\t\tacceptance rate = %.6f" % R)

        # Calculate Nm_steps based on observed acceptance rate
        if Adap_calc_Nsteps == 'yes':
            # increase max Nmcmc with stage number
            Nm_steps_max = min(Nm_steps_max + 1, Nm_steps_maxmax)
            logFile.write("\n\t\tadapted max MCMC steps = %d" % Nm_steps_max)

            acc_rate = max(1. / numProposals, R)
            Nm_steps = min(Nm_steps_max, 1 + int(np.log(1 - 0.99) / np.log(1 - acc_rate)))
            logFile.write("\n\t\tnext MCMC Nsteps = %d" % Nm_steps)

        logFile.write('\n\t\t==========================')

        # scale factor based on observed acceptance ratio
        if Adap_scale_cov == 'yes':
            scalem = (1 / 9) + ((8 / 9) * R)

        # for next beta
        Sm, Postm, Lm = Sm1, Postm1, Lm1

    # save to trace
    mytrace.append([Sm, Lm, np.ones(len(Wm)), 'notValid', 1, 'notValid'])

    # Write last stage data to '.csv' file
    dataToWrite = mytrace[stageNum][0]
    logFile.write("\n\n\t\tWriting samples from stage {} to csv file".format(stageNum))

    stringToAppend = 'resultsStage{}.csv'.format(stageNum)
    resultsFilePath = os.path.join(os.path.abspath(resultsLocation), stringToAppend)

    with open(resultsFilePath, 'w', newline='') as csvfile:
        csvWriter = csv.writer(csvfile)
        csvWriter.writerows(dataToWrite)
    logFile.write("\n\t\t\tWrote to file {}".format(resultsFilePath))

    if parallelize_MCMC == 'yes':
        if run_type == "runningLocal":
            pool.close()

    # logFile.write("\n\t\tevidence = %.10f" % evidence)

    return mytrace
