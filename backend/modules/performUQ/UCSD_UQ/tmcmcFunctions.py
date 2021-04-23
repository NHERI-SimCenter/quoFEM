"""
authors: Mukesh Kumar Ramancha, Maitreya Manoj Kurumbhati, and Prof. J.P. Conte 
affiliation: University of California, San Diego

"""

import numpy as np
from runFEM import runFEM


def initial_population(N, p):
    IniPop = np.zeros((N, len(p)))
    for i in range(len(p)):
        IniPop[:, i] = p[i].generate_rns(N)
    return IniPop


def log_prior(s, p):
    logP = 0
    for i in range(len(s)):
        logP = logP + p[i].log_pdf_eval(s[i])
    return logP


def propose(current, covariance, n):
    return np.random.multivariate_normal(current, covariance, n)


def compute_beta(beta, likelihoods, prev_ESS, threshold):
    old_beta = beta
    min_beta = beta
    max_beta = 2.0
    #rN = int(len(likelihoods) * 0.95)   #pymc3 uses 0.5
    rN = threshold*prev_ESS #purdue prof uses 0.95
    while max_beta - min_beta > 1e-6:
        new_beta = 0.5*(max_beta+min_beta)
        #plausible weights of Sm corresponding to new beta
        inc_beta = new_beta-old_beta
        Wm = np.exp(inc_beta*(likelihoods - likelihoods.max()))
        ESS = int(1/np.sum((Wm/sum(Wm))**2))
        if ESS == rN:
            break
        elif ESS < rN:
            max_beta = new_beta
        else:
            min_beta = new_beta
            
    if new_beta >= 1:
        new_beta = 1
        #plausible weights of Sm corresponding to new beta
        inc_beta = new_beta-old_beta
        Wm = np.exp(inc_beta*(likelihoods - likelihoods.max()))
        
    return new_beta, Wm, ESS


# MCMC
def MCMC_MH(ParticleNum, Em, Nm_steps, current, likelihood_current, posterior_current, beta, numAccepts, AllPars,
            log_likelihood, variables, resultsLocation, rng):
    all_proposals = []
    all_PLP = []

    # deltas = propose(np.zeros(len(current)), Em, Nm_steps)
    deltas = rng.multivariate_normal(np.zeros(len(current)),Em,Nm_steps)
    
    for j2 in range(Nm_steps):
        delta = deltas[j2]
        proposal = current + delta
        prior_proposal = log_prior(proposal, AllPars)

        if np.isfinite(prior_proposal):  # proposal satisfies the prior constraints
            # likelihood_proposal = log_likelihood(ParticleNum, proposal, variables, resultsLocation)
            likelihood_proposal = runFEM(ParticleNum, proposal, variables, resultsLocation, log_likelihood)
            if np.isnan(likelihood_proposal):
                likelihood_proposal = -np.Inf
            posterior_proposal = prior_proposal + likelihood_proposal * beta
        else:
            likelihood_proposal = -np.Inf  # dont run the FE model
            posterior_proposal = -np.Inf

        log_acceptance = posterior_proposal - posterior_current

        all_proposals.append(proposal)
        all_PLP.append([prior_proposal, likelihood_proposal, posterior_proposal])

        # if np.isfinite(log_acceptance) and (np.log(np.random.uniform()) < log_acceptance):
        if np.isfinite(log_acceptance) and (np.log(rng.uniform()) < log_acceptance):
            # accept
            current = proposal
            posterior_current = posterior_proposal
            likelihood_current = likelihood_proposal
            numAccepts += 1

    # gather all last samples
    return current, likelihood_current, posterior_current, numAccepts, all_proposals, all_PLP
