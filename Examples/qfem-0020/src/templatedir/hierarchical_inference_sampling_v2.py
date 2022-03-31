import numpy as np
import hierarchical_standard_distributions
from coupon import Coupon
import time
from coupon_posterior_sample import MHSampleCoupon
from multiprocessing import Pool
import multiprocessing as mp


class HierarchicalInferenceSampling:
    def __init__(self, coupon_strains, coupon_stresses, start_point, n_samples, burn_in, tuning_interval, rngs):
        self.coupon_strains = coupon_strains
        self.coupon_stresses = coupon_stresses
        self.n_samples = n_samples
        self.burn_in = burn_in
        self.start_point = start_point
        self.tuning_interval = tuning_interval
        self.rngs = rngs
        self.n_coupons = len(self.coupon_stresses)

        self.total_data_points = 0
        for coupon in range(self.n_coupons):
            self.total_data_points = self.total_data_points + len(coupon_strains[coupon])

    def initialize(self):

        trace = [self.start_point]

        mh_sampler = MHSampleCoupon()
        coupon_dict = {}

        for coupon_counter in range(len(self.coupon_strains)):
            coupon_dict[str(coupon_counter)] = (Coupon(self.coupon_strains[coupon_counter],
                                                       self.coupon_stresses[coupon_counter], coupon_counter))

            coupon_dict[str(coupon_counter)].current_ai_term = mh_sampler.get_a_term(trace[0][coupon_counter], self.
                                                                                     coupon_strains[coupon_counter],
                                                                                     self.
                                                                                     coupon_stresses[coupon_counter])

        return trace, coupon_dict

    def perform_mcmc_within_gibbs(self):

        mh_sampler = MHSampleCoupon()
        start_time = time.time()

        trace, coupon_dict = self.initialize()

        current_pool = Pool(processes=mp.cpu_count())

        for sample_counter in range(1, self.n_samples + 1):

            hyper_mean_current = trace[-1][-2]
            hyper_cov_current = trace[-1][-3]
            noise_sd = trace[-1][-1]

            if (sample_counter % self.tuning_interval == 0) and (sample_counter < self.burn_in) is True:

                for coupon_counter in range(len(coupon_dict)):
                    coupon_dict[str(coupon_counter)].set_proposal_cov(trace, self.tuning_interval)

            """coupon sampling"""

            # create input for mh

            if len(trace) == 1:

                for coupon_counter in range(len(coupon_dict)):
                    coupon_dict[str(coupon_counter)].current_cov_kernel = hyper_cov_current

            coupons_mh_input = []
            rngs = []
            for coupon_counter in range(len(coupon_dict)):
                temp_input = [coupon_dict[str(coupon_counter)].get_samples_from_trace(trace)[-1],
                              coupon_dict[str(coupon_counter)].strain_history,
                              coupon_dict[str(coupon_counter)].stress_history,
                              coupon_dict[str(coupon_counter)].current_cov_kernel,
                              coupon_dict[str(coupon_counter)].acceptance_counter,
                              coupon_dict[str(coupon_counter)].current_ai_term,
                              hyper_mean_current, hyper_cov_current, noise_sd[coupon_counter]]

                coupons_mh_input.append(temp_input)
                rngs.append(self.rngs[coupon_counter])
                # coupons_mh_input.append(self.rngs[coupon_counter])

            ''' multi-processing toggle '''

            # get mh samples and other outputs
            #
            mh_outputs = current_pool.starmap(mh_sampler.perform_mh, zip(coupons_mh_input, rngs))

            # mh_outputs = [mh_sampler.perform_mh(coupons_mh_input[coupon_counter]) for
            #               coupon_counter in range(len(coupon_dict))]

            sample = [coupon_output[0] for coupon_output in mh_outputs]

            # update coupon attributes

            for coupon_counter in range(len(coupon_dict)):
                coupon_dict[str(coupon_counter)].current_ai_term = mh_outputs[coupon_counter][1]
                coupon_dict[str(coupon_counter)].acceptance_counter = mh_outputs[coupon_counter][2]

            temp_samp = np.asarray(sample)
            temp_samp_mean = np.mean(temp_samp, 0)

            """cov matrix sampling"""

            S_term = 0
            for coupon_counter in range(len(coupon_dict)):
                diff_term_temp = (np.reshape(np.asarray(sample[coupon_counter]), (-1, 1)) -
                                  np.reshape(np.asarray(temp_samp_mean), (-1, 1)))

                S_term = S_term + diff_term_temp @ diff_term_temp.T

            scale_matrix = np.diag(np.ones(7)) * 1e-6

            cov_sample = hierarchical_standard_distributions. \
                get_inverse_wishart_sample(np.shape(S_term)[0] + self.n_coupons, S_term + scale_matrix, rng=self.rngs[self.n_coupons])

            sample.append(cov_sample)

            """mean vector sampling"""

            mu_theta_sample = hierarchical_standard_distributions. \
                get_multivariate_normal_sample(np.asarray(temp_samp_mean), cov_sample / self.n_coupons, rng=self.rngs[self.n_coupons+1])

            sample.append(mu_theta_sample)

            """noise sampling"""

            k_prior_sigma = 1
            sigma0_prior_sigma = 1

            coupon_noise_samples = []

            for coupon_counter in range(len(coupon_dict)):
                a_i_term = coupon_dict[str(coupon_counter)].current_ai_term
                data_length = coupon_dict[str(coupon_counter)].data_length
                posterior_dof_sigma = k_prior_sigma + data_length
                posterior_scale_sigma = (a_i_term * data_length + k_prior_sigma * sigma0_prior_sigma) / \
                                        posterior_dof_sigma

                coupon_noise_samples.append(hierarchical_standard_distributions.
                                            get_scaled_inverse_chi_squared_sample(posterior_dof_sigma,
                                                                                  posterior_scale_sigma, rng=self.rngs[self.n_coupons+2]))

            sample.append(coupon_noise_samples)

            trace.append(sample)

        trace_to_return = trace[self.burn_in+1:]
        time_elapsed = time.time() - start_time

        return trace_to_return, time_elapsed
