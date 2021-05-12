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


def RunTMCMC(N, AllPars, Nm_steps_max, Nm_steps_maxmax, log_likelihood, variables, resultsLocation, seed,
             calDataFile, numExperiments, covarianceMatrixList, edpNamesList, edpLengthsList, covarianceTypeList):
    """ Runs TMCMC Algorithm """

    # Initialize (beta, effective sample size)
    beta = 0
    ESS = N
    mytrace = []

    # Initialize other TMCMC variables
    Nm_steps = Nm_steps_max
    parallelize_MCMC = 'yes'  # yes or no
    Adap_calc_Nsteps = 'yes'  # yes or no
    Adap_scale_cov = 'yes'  # yes or no
    scalem = 1  # cov scale factor
    evidence = 1  # model evidence

    # initial samples
    Sm = tmcmcFunctions.initial_population(N, AllPars)

    # Evaluate posterior at Sm
    Priorm = np.array([tmcmcFunctions.log_prior(s, AllPars) for s in Sm]).squeeze()
    Postm = Priorm  # prior = post for beta = 0

    # Evaluate log-likelihood at current samples Sm
    if parallelize_MCMC == 'yes':
        pool = Pool(processes=mp.cpu_count())
        Lmt = pool.starmap(runFEM, [(ind, Sm[ind], variables, resultsLocation, log_likelihood, calDataFile,
                                     numExperiments, covarianceMatrixList,
                                     edpNamesList, edpLengthsList, covarianceTypeList) for ind in range(N)], )
        pool.close()
        Lm = np.array(Lmt).squeeze()
    else:
        Lm = np.array([runFEM(ind, Sm[ind], variables, resultsLocation, log_likelihood, calDataFile,
                              numExperiments, covarianceMatrixList,
                              edpNamesList, edpLengthsList, covarianceTypeList) for ind in range(N)]).squeeze()

    while beta < 1:
        # adaptivly compute beta s.t. ESS = N/2 or ESS = 0.95*prev_ESS
        # plausible weights of Sm corresponding to new beta
        beta, Wm, ESS = tmcmcFunctions.compute_beta(beta, Lm, ESS, threshold=0.95)

        # update model evidence
        evidence = evidence * (sum(Wm) / N)

        # Calculate covaraince matrix using Wm_n
        Cm = np.cov(Sm, aweights=Wm / sum(Wm), rowvar=0)

        # Resample ###################################################
        # Resampling using plausible weights
        SmcapIDs = np.random.choice(range(N), N, p=Wm / sum(Wm))
        # SmcapIDs = resampling.stratified_resample(Wm_n)
        Smcap = Sm[SmcapIDs]
        Lmcap = Lm[SmcapIDs]
        Postmcap = Postm[SmcapIDs]

        # save to trace
        # stage m: samples, likelihood, weights, next stage ESS, next stage beta, resampled samples
        mytrace.append([Sm, Lm, Wm, ESS, beta, Smcap])

        # print
        print("beta = %.5f" % beta)
        print("ESS = %d" % ESS)
        print("scalem = %.2f" % scalem)

        # Perturb ###################################################
        # perform MCMC starting at each Smcap (total: N) for Nm_steps
        Em = (scalem ** 2) * Cm  # Proposal dist covariance matrix

        numProposals = N * Nm_steps
        numAccepts = 0

        # seed to reproduce results
        ss = SeedSequence(seed)
        child_seeds = ss.spawn(N)

        if parallelize_MCMC == 'yes':
            pool = Pool(processes=mp.cpu_count())
            results = pool.starmap(tmcmcFunctions.MCMC_MH, [(j1, Em, Nm_steps, Smcap[j1], Lmcap[j1], Postmcap[j1], beta,
                                                             numAccepts, AllPars, log_likelihood, variables,
                                                             resultsLocation, default_rng(child_seeds[j1]),
                                                             calDataFile, numExperiments, covarianceMatrixList,
                                                             edpNamesList, edpLengthsList, covarianceTypeList) for j1 in
                                                            range(N)], )
            pool.close()
        else:
            results = [
                tmcmcFunctions.MCMC_MH(j1, Em, Nm_steps, Smcap[j1], Lmcap[j1], Postmcap[j1], beta, numAccepts, AllPars,
                                       log_likelihood, variables, resultsLocation, default_rng(child_seeds[j1]),
                                       calDataFile, numExperiments, covarianceMatrixList,
                                       edpNamesList, edpLengthsList, covarianceTypeList) for j1 in range(N)]

        Sm1, Lm1, Postm1, numAcceptsS, all_proposals, all_PLP = zip(*results)
        Sm1 = np.asarray(Sm1)
        Lm1 = np.asarray(Lm1)
        Postm1 = np.asarray(Postm1)
        numAcceptsS = np.asarray(numAcceptsS)
        numAccepts = sum(numAcceptsS)
        all_proposals = np.asarray(all_proposals)
        all_PLP = np.asarray(all_PLP)

        # total observed acceptance rate
        R = numAccepts / numProposals
        print("acceptance rate = %.2f" % R)

        # Calculate Nm_steps based on observed acceptance rate
        if Adap_calc_Nsteps == 'yes':
            # increase max Nmcmc with stage number
            Nm_steps_max = min(Nm_steps_max + 1, Nm_steps_maxmax)
            print("adapted max MCMC steps = %d" % Nm_steps_max)

            acc_rate = max(1. / numProposals, R)
            Nm_steps = min(Nm_steps_max, 1 + int(np.log(1 - 0.99) / np.log(1 - acc_rate)))
            print("next MCMC Nsteps = %d" % Nm_steps)

        print('========================')

        # scale factor based on observed acceptance ratio
        if Adap_scale_cov == 'yes':
            scalem = (1 / 9) + ((8 / 9) * R)

        # for next beta
        Sm, Postm, Lm = Sm1, Postm1, Lm1

    # save to trace
    mytrace.append([Sm, Lm, np.ones(len(Wm)), 'notValid', 1, 'notValid'])

    print("evidence = %.10f" % evidence)

    return mytrace
