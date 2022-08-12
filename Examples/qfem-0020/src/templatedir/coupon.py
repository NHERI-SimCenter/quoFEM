import numpy as np
# import openseespy.opensees as ops


class Coupon:
    def __init__(self, strain_history, stress_history, index):
        self.strain_history = strain_history
        self.stress_history = stress_history
        self.coupon_index = index
        self.acceptance_counter = 0
        self.proposal_scale = 1
        self.current_ai_term = 0
        self.current_cov_kernel = []
        self.data_length = len(self.strain_history)

    # def get_fe_response(self, sample):
    #     strain_input = self.strain_history
    #
    #     fy, E, b, cR1, cR2, a1, a3 = sample[0], sample[1], sample[2], sample[3], sample[4], sample[5], sample[6]
    #
    #     R0 = 20.0
    #
    #     ops.wipe()
    #     materialTag = 0
    #
    #     ops.uniaxialMaterial('Steel02', materialTag, fy, E, b, R0, cR1, cR2, a1, 1.0, a3, 1.0, 0.0)
    #
    #     ops.testUniaxialMaterial(materialTag)
    #
    #     stress = []
    #
    #     for eps in strain_input:
    #         ops.setStrain(eps)
    #
    #         temp = ops.getStress()
    #
    #         if temp != temp:
    #             temp = 0
    #
    #         stress.append(temp)
    #
    #     return np.asarray(stress)
    #
    # def get_sum_squared_error_pred(self, sample):
    #
    #     stress_history = self.stress_history
    #     fe_prediction = self.get_fe_response(sample)
    #
    #     term_a = np.mean((fe_prediction - stress_history) ** 2)
    #
    #     return term_a

    def get_samples_from_trace(self, trace_input):

        trace = trace_input
        index = self.coupon_index
        coupon_samples = []

        for sample_counter in range(len(trace)):
            coupon_samples.append(trace[sample_counter][index])

        return coupon_samples

    def set_proposal_cov(self, trace_input, tuning_interval):

        coupon_samples = self.get_samples_from_trace(trace_input)
        acc_rate = self.acceptance_counter / tuning_interval
        self.proposal_scale = self.tune(self.proposal_scale, acc_rate)
        self.acceptance_counter = 0

        cov_kernel = self.current_cov_kernel

        if acc_rate > 0.1:

            cov_kernel = np.cov((np.asarray(coupon_samples[-tuning_interval:])).transpose())

        cov_kernel = cov_kernel * self.proposal_scale

        self.current_cov_kernel = cov_kernel

        return cov_kernel

    @staticmethod
    def tune(scale, acc_rate):
        """
        Tunes the scaling parameter for the proposal distribution
        according to the acceptance rate over the last tune_interval:
        Rate    Variance adaptation
        ----    -------------------
        <0.001        x 0.1
        <0.05         x 0.5
        <0.2          x 0.9
        >0.5          x 1.1
        >0.75         x 2
        >0.95         x 10
        """
        if acc_rate < 0.001:
            # reduce by 90 percent
            return scale * 0.1
        elif acc_rate < 0.05:
            # reduce by 50 percent
            return scale * 0.5
        elif acc_rate < 0.2:
            # reduce by ten percent
            return scale * 0.9
        elif acc_rate > 0.95:
            # increase by factor of ten
            return scale * 10.0
        elif acc_rate > 0.75:
            # increase by double
            return scale * 2.0
        elif acc_rate > 0.5:
            # increase by ten percent
            return scale * 1.1

        return scale

    # def get_log_posterior_params(self, sample, trace_input):
    #
    #     hyper_mean_current = trace_input[-1][-3]
    #     hyper_cov_current = trace_input[-1][-2]
    #     noise_sd = trace_input[-1][-1]
    #
    #     a_term = self.get_sum_squared_error_pred(sample)
    #
    #     theta_temp = np.reshape(np.array(sample), (-1, 1))
    #     mu_theta_temp = np.reshape(np.array(hyper_mean_current), (-1, 1))
    #
    #     diff_theta = theta_temp - mu_theta_temp
    #
    #     j_theta = (-0.5) * (diff_theta.T @ hyper_cov_current @ diff_theta)
    #
    #     log_post = (-self.data_length * a_term / 2 / noise_sd) + j_theta[0][0]
    #
    #     return a_term, j_theta, log_post
    #
    # def get_mh_sample(self, trace):
    #
    #     coupon_samples = self.get_samples_from_trace(trace)
    #     current = coupon_samples[-1]
    #
    #     if len(coupon_samples) == 1:
    #
    #         a_term, j_theta, log_post = self.get_log_posterior_params(current, trace)
    #         self.current_ai_term = a_term
    #         self.current_J_theta = j_theta
    #         self.current_log_posterior = log_post
    #         self.current_cov_kernel = trace[-1][-2]
    #
    #     delta_samp = (np.random.multivariate_normal(np.zeros(len(current)), self.current_cov_kernel, 1)).flatten()
    #
    #     proposal = np.asarray(current) + delta_samp
    #     a_term_prop, j_theta_prop, log_post_prop = self.get_log_posterior_params(proposal, trace)
    #
    #     log_acceptance = log_post_prop - self.current_log_posterior
    #
    #     if np.isfinite(log_acceptance) and (np.log(np.random.uniform()) < log_acceptance):
    #
    #         self.current_ai_term = a_term_prop
    #         self.current_J_theta = j_theta_prop
    #         self.current_log_posterior = log_post_prop
    #         self.acceptance_counter += 1
    #
    #         return proposal.tolist()
    #
    #     else:
    #
    #         return current
