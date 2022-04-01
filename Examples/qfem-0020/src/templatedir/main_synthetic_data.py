"""
Main file for hierarchical Bayesian inference of Steel02 material model parameters assuming 
multivariate normal distribution for the population of the material model parameters
"""
import os
import pickle
import sys
import time
from datetime import datetime

import numpy as np


from synthetic_data import read_synthetic_data
from hierarchical_inference_sampling_v2 import HierarchicalInferenceSampling


class HeirBayesSampler(object):
    def perform_sampling(self, n_samples=50, n_burn_in=20, tuning_interval=10, seed=1):
        DATA_LOCATION = os.path.abspath(os.path.join(__file__, "../synthetic_data"))

        coupon_strain_data, coupon_stress_data = read_synthetic_data(DATA_LOCATION)

        NUM_COUPONS = len(coupon_stress_data)

        norm_pars = np.array([480, 205000, 0.019, 0.847, 0.183, 0.029, 0.017])

        mean_theta = norm_pars / norm_pars

        D_theta = np.diag(0.2 * mean_theta)
        R_theta = np.diag(np.ones(7))
        cov_theta = D_theta @ R_theta @ D_theta

        start_point = [mean_theta for coupon_num in range(NUM_COUPONS)]

        start_point.append(cov_theta)
        start_point.append(mean_theta)

        noise = [80.0 for coupon_num in range(NUM_COUPONS)]

        start_point.append(np.array(noise))

        N_SAMPLES = n_samples
        TUNING_INTERVAL = tuning_interval
        BURN_IN = n_burn_in
        SEEDS = np.random.SeedSequence(seed).spawn(NUM_COUPONS+3)
        RNGS = [np.random.default_rng(s) for s in SEEDS]

        inf_object = HierarchicalInferenceSampling(
            coupon_strain_data,
            coupon_stress_data,
            start_point,
            N_SAMPLES,
            BURN_IN,
            TUNING_INTERVAL,
            RNGS
        )

        trace, time_taken = inf_object.perform_mcmc_within_gibbs()
        return trace, time_taken, inf_object, NUM_COUPONS

    def save_results(self, trace, time_taken, inf_object, prefix):
        data_to_dump = [trace, inf_object, time_taken]
        timestamp = datetime.now().strftime("%Y_%m_%d_%H_%M_%S")
        filename = "".join([prefix, "_", timestamp, ".pickle"])
        pickle.dump(
            data_to_dump,
            open(filename, "wb"),
        )
        return filename


def main():
    t1 = time.time()
    sampler = HeirBayesSampler()
    trace, time_taken, inf_object = sampler.perform_sampling(
        n_samples=50, n_burn_in=20, tuning_interval=10, seed=1
    )
    filename = sampler.save_results(
        trace, time_taken, inf_object, prefix="synthetic_data"
    )
    # make_plots(filename)

    print("Done!")
    print("Time elapsed: {:0.2f} minutes".format((time.time() - t1) / 60))


if __name__ == "__main__":
    main()
