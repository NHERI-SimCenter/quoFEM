import scipy.special
import scipy.stats


def get_scaled_inverse_chi_squared_sample(dof, scale_par, rng):
    samp = scipy.stats.invgamma.rvs(a=dof / 2, scale=dof * scale_par / 2, size=1, random_state=rng)
    return samp[0]


def get_inverse_wishart_sample(dof, scale_matrix, rng):
    return scipy.stats.invwishart.rvs(df=dof, scale=scale_matrix, size=1, random_state=rng)


def get_multivariate_normal_sample(mean_vector, cov_matrix, rng):
    sample = scipy.stats.multivariate_normal.rvs(
        mean=mean_vector, cov=cov_matrix, size=1, random_state=rng
    ).flatten()
    return sample.tolist()
