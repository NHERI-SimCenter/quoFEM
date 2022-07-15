import numpy as np
import openseespy.opensees as ops


class MHSampleCoupon:
    def __init__(self):
        pass

    @staticmethod
    def get_fe_response(sample, strain):

        strain_input = strain

        sample = sample * np.array([480, 205000, 0.019, 0.847, 0.183, 0.029, 0.017])

        fy, E, b, cR1, cR2, a1, a3 = sample[0], sample[1], sample[2], sample[3], sample[4], sample[5], sample[6]

        R0 = 20.0

        ops.wipe()
        materialTag = 0

        ops.uniaxialMaterial('Steel02', materialTag, fy, E, b, R0, cR1, cR2, a1, 1.0, a3, 1.0, 0.0)

        ops.testUniaxialMaterial(materialTag)

        stress = []

        for eps in strain_input:
            ops.setStrain(eps)
            stress.append(ops.getStress())

        return np.asarray(stress)

    def get_a_term(self, sample, strain, stress):

        pred = self.get_fe_response(sample, strain)

        a_term = np.mean((pred - stress) ** 2)

        return a_term

    def perform_mh(self, input_list, rng):

        # extract input
        sample = input_list[0]
        strain = input_list[1]
        stress = input_list[2]
        cov_kernel = input_list[3]
        current_acceptance = input_list[4]
        current_a_term = input_list[5]
        mu_theta = input_list[6]
        cov_theta = input_list[7]
        noise = input_list[8]

        # generate proposal
        # delta_samp = (np.random.multivariate_normal(np.zeros(len(sample)), cov_kernel, 1)).flatten()
        delta_samp = (rng.multivariate_normal(np.zeros(len(sample)), cov_kernel, 1)).flatten()
        proposal = np.asarray(sample) + delta_samp

        # compute J theta terms
        mu_theta_temp = np.reshape(np.array(mu_theta), (-1, 1))
        current_temp = np.reshape(np.array(sample), (-1, 1))
        current_diff = current_temp - mu_theta_temp
        j_theta_current = (-0.5) * (current_diff.T @ cov_theta @ current_diff)

        proposal_temp = np.reshape(np.array(sample), (-1, 1))
        proposal_diff = proposal_temp - mu_theta_temp
        j_theta_proposal = (-0.5) * (proposal_diff.T @ cov_theta @ proposal_diff)

        # get FE prediction
        fe_pred = self.get_fe_response(proposal, strain)

        # get term 'a' for proposal
        term_a_prop = self.get_a_term(proposal, strain, stress)

        # compute log posteriors
        log_post_prop = (-len(fe_pred) * term_a_prop / 2 / noise) + j_theta_proposal[0][0]
        log_post_current = (-len(fe_pred) * current_a_term / 2 / noise) + j_theta_current[0][0]

        if np.isnan(fe_pred).any() is True:

            log_post_prop = -np.Inf
            term_a_prop = current_a_term

        log_acceptance = log_post_prop - log_post_current
        # print(log_acceptance)

        if np.isfinite(log_acceptance) and (np.log(rng.uniform()) < log_acceptance):

            current_acceptance += 1

            return [proposal.tolist(), term_a_prop, current_acceptance]

        else:

            return [sample, current_a_term, current_acceptance]
